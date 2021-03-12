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

#ifndef LAPRECORDER_H
#define LAPRECORDER_H

#include "statrecorder.h"

class LapRecorder : public StatRecorder
{
public:
	LapRecorder();

private:
	int laps_leader, car_id_leader;

public:
	virtual void reset() override;
	int get_laps_leader() const { return laps_leader; }
	int get_id_leader() const { return car_id_leader; }
	void inc_laps_leader() { laps_leader++; }
	void set_laps_leader(int i) { laps_leader = i; }
	void set_car_id_leader(int i) { car_id_leader = i; }
};

#endif // LAPRECORDER_H
