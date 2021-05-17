/* Copyright (C) 2021 Krosny Kevin
 *
 */
/* This file is part of KevS.

    KevS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    KevS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "fuelwindow.h"
#include <QLabel>
#include <QGridLayout>
#include <QPalette>
#include <QPainter>
#ifdef QT_DEBUG
	#include <iostream>
#endif

#include "../irsdk/lap_timing.h"
#include "../helper.h"

FuelWindow::FuelWindow(QWidget *parent) : QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_TranslucentBackground);

	base_layout = new QGridLayout(parent);
	base_layout->setHorizontalSpacing(40);
	fuel_layout = new QGridLayout(parent);
	fuel_widget = new QWidget(parent);
	fuel_widget->setLayout(fuel_layout);
	base_layout->addWidget(fuel_widget, AMOUNT_LEFT_HEADER+AMOUNT_ADDITIONAL_HEADER, 1, 1, AMOUNT_TOP_HEADER);

	for (int i = 0; i < 4; i++) {
		verbrauch_layout[i] = new QGridLayout(parent);
		verbrauch_widgets[i] = new QWidget(parent);
		verbrauch_widgets[i]->setLayout(verbrauch_layout[i]);
		if (i < 3)
			base_layout->addWidget(verbrauch_widgets[i], i+1, AMOUNT_TOP_HEADER, 1, 1, Qt::AlignCenter);
		else
			fuel_layout->addWidget(verbrauch_widgets[i], 0, AMOUNT_TOP_HEADER, 1, 1, Qt::AlignCenter);
		verbrauch_layout[i]->setHorizontalSpacing(0);
		verbrauch_layout[i]->setVerticalSpacing(0);
	}
	for (int i = 0; i < 2; i++) {
		laps_layout[i] = new QGridLayout(parent);
		laps_widgets[i] = new QWidget(parent);
		laps_widgets[i]->setLayout(laps_layout[i]);
		fuel_layout->addWidget(laps_widgets[i], 0, i+1, 1, 1, Qt::AlignCenter);
		laps_layout[i]->setHorizontalSpacing(0);
		laps_layout[i]->setVerticalSpacing(0);
	}

	bg_color = QColor(128,128,255,128);

	header[0] = new QLabel("Refuel", parent);
	header[1] = new QLabel("Extra Lap", parent);
	header[2] = new QLabel("1 Lap", parent);
	header[3] = new QLabel("Avg", parent);
	header[4] = new QLabel("Trg 1", parent);
	header[5] = new QLabel("Trg 2", parent);
	header[6] = new QLabel("Laps", parent);
	header[7] = new QLabel("Tank", parent);

	for (int i = 0; i < AMOUNT_TOP_HEADER; i++)
		base_layout->addWidget(header[i], 0, i+1);
	for (int i = 0; i < AMOUNT_LEFT_HEADER; i++)
		base_layout->addWidget(header[i + AMOUNT_TOP_HEADER], i+1, 0);
	for (int i = 0; i < AMOUNT_ADDITIONAL_HEADER; i++) {
		base_layout->addWidget(header[i + AMOUNT_TOP_HEADER + AMOUNT_LEFT_HEADER], i+AMOUNT_LEFT_HEADER+1, 0);
	}

	for (int i = 0; i < AMOUNT_DATA; i++) {
		data[i] = new QLabel(parent);	
	}
	base_layout->addWidget(data[0], 1, 1);
	base_layout->addWidget(data[1], 1, 2);
	base_layout->addWidget(data[3], 2, 1);
	base_layout->addWidget(data[4], 2, 2);
	base_layout->addWidget(data[6], 3, 1);
	base_layout->addWidget(data[7], 3, 2);

	verbrauch_layout[0]->addWidget(new QLabel("Avg"), 0, 0, 1, 1, Qt::AlignBottom);
	verbrauch_layout[0]->addWidget(data[2], 1, 0, 1, 1, Qt::AlignTop);
	verbrauch_layout[1]->addWidget(new QLabel("Delta"), 0, 0, 1, 1, Qt::AlignBottom);
	verbrauch_layout[1]->addWidget(data[5], 1, 0, 1, 1, Qt::AlignTop);
	verbrauch_layout[2]->addWidget(new QLabel("Delta"), 0, 0, 1, 1, Qt::AlignBottom);
	verbrauch_layout[2]->addWidget(data[8], 1, 0, 1, 1, Qt::AlignTop);
	verbrauch_layout[3]->addWidget(new QLabel("Last"), 0, 0, 1, 1, Qt::AlignBottom);
	verbrauch_layout[3]->addWidget(data[13], 1, 0, 1, 1, Qt::AlignTop);

	base_layout->addWidget(data[10], AMOUNT_LEFT_HEADER+1, 1);
	laps_layout[1]->addWidget(new QLabel("FaE"), 0, 0, 1, 1, Qt::AlignBottom);
	laps_layout[1]->addWidget(data[9], 1, 0, 1, 1, Qt::AlignTop);
	fuel_layout->addWidget(data[11], 0, 0);
	laps_layout[0]->addWidget(new QLabel("remain"), 0, 0, 1, 1, Qt::AlignBottom);
	laps_layout[0]->addWidget(data[12], 1, 0, 1, 1, Qt::AlignTop);


	setWindowTitle("Fuel Window - KevS");
	setLayout(base_layout);
	paintEvent(nullptr);
}

void FuelWindow::setNewData(float fuel[])
{
	QPalette pal;

	data[0]->setText(QString(QString::number((fuel[0] > 0) ? fuel[0] : 0, 'f', precision[0]) + " l"));
	data_values[0] = fuel[0];
	for (int i = 1; i < AMOUNT_DATA-4; i++) {
		data[i]->setText(QString(QString::number(fuel[i], 'f', precision[0]) + " l"));
		data_values[i] = fuel[i];
	}
	if (fuel[5] > 0 && fuel[5] - delta_precision > 0) { // More consumed // Too much
		pal.setColor(data[5]->foregroundRole(), delta_color[2]);
		data[5]->setPalette(pal);
	} else if (fuel[5] < 0 && fuel[5] + delta_precision < 0) { // Less consumed // Saved
		pal.setColor(data[5]->foregroundRole(), delta_color[0]);
		data[5]->setPalette(pal);
	} else { // Neutral
		pal.setColor(data[5]->foregroundRole(), delta_color[1]);
		data[5]->setPalette(pal);
	}
	if (fuel[8] > 0 && fuel[8] - delta_precision > 0) { // More consumed // Too much
		pal.setColor(data[8]->foregroundRole(), delta_color[2]);
		data[8]->setPalette(pal);
	} else if (fuel[8] < 0 && fuel[8] + delta_precision < 0) { // Less consumed // Saved
		pal.setColor(data[8]->foregroundRole(), delta_color[0]);
		data[8]->setPalette(pal);
	} else { // Neutral
		pal.setColor(data[8]->foregroundRole(), delta_color[1]);
		data[8]->setPalette(pal);
	}

	data[AMOUNT_DATA-5]->setText(QString(QString::number((fuel[0] < 0) ? fuel[0]*-1 : 0, 'f', precision[2])) + " l");
	data[AMOUNT_DATA-4]->setText(QString("%1 / " + QString::number(fuel[AMOUNT_DATA-4], 'f', precision[1]) + " Laps").arg(fuel[AMOUNT_DATA-5]));
	data[AMOUNT_DATA-2]->setText(QString(QString::number(fuel[AMOUNT_DATA-3], 'f', precision[1])+  " Laps"));
	data[AMOUNT_DATA-1]->setText(QString(QString::number(fuel[AMOUNT_DATA-2], 'f', precision[0]) + " l"));
}

void FuelWindow::paintEvent(QPaintEvent* event)
{
	QPainter customPainter(this);
	QPalette pal;

	setAttribute(Qt::WA_TranslucentBackground); // Ich weiß nicht ob das hier weg darf
	for (int i = 0; i < AMOUNT_TOP_HEADER; i++) {
		pal.setColor(header[i]->foregroundRole(), header_color.toRgb());
		header[i]->setPalette(pal);
		header[i]->setFont(header_font);
	}
	int k = 0;
	for (int i = 0; i < AMOUNT_LEFT_HEADER; i++) {
		pal.setColor(header[i + AMOUNT_TOP_HEADER]->foregroundRole(), row_color[i].toRgb());
		header[i+AMOUNT_TOP_HEADER]->setPalette(pal);
		header[i+AMOUNT_TOP_HEADER]->setFont(row_font[i]);

		data[k]->setPalette(pal);
		data[k++]->setFont(row_font[i]);
		data[k]->setPalette(pal);
		data[k++]->setFont(row_font[i]);
		k++;
	}
	for (int i = 0; i < 4; i++) {
		pal.setColor(verbrauch_widgets[i]->layout()->itemAt(0)->widget()->foregroundRole(), row_color[(i < 3) ? i : 4].toRgb());
		QFont buffer = row_font[(i < 3) ? i : 4];
		buffer.setPointSize(row_font[(i < 3) ? i : 4].pointSize() / 2 + 1);
		verbrauch_widgets[i]->layout()->itemAt(0)->widget()->setFont(buffer);
		verbrauch_widgets[i]->layout()->itemAt(0)->widget()->setPalette(pal);
		verbrauch_widgets[i]->layout()->itemAt(1)->widget()->setFont(row_font[(i < 3) ? i : 4]);
		if (i < 1 || i > 2)
			verbrauch_widgets[i]->layout()->itemAt(1)->widget()->setPalette(pal); // Delta hat eigene Farben
	}

	k = AMOUNT_TOP_HEADER+AMOUNT_LEFT_HEADER;
	pal.setColor(header[k]->foregroundRole(), row_color[AMOUNT_LEFT_HEADER].toRgb());
	header[k]->setPalette(pal);
	header[k]->setFont(row_font[AMOUNT_LEFT_HEADER]);
	data[AMOUNT_DATA-4]->setPalette(pal);
	data[AMOUNT_DATA-4]->setFont(row_font[AMOUNT_LEFT_HEADER]);

	pal.setColor(header[++k]->foregroundRole(), row_color[AMOUNT_LEFT_HEADER+1].toRgb());
	header[k]->setPalette(pal);
	header[k]->setFont(row_font[AMOUNT_LEFT_HEADER+1]);
	data[AMOUNT_DATA-3]->setPalette(pal);
	data[AMOUNT_DATA-3]->setFont(row_font[AMOUNT_LEFT_HEADER+1]);
	for (int i = 0; i < 2; i++) {
		pal.setColor(laps_widgets[i]->layout()->itemAt(0)->widget()->foregroundRole(), row_color[4].toRgb());
		QFont buffer = row_font[4];
		buffer.setPointSize(row_font[4].pointSize() / 2 + 1);
		laps_widgets[i]->layout()->itemAt(0)->widget()->setFont(buffer);
		laps_widgets[i]->layout()->itemAt(0)->widget()->setPalette(pal);
		laps_widgets[i]->layout()->itemAt(1)->widget()->setFont(row_font[4]);
		laps_widgets[i]->layout()->itemAt(1)->widget()->setPalette(pal);
	}

	customPainter.fillRect(rect(), bg_color);
}

void FuelWindow::setNewColors(QColor bg, QColor th, QColor row[], QColor delta[])
{
#ifdef QT_DEBUG
		std::cout << "------"  << "Background" << std::endl;
#endif
	bg_color = bg;
#ifdef QT_DEBUG
		std::cout << "BG: " << bg.red() << " " << bg.green() << " " << bg.blue() << std::endl;
		std::cout << "------"  << "Top Header" << std::endl;
#endif
	header_color = th;
#ifdef QT_DEBUG
		std::cout << "TH: " << th.red() << " " << th.green() << " " << th.blue() << std::endl;
		std::cout << "------"  << "Rows" << std::endl;
#endif
	for (int i = 0; i < AMOUNT_ROW; i++) {
#ifdef QT_DEBUG
		std::cout << i <<": " << row[i].red() << " " << row[i].green() << " " << row[i].blue() << std::endl;
#endif
		row_color[i] = row[i];
	}
	for (int i = 0; i < 3; i++)
		delta_color[i] = delta[i];
	paintEvent(nullptr);
}
void FuelWindow::setNewFonts(QFont hfont, QFont fonts[])
{
	header_font = hfont;
	for (int i = 0; i < AMOUNT_ROW; i++)
		row_font[i] = fonts[i];
	paintEvent(nullptr);
}

void FuelWindow::target_1_changed(double i)
{
	if (i > 0) {
		//data[AMOUNT_TOP_HEADER*AMOUNT_LEFT_HEADER-1]->show();
		verbrauch_widgets[1]->show();
		header[AMOUNT_TOP_HEADER + 1]->show();
		data[AMOUNT_TOP_HEADER]->show();
		data[AMOUNT_TOP_HEADER+1]->show();
		data[AMOUNT_TOP_HEADER+2]->show();
		header[AMOUNT_TOP_HEADER + 1]->setText(QString("Trg ") + QString::number(i, 'f', 2));
		if (b_target_2_visible) {
			verbrauch_widgets[2]->show();
			header[AMOUNT_TOP_HEADER + 2]->show();
			for (int k = 0; k < AMOUNT_TOP_HEADER; k++)
				data[6 + k]->show();
		}
	} else {
		//data[AMOUNT_TOP_HEADER*AMOUNT_LEFT_HEADER-1]->hide();
		verbrauch_widgets[1]->hide();
		verbrauch_widgets[2]->hide();
		header[AMOUNT_TOP_HEADER + 1]->hide();
		header[AMOUNT_TOP_HEADER + 2]->hide();
		for (int k = 0; k < 6; k++)
			data[AMOUNT_TOP_HEADER + k]->hide();
	}
}
void FuelWindow::target_2_changed(double i)
{
	if (i > 0) {
		b_target_2_visible = true;
		if (header[AMOUNT_TOP_HEADER + 1]->isVisible()) {
			verbrauch_widgets[2]->show();
			header[AMOUNT_TOP_HEADER + 2]->show();
			for (int k = 0; k < AMOUNT_TOP_HEADER; k++)
				data[6 + k]->show();
		}
		header[AMOUNT_TOP_HEADER+2]->setText(QString("Trg ") + QString::number(i, 'f', 2));
	} else {
		b_target_2_visible = false;
		//data[AMOUNT_TOP_HEADER*AMOUNT_LEFT_HEADER-1]->hide();
		verbrauch_widgets[2]->hide();
		header[AMOUNT_TOP_HEADER + 2]->hide();
		for (int k = 0; k < AMOUNT_TOP_HEADER; k++)
			data[6 + k]->hide();
	}
}

void FuelWindow::refresh_tank(float i)
{
	data[AMOUNT_DATA-3]->setText(QString(QString::number(i, 'f', 2) + " l"));
}

/*void FuelWindow::show_error(FuelError e)
{
	QString text;
	QMessageBox *mb = new QMessageBox(this);
	mb->setWindowTitle("Error");
	mb->setIcon(QMessageBox::Warning);
	mb->setDefaultButton(QMessageBox::Ok);

	switch (e) {
		case FuelError::No_Session_ID:
			text = QString("SessionID wurde nicht gefunden\nError-Code: F%1").arg(FuelError::No_Session_ID);
			break;
		case FuelError::No_Session_Type:
			text = QString("SessionType wurde nicht gefunden\nError-Code: F%1").arg(FuelError::No_Session_Type);
			break;
		default:
			text = "Unbekannter Error (0)";
	}
	mb->setText(text);
	mb->show();
}*/
