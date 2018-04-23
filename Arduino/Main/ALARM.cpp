#include "ALARM.h"

#define STATUS_OK			B0	// 0
#define STATUS_ALARM		B1	// 1
#define STATUS_NO_SIGNAL	B10 // 2

#define ALARM_0				B0	// 0 Window/Door (REED-Relay)
#define ALARM_1				B1	// 1 Motion Sensor (PIR Sensor)
#define ALARM_2				B10 // 2 Accelerometer (Vibration Detector)

ALARM::ALARM(EEP _eep)
{
	eep = _eep;
}

void ALARM::toggle()
{
	if (eep.memory[stateAddress] == 1) {
		eep.Write(stateAddress, 0);
	}
	else {
		eep.Write(stateAddress, 1);
	}
}

void ALARM::set(int id, int status)
{
	if (alarms[id] == STATUS_ALARM || alarms[id] == STATUS_NO_SIGNAL) {
		if (status == STATUS_OK) {
			if (millis() - lastActivationTime[id] < minimumActivationTime) {
				return;
			}
		}
		else if (alarms[id] == status) {
			return;
		}
	}
	if (status == STATUS_ALARM || status == STATUS_NO_SIGNAL)
	{
		if (this->status()) {
			lastActivationTime[id] = millis();
			triggeredMillis = millis();
		}
	}
	alarms[id] = status;
}

void ALARM::clearTrigger()
{
	triggeredMillis = -1;
}

int ALARM::deactivationTimeRemaining()
{
	if (triggeredMillis == -1) {
		return -1;
	}
	int rem = (timeUntilAlarm - (millis() - triggeredMillis)) / 1000;
	return rem >= 0 ? rem : 0;
}

bool ALARM::status()
{
	if (eep.memory[stateAddress] == 1)
		return true;
	else
		return false;
}

String ALARM::getTriggeredAlarms()
{
	String out = "";
	for (int i = 0; i < alarmsLength; i++)
	{
		if (alarms[i] == STATUS_ALARM) {
			out.concat(i);
			out.concat(" ");
			break;
		}
	}
	out.remove(out.length() - 1);
	return out;
}