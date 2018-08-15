#include "LedDisplayPi.h"
#include <chrono>

// Define pins for the LED display.
// You can change these, just re-wire your board:
#define dataPin 6              // connects to the display's data in
#define registerSelect 7       // the display's register select pin
#define clockPin 8             // the display's clock pin
#define enable 9               // the display's chip enable pin
#define reset 10              // the display's reset pin

#define displayLength 8        // number of characters in the display

int main(void)
{
	char helloWorldArray[] = "Hello world!";
	// create am instance of the LED display library:
	LedDisplay::LedDisplay myDisplay = LedDisplay(dataPin, registerSelect, clockPin,
                         enable, reset, displayLength);
	int brightness = 15;        // screen brightness

	// initialize the display library:
	myDisplay.begin();
	// set the brightness of the display:
	myDisplay.setBrightness(brightness);

	while(1)
	{
		// set cursor to 0
		myDisplay.home();
		// print the time

		myDisplay.printCharArray((uint8_t*)helloWorldArray);
	}


	return 0;
}
