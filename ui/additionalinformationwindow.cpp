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

#include "additionalinformationwindow.h"

#include "QtAwesome/QtAwesome.h"
#include <QGridLayout>
#include <QLabel>
#include <QStylePainter>

#include <unistd.h>

AdditionalInformationWindow::AdditionalInformationWindow(QWidget *parent) : QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_TranslucentBackground);

	qta = new QtAwesome();
	qta->initFontAwesome();

	bg_color = QColor(255,255,255,128);

	base_layout = new QGridLayout(parent);

	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
		elements[i] = new QWidget(parent);
		additional_layouts[i] = new QGridLayout(parent);

		elements[i]->setLayout(additional_layouts[i]);
		base_layout->addWidget(elements[i], i, 0);
	}

	for (int i = 0; i < ValueTypes::AMNT_VALUE_TYPES; i++) {
		values[i] = new QLabel(parent);
	}

	air = new QLabel(QChar(fa::wind), parent);
	track = new QLabel(QChar(fa::road), parent);
	car = new QLabel(QChar(fa::car), parent);
	oil = new QLabel(QChar(fa::oil), parent);
	water = new QLabel(QChar(fa::water), parent);
	battery = new QLabel(QChar(fa::batteryhalf), parent);
	hys_usage = new QLabel(QChar(fa::bolt), parent);
	thermometer = new QLabel(QChar(fa::thermometerhalf), parent);
	air->setFont(qta->font(style::stfab, 32));
	track->setFont(qta->font(style::stfab, 32));
	car->setFont(qta->font(style::stfab, 32));
	oil->setFont(qta->font(style::stfab, 32));
	water->setFont(qta->font(style::stfab, 32));
	battery->setFont(qta->font(style::stfab, 32));
	hys_usage->setFont(qta->font(style::stfab, 32));
	thermometer->setFont(qta->font(style::stfab, 32));

	for (int i = 0; i < ValueTypes::AMNT_VALUE_TYPES; i++)
		values[i]->setText(QString::number(0));

	additional_layouts[ValueCategories::Wind]->addWidget(air, 0, 0);
	additional_layouts[ValueCategories::Car]->addWidget(oil, 0, 0, Qt::AlignRight);
	additional_layouts[ValueCategories::Car]->addWidget(water, 0, 2, Qt::AlignRight);
	additional_layouts[ValueCategories::Temp]->addWidget(thermometer, 0, 0, Qt::AlignRight);
	additional_layouts[ValueCategories::Temp]->addWidget(track, 0, 2, Qt::AlignRight);
	additional_layouts[ValueCategories::Battery]->addWidget(battery, 0, 0, Qt::AlignRight);
	additional_layouts[ValueCategories::Battery]->addWidget(hys_usage, 0, 2, Qt::AlignRight);

	additional_layouts[ValueCategories::Wind]->addWidget(values[ValueTypes::WindSpeed], 0, 2);
	additional_layouts[ValueCategories::Wind]->addWidget(values[ValueTypes::WindDir], 0, 3);
	additional_layouts[ValueCategories::Wind]->addWidget(values[ValueTypes::Humidity], 0, 4);
	additional_layouts[ValueCategories::Car]->addWidget(values[ValueTypes::OilTemp], 0, 1, Qt::AlignLeft);
	additional_layouts[ValueCategories::Car]->addWidget(values[ValueTypes::WaterTemp], 0, 3, Qt::AlignLeft);
	additional_layouts[ValueCategories::Temp]->addWidget(values[ValueTypes::AirTemp], 0, 1);
	additional_layouts[ValueCategories::Temp]->addWidget(values[ValueTypes::TrackTemp], 0, 3, Qt::AlignLeft);
	additional_layouts[ValueCategories::Battery]->addWidget(values[ValueTypes::BatteryStatus], 0, 1, Qt::AlignLeft);
	additional_layouts[ValueCategories::Battery]->addWidget(values[ValueTypes::BatteryUsage], 0, 3, Qt::AlignLeft);

	setWindowTitle("Information Window - KevS");
	setLayout(base_layout);
	paintEvent(nullptr);
}

void AdditionalInformationWindow::setNewData(float data[]) {
	values[ValueTypes::AirTemp]->setText(QString(QString::number(data[ValueTypes::AirTemp], 'f', precision[1]) + " °C"));
	values[ValueTypes::WindSpeed]->setText(QString(QString::number(data[ValueTypes::WindSpeed], 'f', precision[0]) + " km/h"));
	values[ValueTypes::Humidity]->setText(QString(QString::number(data[ValueTypes::Humidity], 'f', 0) + "%"));
	if (data[ValueTypes::WindDir] < 0)
		data[ValueTypes::WindDir] += 360;
	if (data[ValueTypes::WindDir] < 22.5)
		values[ValueTypes::WindDir]->setText(QString("N"));
	else if (data[ValueTypes::WindDir] < 67.5)
		values[ValueTypes::WindDir]->setText(QString("NE"));
	else if (data[ValueTypes::WindDir] < 112.5)
		values[ValueTypes::WindDir]->setText(QString("E"));
	else if (data[ValueTypes::WindDir] < 157.5)
		values[ValueTypes::WindDir]->setText(QString("SE"));
	else if (data[ValueTypes::WindDir] < 202.5)
		values[ValueTypes::WindDir]->setText(QString("S"));
	else if (data[ValueTypes::WindDir] < 247.5)
		values[ValueTypes::WindDir]->setText(QString("SW"));
	else if (data[ValueTypes::WindDir] < 292.5)
		values[ValueTypes::WindDir]->setText(QString("W"));
	else if (data[ValueTypes::WindDir] <337.5)
		values[ValueTypes::WindDir]->setText(QString("NW"));
	else
		values[ValueTypes::WindDir]->setText(QString("N"));

	values[ValueTypes::BatteryStatus]->setText(QString(QString::number(data[ValueTypes::BatteryStatus], 'f', 0) + " %"));
	values[ValueTypes::BatteryUsage]->setText(QString(QString::number(data[ValueTypes::BatteryUsage], 'f', 0) + " %"));

	values[ValueTypes::OilTemp]->setText(QString(QString::number(data[ValueTypes::OilTemp], 'f', 0) + " °C"));
	values[ValueTypes::WaterTemp]->setText(QString(QString::number(data[ValueTypes::WaterTemp], 'f', 0) + " °C"));

	values[ValueTypes::TrackTemp]->setText(QString(QString::number(data[ValueTypes::TrackTemp], 'f', precision[1]) + " °C"));


}
void AdditionalInformationWindow::setNewVisibilityForData(bool bShow, ValueCategories element)
{
	elements[element]->setVisible(bShow);
	row_visible[element] = bShow;
}
void AdditionalInformationWindow::setNewColors(QColor bg, QColor row[])
{
	bg_color = bg;
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++)
		row_color[i] = row[i];
}
void AdditionalInformationWindow::setNewFonts(QFont row[])
{
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++)
		row_font[i] = row[i];
}

void AdditionalInformationWindow::setHybrid(float bat, float usage)
{
	values[ValueTypes::BatteryStatus]->setText(QString(QString::number(bat, 'f', 0) + " %"));
	values[ValueTypes::BatteryUsage]->setText(QString(QString::number(usage, 'f', 0) + " %"));
}

void AdditionalInformationWindow::paintEvent(QPaintEvent *event)
{
	QPainter customPainter(this);
	QPalette pal;

	setAttribute(Qt::WA_TranslucentBackground); // Ich weiß nicht ob das hier weg darf

	pal.setColor(values[ValueTypes::WindDir]->foregroundRole(), row_color[ValueCategories::Wind]);
	values[ValueTypes::WindDir]->setPalette(pal);
	values[ValueTypes::WindSpeed]->setPalette(pal);
	values[ValueTypes::Humidity]->setPalette(pal);
	air->setPalette(pal);
	values[ValueTypes::WindDir]->setFont(row_font[ValueCategories::Wind]);
	values[ValueTypes::WindSpeed]->setFont(row_font[ValueCategories::Wind]);
	values[ValueTypes::Humidity]->setFont(row_font[ValueCategories::Wind]);
	air->setFont(qta->font(style::stfab, row_font[ValueCategories::Wind].pointSize()));

	pal.setColor(values[ValueTypes::AirTemp]->foregroundRole(), row_color[ValueCategories::Temp]);
	values[ValueTypes::AirTemp]->setPalette(pal);
	values[ValueTypes::TrackTemp]->setPalette(pal);
	thermometer->setPalette(pal);
	track->setPalette(pal);
	values[ValueTypes::AirTemp]->setFont(row_font[ValueCategories::Temp]);
	values[ValueTypes::TrackTemp]->setFont(row_font[ValueCategories::Temp]);
	thermometer->setFont(qta->font(style::stfab, row_font[ValueCategories::Temp].pointSize()));
	track->setFont(qta->font(style::stfab, row_font[ValueCategories::Temp].pointSize()));

	pal.setColor(values[ValueTypes::BatteryStatus]->foregroundRole(), row_color[ValueCategories::Battery]);
	values[ValueTypes::BatteryStatus]->setPalette(pal);
	values[ValueTypes::BatteryUsage]->setPalette(pal);
	battery->setPalette(pal);
	hys_usage->setPalette(pal);
	values[ValueTypes::BatteryStatus]->setFont(row_font[ValueCategories::Battery]);
	values[ValueTypes::BatteryUsage]->setFont(row_font[ValueCategories::Battery]);
	battery->setFont(qta->font(style::stfab, row_font[ValueCategories::Battery].pointSize()));
	hys_usage->setFont(qta->font(style::stfab, row_font[ValueCategories::Battery].pointSize()));

	pal.setColor(values[ValueTypes::OilTemp]->foregroundRole(), row_color[ValueCategories::Car]);
	values[ValueTypes::OilTemp]->setPalette(pal);
	values[ValueTypes::WaterTemp]->setPalette(pal);
	oil->setPalette(pal);
	water->setPalette(pal);
	values[ValueTypes::OilTemp]->setFont(row_font[ValueCategories::Car]);
	values[ValueTypes::WaterTemp]->setFont(row_font[ValueCategories::Car]);
	oil->setFont(qta->font(style::stfab, row_font[ValueCategories::Car].pointSize()));
	water->setFont(qta->font(style::stfab, row_font[ValueCategories::Car].pointSize()));

	customPainter.fillRect(rect(), bg_color);
}

AdditionalInformationWindow::~AdditionalInformationWindow() {}
