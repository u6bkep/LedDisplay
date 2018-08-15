
//#include <cstring.h>
#include <wiringPi.h>
#include <wiringShift.h>

#include <chrono>
#include <thread>

// Pascal Stang's 5x7 font library:
#include "font5x7.h"
#include "LedDisplayPi.h"

// version 4 was the avr code
#define VERSION_NUMBER 5;

using namespace std;

namespace LedDisplay
{
LedDisplay::LedDisplay(
	uint8_t _dataPin,
	uint8_t _registerSelect,
	uint8_t _clockPin,
	uint8_t _chipEnable,
	uint8_t _resetPin,
	uint8_t _displayLength
)
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

/*
 * 	print replacment
 */
void LedDisplay::printCharArray(
	uint8_t *charPtr
)
{
	int printLength = strlen((char*)charPtr);
	for(int index = 0; index < printLength; index++)
	{
		LedDisplay::write(*charPtr); 
		charPtr++;
	}
}

/*
 * 	Initialize the display.
 */
void LedDisplay::begin(
	void
)
{
	// initialise wiringPi
	// is this the best place to initialise this?
	wiringPiSetup () ;

	// set pin modes for connections:
	//NOTE(Ben): these funcion names and argumants are the same as used in wiringPi, thus there is probably no need to rewrite these.
	pinMode(dataPin, OUTPUT);
	pinMode(registerSelect, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(chipEnable, OUTPUT);
	pinMode(resetPin, OUTPUT);

	// reset the display:
	digitalWrite(resetPin, LOW);
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	digitalWrite(resetPin, HIGH);


	// load dot register with lows
	loadDotRegister(); //done: leave as is.

	// set control register 0 for max brightness, and no sleep:
	loadAllControlRegisters(0x7F);
}

/*
 * 	Clear the display
 */
//done: leave as is.
void LedDisplay::clear(
	void
)
{
	this->setString(stringBuffer);
	for (int displayPos = 0; displayPos < displayLength; displayPos++) {
		// put the character in the dot register:
		writeCharacter(' ', displayPos);
	}

	// send the dot register array out to the display:
	loadDotRegister();
}

/*
 * 	set the cursor to the home position (0)
 */
//done: leave as is.
void LedDisplay::home(
	void
)
{
	// set the cursor to the upper left corner:
	this->cursorPos = 0;
}

/*
 * 	set the cursor anywhere
 */
//done: leave as is.
void LedDisplay::setCursor(
	int whichPosition
)
{
	this->cursorPos = whichPosition;
}

/*
 * 	return the cursor position
 */
//done: leave as is.
int LedDisplay::getCursor(
	void
)
{
	return this->cursorPos;
}

/*
 * 	write a byte out to the display at the cursor position,
 *  and advance the cursor position.
 */
//done: leave as is.
size_t LedDisplay::write(
	uint8_t b
)
{
	// make sure cursorPos is on the display:
	if (cursorPos >= 0 && cursorPos < displayLength) {	
		// put the character into the dot register:
		writeCharacter(b, cursorPos);
		// put the character into the displayBuffer
		// but do not write the string constants pass
		// to us from the user by setString()
		if (this->displayString == stringBuffer && cursorPos < LEDDISPLAY_MAXCHARS) {
			stringBuffer[cursorPos] = b;
		}		
		cursorPos++;	
		// send the dot register array out to the display:
		loadDotRegister();
	}
}

/*
 * 	Scroll the displayString across the display.  left = -1, right = +1
 */
void LedDisplay::scroll(
	int direction
)
{
	cursorPos += direction;
	//  length of the string to display:
	int stringEnd = strlen(displayString);

	// Loop over the string and take displayLength characters to write to the display:
   	for (int displayPos = 0; displayPos < displayLength; displayPos++) {
		// which character in the strings you want:
		int whichCharacter =  displayPos - cursorPos;
		// which character you want to show from the string:
		char charToShow;
		// display the characters until you have no more:
		if ((whichCharacter >= 0) && (whichCharacter < stringEnd)) {
		charToShow = displayString[whichCharacter];
		} else {
		// if none of the above, show a space:
		charToShow = ' ';
		}
		// put the character in the dot register:
		writeCharacter(charToShow, displayPos);
	}
	// send the dot register array out to the display:
	loadDotRegister();
}

/*
 * 	set displayString
 */
//done: leave as is.
void LedDisplay::setString(
	const char * _displayString
)
{
	this->displayString = _displayString;
}

/*
 * 	return displayString
 */
//done: leave as is.
const char * LedDisplay::getString(
	void
)
{
	return displayString;
}

/*
 * 	return displayString length
 */
//done: leave as is.	
int LedDisplay::stringLength(
	void
)
{
	return strlen(displayString);
}	

/*
 * 	set brightness (0 - 15)
 */
//done: leave as is.	
void LedDisplay::setBrightness(
	uint8_t bright
)
{
    // Limit the brightness
    if (bright > 15) {
        bright = 15;
    }
  
    // set the brightness:
    loadAllControlRegisters(0x70 + bright);
}

/* this method loads bits into the dot register array. It doesn't
 * actually communicate with the display at all,
 * it just prepares the data:
*/

void LedDisplay::writeCharacter(
	char whatCharacter, 
	char whatPosition
)
{
  // calculate the starting position in the array.
  // every character has 5 columns made of 8 bits:
  char thisPosition =  whatPosition * 5;

  // copy the appropriate bits into the dot register array:
  for (int i = 0; i < 5; i++) {
    dotRegister[thisPosition+i] = (uint8_t) Font5x7[((whatCharacter - 0x20) * 5) + i];
  }
}

// This method sends 8 bits to one of the control registers:
void LedDisplay::loadControlRegister(
	uint8_t dataByte
)
{
  // select the control registers:
  digitalWrite(registerSelect, HIGH);
  // enable writing to the display:
  digitalWrite(chipEnable, LOW);
  // shift the data out:
  shiftOut(dataPin, clockPin, MSBFIRST, dataByte);
  //TODO: this should work now but would be better to replace with RPi spi HW
  // disable writing:
  digitalWrite(chipEnable, HIGH);
}



// This method sends 8 bits to the control registers in all chips:
//done: leave as is.
void LedDisplay::loadAllControlRegisters(
	uint8_t dataByte
)
{

  // Each display can have more than one control chip, and displays
  // can be daisy-chained into long strings. For some operations, such
  // as setting the brightness, we need to ensure that a single
  // control word reaches all displays simultaneously. We do this by
  // putting each chip into simultaneous mode - effectively coupling
  // all their data-in pins together. (See section "Serial/Simultaneous
  // Data Output D0" in datasheet.)


  // One chip drives four characters, so we compute the number of
  // chips by diving by four:
  int chip_count = displayLength / 4;

  // For each chip in the chain, write the control word that will put
  // it into simultaneous mode (seriel mode is the power-up default).
  for (int i = 0; i < chip_count; i++) {
    loadControlRegister(0x81);
  }

  // Load the specified value into the control register.
  loadControlRegister(dataByte);

  // Put all the chips back into serial mode. Because they're still
  // all in simultaneous mode, we only have to write this word once.
  loadControlRegister(0x80);
}


// this method originally sent 320 bits to the dot register: 12_30_09 ML
void LedDisplay::loadDotRegister(
	void
)
{

  // define max data to send, patch for 4 length displays by KaR]V[aN
  int maxData = displayLength * 5;

  // select the dot register:
  digitalWrite(registerSelect, LOW);
  // enable writing to the display:
  digitalWrite(chipEnable, LOW);
  // shift the data out:
  for (int i = 0; i < maxData; i++) {
	shiftOut(dataPin, clockPin, MSBFIRST, dotRegister[i]);
	//TODO: this should work now but would be better to replace with RPi spi HW
  }
  // disable writing:
  digitalWrite(chipEnable, HIGH);
}

/*
  version() returns the version of the library:
*/
int LedDisplay::version(
	void
)
{
  return VERSION_NUMBER;
}



}


