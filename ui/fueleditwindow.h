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

#ifndef FUELEDITWINDOW_H
#define FUELEDITWINDOW_H

#include <QWidget>

#define AMOUNT_DIF_COLORS 7 // Muss selber immer angepasst werden :/

class QPushButton;
class QComboBox;
class QGridLayout;
class QGroupBox;
class FuelWindow;
class QSpinBox;
class QDoubleSpinBox;
class QColor;
class QLabel;

class FuelEditWindow : public QWidget
{
	Q_OBJECT
private:
	QGridLayout *base_layout;
	QPushButton *btn_save, *btn_cancel;
	QLabel *lbl_color[AMOUNT_DIF_COLORS];
	QPushButton *btn_color[AMOUNT_DIF_COLORS];
	QPushButton *btn_font[AMOUNT_DIF_COLORS-1];
	QColor color_values[AMOUNT_DIF_COLORS];
	QGroupBox *gb_delta, *gb_precision;
	QGridLayout *delta_layout, *precision_layout;
	QPushButton *btn_delta[3];
	QColor delta_color[3];
	QFont font_values[AMOUNT_DIF_COLORS-2];
	QFont font_header;
	FuelWindow *fw;
	QSpinBox *alpha, *float_numbers[3];
	QDoubleSpinBox *delta_precision;

	int number_precision[3];
	double delta_number_precision;

private slots:
	// Colors
	void edit_color_background();
	void edit_color_header();
	void edit_color_row_average();
	void edit_color_row_target();
	void edit_color_row_verbrauch();
	void edit_color_row_laps();
	void edit_color_row_tank();
	// Delta
	void edit_color_delta_minus();
	void edit_color_delta_neutral();
	void edit_color_delta_plus();

	// Fonts
	void edit_font_header();
	void edit_font_row_average();
	void edit_font_row_target();
	void edit_font_row_verbrauch();
	void edit_font_row_laps();
	void edit_font_row_tank();

	// Rest
	void save_settings();
	void cancel();

public:
	FuelEditWindow(QWidget* parent = nullptr);
	void sync_numeric_values(int, int, int, double);
	void sync_color_values(QColor colors[], FuelWindow*);
	void sync_font_values(QFont[]);
	~FuelEditWindow();
};

#endif // FUELEDITWINDOW_H
