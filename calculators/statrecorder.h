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

#ifndef STATRECORDER_H
#define STATRECORDER_H

#define MAX_MONITORED_LAPS 7

class StatRecorder
{
public:
	StatRecorder();

protected:
	int current_lap, amount_valid_laps, amount_calc_laps;
	float laps[MAX_MONITORED_LAPS];

public:
	bool add_lap(float);
	int get_lap_count() { return current_lap; } // Vielleicht etwas mehr Logik dahinter
	int get_valid_laps() { return amount_valid_laps; }
	int get_calc_laps() { return amount_calc_laps; }
	float get_lap(int i) { return (i < MAX_MONITORED_LAPS && i >= 0) ? laps[i] : -1; }
	virtual void reset();
};

#endif // STATRECORDER_H
