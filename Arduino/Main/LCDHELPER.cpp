#include "LCDHELPER.h"
#include <Arduino.h>

LCDHELPER::LCDHELPER()
{
	LiquidCrystal 
}

void LCDHELPER::test()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Hello");

}