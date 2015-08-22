/*******************************************************************************
	switchchar.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This sample program demonstrates lighting LEDs and transmission and reception
	of ASCII strings via RS-232 communication between serial ports E and F in
	response to debounced switch S2 and / or S3 press and release cycles.  The
	ASCII strings communicated between the serial ports are displayed in the
	STDIO window.

	Proto-Board Connections
	=======================
	Make the following connections on the prototyping board's RS-232 connector:
		TXF <---> RXE
		RXF <---> TXE

	Instructions
	============
	1. Compile and run this sample program in debug mode.

	2. Press and release switches S2 and / or S3 on the prototyping board.

	3. Observe the prototyping board and the STDIO window to see the LEDs that
	   are lit and the messages that printed in response to the switch presses.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6

#define S2  2
#define S3  3

#define ON	1
#define OFF 0

#define BAUD_RATE 19200L
/*
	The serial input and output buffers sizes are defined here. If these are not
	defined to be (2^n)-1, where n = 1...15, or they are not defined at all, they
	will default to 31 and a compiler warning will be displayed.
*/
#define FINBUFSIZE 	255
#define FOUTBUFSIZE 	255
#define EINBUFSIZE 	255
#define EOUTBUFSIZE  255

const char * const string1 = "This message sent via serial ports E --> F.\n\r";
const char * const string2 = "This message sent via serial ports F --> E.\n\r";

void main(void)
{
	// auto variables in main() permanently consume precious stack space
	static char buffer1[64], buffer2[64];	// buffers used for serial data
	static int ch1, ch2, i1, i2;
	static int led1, led2, sw1, sw2;


	//---------------------------------------------------------------------
	//	initialize the controller
	//---------------------------------------------------------------------

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	led1 = led2 = 1;	// initialize LEDs to off value
	sw1 = sw2 = 0;		// initialize switch press+release flags to false value

	// initialize serial port E, set baud rate
	serEopen(BAUD_RATE);
	serEwrFlush();
	serErdFlush();

	// initialize serial port F, set baud rate
	serFopen(BAUD_RATE);
	serFwrFlush();
	serFrdFlush();

	printf("Start of Sample Program . . .\n\n");

	//---------------------------------------------------------------------
	// continuous loop, periodically lighting LEDs and transmitting data
	//  between serial ports E and F
	//---------------------------------------------------------------------
	while (1) {

		// S2 switch monitor task
		costate {
			waitfor (!switchIn(S2));	// wait for switch S2 press
			waitfor(DelayMs(50));
			if (switchIn(S2)) {
				// S2 pressed time is too short, ignore this switch press
				abort;
			}
			waitfor (switchIn(S2));	// wait for switch S2 release
			sw1 = !sw1;	// flag the valid S2 press+release
		}

		// S3 switch monitor task
		costate {
			waitfor (!switchIn(S3));	// wait for switch S3 press
			waitfor(DelayMs(50));
			if (switchIn(S3)) {
				// S3 pressed time is too short, ignore this switch press
				abort;
			}
			waitfor (switchIn(S3));	// wait for switch S3 release
			sw2 = !sw2;	// flag the valid S3 press+release
		}

		// sw1 press / release monitor task
		costate {
			if (sw1) {
				ledOut(DS3, ON);	// turn on our LED
				sw1 = !sw1;			// return press+release flag to false value
				memset(buffer1, 0x00, sizeof(buffer1));	// clear our buffer
				// transmit an ASCII string from serial port E to serial port F
				serEputs(string1);
				// get the data that was just transmitted from serial port E
				i1 = 0;
				while (sizeof(buffer1) - 2 > i1 && (ch1 = serFgetc()) != '\r') {
					// copy only valid received characters to the buffer
					if (-1 == ch1) {
						yield;
					} else {
						buffer1[i1++] = ch1;
					}
				}
				buffer1[i1] = '\r';	// copy '\r' to our data buffer
				printf(buffer1);		// display the received ASCII string
				ledOut(DS3, OFF);		// turn off our LED
			}
		}

		// sw2 press / release monitor task
		costate {
			if (sw2) {
				ledOut(DS4, ON);	// turn on our LED
				sw2 = !sw2;			// return press+release flag to false value
				memset(buffer2, 0x00, sizeof(buffer2));	// clear our buffer
				// transmit an ASCII string from serial port F to serial port E
				serFputs(string2);
				// get the data that was just transmitted from serial port F
				i2 = 0;
				while (sizeof(buffer2) - 2 > i2 && (ch2 = serEgetc()) != '\r') {
					// copy only valid received characters to the buffer
					if (-1 == ch2) {
						yield;
					} else {
						buffer2[i2++] = ch2;
					}
				}
				buffer2[i2] = '\r';	// copy '\r' to our data buffer
				printf(buffer2);		// display the received ASCII string
				ledOut(DS4, OFF);		// turn off our LED
			}
		}

	}
}

