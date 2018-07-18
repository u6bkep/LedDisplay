#include "LedDisplayPi.h"

// Pascal Stang's 5x7 font library:
#include "font5x7.h"


LedDisplay::LedDisplay(uint8_t _dataPin,
					   uint8_t _registerSelect,
					   uint8_t _clockPin,
					   uint8_t _chipEnable,
					   uint8_t _resetPin,
					   uint8_t _displayLength)
{
	// Define pins for the LED display:
	this->dataPin = _dataPin;         			// connects to the display's data in
	this->registerSelect = _registerSelect;   	// the display's register select pin
	this->clockPin = _clockPin;        			// the display's clock pin
	this->chipEnable = _chipEnable;       		// the display's chip enable pin
	this->resetPin = _resetPin;         		// the display's reset pin
	this->displayLength = _displayLength;    	// number of bytes needed to pad the string
	this->cursorPos = 0;						// position of the cursor in the display

	// do not allow a long multiple display to use more than LEDDISPLAY_MAXCHARS
	if (_displayLength > LEDDISPLAY_MAXCHARS) {
		_displayLength = LEDDISPLAY_MAXCHARS;
	}

	// fill stringBuffer with spaces, and a trailing 0:
	for (unsigned int i = 0; i < sizeof(stringBuffer); i++) {
		stringBuffer[i] = ' ';
	}
	
	this->setString(stringBuffer);				// give displayString a default buffer
}