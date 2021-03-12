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
void gather_new_data(bool *bKeepAlive, bool *bShowFuel, DebugWindow *dw);
#else
void gather_new_data(bool *bKeepAlive, bool *bShowFuel);
#endif
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

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OptionWindow w;
	FWSettings fws;
#ifdef QT_DEBUG
		DebugWindow *dw;
		 dw = new DebugWindow();
		dw->show();
#endif

		f = w.get_fuel_ref();
	{
		using namespace std;
		vector<string> stuff;
		ifstream settings;
		settings.open("latest_settings.ksf");
		std::string fuel_settings;
		if (settings.good()) { // Check falls existiert
			getline(settings, fuel_settings, '\n');
			settings.close();
			if (strcmp(fuel_settings.c_str(), VERSION_TOOL) == 0) {
				if (!load_settings(&fws))
					switch(show_file_error(MainError::Error_while_parsing, &w)) {
						case QMessageBox::Ok:
							set_default_settings(&fws);
							break;
						default:
							return 0;
					}
			} else {
				switch(show_file_error(MainError::Not_compatible, &w)) {
					case QMessageBox::Ok:
						set_default_settings(&fws);
						break;
					default:
						return 0;
				}
			}
		} else {
			switch(show_file_error(MainError::Wrong_Settings_File, &w)) {
				case QMessageBox::Ok:
					set_default_settings(&fws);
					break;
				default:
					return 0;
			}
		}
	}
	w.set_versions(VERSION_TOOL, VERSION_FUELCALC, VERSION_RELATIVE);
	w.set_fuel_window_settings(fws);

    bool bKeepAlive = true, *bShowFuel = w.get_fuel_state();
#ifdef QT_DEBUG
    std::thread t_newData(gather_new_data, &bKeepAlive, bShowFuel, dw);
#else
    std::thread t_newData(gather_new_data, &bKeepAlive, bShowFuel);
#endif
    std::thread t_lapTiming(get_lap_timing, &w);

    w.show();

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
void gather_new_data(bool *bKeepAlive, bool *bShowFuel, DebugWindow *dw) {
#else
void gather_new_data(bool *bKeepAlive, bool *bShowFuel) {
#endif
	irsdk_client* ir;
	char answer[10];
	float laptime, fuel;

	// reset
	/// kann auch als Initialisierung benutzt werden
	/// reset() wird nicht benutzt, da das fuel_target nicht resetet werden muss
	for (int i = 0; i < AMOUNT_DATA; i++)
		stuff[i] = .0f;
	last_stand = 0.0f;
	last_session = 0;
	current_lap= -1;
	ignore_current_lap = true;
	fr->reset();
	lc->reset();

	f->setNewData(stuff);

	while (*bKeepAlive) {
		/* TODOs for Main-Logic-Thread
		 *
		 * Resets:
		 *  - Reset von Durchschnitt und Fuel-Save
		 *	- Falls Fuel-Save eintritt zum ersten mal, seit Beginn
		 *	- Falls Fuel-Save aufhört zum ersten mal, seit Beginn
		*/

		ir = &irsdk_client::instance();
		if (ir->isConnected()) {
			// std::cout << "Is Connected\n" << std::endl;
			if (ir->waitForData(500)) {
				if (*bShowFuel) {
					f->refresh_tank(ir_info::g_FuelLevel.getFloat());
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
			}
		} else {
			// std::cout << "Not\n" << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
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


void reset()
{
	for (int i = 0; i < AMOUNT_DATA; i++)
		stuff[i] = .0f;
	last_stand = 0.0f;
	last_session = 0;
	update_fuel_target_1(0.0f, f);
	update_fuel_target_2(0.0f, f);
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

bool load_settings(FWSettings *fws, const QString fileName)
{
	std::ifstream settings;
	settings.open((fileName.isEmpty()) ? "latest_settings.ksf" : fileName.toStdString());
	std::string fuel_settings;
	std::vector<std::string> to_load;
	if (settings.good()) { // Check falls existiert
		getline(settings, fuel_settings, '\n');
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
			settings.close();
		}
		return true;
	} else
		settings.close();
	return false;
}
void set_default_settings(FWSettings *fws)
{
	fws->pos_x = 500;
	fws->pos_y = 500;
	fws->bActivateFuel = true;
	fws->target_1 = fuel_target_1 = 3.0f;
	fws->target_2 = fuel_target_2 = 2.0f;
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
}