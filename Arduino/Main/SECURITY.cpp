#include "SECURITY.h"
#include <Arduino.h>

SECURITY::SECURITY(EEP _eep)
{
	eep = _eep;

	// Check userCount
	userCount = eep.memory[0];
	if (userCount == 0) {
		_isFirst = true;
		return;
	}
	
	// Validate all memory bytes //
	int a = 0;
	for (int i = 0; i < eep.memorySize; i++) {
		a = a + eep.memory[i];
	}
	// If all memory bytes are added, equals 0, then this is the first user.
	if (a == 0) {
		_isFirst = true;
	}
}

/*
 * Validates UID through saved EEPROM.
 */
void SECURITY::validate(int a[4])
{
	hasChipBeenRead = true;
	// Put known data into current array (for temporary reads)
	for (unsigned int i = 0; i < sizeof(a); i++) {
		current[i] = a[i];
	}

	// Read EEPROM data and find data based on this one
	// Find amount of users:
	// FIRST BYTE is from 0-255 (Amount of users) max: 16, i guess?
	userCount = eep.memory[0];

	bool wasFound = false;
	_isAdmin = false;
	_isAllowed = false;
	
	const int userBytesAllocated = 6;
	int userBytesSelected[userBytesAllocated];
	
	if (userCount > 0) {
		// Loop through with userIndex as the current user number (1,2,3,4,5,6,)
		for (int userIndex = 0; userIndex < userCount; userIndex++) {

			int userLocation = userIndex == 0 ? 1 : 1 + (userIndex * userBytesAllocated);
			for (int e = 0; e < userBytesAllocated; e++) {
				userBytesSelected[e] = eep.memory[userLocation + e];
			}

			// Check if UID matches stored UID
			if (userBytesSelected[0] == a[0] && userBytesSelected[1] == a[1] && userBytesSelected[2] == a[2] && userBytesSelected[3] == a[3]) {
				wasFound = true;
				break;
			}
		}
	}

	if (wasFound == true) {
		this->Activity();
		if (userBytesSelected[4] == 1) {
			_isAdmin = true;
		}
		if (userBytesSelected[5] == 1) {
			_isAllowed = true;
		}
	}

}

void SECURITY::registerFirst(int a[4])
{
	hasChipBeenRead = true;
	_isFirst = false;
	_isAdmin = true;
	_isAllowed = true;
	_isActive = true;

	this->Activity();

	int location = 1;
	for (int i = 0; i < 4; i++) {
		eep.Write(location + i, a[i]);
	}
	eep.Write(location + 4, 1); // Administrator? (1=yes,0=no)
	eep.Write(location + 5, 1); // Allowed? (1=yes,0=no)

	userCount++; // Add one to userCount
	eep.Write(0, userCount); // Write int 1 to address 0x0 (Usercount)
	
}

void SECURITY::registerAdmin(int a[4])
{
	hasChipBeenRead = true;
	this->Activity();
	int location = userCount == 0 ? 1 : 1 + (userCount * userBytesAllocated);
	for (int i = 0; i < 4; i++) {
		eep.Write(location + i, a[i]);
	}
	
	eep.Write(location + 4, 1); // Administrator? (1=yes,0=no)
	eep.Write(location + 5, 1); // Allowed? (1=yes,0=no)

	userCount++; // Add one to userCount
	eep.Write(0, userCount); // Write int 1 to address 0x0 (Usercount)
}

void SECURITY::registerUser(int a[4])
{
	hasChipBeenRead = true;
	this->Activity();
	int location = userCount == 0 ? 1 : 1 + (userCount * userBytesAllocated);

	for (int i = 0; i < 4; i++) {
		eep.Write(location + i, a[i]);
	}

	eep.Write(location + 4, 0); // Administrator? (1=yes,0=no)
	eep.Write(location + 5, 1); // Allowed? (1=yes,0=no)

	userCount++; // Add one to userCount
	eep.Write(0, userCount); // Write int 1 to address 0x0 (Usercount)
}

bool SECURITY::isRegistered(int a[4])
{
	hasChipBeenRead = true;
	userCount = eep.memory[0];

	const int userBytesAllocated = 6;
	if (userCount > 0) {
		// Loop through with userIndex as the current user number (1,2,3,4,5,6,)
		for (int userIndex = 0; userIndex < userCount; userIndex++) {

			int checkBytes[userBytesAllocated];

			int userLocation = userIndex == 0 ? 1 : 1 + (userIndex * userBytesAllocated);
			for (int e = 0; e < userBytesAllocated; e++) {
				checkBytes[e] = eep.memory[userLocation + e];
			}

			// Check if UID matches stored UID
			if (checkBytes[0] == a[0] && checkBytes[1] == a[1] && checkBytes[2] == a[2] && checkBytes[3] == a[3]) {
				return true;
			}
		}
	}
	return false;
}

void SECURITY::Activity()
{
	lastActivityMillis = millis();
}

bool SECURITY::IsTimeframePassed()
{
	currentMillis = millis();

	unsigned long lclTime = (currentMillis - lastActivityMillis);

	if (lclTime >= maxInactivityMillis) {
		return true;
	}

	return hasChipBeenRead ? false : true;
}

bool SECURITY::registerNext(int uid[4])
{
	if (isRegistered(uid)) {
		return false;
	}
	if (_next == "User") {
		_next = "";
		registerUser(uid);
		return true;
	}
	else if (_next == "Admin") {
		_next = "";
		registerAdmin(uid);
		return true;
	}
	return false;
}

bool SECURITY::isAdmin()
{
	return _isAdmin;
}

bool SECURITY::isAllowed()
{
	return _isAllowed;
}

bool SECURITY::isFirst()
{
	return _isFirst;
}