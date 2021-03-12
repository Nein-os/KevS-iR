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

#include "statrecorder.h"
#include "../irsdk/lap_timing.h"

StatRecorder::StatRecorder()
{
	current_lap = 0;
	amount_valid_laps = 0;
	amount_calc_laps = 0;
	for (int i = 0; i < MAX_MONITORED_LAPS; i++)
		laps[i] = 0.0f;
}

bool StatRecorder::add_lap(float val)
{
	float buffer;

	current_lap = ir_info::g_Lap.getInt();
	if (val > 0.0f) {
		++amount_valid_laps;
		if (amount_calc_laps == MAX_MONITORED_LAPS) {
			for (int i = 0; i < MAX_MONITORED_LAPS-1; i++) {
				buffer = laps[i+1];
				laps[i] = buffer;
			}
			laps[MAX_MONITORED_LAPS-1] = val;
		} else {
			amount_calc_laps++;
			if (amount_calc_laps > 0)
				laps[amount_calc_laps-1] = val;
		}

	} else
		return false;
	return true;
}

void StatRecorder::reset()
{
	for (int i = 0; i < MAX_MONITORED_LAPS; i++)
		laps[i] = .0f;
	current_lap = 0;
	amount_valid_laps = 0;
	amount_calc_laps = 0;
}
