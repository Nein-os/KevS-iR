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

#ifndef FUELRECORDER_H
#define FUELRECORDER_H

#include "statrecorder.h"

class FuelRecorder : public StatRecorder
{
public:
	FuelRecorder();

private:
	float amount_in_tank;

public:
	float *get_fuelsave_laps();
	float get_tank() { return amount_in_tank; }
	void update_tank(float t) { amount_in_tank = t; }
};

#endif // FUELRECORDER_H
