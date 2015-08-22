/*******************************************************************************

	toggleswitch.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This program uses costatements to detect switches with press and release
	method debouncing.  Corresponding LEDs, DS2 and DS3, are turned on or off.

	Instructions
	============
	1. Compile and run this program.
	2. Press and release switch S2 to toggle LED DS3 between on and off.
	3. Press and release switch S3 to toggle LED DS4 between on and off.

*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define DS3 3
#define DS4 4

#define S2  2
#define S3  3

#define ON 1
#define OFF 0

#define DEBOUNCE_MILLISECONDS 25L

void main(void)
{
	auto int sw1, sw2, led1, led2;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	led1 = led2 = OFF;	// initialize LEDs to off value
	sw1 = sw2 = OFF;		// initialize switches to off value

	while (1) {
		costate {
			waitfor(!switchIn(S2));	// wait for switch S2 initial press detected
			waitfor(DelayMs(DEBOUNCE_MILLISECONDS));
			if (switchIn(S2)) abort;	// debounce: if S2 not still pressed, abort
			waitfor(switchIn(S2));	// wait for switch S2 initial release detected
			waitfor(DelayMs(DEBOUNCE_MILLISECONDS));
			if(!switchIn(S2)) abort;	// debounce: if S2 not still released, abort
			sw1 = !sw1;	// set switch on
		}

		costate {
			waitfor(!switchIn(S3));	// wait for switch S3 initial press detected
			waitfor(DelayMs(DEBOUNCE_MILLISECONDS));
			if (switchIn(S3)) abort;	// debounce: if S3 not still pressed, abort
			waitfor(switchIn(S3));	// wait for switch S3 initial release detected
			waitfor(DelayMs(DEBOUNCE_MILLISECONDS));
			if(!switchIn(S3)) abort;	// debounce: if S3 not still released, abort
			sw2 = !sw2;	// set switch on
		}

		costate {
			// toggle LED DS3 upon valid S2 press / release and clear switch
			waitfor(sw1);
			led1 = !led1;
			ledOut(DS3, led1);
			sw1 = !sw1;	// set switch off
		}

		costate {
			// toggle LED DS4 upon valid S3 press / release and clear switch
			waitfor(sw2);
			led2 = !led2;
			ledOut(DS4, led2);
			sw2 = !sw2;	// set switch off
		}
	}
}

