#ifndef ALARM_h
#define ALARM_h

#if ARDUINO >= 100
	#include <Arduino.h>
#endif
#ifndef EEP_h
#include "EEP.h"
#endif
class ALARM
{
public:
	ALARM(EEP _eep);

	void set(int id, int state); // Sets alarm (ID) -> state
	void toggle(); // Toggles alarm state
	void clearTrigger();

	int deactivationTimeRemaining();
	bool status();

	String getTriggeredAlarms();
	const int alarmsLength = 3;
	int alarms[3] = {};

private:
	// Defines the EEPROM address for the alarm status.
	const int stateAddress = (eep.memorySize-10); 

	// Minimum time for an active alarm:

	// Example: Alarm is set once to STATUS_ALARM, but immediately set to STATUS_OK.
	// This requires the alarm to be active for at least 5 seconds before it can be deactivated again.
	const long minimumActivationTime = 25000;
	long lastActivationTime[4] = { -1,-1,-1,-1 };
	// Stores all alarms and their associated status

	// Stores an EEPROM instance
	EEP eep;

	// defines the time when an alarm was last triggered.
	long triggeredMillis = -1;

	// defines the timeframe of a deactivation code
	long timeUntilAlarm = 25000;

};
#endif

