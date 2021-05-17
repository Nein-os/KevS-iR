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

#include "optionwindow.h"
#include "../helper.h"
#include "fuelwindow.h"
#include "fueleditwindow.h"
#include "additionalinformationwindow.h"
#include "additionalinformationeditwindow.h"
#include "debugwindow.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QMessageBox>
#include <QObject>
#include <QDoubleSpinBox>
#include <QCloseEvent>
#include <QGroupBox>
#include <QFileDialog>
#include <QCheckBox>

#include <iostream>
#include <fstream>

OptionWindow::OptionWindow(QWidget *parent)
    : QMainWindow(parent)
{
	// Vorbereitung
	fw = new FuelWindow();
	edit_fuel_widget = new FuelEditWindow();
	edit_fuel_widget->hide();

	aiw = new AdditionalInformationWindow();
	edit_aiw_widget = new AdditionalInformationEditWindow();
	edit_aiw_widget->hide();

	main_widget = new QWidget(parent);
	setCentralWidget(main_widget);

	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
	setGeometry(250, 250, 400, 150);
	////////////////////////////////////////

	// Layouts und Basisvariablen
	base_layout = new QGridLayout(parent);
	fuel_layout = new QGridLayout(parent);
	version_layout = new QGridLayout(parent);
	additional_layout = new QGridLayout(parent);
	organisation_layout = new QGridLayout(parent);
	bShowFuel = false;
	////////////////////////////////////////

	// Fuel Calculator
	gb_fuel = new QGroupBox("Fuel Calculator", parent);
	gb_fuel->setLayout(fuel_layout);
	base_layout->addWidget(gb_fuel, 0, 0);

	edit_fuel = new QPushButton("Edit", parent);
	fuel_layout->addWidget(edit_fuel, 0, 2);

	adjust_fuel = new QPushButton("Move Mode Fuel", parent);
	fuel_layout->addWidget(adjust_fuel, 0, 1);

	toggle_fuel = new QPushButton("Start Fuel", parent);
	fuel_layout->addWidget(toggle_fuel, 0, 0);

	lbl_target = new QLabel("Target 1:", parent);
	fuel_layout->addWidget(lbl_target, 1, 0);
	fuel_layout->addWidget(new QLabel("(0 to deactivate)", parent), 1, 2);

	dsb_target_1 = new QDoubleSpinBox(parent);
	dsb_target_1->setMaximum(22);
	dsb_target_1->setMinimum(0);
	dsb_target_1->setDecimals(4);
	fuel_layout->addWidget(dsb_target_1, 1, 1);

	fuel_layout->addWidget(new QLabel("Target 2:", parent), 2, 0);
	dsb_target_2 = new QDoubleSpinBox(parent);
	dsb_target_2->setMaximum(22);
	dsb_target_2->setMinimum(0);
	dsb_target_2->setDecimals(4);
	fuel_layout->addWidget(dsb_target_2, 2, 1);
	fuel_layout->addWidget(new QLabel("(Activ only T1 not 0)", parent), 2, 2);

	/*cb_refuel = new QComboBox(parent);
	cb_refuel->addItems(refuel_options);
	fuel_layout->addWidget(new QLabel("Nachtanken:"), 3, 0);
	fuel_layout->addWidget(cb_refuel, 3, 1);
	fuel_layout->addWidget(new QLabel("(Nach 7 Runden)"));*/
	////////////////////////////////////////

	// Additional Information Window
	gb_additional = new QGroupBox("InforWindow", parent);
	gb_additional->setLayout(additional_layout);
	base_layout->addWidget(gb_additional, 1, 0);

	toggle_aiw = new QPushButton("Start InfoWindow", parent);
	additional_layout->addWidget(toggle_aiw, 0, 0);

	edit_aiw = new QPushButton("Edit", parent);
	additional_layout->addWidget(edit_aiw, 0, 2);

	adjust_aiw = new QPushButton("Move Mode InfoWindow", parent);
	additional_layout->addWidget(adjust_aiw, 0, 1);

	aiw_widget = new QWidget(parent);
	aiw_layout = new QGridLayout(parent);
	aiw_widget->setLayout(aiw_layout);
	additional_layout->addWidget(aiw_widget, 1, 0, 1, 3);
	cb_aiw[ValueCategories::Wind] = new QCheckBox("Weather", parent);
	cb_aiw[ValueCategories::Temp] = new QCheckBox("Temperatures", parent);
	cb_aiw[ValueCategories::Car] = new QCheckBox("Fluids", parent);
	cb_aiw[ValueCategories::Battery] = new QCheckBox("Hybrid", parent);
	aiw_layout->addWidget(cb_aiw[ValueCategories::Wind], 0, 0);
	aiw_layout->addWidget(cb_aiw[ValueCategories::Temp], 0, 1);
	aiw_layout->addWidget(cb_aiw[ValueCategories::Car], 0, 2);
	aiw_layout->addWidget(cb_aiw[ValueCategories::Battery], 0, 3);
	////////////////////////////////////////

	// Version und Stats
	gb_version = new QGroupBox("Version", parent);
	gb_version->setLayout(version_layout);
	base_layout->addWidget(gb_version, 3, 0);

	version_layout->addWidget(new QLabel("KevS:"), 0, 0);
	version_layout->addWidget(new QLabel("FuelCalc:"), 1, 0);
	version_layout->addWidget(new QLabel("Information:"), 2, 0);
	version_layout->addWidget(new QLabel("Relative:"), 3, 0);

	lbl_kevs_version = new QLabel(parent);
	version_layout->addWidget(lbl_kevs_version, 0, 1);

	lbl_fuelcalc_version = new QLabel(parent);
	version_layout->addWidget(lbl_fuelcalc_version, 1, 1);

	lbl_aiw_version = new QLabel(parent);
	version_layout->addWidget(lbl_aiw_version, 2, 1);

	lbl_relative_version = new QLabel(parent);
	version_layout->addWidget(lbl_relative_version, 3, 1);
	////////////////////////////////////////

	// Organisation
	gb_organisation = new QGroupBox("Organisation", parent);
	gb_organisation->setLayout(organisation_layout);
	base_layout->addWidget(gb_organisation, 4, 0);

	btn_reset = new QPushButton("Reset", parent);
	btn_save = new QPushButton("Save Settings", parent);
	btn_load = new QPushButton("Load Settings", parent);

	organisation_layout->addWidget(btn_reset, 0, 0);
	organisation_layout->addWidget(btn_save, 0, 1);
	organisation_layout->addWidget(btn_load, 0, 2);
	////////////////////////////////////////

	// 'Events': Verbindung von Button-Clicks auf der graphischen Oberfläche mit Funktionen
	connect(toggle_fuel, &QPushButton::released, this, &OptionWindow::exec_start_fuel);
	connect(edit_fuel, &QPushButton::released, this, &OptionWindow::exec_edit_fuel);
	connect(adjust_fuel, &QPushButton::released, this, &OptionWindow::exec_adjust_fuel);
	connect(dsb_target_1, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
	    [=](double i){
		update_fuel_target_2(dsb_target_2->value(), fw);
		update_fuel_target_1(i, fw);
	});
	connect(dsb_target_2, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[=](double i) {
			update_fuel_target_2(i, fw);
	});
	connect(toggle_aiw, &QPushButton::released, this, &OptionWindow::exec_start_aiw);
	connect(edit_aiw, &QPushButton::released, this, &OptionWindow::exec_edit_aiw);
	connect(adjust_aiw, &QPushButton::released, this, &OptionWindow::exec_adjust_aiw);
	connect(btn_save, &QPushButton::released, this, &OptionWindow::exec_save);
	connect(btn_load, &QPushButton::released, this, &OptionWindow::exec_load);
	connect(btn_reset, &QPushButton::released, this, &OptionWindow::exec_reset);

	connect(cb_aiw[ValueCategories::Battery], &QCheckBox::clicked, this, &OptionWindow::exec_toggle_aiw_battery);
	connect(cb_aiw[ValueCategories::Wind], &QCheckBox::clicked, this, &OptionWindow::exec_toggle_aiw_weather);
	connect(cb_aiw[ValueCategories::Temp], &QCheckBox::clicked, this, &OptionWindow::exec_toggle_aiw_temps);
	connect(cb_aiw[ValueCategories::Car], &QCheckBox::clicked, this, &OptionWindow::exec_toggle_aiw_car);

	setWindowTitle("Option Window - KevS");
	main_widget->setLayout(base_layout);
}

void OptionWindow::set_fuel_window(FuelWindow *w)
{
	FWSettings fws;
	fws.bActivateFuel = false;
	fws.target_1 = dsb_target_1->value();
	fws.target_2 = dsb_target_2->value();
	set_fuel_window_settings(fws);
	fw = w;
}

void OptionWindow::set_fuel_window_settings(FWSettings fws)
{
	QColor buffer[AMOUNT_ROW + 2];
	QFont fbuffer[AMOUNT_ROW + 1];
	bShowFuel = fws.bActivateFuel;

	update_fuel_target_2(fws.target_2, fw);
	dsb_target_2->setValue(fws.target_2);
	update_fuel_target_1(fws.target_1, fw);
	dsb_target_1->setValue(fws.target_1);

	fw->set_max_laps_precision(fws.precision_max_laps);
	fw->set_fae_precision(fws.precision_fae);
	fw->set_refuel_precision(fws.precision_refuel);
	fw->set_delta_precision(fws.delta_precision);

	fw->setGeometry(fws.pos_x, fws.pos_y, fws.width, fws.height);
	fw->setNewColors(fws.background, fws.top_header, fws.row, fws.delta);
	fw->setNewFonts(fws.top_header_font, fws.row_font);

	buffer[0] = fws.background;
	buffer[1] = fws.top_header;
	fbuffer[0] = fws.top_header_font;
	for (int i = 0; i < AMOUNT_ROW; i++) {
		buffer[i+2] = fws.row[i];
		fbuffer[i+1] = fws.row_font[i];
	}
	edit_fuel_widget->sync_numeric_values(fws.precision_refuel, fws.precision_max_laps, fws.precision_fae, fws.delta_precision);
	edit_fuel_widget->sync_color_values(buffer, fw);
	edit_fuel_widget->sync_font_values(fbuffer);
	init_settings();
}
void OptionWindow::set_aiw_window_settings(AIWSettings aiws)
{
	bShowAIW = aiws.bActivateInformation;

	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
		cb_aiw[i]->setChecked(aiws.row_visible[i]);
		aiw->setNewVisibilityForData(aiws.row_visible[i], (ValueCategories)i);
	}

	aiw->set_wind_precision(aiws.precision_wind);
	aiw->set_temp_precision(aiws.precision_temps);

	aiw->setGeometry(aiws.pos_x, aiws.pos_y, aiws.width, aiws.height);
	aiw->setNewColors(aiws.background, aiws.row_color);
	aiw->setNewFonts(aiws.row_font);

	edit_aiw_widget->sync_numeric_values(aiws.precision_wind, aiws.precision_temps);
	edit_aiw_widget->sync_color_values(aiws.background, aiws.row_color, aiw);
	edit_aiw_widget->sync_font_values(aiws.row_font);
	init_settings();
}

void OptionWindow::init_settings()
{
	if (bShowFuel) {
		fw->show();
		// Billiger Workaroung
		/// Andernfalls funktioniert WA_TransparentForMouseEvents und
		/// WA_TranslucentBackground nicht richtig...
		exec_adjust_fuel();
		exec_adjust_fuel();
		exec_adjust_fuel();
		exec_adjust_fuel();
		/////////////////////////
		toggle_fuel->setText("Stop Fuel");
	} else {
		fw->hide();
		toggle_fuel->setText("Start Fuel");
	}
	if (bShowAIW) {
		aiw->show();
		// Billiger Workaroung
		/// Andernfalls funktioniert WA_TransparentForMouseEvents und
		/// WA_TranslucentBackground nicht richtig...
		exec_adjust_aiw();
		exec_adjust_aiw();
		exec_adjust_aiw();
		exec_adjust_aiw();
		/////////////////////////
		toggle_aiw->setText("Stop InfoWindow");
	} else {
		aiw->hide();
		toggle_aiw->setText("Start InfoWindow");
	}
}

void OptionWindow::set_versions(std::string kevs, std::string fc, std::string rel, std::string aiw)
{
	lbl_kevs_version->setText(kevs.c_str());
	lbl_fuelcalc_version->setText(fc.c_str());
	lbl_relative_version->setText(rel.c_str());
	lbl_aiw_version->setText(aiw.c_str());
}

void OptionWindow::closeEvent(QCloseEvent *event)
{
	save_settings();

	// Schließen
	edit_fuel_widget->close();
	fw->close();
	edit_aiw_widget->close();
	aiw->close();
#ifdef QT_DEBUG
	dw->close();
#endif

	// Endgültig beenden
	event->accept();
}

void OptionWindow::exec_save()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("KevS - Save Settings"), "",
		tr("KevS Settings-File (*.ksf)"));
	if (!(fileName.isEmpty()))
		save_settings(fileName);
	else
		return;
}

void OptionWindow::save_settings(QString file)
{
	{
		using namespace std;
		ofstream zeug;
		zeug.open(file.toStdString());
		zeug << lbl_kevs_version->text().toStdString() << endl;
		zeug << fw->x() << "," << fw->y() << "," << fw->width() << "," << fw->height() << "," << bShowFuel << "," << dsb_target_1->value() << "," << dsb_target_2->value() << ","
		     << fw->get_background_color().red() << "," << fw->get_background_color().green() << "," << fw->get_background_color().blue() << "," << fw->get_background_color().alpha() << ","
		     << fw->get_top_header_color().red() << "," << fw->get_top_header_color().green() << "," << fw->get_top_header_color().blue() << ","
		     << fw->get_refuel_precision() << "," << fw->get_max_laps_precision() << "," << fw->get_fae_precision() << "," << fw->get_delta_precision() << endl;
		for (int i = 0; i < AMOUNT_ROW; i++) {
			if (i)
				zeug << ",";
			zeug << fw->get_row_color(i).red() << "," << fw->get_row_color(i).green() << "," << fw->get_row_color(i).blue();
		}
		zeug << ",";
		for (int i = 0; i < 3; i++) {
			if (i)
				zeug << ",";
			zeug << fw->get_delta_color(i).red() << "," << fw->get_delta_color(i).green() << "," << fw->get_delta_color(i).blue();
		}
		zeug << endl;
		for (int i = 0; i < AMOUNT_ROW; i++) {
			if (i)
				zeug << ",";
			zeug << fw->get_row_font(i).family().toStdString() << ","
			     << fw->get_row_font(i).pointSize()	<< ","
			     << fw->get_row_font(i).weight() << ","
			     << fw->get_row_font(i).italic() << ","
			     << fw->get_row_font(i).underline() << ","
			     << fw->get_row_font(i).strikeOut();
		}
		zeug << "," << fw->get_header_font().family().toStdString() << ","
		     << fw->get_header_font().pointSize() << ","
		     << fw->get_header_font().weight() << ","
		     << fw->get_header_font().italic() << ","
		     << fw->get_header_font().underline() << ","
		     << fw->get_header_font().strikeOut();
		zeug << endl;

		// AdditionalWindowSettings
		zeug << aiw->x() << "," << aiw->y() << "," << aiw->width() << "," << aiw->height() << "," << bShowAIW << ","
		     << aiw->get_bg_color().red() << "," << aiw->get_bg_color().green() << "," << aiw->get_bg_color().blue() << "," << aiw->get_bg_color().alpha() << ","
		     << aiw->is_row_visible(ValueCategories::Wind) << "," << aiw->is_row_visible(ValueCategories::Temp) << "," << aiw->is_row_visible(ValueCategories::Car) << "," << aiw->is_row_visible(ValueCategories::Battery) << ","
		     << aiw->get_wind_precision() << "," << aiw->get_temp_precision() << endl;
		for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
			if (i)
				zeug << ",";
			zeug << aiw->get_row_color(i).red() << "," << aiw->get_row_color(i).green() << "," << aiw->get_row_color(i).blue();
		}
		zeug << endl;
		for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
			if (i)
				zeug << ",";
			zeug << aiw->get_row_font(i).family().toStdString() << ","
			     << aiw->get_row_font(i).pointSize() << ","
			     << aiw->get_row_font(i).weight() << ","
			     << aiw->get_row_font(i).italic() << ","
			     << aiw->get_row_font(i).underline() << ","
			     << aiw->get_row_font(i).strikeOut();
		}
		zeug.close();
	}
}
void OptionWindow::save_settings()
{
	save_settings("latest_settings.ksf");
}

void OptionWindow::exec_load()
{
	FWSettings fws;
	AIWSettings aiws;
	QString fileName = QFileDialog::getOpenFileName(this,
							tr("KevS - Load Settings File"), "",
							tr("KevS Settings-File (*.ksf)"));
	if (!(fileName.isEmpty())) {
		load_settings(&fws, &aiws, fileName);
		set_fuel_window_settings(fws);
		set_aiw_window_settings(aiws);
	} else
		return;
}
void OptionWindow::exec_reset()
{
	reset();
	dsb_target_1->setValue(0.0f);
	dsb_target_2->setValue(0.0f);
}

void OptionWindow::refresh_fuel() { exec_adjust_fuel(); exec_adjust_fuel(); }
/* Signals
 * Die 'slots' um Events von Buttons entgegen zu wirken
 */
void OptionWindow::exec_adjust_fuel()
{
	if (fw->isHidden()) {
		QMessageBox *mb = new QMessageBox(this);
		mb->setWindowTitle("Warning");
		mb->setText("Activate first Fuel-Window");
		mb->setIcon(QMessageBox::Warning);
		mb->setDefaultButton(QMessageBox::Ok);
		mb->show();
	} else {
		if (adjust_fuel->text() == "Move Mode Fuel") {
			adjust_fuel->setText("Race Mode Fuel");
			fw->setWindowFlags(Qt::WindowStaysOnTopHint);
			// WindowStysOnTopHint muss angefügt werden, mit setFlags werden vorherige Flags überschrieben
			fw->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			fw->show();
		} else {
			adjust_fuel->setText("Move Mode Fuel");
			fw->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
			// WindowStysOnTopHint muss angefügt werden, mit setFlags werden vorherige Flags überschrieben
			fw->setAttribute(Qt::WA_TransparentForMouseEvents, false);
			fw->show();
		}
	}
}
void OptionWindow::exec_edit_fuel()
{
	if (fw->isVisible()) {
		if (!edit_fuel_widget->isVisible())
			edit_fuel_widget->show();
	} else {
		QMessageBox *mb = new QMessageBox(this);
		mb->setWindowTitle("Warning");
		mb->setText("Activate first Fuel-Window");
		mb->setIcon(QMessageBox::Warning);
		mb->setDefaultButton(QMessageBox::Ok);
		mb->show();
	}
}
void OptionWindow::exec_start_fuel()
{
	if (toggle_fuel->text() == "Start Fuel") {
		toggle_fuel->setText("Stop Fuel");
#ifdef QT_DEBUG
		std::cout << "Ich war hier" << std::endl;
#endif
		fw->show();
		// Billiger Workaroung
		/// Andernfalls funktioniert WA_TransparentForMouseEvents und
		/// WA_TranslucentBackground nicht richtig...
		exec_adjust_fuel();
		exec_adjust_fuel();
		exec_adjust_fuel();
		exec_adjust_fuel();
		/////////////////////////
		bShowFuel = true;
	} else if (edit_fuel_widget->isVisible()) {
		QMessageBox *mb = new QMessageBox(this);
		mb->setWindowTitle("Warning");
		mb->setText("Close first Fule-Edit window");
		mb->setIcon(QMessageBox::Warning);
		mb->setDefaultButton(QMessageBox::Ok);
		mb->show();
	} else {
		toggle_fuel->setText("Start Fuel");
		fw->hide();
		bShowFuel = false;
	}
}

void OptionWindow::exec_adjust_aiw()
{
	if (aiw->isHidden()) {
		QMessageBox *mb = new QMessageBox(this);
		mb->setWindowTitle("Warning");
		mb->setText("Activate first Info-Window");
		mb->setIcon(QMessageBox::Warning);
		mb->setDefaultButton(QMessageBox::Ok);
		mb->show();
	} else {
		if (adjust_aiw->text() == "Move Mode InfoWindow") {
			adjust_aiw->setText("Race Mode InfoWindow");
			QSize n = aiw->size(); // See last line of Block
			aiw->setWindowFlags(Qt::WindowStaysOnTopHint);
			// WindowStysOnTopHint muss angefügt werden, mit setFlags werden vorherige Flags überschrieben
			aiw->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			aiw->show();
			aiw->resize(n); // Why the fuck do you need that here
						// FuelWindow doesn't need that shit...
		} else {
			adjust_aiw->setText("Move Mode InfoWindow");
			aiw->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
			// WindowStysOnTopHint muss angefügt werden, mit setFlags werden vorherige Flags überschrieben
			aiw->setAttribute(Qt::WA_TransparentForMouseEvents, false);
			aiw->show();
		}
	}
}
void OptionWindow::exec_start_aiw()
{
	if (toggle_aiw->text() == "Start InfoWindow") {
		toggle_aiw->setText("Stop InfoWindow");

		aiw->show();
		// Billiger Workaroung
		/// Andernfalls funktioniert WA_TransparentForMouseEvents und
		/// WA_TranslucentBackground nicht richtig...
		exec_adjust_aiw();
		exec_adjust_aiw();
		exec_adjust_aiw();
		exec_adjust_aiw();
		/////////////////////////
		bShowAIW = true;
	} else if (edit_aiw_widget->isVisible()) {
		QMessageBox *mb = new QMessageBox(this);
		mb->setWindowTitle("Warning");
		mb->setText("Close first Info-Edit window");
		mb->setIcon(QMessageBox::Warning);
		mb->setDefaultButton(QMessageBox::Ok);
		mb->show();
	} else {
		toggle_aiw->setText("Start InfoWindow");
		aiw->hide();
		bShowAIW = false;
	}
}
void OptionWindow::exec_edit_aiw()
{
	if (aiw->isVisible()) {
		if (!edit_aiw_widget->isVisible())
			edit_aiw_widget->show();
	} else {
		QMessageBox *mb = new QMessageBox(this);
		mb->setWindowTitle("Warning");
		mb->setText("Activate first Info-Window");
		mb->setIcon(QMessageBox::Warning);
		mb->setDefaultButton(QMessageBox::Ok);
		mb->show();
	}
}
void OptionWindow::exec_toggle_aiw_battery()
{
	aiw->setNewVisibilityForData(cb_aiw[ValueCategories::Battery]->isChecked(), ValueCategories::Battery);
}
void OptionWindow::exec_toggle_aiw_weather()
{
	aiw->setNewVisibilityForData(cb_aiw[ValueCategories::Wind]->isChecked(), ValueCategories::Wind);
}
void OptionWindow::exec_toggle_aiw_temps()
{
	aiw->setNewVisibilityForData(cb_aiw[ValueCategories::Temp]->isChecked(), ValueCategories::Temp);
}
void OptionWindow::exec_toggle_aiw_car()
{
	aiw->setNewVisibilityForData(cb_aiw[ValueCategories::Car]->isChecked(), ValueCategories::Car);
}

#ifdef QT_DEBUG
void OptionWindow::set_debug_window(DebugWindow *w)
{
	dw = w;
}
#endif

OptionWindow::~OptionWindow()
{
}

