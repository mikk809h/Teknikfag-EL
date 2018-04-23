/*
Note: Nogle kommentarer i filen her er oprindeligt på engelsk, 
		dog er der i udsnit til rapporten oversat til danks for lettere forståelse.

Navn:			BRAINIE.ino
Ejer:			Mikkel Andersen
Beskrivelse:
				This arduino project and code was made for a final exam.
				The project is about Alarm systems and how to make a civilian home more secure.
Afhængigheder:
				This project relies on custom libraries in order to function properly.
				Said libraries contain functions which make communication with used components easier.
*/

// Includes
#include <EEPROM.h>			// Includes the EEPROM library
#include <SoftwareSerial.h>	// Includes the SoftwareSerial library
#include <Key.h>			// Includes the Key library for the keypad
#include <Keypad.h>			// Includes the Keypad library
#include <SPI.h>			// Includes the SPI library (For RFID Reader)
#include <MFRC522.h>		// Includes the RFID reader library
#include <stdlib.h>			// Includes the standard C library
#include <LiquidCrystal.h>	// Includes the LCD library

#include "EEP.h"			// Includes the custom EEP library
#include "SECURITY.h"		// Includes the custom Security library
#include "COMMAND.h"		// Includes the custom Command library
#include "ALARM.h"			// Includes the custom alarm library


#define DEBUG true			// Should the program debug information?
#define DUMP_EEP true		// Should the program show the EEPROM data on start.

// Definer pins til RFID læseren
#define SS_PIN 8
#define RST_PIN 9

// LCD pins
const int	rs = 48, en = 49, d4 = 46, d5 = 47, d6 = 44, d7 = 45;

// Keypad opsætning
const byte keypadRows = 4;
const byte keypadCols = 3;
byte keypadRowPins[keypadRows] = { 24, 27, 26, 29 };
byte keypadColPins[keypadCols] = { 28, 31, 30 };
char keypadKeys[keypadRows][keypadCols] = {
	{ '1','2','3' },
	{ '4','5','6' },
	{ '7','8','9' },
	{ '*','0','#' }
};

// Biblioteksopsætning
Keypad keypad = Keypad(makeKeymap(keypadKeys), keypadRowPins, keypadColPins, keypadRows, keypadCols);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MFRC522 RFID522(SS_PIN, RST_PIN);
EEP eep = EEP();
SECURITY security = SECURITY(eep);
COMMAND command = COMMAND(eep);
ALARM alarm = ALARM(eep);

// Define danish characters.
uint8_t newChar0[8] = { B10000,B01000,B00100,B00010,B00001,B00010,B00100,B00000 };	// >
uint8_t Lae[8]		= { B00000,B00000,B11010,B00101,B01111,B10100,B11111,B00000 };	// æ
uint8_t Loe[8]		= { B00000,B00001,B01110,B10101,B10101,B01110,B10000,B00000 };	// ø
uint8_t Laa[8]		= { B00100,B00000,B01110,B00001,B01111,B10001,B01111,B00000 };	// å
uint8_t Sae[8]		= { B01111,B10100,B10100,B11110,B10100,B10100,B10111,B00000 };	// Æ
uint8_t Soe[8]		= { B00001,B01110,B10011,B10101,B11001,B01110,B10000,B00000 };	// Ø
uint8_t Saa[8]		= { B00100,B00000,B01110,B10001,B11111,B10001,B10001,B00000 };	// Å

void setup() {
	// Upload custom danish characters to LCD
	lcd.createChar(0, newChar0);	// >
	lcd.createChar(1, Lae);			// æ
	lcd.createChar(2, Loe);			// ø
	lcd.createChar(3, Laa);			// å
	lcd.createChar(4, Sae);			// Æ
	lcd.createChar(5, Soe);			// Ø
	lcd.createChar(6, Saa);			// Å

	// Start LCD and clear the screen
	lcd.begin(16, 4);		// Initialize LCD
	lcd.clear();			// Clear Screen

	// Open Serial Communication ports
	// -> With Computer (Debugging/Logs)
	// -> With PIC		(Alarm state)

	Serial.begin(9600);		// Open Serial Communication port with Computer at 9600 bps
	Serial1.begin(9600);	// Open Serial Communication port with PIC		at 9600 bps

	// Begin RFID Reader with SPI and RFID522
	SPI.begin();			// Initiate  SPI bus
	RFID522.PCD_Init();		// Initiate MFRC522    mfrc522.

	// Increase RFID Reader Range to MAX (To read through thin materials)
	RFID522.PCD_SetAntennaGain(RFID522.RxGain_max);

	// Dumping (showing) the current EEPROM on startup.
	#pragma region DUMP_EEP
		#if DUMP_EEP
			Serial.println("===================================");
			Serial.print("EEPROM_DUMP: (");
			Serial.println((String)eep.memorySize + " bytes)");
			for (int a = 0; a < eep.memorySize; a++) {
				if (eep.memory[a] < 10) {
					Serial.print("   ");
				}
				else if (eep.memory[a] >= 10 && eep.memory[a] < 100) {
					Serial.print("  ");
				}
				else if (eep.memory[a] >= 100 && eep.memory[a] <= 255) {
					Serial.print(" ");
				}
				Serial.print(eep.memory[a]);
				if ((a + 1) % 10 == 0 && (a + 1) != eep.memorySize) {
					Serial.println();
				}
				else {
					Serial.print(" ");
				}
			}
			Serial.println();
			Serial.println("=============================");
			Serial.println();


			//
			// Validate EEPROM data (If cleared between startups)
			//
			Serial.println("Validating memory... ");
			if (security.isFirst()) {
				Serial.println("	Memory empty. Register new Admin tag...");
			}
			else {
				Serial.println("	Memory contains configuration!");
				Serial.println();

				// Show all users and their IDs
				int userCount = eep.memory[0];
				Serial.print("Users: ");
				Serial.print(userCount);
				Serial.println();

				const int userBytesAllocated = 6;
				int userBytesSelected[userBytesAllocated];

				if (userCount > 0) {
					// Loop through with userIndex as the current user number (1,2,3,4,5,6,)
					for (int userIndex = 0; userIndex < userCount; userIndex++) {
						Serial.print("  ");
						String currentUserID = "";
						int userLocation = userIndex == 0 ? 1 : 1 + (userIndex * userBytesAllocated);
						for (int e = 0; e < userBytesAllocated; e++) {
							userBytesSelected[e] = eep.memory[userLocation + e];
							currentUserID.concat(userBytesSelected[e]);
							if (userBytesSelected[e] < 10) {
								currentUserID.concat("     ");
							}
							else if (userBytesSelected[e] >= 10 && userBytesSelected[e] < 100) {
								currentUserID.concat("    ");
							}
							else if (userBytesSelected[e] >= 100 && userBytesSelected[e] <= 255) {
								currentUserID.concat("   ");
							}
						}
						Serial.print(currentUserID);
						Serial.println();
					}
				}
			}
			Serial.println();
			Serial.println("==========================");
		#endif
	#pragma endregion

	Serial.println("Ready!");


	// End of setup()
	// Continue in loop()
}

bool setAlarmOn = false; // Boolean that keeps track of when the alarm should be activated
// This makes sure that no alarms have went off when the alarm is being activated.

unsigned int setAlarmOnTime = -1; // Keeps track of when the alarm has begun the activation "phase"

bool alarmIsTriggered = false; // Variable that states when to call the alarm company/police if an alarm has been triggered.

// Runs constantly
void loop()
{
	// Checks if alarm needs to run any more.
	// If true, alarm has been triggered and not deactivated in time.
	// If false, alarm is functional and listening to communication with PIC.
	if (alarmIsTriggered == false) {
		KeypadListener(); // Reads any key presses on the keypad
		AlarmListener();  // Reads Communication from PIC controller
		RFIDListener();	  // Reads RFID chip, if present
		AccountHandler(); // Handles current user (through last RFID scan)

		ActivateAlarm();  // Activation of alarm (A phase)
		Draw();			  // Prints data onto the LCD screen
	}
	else {
		// Prints a screen, showing a "last" phase of the alarm and what alarm was triggered.
		lcd.setCursor(0, 0);
		lcd.print("================");
		lcd.setCursor(0, 1);
		lcd.print("     ALARM      ");
		lcd.setCursor(0, 2);
		lcd.print("       ");
		lcd.setCursor(7, 2);
		lcd.print(alarm.getTriggeredAlarms());
		lcd.setCursor(0, 3);
		lcd.print("================");

		// TODO: Alarm police/Alarm company

		delay(5000); // Waits five seconds

		alarmIsTriggered = false; // Causes a "restart" of the loop, making the alarm ready for usage again.
		lcd.clear();
	}
}

// Definer den byte som læses
byte incomingByte;
void AlarmListener()
{
// Vent på et Serielt signal fra PIC16F688
	if (Serial1.available() > 0) {
// Aflæs data/byte
		incomingByte = (byte)Serial1.read();

// Brug AND for at få de to første bits og flyt disse to bits til starten
		int prefix = (incomingByte & B11000000) >> 6;

byte _1 = B10100000; // Definer byte 1

byte _2 = _1 >> 2;	 // Benyt "højre-shift" operator og flyt alt 2 bits til højre.
// byte _2 vil nu være B00101000


// Brug AND for at få de tre bits fra bit 2 og flyt disse tre bits til starten.
		int id = (incomingByte & B00111000) >> 3;

// Brug AND for at få de sidste tre bits
		int status = incomingByte & B00000111;

// Hvis præfikset er lig med den binære værdi for 2
		if (prefix == B10) {

// Opdater alarm status for ID'et
			alarm.set(id, status);
		}
	}
}

// Hvornår var den sidste chip læst?
long previousReadTime = 0;

void RFIDListener()
{
// Hvis Ingen chip er tæt på læseren, stop funktionen.
	if (!RFID522.PICC_IsNewCardPresent())
		return;
// Ellers, fortsæt

// Hvis programmet ikke kan læse chippen, stop funktionen.
	if (!RFID522.PICC_ReadCardSerial())
		return;
// Ellers, fortsæt

// Tjek hvis der er gået mere end ét sekund siden sidste indlæsning.
// Hvis tiden ikke er gået, stop funktionen.
	if (millis() - previousReadTime <= 1000)
		return;

// Sæt nuværende tid ind, for at holde styr 
// på hver indlæsning.
	previousReadTime = millis();

// Variabel til at gemme den indlæste chips UID
	int convertedUid[4];

// Gennemgå hver byte i chippens UID og 
// sæt ind i ovenstående variabel
	for (byte i = 0; i < RFID522.uid.size; i++) {
		convertedUid[i] = RFID522.uid.uidByte[i];
	}

// Skriv til computer og LCD at chippen bliver indlæst.
	Serial.println("Reading RFID Chip...");
	lcd.setCursor(0, 2);
	lcd.print("Afl");
	lcd.print(char(1));
	lcd.print("ser...");

// Tjek hvis denne indlæste chip skal
// registreres som første chip i systemet
	if (security.isFirst()) {
// Registrer denne chip i systemet og 
// skriv dette til LCD skærmen
		security.registerFirst(convertedUid);
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("Registreret");
	}
	else {
// Tjek hvis denne chip skal registreres og registrer den.
		if (security.registerNext(convertedUid)) {
// Chippen er nu registreret
			security.validate(convertedUid);
			lcd.clear();
			lcd.setCursor(0, 1);
			lcd.print("Registreret");
		}
		else {
// Hvis chippen ikke blev registreret, prøv da
// at tjekke hvis chippen har tilladelse til at 
// slå alarmen til eller fra.
			security.validate(convertedUid);

// Skriv chippens tilladelser gennem Serial.
			Serial.print("Administrator: ");
			Serial.println(security.isAdmin());

			Serial.print("Allowed: ");
			Serial.println(security.isAllowed());

			lcd.clear();

// Tjek hvis alarmen er tilkoblet, og hvis alarmen er gået i gang.
			if (alarm.status() == true && alarm.getTriggeredAlarms() != "") {
	// Hvis alarmen er gået i gang og tilkoblet,
	// skriv til LCD'en at der kræves en kode.
				lcd.setCursor(0, 2);
				lcd.print("Kode:");
			}
			else if (alarm.status() == true) {
	// Hvis alarmen er tilkoblet, men ikke gået i gang,
	// slå da alarmen fra
				alarm.toggle();
			}
			else if (alarm.status() == false) {
	// Hvis alarmen er frakoblet, tilkobl alarmen ved
	// at sætte variablerne setAlarmOn og setAlarmOnTime.
				setAlarmOn = true;
				setAlarmOnTime = millis();
			}
		}
	}
}

bool clearSetAlarmOn = false;
void ActivateAlarm()
{
	int curr = millis();
	if (setAlarmOn) {
		if (curr - setAlarmOnTime >= 1000) {
			if (alarm.getTriggeredAlarms() != "") {
				Serial.println("Cannot activate alarm. Alarm went off!");
				Serial.println(alarm.getTriggeredAlarms());
				lcd.setCursor(0, 1);
				lcd.print("ALARM: ");
				lcd.print(alarm.getTriggeredAlarms());
				lcd.setCursor(0, 2);
				lcd.setCursor(12, 0);
				lcd.print("Fejl");
			}
			else {
				Serial.println("Activating...");
				alarm.toggle();
			}
			setAlarmOn = false;
			clearSetAlarmOn = true;
		}
		else {
			Serial.println("Preparing activation...");
			lcd.setCursor(0, 0);
			lcd.print("Aktiverer...");
		}
	}
	if (clearSetAlarmOn) {
		if (curr - setAlarmOnTime >= 5000) {
			Serial.println("Cleared");
			lcd.setCursor(0, 0);
			lcd.print("                ");
			lcd.setCursor(0, 1);
			lcd.print("                ");
			clearSetAlarmOn = false;
		}
	}
}

// Definér en reset funktion som ved kørsel genstarter arduinoen.
void(*resetSystem) (void) = 0;

// Variabel for adgangskoden
String input = "";
// Variabel for adgangskoden, maskeret med *
String hiddenInput = "";

void KeypadListener() {
// Læs keypad for en værdi og fortsæt kun, hvis en karakter er blevet modtaget.
	char key = keypad.getKey();
	if (key != NO_KEY) {

	// Tjek hvis den nuværende bruger stadig er til stede.
		if (!security.IsTimeframePassed()) {
	// Flyt den tastede karakter ind i kommandohåndteringen
			command.SaveKey(key);
		// Tjek hvis alarmen er tilkoblet og om en alarm er aktiveret
			if (alarm.status() && alarm.getTriggeredAlarms() != "") {
		// Sørg for at kun at kræve adgangskode når alarmen har været i gang i under 25 sekunder
				if (alarm.deactivationTimeRemaining() > 0) {
		// Hvis karakteren er firkant, antag at koden er indtastet.
					if (key == '#') {
						lcd.setCursor(0, 2);
						lcd.print("                ");
		// Sammenlign den indtastede kode med "1234"
						if (input == "1234") {
		// Frakobl alarmen
							lcd.clear();
							lcd.print("Alarm frakoblet!");
							alarm.toggle();
							alarm.clearTrigger();
						}
		// Sæt input-værdierne til en tom streng.
						input = "";
						hiddenInput = "";
					} else {
		// Indsæt karakter i adgangskodevariablerne
						input.concat(key);
						hiddenInput.concat('*');
						lcd.setCursor(5, 2);
						lcd.print(hiddenInput);
						lcd.print(" ");
			}	}	}

		// Håndter nuværende indtastning som kommando
			String cmd = command.HandleCommand();
		// Hvis en kommando er indtastet, fortsæt 
			if (cmd != "") {
		// Tjek hvilke privilegier brugeren har
				if (security.isAdmin()) {
		// Hvis brugeren er administrator og kommandoen er CLEAR_EEPROM,
		// vil Arduinoen slette dens hukommelse.
					if (cmd == "CLEAR_EEPROM") {
						lcd.setCursor(0, 0);
						lcd.print("Clearing EEPROM");
						eep.Clear();
						delay(200);
						resetSystem();
					} else if (cmd == "REGISTER_ADMIN") {
		// Registrér næste chip som administrator
						security._next = "Admin";
					} else if (cmd == "REGISTER_USER") {
		// Registrér næste chip som bruger
						security._next = "User";
				}	}
		// Hvis brugeren er tilladt, skift alarmens status
				if (security.isAllowed()) {
					if (cmd == "TOGGLE_ALARM") {
						if (alarm.status()) {
							alarm.toggle();
						} else {
							setAlarmOn = true;
							setAlarmOnTime = millis();
			}	}	}	}
		// Hvis kommandoen var RESET, genstart arduinoen.
			if (cmd == "RESET") {
				delay(200);
				resetSystem();
			}
		// Opdater den sidste aktivitet for den nuværende bruger
			security.Activity();
}	}	}


// Hold styr på om der er blevet skrevet hvis brugerens tid er udløbet
bool printedLogOut = false;

// Hold styr på hvilken chip som sidst var indlæst.
void AccountHandler()
{
// Tjek hvis den sidste chip som var indlæst og har tilladelse
	if (security.isAllowed()) {
	// Tjek hvis brugeren ikke har haft aktivitet indenfor grænsen.
		unsigned long lclTime = (security.currentMillis - security.lastActivityMillis);
		if (lclTime >= security.maxInactivityMillis && lclTime <= security.maxInactivityMillis + 1500) {
		// Skriv "Logget ud" på LCD skærmen og sæt variablen printedLogOut til true
			lcd.setCursor(0, 2);
			lcd.print("Logget ud        ");
			printedLogOut = true;
		}
		else if (lclTime >= security.maxInactivityMillis + 1500) {
		// Hvis der er gået 1,5 sekund mere end da brugeren blev logget ud, tjek hvis der blev skrevet "Logget ud" 
			if (printedLogOut) {
			// Hvis ja, clear denne linje på LCD'en
				lcd.setCursor(0, 2);
				lcd.print("                  ");
				printedLogOut = false; // Sæt variablen tilbage til den oprindelige tilstand
			}
		}
	}
}


bool Alarm_shown = false; // Er der vist en alarm på skærmen?

const unsigned long alarm_period = 1000; // Alarm blinkefrekvens
unsigned long alarm_now = millis(); // Nuværende tid
unsigned long alarm_begin = millis(); // Starttid

void Draw()
{
// Tjek hvis der er en bruger som stadig logget ind.
	if (!security.isAllowed()) {
	// Skriv "Fremvis nøgle" på LCD
		lcd.setCursor(0, 0);
		lcd.print("Fremvis n");
		lcd.print(char(2));
		lcd.print("gle");
	}

// Skriv alarmens status nederst på skærmen.
	lcd.setCursor(0, 3);
	lcd.print("Alarm ");
	lcd.print(alarm.status() ? "tilkoblet" : "frakoblet");

	// Hvis ingen alarmer er gået i gang, tjek hvis der skal vises 
	// om den næste chip skal registreres eller ej.
	if (alarm.getTriggeredAlarms() == "") {
		// Hvis ingen brugere er konfigurerede, skriv at der skal oprettes en administrator
		if (security.isFirst()) {
			lcd.setCursor(0, 0);
			lcd.print("Opret admin.");
		}// Hvis næste bruger skal registreres, skriv at der skal oprettes en administrator
		else if (security._next == "ADMIN") {
			lcd.setCursor(0, 0);
			lcd.print("Opret admin.");
		}// Hvis næste bruger skal registreres, skriv at der skal oprettes en bruger
		else if (security._next == "USER") {
			lcd.setCursor(0, 0);
			lcd.print("Opret bruger");
		}
	}
	else {
		// Hvis alarmen er tilkoblet
		if (alarm.status()) {
			// Hvis alarmen er gået og tiden til at deaktivere den er større end -1
			if (alarm.deactivationTimeRemaining() > -1) {
			// Vis hvor lang tid der er tilbage før alarmen går i gang
				lcd.setCursor(14, 0);
				if (alarm.deactivationTimeRemaining() < 10 && alarm.deactivationTimeRemaining() > -1)
					lcd.print(" ");

				lcd.print(alarm.deactivationTimeRemaining());

				if (alarm.deactivationTimeRemaining() == 0) {
				// Sæt alarmen i gang hvis tiden er udløbet
					alarmIsTriggered = true;
				}
			}

			// Hold styr på den nuværende tid
			alarm_now = millis();
			// blink "ALARM " hver alarm_period på LCD'en.
			if (alarm_now - alarm_begin >= alarm_period) {
			// Hvis en bruger er logget ind, spørg om en kode.
				if (security.isAllowed()) {
					lcd.setCursor(0, 2);
					lcd.print("Kode:");
				}
			// Hvis ALARM allerede er vist på LCD'en, fjern teksten.
				if (Alarm_shown) {
					lcd.setCursor(4, 1);
					lcd.write("        ");
				}
				else {
				// Ellers hvis den på skærmen, samt den alarm/de alarmer som er gået.
					lcd.setCursor(4, 1);
					lcd.write("ALARM ");
					lcd.print(alarm.getTriggeredAlarms());
					lcd.print("    ");
				}
				// Opdater variablen som holder styr på om der er vist alarm eller ej
				Alarm_shown = !Alarm_shown;
				// Opdater begyndelsestidspunktet på alarmens blink.
				alarm_begin = millis();
			}
		}
	}

	// Hvis alarmen er tilkoblet og ingen alarm er gået i gang
	if (alarm.status() && alarm.getTriggeredAlarms() == "") {
		// fjern hvad der står på 1. linje (linje 2 i et nulsystem)
		lcd.setCursor(0, 1);
		lcd.print("              ");
	}
}