/* Copyright 2021 Krosny Kevin
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

#include "ui/optionwindow.h"
#include "ui/fuelwindow.h"
#include "ui/debugwindow.h"
#include "ui/additionalinformationwindow.h"
#include "irsdk/irsdk_client.h"
#include "irsdk/lap_timing.h"
#include "calculators/fuelrecorder.h"
#include "calculators/lapcalculator.h"
#include "calculators/fuelcalculator.h"
#include "helper.h"

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>

#ifdef QT_DEBUG
void gather_new_data(bool *bShowFuel, bool *bShowAIW, DebugWindow *dw);
#else
void gather_new_data(bool *bShowFuel, bool *bShowAIW);
#endif
void gather_immediat_data(bool *bShowFuel, bool *bShowAIW);
void get_lap_timing(OptionWindow*);
float get_verbrauch(float);

float stuff[AMOUNT_DATA];
float last_stand = 0.0f;
double fuel_target_1 = 0.0f;
double fuel_target_2 = 0.0f;
unsigned long last_session = 0;
int current_lap = -1;
bool ignore_current_lap;
StatRecorder *fr = new FuelRecorder();
LapCalculator *lc = new LapCalculator();
FuelCalculator *fc = new FuelCalculator((FuelRecorder*)fr, lc);
FuelWindow *f;
AdditionalInformationWindow *aiw;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OptionWindow w;
	FWSettings fws;
	AIWSettings aiws;
#ifdef QT_DEBUG
		DebugWindow *dw;
		 dw = new DebugWindow();
		 w.set_debug_window(dw);
		dw->show();
#endif

		f = w.get_fuel_ref();
		aiw = w.get_aiw_ref();
	{
		using namespace std;
		vector<string> stuff;
		ifstream settings;
		settings.open("latest_settings.ksf");
		std::string fuel_settings;
		if (settings.good()) { // Check falls existiert
			getline(settings, fuel_settings, '\n');
			settings.close();
			if (strcmp(fuel_settings.c_str(), VERSION_TOOL) == 0 || strcmp(fuel_settings.c_str(), "v0.3.0") == 0 || strcmp(fuel_settings.c_str(), "v0.2.4") == 0) {
				if (!load_settings(&fws, &aiws))
					switch(show_file_error(MainError::Error_while_parsing, &w)) {
						case QMessageBox::Ok:
							set_default_settings(&fws, &aiws);
							break;
						default:
							return 0;
					}
			} else {
				switch(show_file_error(MainError::Not_compatible, &w)) {
					case QMessageBox::Ok:
						set_default_settings(&fws, &aiws);
						break;
					default:
						return 0;
				}
			}
		} else {
			switch(show_file_error(MainError::Wrong_Settings_File, &w)) {
				case QMessageBox::Ok:
					set_default_settings(&fws, &aiws);
					break;
				default:
					return 0;
			}
		}
	}
	w.set_versions(VERSION_TOOL, VERSION_FUELCALC, VERSION_RELATIVE, VERSION_AIW);
	w.set_fuel_window_settings(fws);
	w.set_aiw_window_settings(aiws);

    bool *bShowFuel = w.get_fuel_state(), *bShowAIW = w.get_aiw_state();
#ifdef QT_DEBUG
    std::thread t_newData(gather_new_data, bShowFuel, bShowAIW, dw);
#else
    std::thread t_newData(gather_new_data, bShowFuel, bShowAIW);
#endif
    std::thread t_immediatData(gather_immediat_data, bShowFuel, bShowAIW);
    std::thread t_lapTiming(get_lap_timing, &w);

    w.show();
    update_aiw();

    return a.exec();
}

void get_lap_timing(OptionWindow *w)
{
	if(ir_info::init())
	{
	    while(true)
		ir_info::run();

	    printf("Shutting down.\n\n");
	    //timeEndPeriod(1);
	}
	else {
		show_error(MainError::Could_not_init, w);
	}
}

#ifdef QT_DEBUG
void gather_new_data(bool *bShowFuel, bool *bShowAIW, DebugWindow *dw) {
#else
void gather_new_data(bool *bShowFuel, bool *bShowAIW) {
#endif
	irsdk_client* ir;
	char answer[10];
	float laptime, fuel;
	ir = &irsdk_client::instance();
	// reset kann auch als Initialisierung benutzt werden
	reset();
	f->setNewData(stuff);

reconnect_data:
	if (ir->isConnected()) {
#ifdef QT_DEBUG
			 std::cout << "Is Connected\n" << std::endl;
#endif
			while (ir->waitForData(500)) {
				if (*bShowFuel) {
					if (ir_info::g_Lap.getInt() > current_lap) {
						current_lap = ir_info::g_Lap.getInt();
						if (ir->getSessionStrVal("WeekendInfo:SessionID:", answer, 10)) {
							if (last_session != std::stoul(answer)) {
#ifdef QT_DEBUG
								std::cout << "Ich resette wegen neuer Session" << std::endl;
#endif
								reset(); // Theoretisch neue Session
								f->setNewData(stuff);
								last_session = std::stoul(answer);
#ifdef QT_DEBUG
								std::cout << "SessionID: " << last_session << std::endl;
#endif
							}
							fuel = get_verbrauch(ir_info::g_FuelLevel.getFloat());
							if  (fuel < .0625)
								ignore_current_lap = true;
							if (ignore_current_lap) {
								lc->add_lap(-1); // Negativ, dass eine Runde gezählt wird, aber nicht als valide
								fr->add_lap(-1);
								ignore_current_lap = false; // Falls die Runde ignoriert wird, soll die nächste gewertet werden
							} else {
								int i = 0;
								std::this_thread::sleep_for(std::chrono::milliseconds(3000));
								laptime = ir_info::g_LastLapTime.getFloat();
								while (laptime < 1 && i++ < 20) {
									std::this_thread::sleep_for(std::chrono::milliseconds(35));
									laptime = ir_info::g_LastLapTime.getFloat();
								}
								if (fr->add_lap(fuel)) {
									((FuelRecorder*)fr)->update_tank(ir_info::g_FuelLevel.getFloat());
									if (fuel_target_1 > 0) {
										if (fuel_target_2 > 0)
											fc->calc_averages(stuff, fuel_target_1, fuel_target_2);
										else
											fc->calc_averages(stuff, fuel_target_1);
									} else {
										fc->calc_averages(stuff);
									}
								} else {
#ifdef QT_DEBUG
									std::cout << "Fuel nicht hinzugefügt: " << fuel << std::endl;
#endif
									stuff[8] = lc->calc_laps_left() + ir_info::g_Lap.getInt() - 1;
								}
								if (!lc->add_lap(laptime)) {
									stuff[8] = lc->calc_laps_left() + ir_info::g_Lap.getInt() - 1;
#ifdef QT_DEBUG
									std::cout << "Zeit nicht hinzugefügt: " << laptime << std::endl;
#endif
								}
							}
							f->setNewData(stuff);
						} else {
#ifdef QT_DEBUG
							std::cout << "Error F" << FuelError::No_Session_ID << std::endl;
#else
							//show_error(FuelError::No_Session_ID);
#endif
						}
					} else /*{
						if (ir_info::g_Lap.getInt() != current_lap) {
							if (current_lap != -1) {
								reset();
								get_verbrauch(ir_info::g_FuelLevel.getFloat()); // Update für letzten Füllstand
								f->setNewData(stuff);
								last_session = std::stoul(answer);
								std::cout << "SessionID: " << last_session << std::endl;
							}
						}
					}*/
						if (ir_info::g_Lap.getInt() != current_lap)
							current_lap = ir_info::g_Lap.getInt(); // Kein Reset, weil die Runden nicht passen
							// iRacing ist im Init nicht Sync in einem Rejoin // Rundendaten werden erst später versendet
				}
#ifdef QT_DEBUG
				dw->setNewData(ir_info::g_Lap.getInt(), ir_info::g_LastLapTime.getFloat(), ir_info::g_FuelLevel.getFloat(), ignore_current_lap);
#endif
				if (*bShowAIW) {
					update_aiw();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		} else {
			// std::cout << "Not\n" << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		goto reconnect_data;
}

void gather_immediat_data(bool *bShowFuel, bool *bShowAIW) {
	irsdk_client *ir;
	ir = &irsdk_client::instance();
reconnect_immediat_data:
	if (ir->isConnected()) {
		while (*bShowFuel || *bShowAIW) {
			f->refresh_tank(ir_info::g_FuelLevel.getFloat());
			if (ir_info::g_BatteryUsed.isValid())
				aiw->setHybrid(ir_info::g_BatteryState.getFloat() * 100, ir_info::g_BatteryUsed.getFloat() * 100);
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	goto reconnect_immediat_data;
}

float get_verbrauch(float stand)
{
	float buffer;
	if (stand > 0) {
		if (last_stand < stand) {
			last_stand = stand;
		} else {
			buffer = last_stand;
			last_stand = stand;
			return buffer - last_stand;
		}
	}
	return 0.0f;
}

const std::vector<std::string> explode(const std::string &s, const char &c)
{
	std::string buff{""};
	std::vector<std::string> v;

	for (auto n:s) {
		if (n != c)
			buff+=n;
		else
			if (n == c && buff != "") {
				v.push_back(buff);
				buff = "";
			}
	}
	if (buff != "")
		v.push_back(buff);

	return v;
}

// Kommunikations-Schnittstelle zwischen OptionWindow und dem FuelCalculator
void update_fuel_target_1(double i, FuelWindow *fw) { fuel_target_1 = i; fw->target_1_changed(i); } // Das ist absolut furchtbar und sollte definitv anders umgesetzt werden
// fuel_target muss nicht resettet werden // Wird vom Benutzer gesetzt
void update_fuel_target_2(double i, FuelWindow *fw) { fuel_target_2 = i; fw->target_2_changed(i); } // Das ist absolut furchtbar und sollte definitv anders umgesetzt werden

void update_decimals_fw()
{
	f->setNewData(stuff);
}
void update_aiw()
{
	float aiw_data[ValueTypes::AMNT_VALUE_TYPES-2];
	aiw_data[ValueTypes::AirTemp] = ir_info::g_AirTemp.getFloat();
	aiw_data[ValueTypes::WindDir] = ir_info::g_WindDir.getFloat();
	aiw_data[ValueTypes::WindSpeed] = ir_info::g_WindVel.getFloat();
	aiw_data[ValueTypes::Humidity] = ir_info::g_RelativeHumidity.getFloat() * 100;
	aiw_data[ValueTypes::OilTemp] = ir_info::g_PlayerOilTemp.getInt();
	aiw_data[ValueTypes::WaterTemp] = ir_info::g_PlayerWaterTemp.getInt();
	aiw_data[ValueTypes::TrackTemp] = ir_info::g_TrackTempCrew.getFloat();
	aiw->setNewData(aiw_data);
}

void reset()
{
	for (int i = 0; i < AMOUNT_DATA; i++)
		stuff[i] = .0f;
	last_stand = 0.0f;
	last_session = 0;
	current_lap= -1;
	ignore_current_lap = true;
	fr->reset();
	lc->reset();
}

void show_error(MainError e, QWidget *w)
{
	QString text;
	QMessageBox *mb = new QMessageBox(w);
	mb->setWindowTitle("Error");
	mb->setIcon(QMessageBox::Warning);
	mb->setDefaultButton(QMessageBox::Ok);
	switch (e) {
		case MainError::Wrong_Settings_File:
			text = text = QString("Settingsdatei fehlerhaft.\nLöschen Sie die aktuelle Datei oder kontaktieren Sie den Entwickler.\nError-Code: M%1").arg(e);
			break;
		case MainError::Could_not_init:
			text = QString("iRacing-SDK konnte nicht initialisiert werden");
			break;
		default:
			text = QString("Unerwarteter Fehler");
	}
	mb->setText(text);
	mb->exec();
}
int show_file_error(MainError e, QWidget *w)
{
	QString text;
	QMessageBox *mb = new QMessageBox(w);
	mb->setWindowTitle("Corrupt Settings file");
	mb->setIcon(QMessageBox::Warning);
	if (MainError::Wrong_Settings_File == e)
		mb->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	mb->setDefaultButton(QMessageBox::Ok);
	switch (e) {
		case MainError::Wrong_Settings_File:
			text = text = QString("Error-Code: M%1\nSettings-File has some issues. Load default settings or abort?\nSettings-File will not be written.").arg(e);
			break;
		case MainError::Could_not_init:
			text = QString("iRacing-SDK could not be initialised");
			break;
		case MainError::Too_old_to_load:
			text = QString("Error-Code: M%1\nSettings-File is too old. Load default settings or abort?\nSettings-File will not be written.").arg(e);
			break;
		case MainError::Not_compatible:
			text = QString("Error-Code: M%1\nSettings-File is broken.").arg(e);
			break;
		case MainError::Error_while_parsing:
			text = QString("Error-Code: M%1\nSettings-File could not be parsed.");
			break;
		default:
			text = QString("Unexpected Error");
	}
	mb->setText(text);
	return mb->exec();
}
/*void show_error(FuelError e) {
	QString text;
	f->show_error(e);
}
*/

bool load_settings(FWSettings *fws, AIWSettings *aiws, const QString fileName)
{
	std::ifstream settings;
	settings.open((fileName.isEmpty()) ? "latest_settings.ksf" : fileName.toStdString());
	std::string fuel_settings, version_number;
	std::vector<std::string> to_load;
	if (settings.good()) { // Check falls existiert
		getline(settings, fuel_settings, '\n');
		version_number = fuel_settings;
		getline(settings, fuel_settings, '\n');
		{
			using namespace std;
			to_load = explode(fuel_settings, ',');
			fws->pos_x = stoi(to_load[0]);
			fws->pos_y = stoi(to_load[1]);
			fws->width = stoi(to_load[2]);
			fws->height = stoi(to_load[3]);
			fws->bActivateFuel = stoi(to_load[4]);
			fws->target_1 = fuel_target_1 = stod(to_load[5]);
			fws->target_2 = fuel_target_2 = stod(to_load[6]);
			//std::cout << stod(to_load[5]) << "=" << fuel_target << std::endl;
			fws->background = QColor(stoi(to_load[7]),stoi(to_load[8]),stoi(to_load[9]),stoi(to_load[10]));
			fws->top_header = QColor(stoi(to_load[11]),stoi(to_load[12]),stoi(to_load[13]));
			if (strcmp(version_number.c_str(), "v0.3.1") == 0) {
				fws->precision_refuel = stoi(to_load[14]);
				fws->precision_max_laps = stoi(to_load[15]);
				fws->precision_fae = stoi(to_load[16]);
				fws->delta_precision = stod(to_load[17]);
			} else {
				fws->precision_refuel = 2;
				fws->precision_max_laps = 2;
				fws->precision_fae = 2;
				fws->delta_precision = .05;
			}
			getline(settings,fuel_settings,'\n');
			to_load = explode(fuel_settings, ',');
			for (int i = 0; i < AMOUNT_ROW; i++) {
#ifdef QT_DEBUG
				cout << i << ". :" << stoi(to_load[i*3]) << " " << stoi(to_load[i*3 + 1]) << " " << stoi(to_load[i*3 + 2]) << endl;
#endif
				fws->row[i] = QColor(stoi(to_load[i*3]), stoi(to_load[i*3 + 1]), stoi(to_load[i*3 + 2])); // 3 für RGB-Werte
			}
			for (int i = AMOUNT_ROW; i < AMOUNT_ROW+3; i++)
				fws->delta[i-AMOUNT_ROW] = QColor(stoi(to_load[i*3]), stoi(to_load[i*3 + 1]), stoi(to_load[i*3 + 2]));
			getline(settings,fuel_settings,'\n');
			to_load = explode(fuel_settings, ',');
			for (int i = 0; i < AMOUNT_ROW; i++) {
				fws->row_font[i] = QFont(to_load[i*6].c_str(), stoi(to_load[i*6+1]),  stoi(to_load[i*6+2]), stoi(to_load[i*6+3]));
				fws->row_font[i].setUnderline(stoi(to_load[i*6+4]));
				fws->row_font[i].setStrikeOut(stoi(to_load[i*6+5]));
			}
			fws->top_header_font = QFont(to_load[AMOUNT_ROW*6].c_str(),
					stoi(to_load[AMOUNT_ROW*6+1]),
					stoi(to_load[AMOUNT_ROW*6+2]),
					stoi(to_load[AMOUNT_ROW*6+3]));
			fws->top_header_font.setUnderline(stoi(to_load[AMOUNT_ROW*6+4]));
			fws->top_header_font.setStrikeOut(stoi(to_load[AMOUNT_ROW*6+5]));
#ifdef QT_DEBUG
			cout << "Header-Font:" << to_load[AMOUNT_ROW*6] << ", " << to_load[AMOUNT_ROW*6+1] << ","
			     << to_load[AMOUNT_ROW*6+2] << "," << to_load[AMOUNT_ROW*6+3] << ","
			     << to_load[AMOUNT_ROW*6+4] << "," << to_load[AMOUNT_ROW*6+5] << endl << endl;
#endif
			if (strcmp(version_number.c_str(), "v0.3.1") == 0 || strcmp(version_number.c_str(), "v0.3.0") == 0) {
				getline(settings,fuel_settings,'\n');
				to_load = explode(fuel_settings, ',');
				aiws->pos_x = stoi(to_load[0]);
				aiws->pos_y = stoi(to_load[1]);
				aiws->width = stoi(to_load[2]);
				aiws->height = stoi(to_load[3]);
				aiws->bActivateInformation = stoi(to_load[4]);
				aiws->background = QColor(stoi(to_load[5]), stoi(to_load[6]), stoi(to_load[7]), stoi(to_load[8]));
				for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++)
					aiws->row_visible[i] = stoi(to_load[i+9]);
				if (strcmp(version_number.c_str(), "v0.3.1") == 0) {
					aiws->precision_wind = stoi(to_load[ValueCategories::AMNT_VALUE_CATEGORIES+9]);
					aiws->precision_temps = stoi(to_load[ValueCategories::AMNT_VALUE_CATEGORIES+10]);
				} else {
					aiws->precision_temps = 1;
					aiws->precision_wind = 0;
				}
				getline(settings,fuel_settings,'\n');
				to_load = explode(fuel_settings, ',');
				for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++)
					aiws->row_color[i] = QColor(stoi(to_load[3*i]), stoi(to_load[3*i+1]), stoi(to_load[3*i+2]));
				getline(settings,fuel_settings,'\n');
				to_load = explode(fuel_settings, ',');
				for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
					aiws->row_font[i] = QFont(to_load[i*6].c_str(), stoi(to_load[i*6+1]),
								stoi(to_load[i*6+2]), stoi(to_load[i*6+3]));
					aiws->row_font[i].setUnderline(stoi(to_load[i*6+4]));
					aiws->row_font[i].setStrikeOut(stoi(to_load[i*6+5]));
				}
			} else {
				set_default_settings(aiws);
			}
			settings.close();
		}
		return true;
	} else
		settings.close();
	return false;
}
void set_default_settings(FWSettings *fws, AIWSettings *aiws)
{
	fws->pos_x = 500;
	fws->pos_y = 500;
	fws->bActivateFuel = true;
	fws->target_1 = fuel_target_1 = 3.0f;
	fws->target_2 = fuel_target_2 = 2.0f;
	fws->precision_refuel = 2;
	fws->precision_max_laps = 2;
	fws->precision_fae = 2;
	fws->delta_precision = .05;
	fws->background = QColor(0,0,0,135);
	fws->top_header = QColor(255,255,255);
	fws->row[0] = QColor(255,255,255);
	fws->row[1] = QColor(250,50, 50);
	fws->row[2] = QColor(120,120,250);
	fws->row[3] = QColor(200,200,0);
	fws->row[4] = QColor(200,200,0);

	fws->top_header_font = QFont("Arial", 16);
	fws->row_font[0] = QFont("Arial", 25);
	fws->row_font[1] = QFont("Arial", 18);
	fws->row_font[2] = QFont("Arial", 18);
	fws->row_font[3] = QFont("Arial", 14);
	fws->row_font[4] = QFont("Arial", 14);

	fws->delta[0] = QColor(0,255,0);
	fws->delta[1] = QColor(0,0,0);
	fws->delta[2] = QColor(255,0,0);

	set_default_settings(aiws);
}

void set_default_settings(AIWSettings *aiws)
{
	aiws->pos_x = 1000;
	aiws->pos_y = 1000;
	aiws->bActivateInformation = true;
	aiws->width = 250;
	aiws->height = 500;
	aiws->background = QColor(255,255,255,135);
	for (int i = 0; i < ValueCategories::AMNT_VALUE_CATEGORIES; i++) {
		aiws->row_font[i] = QFont("Arial", 28);
		aiws->row_color[i] = QColor(0,0,0);
		aiws->row_visible[i] = true;
	}
	aiws->precision_temps = 1;
	aiws->precision_wind = 0;
}
