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

#include "additionalinformationeditwindow.h"
#include "helper.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QFontDialog>
#include <QSpinBox>
#include <QGroupBox>

AdditionalInformationEditWindow::AdditionalInformationEditWindow(QWidget *parent) : QWidget(parent)
{
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

	base_layout = new QGridLayout();

	btn_save = new QPushButton("Save", parent);
	base_layout->addWidget(btn_save, ValueCategories::AMNT_VALUE_CATEGORIES+2, 0);

	btn_cancel = new QPushButton("Cancel", parent);
	base_layout->addWidget(btn_cancel, ValueCategories::AMNT_VALUE_CATEGORIES+2, 2);

	alpha = new QSpinBox(parent);
	btn_bg_color = new QPushButton("Color", parent);
	base_layout->addWidget(new QLabel("Background:", parent), 0, 0);
	alpha->setMaximum(255);
	alpha->setMinimum(0);
	base_layout->addWidget(btn_bg_color, 0, 1);
	base_layout->addWidget(alpha, 0, 2);

	gb_precision = new QGroupBox("Numbers precision", parent);
	precision_layout = new QGridLayout(parent);
	gb_precision->setLayout(precision_layout);
	base_layout->addWidget(gb_precision, ValueCategories::AMNT_VALUE_CATEGORIES+1, 0, 1, 3);


	lbl[ValueCategories::Battery] = new QLabel("Hybrid:", parent);
	lbl[ValueCategories::Car] = new QLabel("Car Temps:", parent);
	lbl[ValueCategories::Temp] = new QLabel("Temps:", parent);
	lbl[ValueCategories::Wind] = new QLabel("Weather:", parent);
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
		btn_colors[i] = new QPushButton("Color", parent);
		btn_fonts[i] = new QPushButton("Font", parent);

		base_layout->addWidget(lbl[i], i+1, 0);
		base_layout->addWidget(btn_colors[i], i+1, 1);
		base_layout->addWidget(btn_fonts[i], i+1, 2);
	}
	float_numbers[0] = new QSpinBox(parent);
	float_numbers[0]->setMaximum(4);
	float_numbers[0]->setMinimum(0);
	precision_layout->addWidget(new QLabel("Weather:", parent), 0, 0);
	precision_layout->addWidget(float_numbers[0], 0, 1);
	float_numbers[1] = new QSpinBox(parent);
	float_numbers[1]->setMaximum(4);
	float_numbers[1]->setMinimum(0);
	precision_layout->addWidget(new QLabel("Temps:", parent), 1, 0);
	precision_layout->addWidget(float_numbers[1],1, 1);


	connect(btn_save, &QPushButton::released, this, &AdditionalInformationEditWindow::save_settings);
	connect(btn_cancel, &QPushButton::released, this, &AdditionalInformationEditWindow::cancel);
	connect(btn_bg_color, &QPushButton::released, this, &AdditionalInformationEditWindow::edit_color_background);
	connect(btn_colors[ValueCategories::Battery], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_color_hybrid);
	connect(btn_colors[ValueCategories::Car], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_color_car);
	connect(btn_colors[ValueCategories::Temp], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_color_temp);
	connect(btn_colors[ValueCategories::Wind], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_color_weather);
	connect(alpha, QOverload<int>::of(&QSpinBox::valueChanged),
		[=](int i){ bg_color.setAlpha(i); });
	connect(btn_fonts[ValueCategories::Battery], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_font_hybrid);
	connect(btn_fonts[ValueCategories::Car], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_font_car);
	connect(btn_fonts[ValueCategories::Temp], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_font_temp);
	connect(btn_fonts[ValueCategories::Wind], &QPushButton::released, this, &AdditionalInformationEditWindow::edit_font_weather);
	connect(float_numbers[0], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int i) { wind_precision = i; });
	connect(float_numbers[1], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int i) { temp_precision = i; });

	setWindowTitle("Info-Edit Window - KevS");
	setLayout(base_layout);
}

void AdditionalInformationEditWindow::edit_color_background()
{
	int buffer_alpha = bg_color.alpha();
	bg_color = QColorDialog::getColor(bg_color, this);
	bg_color.setAlpha(buffer_alpha);
}
void AdditionalInformationEditWindow::edit_color_weather() { row_color[ValueCategories::Wind] =  QColorDialog::getColor(row_color[ValueCategories::Wind], this); }
void AdditionalInformationEditWindow::edit_color_temp() { row_color[ValueCategories::Temp] =  QColorDialog::getColor(row_color[ValueCategories::Temp], this); }
void AdditionalInformationEditWindow::edit_color_hybrid() { row_color[ValueCategories::Battery] =  QColorDialog::getColor(row_color[ValueCategories::Battery], this); }
void AdditionalInformationEditWindow::edit_color_car() { row_color[ValueCategories::Car] =  QColorDialog::getColor(row_color[ValueCategories::Car], this); }
void AdditionalInformationEditWindow::edit_font_weather()
{
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, row_font[ValueCategories::Wind], this);
	if (ok)
		row_font[ValueCategories::Wind] = buffer;
}
void AdditionalInformationEditWindow::edit_font_temp()
{
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, row_font[ValueCategories::Temp], this);
	if (ok)
		row_font[ValueCategories::Temp] = buffer;
}
void AdditionalInformationEditWindow::edit_font_hybrid()
{
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, row_font[ValueCategories::Battery], this);
	if (ok)
		row_font[ValueCategories::Battery] = buffer;
}
void AdditionalInformationEditWindow::edit_font_car()
{
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, row_font[ValueCategories::Car], this);
	if (ok)
		row_font[ValueCategories::Car] = buffer;
}
void AdditionalInformationEditWindow::save_settings()
{
	aiw->set_temp_precision(temp_precision);
	aiw->set_wind_precision(wind_precision);
	aiw->setNewColors(bg_color, row_color);
	aiw->setNewFonts(row_font);
	update_aiw();
	aiw->repaint();
}
void AdditionalInformationEditWindow::cancel()
{
	// Reset
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
		row_color[i] = aiw->get_row_color(i);
		row_font[i] = aiw->get_row_font(i);
	}
	bg_color = aiw->get_bg_color();
	wind_precision = aiw->get_wind_precision();
	temp_precision = aiw->get_temp_precision();

	hide();
}

void AdditionalInformationEditWindow::sync_numeric_values(int wind, int temps)
{
	float_numbers[0]->setValue(wind);
	float_numbers[1]->setValue(temps);
	wind_precision = wind;
	temp_precision = temps;
}
void AdditionalInformationEditWindow::sync_color_values(QColor bg_color, QColor new_colors[], AdditionalInformationWindow *aiw)
{
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++)
		row_color[i] = new_colors[i];
	this->bg_color = bg_color;
	this->aiw = aiw;
	alpha->setValue(bg_color.alpha());
}
void AdditionalInformationEditWindow::sync_font_values(QFont *new_fonts)
{
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++)
		row_font[i] = new_fonts[i];
}

AdditionalInformationEditWindow::~AdditionalInformationEditWindow() {}
