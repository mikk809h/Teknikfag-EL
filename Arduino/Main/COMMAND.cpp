#include "COMMAND.h"

COMMAND::COMMAND(EEP _eep)
{
	eep = _eep;
}

void COMMAND::SaveKey(char key)
{
	if (key == COMMANDKEY) {
		if (isCommand == false) {
			isCommand = true;
			Clear();
		}
		else {
			isCommand = false;
			isCommandEntered = true;
		}
	}
	else {
		currentKeyChain[currentKeyChainIndex] = key;
		currentKeyChainIndex++;
	}

}

String COMMAND::isInCommand()
{
	return (isCommand ? "In command" : "Not in command");
}

String COMMAND::HandleCommand()
{
	if (isCommandEntered == true) {
		String command = this->GetKeyChain();
		isCommandEntered = false;
		Clear();
		if (command == "0000")
		{
			return "CLEAR_EEPROM";
		}
		else if (command == "0999")
		{
			return "REGISTER_ADMIN";
		}
		else if (command == "0099")
		{
			return "REGISTER_USER";
		}
		else if (command == "0001")
		{
			return "RESET";
		}
		else if (command == "1")
		{
			return "TOGGLE_ALARM";
		}
	}
	return "";
}

void COMMAND::Clear()
{
	for (unsigned int i = 0; i < sizeof(currentKeyChain); i++) {
		currentKeyChain[i] = 0;
	}
	currentKeyChainIndex = 0;
}

String COMMAND::GetKeyChain()
{
	String out = "";
	for (unsigned int i = 0; i < sizeof(currentKeyChain); i++) {
		if (currentKeyChain[i] != 0) {
			out.concat(currentKeyChain[i]);
		}
	}
	return out;
}