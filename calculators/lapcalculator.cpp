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
#include <QMessageBox>

#include "../irsdk/lap_timing.h"
#include "../irsdk/irsdk_client.h"
#include "../helper.h"
#include "lapcalculator.h"
#include "laprecorder.h"

LapCalculator::LapCalculator()
{
	stats = new LapRecorder();
	ir = &irsdk_client::instance();
}
LapCalculator::LapCalculator(LapRecorder* recorder)
{
	stats = recorder;
	ir = &irsdk_client::instance();
}

float LapCalculator::calc_average()
{
	float avg = 0.0f;

	if (stats->get_calc_laps() == 0)
		return 0.0f;

	for (int i = 0; i < stats->get_calc_laps(); i++)
		avg += stats->get_lap(i);
	return avg / stats->get_calc_laps();
}

float LapCalculator::calc_average_leader()
{
	char answer[3];
	char id[2];
	char question_lap[] = "SessionInfo:Sessions:SessionNum:{x}ResultsPositions:Position:{1}LapsComplete:";
	char question_id[] = "SessionInfo:Sessions:SessionNum:{x}ResultsPositions:Position:{1}CarIdx:";

	question_lap[33] = ir_info::g_SessionNum.getInt() + '0';
	question_id[33] = ir_info::g_SessionNum.getInt() + '0';

	if (ir->getSessionStrVal(question_lap, answer, 3)) {
#ifdef QT_DEBUG
		std::cout << "workde: " << answer << std::endl;
#endif
		if (ir->getSessionStrVal(question_id, id, 2)) {
#ifdef QT_DEBUG
			std::cout << "workde: " << id << std::endl;
#endif
			try {
				stats->set_laps_leader(std::stoi(answer));
			} catch (std::invalid_argument) {
				stats->inc_laps_leader();
			}
			try {
				stats->set_car_id_leader(std::stoi(id));
			} catch (std::invalid_argument) {
				if (stats->get_id_leader() == -1)
					stats->set_car_id_leader(ir_info::g_PlayerCarIdx.getInt());
			}
			last_leader_avg = ir_info::g_SessionTime.getDouble() / (stats->get_laps_leader() + ir_info::g_CarIdxLapDistPct.getFloat(stats->get_id_leader()));
		}
	}
	return last_leader_avg;
}

float LapCalculator::calc_laps_left()
{
	float own_laps_left, leader_laps_left;
	float leader_dist;
	char answer[3];
	char question[] = "SessionInfo:Sessions:SessionNum:{x}SessionType:";

	question[33] = ir_info::g_SessionNum.getInt() + '0';
#ifdef QT_DEBUG
	std::cout << question << std::endl;
#endif
	if (ir->getSessionStrVal(question, answer, 3)) {
		own_laps_left = ir_info::g_SessionTimeRemain.getDouble() / calc_average();
		if (strcmp("Pra", answer) != 0) {
			if (ir_info::g_SessionLapsRemainEx.getInt() > 1500 || ir_info::g_SessionLapsRemainEx.getInt() < -1) { // Ist die Zahl zu klein?
				if (ir_info::g_PlayerPosition.getInt() == 1) {
					last_leader_avg = calc_average();
					return own_laps_left;
				} else {
					leader_laps_left = ir_info::g_SessionTimeRemain.getDouble() / calc_average_leader();
					leader_dist = ceil(leader_laps_left) * calc_average_leader();
					return leader_dist / calc_average();
				}
			} else {
				return ir_info::g_SessionLapsRemainEx.getInt();
			}
		} else {
			// TODO: ?
		}
	} else {
#ifdef QT_DEBUG
		//std::cout << "Error #" << ErrorList::No_Session_Type << std::endl;
#else
		//show_error(FuelError::No_Session_Type);
#endif
	}
	return 0;
}

bool LapCalculator::add_lap(float time)
{
	return stats->add_lap(time);
}

void LapCalculator::reset() { stats->reset(); }

LapCalculator::~LapCalculator()
{
	free(stats);
}
