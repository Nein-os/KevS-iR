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

#ifndef ADDITIONALINFORMATIONEDITWINDOW_H
#define ADDITIONALINFORMATIONEDITWINDOW_H

#include <QWidget>

#include "additionalinformationwindow.h"

class QPushButton;
class QGridLayout;
class QLabel;
class QColor;
class QSpinBox;
class AdditionalInformationWindow;

class AdditionalInformationEditWindow : public QWidget
{
	Q_OBJECT
public:
	AdditionalInformationEditWindow(QWidget *parent = nullptr);
	virtual ~AdditionalInformationEditWindow();

private:
	QGridLayout *base_layout;
	QLabel *lbl[ValueCategories::AMNT_VALUE_CATEGORIES];
	QPushButton *btn_fonts[ValueCategories::AMNT_VALUE_CATEGORIES];
	QPushButton *btn_colors[ValueCategories::AMNT_VALUE_CATEGORIES], *btn_bg_color;
	QPushButton *btn_save, *btn_cancel;
	AdditionalInformationWindow *aiw;

	QSpinBox *alpha;
	QFont row_font[ValueCategories::AMNT_VALUE_CATEGORIES];
	QColor row_color[ValueCategories::AMNT_VALUE_CATEGORIES];
	QColor bg_color;

private slots:
	// Colors
	void edit_color_weather();
	void edit_color_temp();
	void edit_color_hybrid();
	void edit_color_car();
	void edit_color_background();

	// Fonts
	void edit_font_weather();
	void edit_font_temp();
	void edit_font_hybrid();
	void edit_font_car();

	// Rest
	void save_settings();
	void cancel();

public:
	void sync_color_values(QColor bg_color, QColor new_colors[], AdditionalInformationWindow*);
	void sync_font_values(QFont new_fonts[]);
};

#endif // ADDITIONALINFORMATIONEDITWINDOW_H
