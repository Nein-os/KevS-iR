#include "additionalinformationcalculator.h"

#include <QString>

#include "../irsdk/lap_timing.h"

AdditionalInformationCalculator::AdditionalInformationCalculator()
{
	for (int i = 0; i < ValueTypes::AMNT_VALUE_TYPES; i++)
		activated[i] = true;

	irsdk = &irsdk_client::instance();
}

bool AdditionalInformationCalculator::update_values(std::string values[])
{
	if (irsdk->isConnected()) {
		for (int i = 0; i < ValueTypes::AMNT_VALUE_TYPES; i++) {
			if (activated[i])
				switch (i) {
					case ValueTypes::AirTemp:
						values[ValueTypes::AirTemp] = QString::number(ir_info::g_AirTemp.getFloat(), 'f', 2).toStdString();
						break;
					case ValueTypes::TrackTemp:
						values[ValueTypes::TrackTemp] = QString::number(ir_info::g_TrackTempCrew.getFloat(), 'f', 2).toStdString();
						break;
					case ValueTypes::WindSpeed:
						values[ValueTypes::WindSpeed] = QString::number(ir_info::g_WindVel.getFloat(), 'f', 2).toStdString();
						break;
					case ValueTypes::WindDir:
						values[ValueTypes::WindDir] = QString::number(ir_info::g_WindDir.getFloat(), 'f', 0).toStdString();
						break;
					default:
						values[i] = QString("ERR").toStdString();
				}
		}
		return true;
	}
	return false;
}
