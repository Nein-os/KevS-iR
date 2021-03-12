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

#ifndef LAPCALCULATOR_H
#define LAPCALCULATOR_H

class LapRecorder;
class irsdk_client;

class LapCalculator
{
public:
	LapCalculator();
	LapCalculator(LapRecorder*);
	~LapCalculator();

private:
	LapRecorder *stats;
	irsdk_client *ir;
	float last_leader_avg;

public:
	float calc_average(); // Berechnet die Durchschnittszeit eines selbst
	float calc_average_leader(); // Durchschnittszeit des Leaders des gesamten Rennens
	float calc_laps_left();
	bool add_lap(float);
	void reset();
};

#endif // LAPCALCULATOR_H
