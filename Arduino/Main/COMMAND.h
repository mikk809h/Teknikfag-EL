#ifndef COMMAND_h
#define COMMAND_h

#if ARDUINO >= 100
	#include <Arduino.h>
#endif
#ifndef EEP_h
#include "EEP.h"
#endif

class COMMAND
{
public:
	COMMAND(EEP _eep);

	void SaveKey(char key);
	void Clear();
	String isInCommand();
	String HandleCommand();

	String GetKeyChain();
	
private:
	EEP eep;
	char currentKeyChain[16];
	int currentKeyChainIndex = 0;
	bool isCommand = false;
	bool isCommandEntered = false;
	const char COMMANDKEY = '*';
};


#endif

