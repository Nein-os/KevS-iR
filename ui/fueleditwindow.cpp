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

#include "fueleditwindow.h"
#include "fuelwindow.h"
#include "optionwindow.h"
#include "helper.h"

#include <QPushButton>
#include <QComboBox>
#include <QColor>
#include <QLabel>
#include <QGridLayout>
#include <QColorDialog>
#include <QFontDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>

FuelEditWindow::FuelEditWindow(QWidget *parent) : QWidget(parent)
{
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

	base_layout = new QGridLayout();

	btn_save = new QPushButton("Save", parent);
	base_layout->addWidget(btn_save, AMOUNT_DIF_COLORS+3, 0);

	btn_cancel = new QPushButton("Cancel", parent);
	base_layout->addWidget(btn_cancel, AMOUNT_DIF_COLORS+3, 2);

	lbl_color[0] = new QLabel("Background:", parent);
	btn_color[0] = new QPushButton("Color", parent);
	base_layout->addWidget(lbl_color[0], 0, 0);
	alpha = new QSpinBox();
	alpha->setMaximum(255);
	alpha->setMinimum(0);
	base_layout->addWidget(btn_color[0], 0, 1);
	base_layout->addWidget(alpha, 0, 2);

	for (int i = 1; i < AMOUNT_DIF_COLORS; i++) {
		lbl_color[i] = new QLabel(parent);
		btn_color[i] = new QPushButton("Color", parent);
		if (i) {
			btn_font[i-1] = new QPushButton("Font", parent);
			base_layout->addWidget(btn_font[i-1], i, 2);
		}
		base_layout->addWidget(lbl_color[i], i, 0);
		base_layout->addWidget(btn_color[i], i, 1);
	}
	lbl_color[1]->setText("Top Header:");
	lbl_color[2]->setText("AVG Row:");
	lbl_color[3]->setText("Target 1 Row:");
	lbl_color[4]->setText("Target 2 Row:");
	lbl_color[5]->setText("Lap Row:");
	lbl_color[6]->setText("Tank Row:");

	// Number precision
	gb_precision = new QGroupBox("Numbers precision", parent);
	precision_layout = new QGridLayout(parent);
	gb_precision->setLayout(precision_layout);
	base_layout->addWidget(gb_precision, AMOUNT_DIF_COLORS+2, 0, 1, 3);
	for (int i = 0; i < 3; i++) {
		float_numbers[i] = new QSpinBox(parent);
		precision_layout->addWidget(float_numbers[i], i, 1);
		float_numbers[i]->setMinimum(0);
		float_numbers[i]->setMaximum(4);
	}
	delta_precision = new QDoubleSpinBox(parent);
	precision_layout->addWidget(delta_precision, 3, 1);
	delta_precision->setMinimum(0);
	delta_precision->setMaximum(1);
	delta_precision->setDecimals(4);
	precision_layout->addWidget(new QLabel("Refuel:", parent), 0, 0);
	precision_layout->addWidget(new QLabel("Laps of race:", parent), 1, 0);
	precision_layout->addWidget(new QLabel("Fuel at End:", parent), 2, 0);
	precision_layout->addWidget(new QLabel("Neutral delta:", parent), 3, 0);

	// Delta
	gb_delta = new QGroupBox("Delta", parent);
	delta_layout = new QGridLayout(parent);
	gb_delta->setLayout(delta_layout);

	btn_delta[0] = new QPushButton("Minus Color");
	delta_layout->addWidget(btn_delta[0], 0, 0);
	btn_delta[1] = new QPushButton("Neutral Color");
	delta_layout->addWidget(btn_delta[1], 0, 1);
	btn_delta[2] = new QPushButton("Positive Color");
	delta_layout->addWidget(btn_delta[2], 0, 2);
	base_layout->addWidget(gb_delta, AMOUNT_DIF_COLORS+1, 0, 1, 3);

	// Colors
	connect(btn_save, &QPushButton::released, this, &FuelEditWindow::save_settings);
	connect(btn_cancel, &QPushButton::released, this, &FuelEditWindow::cancel);
	connect(btn_color[0], &QPushButton::released, this, &FuelEditWindow::edit_color_background);
	connect(btn_color[1], &QPushButton::released, this, &FuelEditWindow::edit_color_header);
	connect(btn_color[2], &QPushButton::released, this, &FuelEditWindow::edit_color_row_average);
	connect(btn_color[3], &QPushButton::released, this, &FuelEditWindow::edit_color_row_target);
	connect(btn_color[4], &QPushButton::released, this, &FuelEditWindow::edit_color_row_verbrauch);
	connect(btn_color[5], &QPushButton::released, this, &FuelEditWindow::edit_color_row_laps);
	connect(btn_color[6], &QPushButton::released, this, &FuelEditWindow::edit_color_row_tank);
	connect(alpha, QOverload<int>::of(&QSpinBox::valueChanged),
	    [=](int i){ color_values[0].setAlpha(i); });
	// Delta
	connect(btn_delta[0], &QPushButton::released, this, &FuelEditWindow::edit_color_delta_minus);
	connect(btn_delta[1], &QPushButton::released, this, &FuelEditWindow::edit_color_delta_neutral);
	connect(btn_delta[2], &QPushButton::released, this, &FuelEditWindow::edit_color_delta_plus);

	// Fonts
	connect(btn_font[0], &QPushButton::released, this, &FuelEditWindow::edit_font_header);
	connect(btn_font[1], &QPushButton::released, this, &FuelEditWindow::edit_font_row_average);
	connect(btn_font[2], &QPushButton::released, this, &FuelEditWindow::edit_font_row_target);
	connect(btn_font[3], &QPushButton::released, this, &FuelEditWindow::edit_font_row_verbrauch);
	connect(btn_font[4], &QPushButton::released, this, &FuelEditWindow::edit_font_row_laps);
	connect(btn_font[5], &QPushButton::released, this, &FuelEditWindow::edit_font_row_tank);

	// Numeric Values
	connect(float_numbers[0], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int i){ number_precision[0] = i; });
	connect(float_numbers[1], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int i){ number_precision[1] = i; });
	connect(float_numbers[2], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int i){ number_precision[2] = i; });
	connect(delta_precision, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[=](double i){ delta_number_precision = i; });

	setWindowTitle("Fuel-Edit Window - KevS");
	setLayout(base_layout);
}

// Colors
void FuelEditWindow::edit_color_background() {
	int buffer_alpha = color_values[0].alpha();
	color_values[0] = QColorDialog::getColor(color_values[0], this);
	color_values[0].setAlpha(buffer_alpha);
}
void FuelEditWindow::edit_color_header() { color_values[1] = QColorDialog::getColor(color_values[1], this); }
void FuelEditWindow::edit_color_row_average() { color_values[2] = QColorDialog::getColor(color_values[2], this); }
void FuelEditWindow::edit_color_row_target() { color_values[3] = QColorDialog::getColor(color_values[3], this); }
void FuelEditWindow::edit_color_row_verbrauch() { color_values[4] = QColorDialog::getColor(color_values[4], this); }
void FuelEditWindow::edit_color_row_laps() { color_values[5] = QColorDialog::getColor(color_values[5], this); }
void FuelEditWindow::edit_color_row_tank() { color_values[6] = QColorDialog::getColor(color_values[6], this); }
// Delta
void FuelEditWindow::edit_color_delta_minus() { delta_color[0] = QColorDialog::getColor(delta_color[0], this); }
void FuelEditWindow::edit_color_delta_neutral() { delta_color[1] = QColorDialog::getColor(delta_color[1], this); }
void FuelEditWindow::edit_color_delta_plus() { delta_color[2] = QColorDialog::getColor(delta_color[2], this); }

// Fonts
void FuelEditWindow::edit_font_header() {
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, font_header, this);
	if (ok)
		font_header = buffer;
}
void FuelEditWindow::edit_font_row_average() {
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, font_values[0], this);
	if (ok)
		font_values[0] = buffer;
}
void FuelEditWindow::edit_font_row_target() {
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, font_values[1], this);
	if (ok)
		font_values[1] = buffer;
}
void FuelEditWindow::edit_font_row_verbrauch() {
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, font_values[2], this);
	if (ok)
		font_values[2] = buffer;
}
void FuelEditWindow::edit_font_row_laps() {
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, font_values[3], this);
	if (ok)
		font_values[3] = buffer;
}
void FuelEditWindow::edit_font_row_tank(){
	bool ok;
	QFont buffer = QFontDialog::getFont(&ok, font_values[4], this);
	if (ok)
		font_values[4] = buffer;
}

// Rest
void FuelEditWindow::cancel() {
	// Reset
	for (int i = 0; i < AMOUNT_ROW; i++)
		font_values[i] = fw->get_row_font(i);
	font_header = fw->get_header_font();

	for (int i = 0; i < AMOUNT_DIF_COLORS; i++)
		color_values[i] = fw->get_row_color(i);
	for (int i = 0; i < 3; i++)
		delta_color[i] = fw->get_delta_color(i);

	number_precision[0] = fw->get_refuel_precision();
	number_precision[1] = fw->get_max_laps_precision();
	number_precision[2] = fw->get_fae_precision();
	delta_number_precision = fw->get_delta_precision();

	hide();
}
void FuelEditWindow::save_settings()
{
	QColor buffer[AMOUNT_ROW];
	QColor deltas[3];
	for (int i = 0; i < AMOUNT_ROW; i++)
		buffer[i] = color_values[i+2];
	for (int i = 0; i < 3; i++)
		deltas[i]  = delta_color[i];
	fw->setNewColors(color_values[0], color_values[1], buffer, deltas);
	fw->setNewFonts(font_header, font_values);
	fw->set_refuel_precision(number_precision[0]);
	fw->set_max_laps_precision(number_precision[1]);
	fw->set_fae_precision(number_precision[2]);
	fw->set_delta_precision(delta_number_precision);
	update_decimals_fw();
	fw->repaint();
}

void FuelEditWindow::sync_numeric_values(int refuel, int laps, int fae, double delta)
{
	float_numbers[0]->setValue(refuel);
	float_numbers[1]->setValue(laps);
	float_numbers[2]->setValue(fae);
	delta_precision->setValue(delta);
}
void FuelEditWindow::sync_color_values(QColor colors[], FuelWindow *fw)
{
	for (int i = 0; i < AMOUNT_DIF_COLORS; i++)
		color_values[i] = colors[i];
	this->fw = fw;
	alpha->setValue(color_values[0].alpha());
}
void FuelEditWindow::sync_font_values(QFont fonts[])
{
	font_header = fonts[0];
	for (int i = 1; i < AMOUNT_ROW+1; i++)
		font_values[i-1] = fonts[i];
}

FuelEditWindow::~FuelEditWindow()
{
	fw = nullptr;
}
