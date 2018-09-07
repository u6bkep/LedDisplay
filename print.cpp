#include "LedDisplayPi.h"
#include <chrono>
#include <thread>
#include <cstdio>

// Define pins for the LED display.
// You can change these, just re-wire your board:
#define dataPin 21              // connects to the display's data in
#define registerSelect 22       // the display's register select pin
#define clockPin 23             // the display's clock pin
#define enable 24               // the display's chip enable pin
#define reset 25              // the display's reset pin

#define displayLength 16        // number of characters in the display

using namespace LedDisplaynstest;

int main(void)
{
	std::string helloWorldstring("Hello world!");
	// create am instance of the LED display library:

	LedDisplay myDisplay(dataPin, registerSelect, clockPin, enable, reset, displayLength);

	int brightness = 15;        // screen brightness
	int fade = -1;
	// initialize the display library:
	myDisplay.begin();
	// set the brightness of the display:
	myDisplay.setBrightness(brightness);

	while(1)
	{
		// set cursor to 0
		myDisplay.home();
		// print the time

		myDisplay.printf("%s",helloWorldstring.c_str());
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		brightness += fade;
		if ( brightness <= 5 )
		{
			fade = 1;
		}
		else if ( brightness >= 15 )
		{
			fade = -1;
		}
		myDisplay.setBrightness(brightness);
	}


	return 0;
}
