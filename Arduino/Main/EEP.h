#ifndef EEP_h
#define EEP_h

#if ARDUINO >= 100
	#include "Arduino.h"
#endif

class EEP
{
public:
	EEP();
	void Write(int a, int i);
	void Clear();
	int Read(int a);
	static const int memorySize = 80;
	byte memory[memorySize];
};

#endif