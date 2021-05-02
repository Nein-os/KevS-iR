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

#ifndef OPTIONWINDOW_H
#define OPTIONWINDOW_H

#include "../helper.h"

#include <QMainWindow>

class QWidget;
class QLabel;
class QDoubleSpinBox;
class QGridLayout;
class QPushButton;
class FuelWindow;
class FuelEditWindow;
class AdditionalInformationWindow;
class AdditionalInformationEditWindow;
class QCloseEvent;
class QGroupBox;
class QCheckBox;
#ifdef QT_DEBUG
	class DebugWindow;
#endif

class OptionWindow : public QMainWindow
{
    Q_OBJECT

private:
	QWidget *main_widget;
	QGridLayout *base_layout, *fuel_layout, *version_layout, *organisation_layout, *additional_layout;
	QGroupBox *gb_fuel, *gb_version, *gb_organisation, *gb_additional;

	// Fuel
	QPushButton *toggle_fuel, *edit_fuel, *adjust_fuel;
	QLabel *lbl_target;
	QDoubleSpinBox *dsb_target_1, *dsb_target_2;
	FuelEditWindow *edit_fuel_widget;
	FuelWindow *fw;

	// Additional Informations
	QWidget *aiw_widget;
	QGridLayout *aiw_layout;
	QCheckBox *cb_aiw[AIW_AMOUNT_ROW];
	QPushButton *toggle_aiw, *adjust_aiw, *edit_aiw;
	AdditionalInformationWindow *aiw;
	AdditionalInformationEditWindow *edit_aiw_widget;

	// Version
	QLabel *lbl_kevs_version, *lbl_fuelcalc_version, *lbl_relative_version, *lbl_aiw_version;

	// Additional
	QPushButton *btn_reset, *btn_load, *btn_save;

	bool bShowFuel, bShowAIW;

	void init_settings();
	void save_settings();
	void save_settings(QString);
	virtual void closeEvent(QCloseEvent *event) override;

#ifdef QT_DEBUG
	DebugWindow *dw;
#endif

private slots:
	void exec_edit_fuel();
	void exec_start_fuel();
	void exec_adjust_fuel();
	void exec_edit_aiw();
	void exec_start_aiw();
	void exec_adjust_aiw();
	void exec_save();
	void exec_load();
	void exec_reset();
	void exec_toggle_aiw_battery();
	void exec_toggle_aiw_weather();
	void exec_toggle_aiw_temps();
	void exec_toggle_aiw_car();

public:
    OptionWindow(QWidget *parent = nullptr);

    void refresh_fuel();
    OptionWindow *get_ref() { return this; }
    void set_fuel_window(FuelWindow *w);
    void set_aiw_window(AdditionalInformationWindow *w);
    void set_versions(std::string, std::string, std::string, std::string);
    void set_fuel_window_settings(FWSettings fws);
    void set_aiw_window_settings(AIWSettings aiws);
    bool *get_fuel_state() { return &bShowFuel; }
    bool *get_aiw_state() { return &bShowAIW; }
    FuelWindow *get_fuel_ref() { return fw; }
    AdditionalInformationWindow *get_aiw_ref() { return aiw; }
#ifdef QT_DEBUG
    void set_debug_window(DebugWindow*);
#endif

    ~OptionWindow();
};
#endif // OPTIONWINDOW_H
