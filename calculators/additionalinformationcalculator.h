#ifndef ADDITIONALINFORMATIONCALCULATOR_H
#define ADDITIONALINFORMATIONCALCULATOR_H

#include <string>
#include "../irsdk/irsdk_client.h"

typedef enum _AdditionalTypeDefintions : int {
	TrackTemp = 0,
	AirTemp = 1,
	WindSpeed = 2,
	WindDir =3,
	OilTemp = 4,
	WaterTemp = 5,
	Humidity = 6,
	BatteryStatus =7,
	BatteryUsage = 8,
	AMNT_VALUE_TYPES
} ValueTypes;

class AdditionalInformationCalculator
{
private:
	bool activated[ValueTypes::AMNT_VALUE_TYPES];
	irsdk_client *irsdk;

public:
	AdditionalInformationCalculator();

	bool update_values(std::string values[]);
	void update_state(int i, bool new_state) { if (i >=0 && i < ValueTypes::AMNT_VALUE_TYPES) activated[i] = new_state; }
	bool get_state(int i) { return (i >= 0 && i < ValueTypes::AMNT_VALUE_TYPES) ? activated[i] : false; }
};

#endif // ADDITIONALINFORMATIONCALCULATOR_H
