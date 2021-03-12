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

#ifndef HELPER_H
#define HELPER_H

#define AMOUNT_ROW 5

#define VERSION_TOOL "v0.2.4"
#define VERSION_FUELCALC "v1.2.6"
#define VERSION_RELATIVE "N/A"

//#define IS_EXPERIMENTAL

#include <QColor>
#include <QFont>
#include <QMessageBox>
#include <string>

class FuelWindow;

typedef struct _FuelWindowSettingsStruct {
	bool bActivateFuel;
	int pos_x = 0;
	int pos_y = 0;
	int width = 800;
	int height = 500;
	double target_1 = 0.f;
	double target_2 = 0.f;
	QColor background;
	QColor top_header;
	QColor delta[3];
	QColor row[AMOUNT_ROW];
	QFont top_header_font;
	QFont row_font[AMOUNT_ROW];
} FWSettings;

typedef enum _KevS_Fuel_Error_List : int {
	No_Session_ID = 1,
	No_Session_Type = 2
} FuelError;
typedef enum _KevS_Main_Error_List : int {
	Wrong_Settings_File = 1,
	Could_not_init = 2,
	Too_old_to_load = 3,
	Not_compatible = 4,
	Error_while_parsing = 5
} MainError;

const std::vector<std::string> explode(const std::string& s, const char& c);
void reset();
bool load_settings(FWSettings *fws, const QString fileName = NULL);
void set_default_settings(FWSettings *fws);

void update_fuel_target_1(double, FuelWindow*);
void update_fuel_target_2(double, FuelWindow*);
//void show_error(FuelError e);
void show_error(MainError e, QWidget*);
int show_file_error(MainError e, QWidget*);

#endif // HELPER_H
