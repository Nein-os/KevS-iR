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

#include <stdlib.h>
#include <cmath>
#include <iostream>

#include "statrecorder.h"
#include "lapcalculator.h"
#include "fuelcalculator.h"
#include "fuelrecorder.h"
#include "../helper.h"

FuelCalculator::FuelCalculator()
{
	stats = new FuelRecorder();
	lap_helper = new LapCalculator();
}
FuelCalculator::FuelCalculator(FuelRecorder* recorder)
{
	stats = recorder;
	lap_helper = new LapCalculator();
}
FuelCalculator::FuelCalculator(FuelRecorder* recorder, LapCalculator* calc)
{
	stats = recorder;
	lap_helper = calc;
}

float FuelCalculator::calc_average()
{
	float avg = 0.f;

	if (stats->get_calc_laps() == 0)
		return 0.0f;

	for (int i = 0; i < stats->get_calc_laps(); i++)
		avg += stats->get_lap(i);

	return avg / stats->get_calc_laps();
}

float FuelCalculator::calc_fuel_save()
{
	// TODO
	// Not finished and should not be used
	float avg = 0.f;
	float *laps = stats->get_fuelsave_laps();

	if (laps == nullptr)
		return 0.0f;

	for (int i = 0; i < stats->get_valid_laps(); i++)
		avg += laps[i];
	return avg / stats->get_valid_laps();
}

void FuelCalculator::calc_averages(float values[])
{
	values[2] = calc_average();
	float buffer = lap_helper->calc_laps_left();
	float refuel = calc_refuel(values[2]);
	float extra = refuel + values[2];
	// Legende für die Indizes
	/// 1. Zeile: 0 - 2: Averages
	/// 2.Zeile: 3 - 5: Target 1 (mit Delta)
	/// 3.Zeile: 6 - 8: Target 2 (mit Delta)
	/// 4. Zeile: 9 - 10: Aktuell angefangene Runden / Zu fahrende Runden
	/// 5. Zeile: 11 - 12: Möglichfahrende Runden vom Durchschnitt / Letzte Runde Verbrauch
	values[0] = refuel;
	values[1] = (extra > .0f) ? extra : 0;
	values[9] = stats->get_lap_count() + 1;
	values[10] = (buffer > 0) ? buffer + stats->get_lap_count() : 0;
	values[11] = stats->get_tank() / values[2];
	values[12] = stats->get_lap(stats->get_calc_laps()-1);
}
void FuelCalculator::calc_averages(float values[], float target)
{
	float refuel = calc_refuel(target);
	float extra = refuel + target;
	calc_averages(values);
	values[3] = (refuel > 0) ? refuel : 0;
	values[4] = (extra > 0) ? extra : 0;
	values[5] = values[12] - target;
}
void FuelCalculator::calc_averages(float values[], float target, float target_2)
{
	float refuel = calc_refuel(target_2);
	float extra = refuel + target_2;
	calc_averages(values, target);
	values[6] = (refuel > 0) ? refuel : 0;
	values[7] = (extra > 0) ? extra : 0;
	values[8] = values[12] - target_2;
}

float FuelCalculator::calc_refuel(float avg_per_lap)
{
	float result = (ceil(lap_helper->calc_laps_left()) * avg_per_lap) - stats->get_tank();
	return result;
}

FuelCalculator::~FuelCalculator() {
	free(stats);
	free(lap_helper);
}
