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

#ifndef FUELWINDOW_H
#define FUELWINDOW_H

#include <QMainWindow>

#include "../helper.h"

#define AMOUNT_TOP_HEADER 3
#define AMOUNT_LEFT_HEADER 3
#define AMOUNT_ADDITIONAL_HEADER 2
#define AMOUNT_DATA ((AMOUNT_TOP_HEADER*AMOUNT_LEFT_HEADER) + AMOUNT_ADDITIONAL_HEADER + 3) // 14

class QGridLayout;
class QLabel;
class QColor;

class FuelWindow : public QWidget
{
    Q_OBJECT
public:
	explicit FuelWindow(QWidget *parent = nullptr);

private:
	QGridLayout *base_layout, *fuel_layout;
	QGridLayout *verbrauch_layout[4], *laps_layout[2];
	QWidget *verbrauch_widgets[4], *fuel_widget, *laps_widgets[2];
	QLabel *header[AMOUNT_TOP_HEADER + AMOUNT_LEFT_HEADER + AMOUNT_ADDITIONAL_HEADER];
	QLabel *data[AMOUNT_DATA];
	QColor bg_color;
	QColor header_color;
	QColor delta_color[3];
	QColor row_color[AMOUNT_ROW];
	QFont header_font;
	QFont row_font[AMOUNT_ROW];
	bool b_target_2_visible;

	int precision[3];
	double delta_precision;
	float data_values[AMOUNT_DATA-1];

protected:
	virtual void paintEvent(QPaintEvent* event) override;

public:
	void setNewData(float data[]);
	void setNewColors(QColor, QColor, QColor[], QColor[]);
	void setNewFonts(QFont, QFont[]);
	void target_1_changed(double);
	void target_2_changed(double);
	void refresh_tank(float);
	void set_refuel_precision(int i) { precision[0] = i; }
	void set_max_laps_precision(int i) { precision[1] = i; }
	void set_fae_precision(int i) { precision[2] = i; }
	void set_delta_precision(double d) { delta_precision = d; }
	int get_refuel_precision() const { return precision[0]; }
	int get_max_laps_precision() const { return precision[1]; }
	int get_fae_precision() const { return precision[2]; }
	double get_delta_precision() const { return delta_precision; }
	//void show_error(FuelError e);
	QColor get_top_header_color() const { return header_color; }
	QColor get_background_color() const { return bg_color; }
	QColor get_row_color(int i) { return (i >= 0 && i < AMOUNT_ROW) ? row_color[i] : QColor(); }
	QColor get_delta_color(int i) { return (i >= 0 && i < 3) ? delta_color[i] : QColor(); }
	QFont get_header_font() { return header_font; }
	QFont get_row_font(int i) { return (i >= 0 && i < AMOUNT_ROW) ? row_font[i] : QFont(); }
};

#endif // FUELWINDOW_H
