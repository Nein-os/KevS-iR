#ifndef ADDITIONALINFORMATIONWINDOW_H
#define ADDITIONALINFORMATIONWINDOW_H

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

#include <QWidget>

#define ADDITIONAL_VALUES 2

class QLabel;
class QGridLayout;
class QtAwesome;
class QApplication;

typedef enum _SingeValueTypes : int {
	WindSpeed,
	WindDir,
	Humidity,
	AirTemp,
	OilTemp,
	WaterTemp,
	TrackTemp,
	BatteryUsage,
	BatteryStatus,
	AMNT_VALUE_TYPES
} ValueTypes;

typedef enum _LineValueCategories : int {
	Wind,
	Temp,
	Car,
	Battery,
	AMNT_VALUE_CATEGORIES
} ValueCategories;

class AdditionalInformationWindow : public QWidget
{
	Q_OBJECT

private:
	QGridLayout *base_layout;
	QLabel *values[ValueTypes::AMNT_VALUE_TYPES];
	QWidget *elements[ValueCategories::AMNT_VALUE_CATEGORIES];
	QGridLayout *additional_layouts[ValueCategories::AMNT_VALUE_CATEGORIES];
	QtAwesome *qta = nullptr;
	QLabel *car, *oil, *air, *track, *water, *battery, *hys_usage, *thermometer;

	bool row_visible[ValueCategories::AMNT_VALUE_CATEGORIES];

	QColor bg_color, row_color[ValueCategories::AMNT_VALUE_CATEGORIES];
	QFont row_font[ValueCategories::AMNT_VALUE_CATEGORIES];

	float precision[2];

protected:
	virtual void paintEvent(QPaintEvent* event) override;

public:
	AdditionalInformationWindow(QWidget *parent = nullptr);
	virtual ~AdditionalInformationWindow();
	void setHybrid(float bat, float usage);
	void setNewData(float data[]);
	void setNewVisibilityForData(bool bShow, ValueCategories element);
	void setNewColors(QColor, QColor[]);
	void setNewFonts(QFont[]);
	QColor get_row_color(int i) { return (i < ValueCategories::AMNT_VALUE_CATEGORIES && i >= 0) ? row_color[i] : QColor(); }
	QColor get_bg_color() { return bg_color; }
	QFont get_row_font(int i) { return (i < ValueCategories::AMNT_VALUE_CATEGORIES && i >= 0) ? row_font[i] : QFont(); }
	bool is_row_visible(int i) { return (i < ValueCategories::AMNT_VALUE_CATEGORIES && i >= 0) ? row_visible[i] : false; }
	void set_wind_precision(int i) { precision[0] = i; }
	void set_temp_precision(int i) { precision[1] = i; }
	int get_wind_precision() { return precision[0]; }
	int get_temp_precision() { return precision[1]; }
};

#endif // ADDITIONALINFORMATIONWINDOW_H
