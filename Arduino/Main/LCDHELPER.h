#ifndef LCDHELPER_h
#define LCDHELPER_h

#if ARDUINO >= 100
	#include "arduino.h"
#endif
#include <LiquidCrystal.h>

class LCDHELPER
{
public:
	LCDHELPER();
	void test();
private:
	LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
};

#endif

