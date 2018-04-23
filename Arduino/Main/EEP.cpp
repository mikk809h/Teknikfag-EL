#include <EEPROM.h>
#include <Arduino.h>

#include "EEP.h"

// Constructor
EEP::EEP()
{
	for (int c = 0; c < memorySize; c++) {
		memory[c] = EEPROM.read(c);
	}
}

void EEP::Write(int a, int i)
{
	EEPROM.update(a, i);
	memory[a] = i;
}

int EEP::Read(int a)
{
	memory[a] = EEPROM.read(a);
	return memory[a];
}

void EEP::Clear()
{
	for (int c = 0; c < memorySize; c++) {
		memory[c] = 0;
		EEPROM.update(c, 0);
	}
}