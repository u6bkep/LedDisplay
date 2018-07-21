// ensure this library description is only included once
#ifndef LedDisplay_h
#define LedDisplay_h

#include <cstring>
#include <cstdint>
//#include <iostream>
//#include <string>
//#include <stdlib.h>
//#include "Print.h"

// supports up to four 8 character displays, connected as documented here,
// under "Multiple Displays" http://playground.arduino.cc/Main/LedDisplay
#define LEDDISPLAY_MAXCHARS  32 //TODO: find a reason this is limited or remove the limitation

class LedDisplay {
  public:
    // constructor:
   LedDisplay(uint8_t _dataPin,
			  uint8_t _registerSelect,
			  uint8_t _clockPin,
			  uint8_t _chipEnable,
			  uint8_t _resetPin,
			  uint8_t _displayLength);


	// initializer method:
	void begin();

	void clear();						// clear the display
	void home();						// set cursor to far left hand position
	void setCursor(int whichPosition);	// set cursor to any position
	int  getCursor();					// get the cursor position
	size_t write(uint8_t b);    // write a character to the display and advance cursor
	void setString(const char* _stringToDisplay);	// set the displayString variable
	const char * getString();						// get the displayString
	int stringLength();							// get the length of displayString

	void scroll(int direction);			// scroll whatever string is stored in library's displayString variable

	void setBrightness(uint8_t bright);			// set display brightness, 0 - 15

	// Control register setters. for addressing the display directly:
	void loadControlRegister(uint8_t dataByte);
	void loadAllControlRegisters(uint8_t dataByte);
	void loadDotRegister();

    int version(void);							// return library version

  private:
  	// Character display setters:
	void writeCharacter(char whatCharacter, byte whatPosition);	// write a character to a buffer which will
																// be sent to the display by loadDotRegister()

	int cursorPos;				// position of the cursor
	uint8_t dotRegister[LEDDISPLAY_MAXCHARS * 5]; // 5 bytes per character * maxchars

	// Define pins for the LED display:
	uint8_t dataPin;         	// connects to the display's data in
	uint8_t registerSelect;   	// the display's register select pin
	uint8_t clockPin;         	// the display's clock pin
	uint8_t chipEnable;       	// the display's chip enable pin
	uint8_t resetPin;         	// the display's reset pin
	uint8_t displayLength;    	// number of bytes needed to pad the string
	char stringBuffer[LEDDISPLAY_MAXCHARS+1];  // buffer to hold initial display string
	const char * displayString;	// string for scrolling
};

#endif
