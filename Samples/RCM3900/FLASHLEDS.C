/*******************************************************************************
	flashleds.c
	Rabbit, 2007

	This sample program is used with RCM3900 series controllers and RCM3300
	and RCM3300 prototyping boards.

	Description
	===========
	This simple program demonstrates flashing	the following LEDs on and off:
		The User LED on the RCM39xx board;
		DS3, DS4, DS5 and DS6 on the prototyping board.


	Instructions
	============
	1. Compile and run this program.
	2. DS3, DS4, DS5, DS6 and User LEDs will flash on/off at individual rates.

*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6

#define USERLED 0

#define ON 1
#define OFF 0

void main(void)
{
	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	// use costates to "simultaneously" flash the LEDs at individual rates
	for (;;) {
		costate {
			ledOut(DS3, ON);
			waitfor(DelayMs(300L));
			ledOut(DS3, OFF);
			waitfor(DelayMs(600L));
		}

		costate {
			ledOut(DS4, ON);
			waitfor(DelayMs(400L));
			ledOut(DS4, OFF);
			waitfor(DelayMs(550L));
		}

		costate {
			ledOut(DS5, ON);
			waitfor(DelayMs(500L));
			ledOut(DS5, OFF);
			waitfor(DelayMs(500L));
		}

		costate {
			ledOut(DS6, ON);
			waitfor(DelayMs(600L));
			ledOut(DS6, OFF);
			waitfor(DelayMs(450L));
		}

		costate {
			ledOut(USERLED, ON);
			waitfor(DelayMs(700L));
			ledOut(USERLED, OFF);
			waitfor(DelayMs(400L));
		}
	}
}