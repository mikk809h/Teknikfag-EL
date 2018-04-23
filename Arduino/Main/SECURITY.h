#ifndef SECURITY_h
#define SECURITY_h

#if ARDUINO >= 100
	#include <Arduino.h>
#endif
#ifndef EEP_h
#include "EEP.h"
#endif

class SECURITY
{
public:
	SECURITY(EEP _eep);

	void validate(int a[4]);

	void registerAdmin(int a[4]);
	void registerUser(int a[4]);
	void registerFirst(int a[4]);

	bool isRegistered(int a[4]);
	
	void Activity();
	bool IsTimeframePassed();

	bool isAdmin();
	bool isAllowed();
	bool isFirst();
	bool hasChipBeenRead = false;

	bool registerNext(int uid[4]);
	String _next = "";

	// Login timer variables
	const unsigned long maxInactivityMillis = 15000; // 20 Seconds of invalidation timeframe

	unsigned long currentMillis;
	unsigned long lastActivityMillis;

private:
	EEP eep;
	const int userBytesAllocated = 6;
	
	int userCount;
	int current[6];


	bool _isActive = false;
	bool _isAdmin = false;
	bool _isAllowed = false;
	bool _isFirst = false;
};

#endif

