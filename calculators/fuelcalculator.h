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

#ifndef FUELCALCULATOR_H
#define FUELCALCULATOR_H

class FuelRecorder;
class LapCalculator;

class FuelCalculator
{
public:
	FuelCalculator();
	FuelCalculator(FuelRecorder*);
	FuelCalculator(FuelRecorder*, LapCalculator*);
	~FuelCalculator();

private:
	FuelRecorder *stats;
	LapCalculator *lap_helper;
	float target_1, target_2;

public:
	void calc_averages(float []); // Gibt ein Array für alle Errechenbarenwerte zurück, ohne ein spezifisches Fuel-Target
	void calc_averages(float [], float); // Mit spezifischen Target
	void calc_averages(float [], float, float); // Mit spezifischen Target
	float calc_fuel_save();
	float calc_average(); // Berechnet den allgemeinen Durchschnitt
	float calc_refuel(float);
};

#endif // FUELCALCULATOR_H
