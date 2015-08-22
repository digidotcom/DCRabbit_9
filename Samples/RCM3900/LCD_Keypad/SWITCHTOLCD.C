/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/*******************************************************************************
	switchtolcd.c

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	An optional LCD/Keypad Module is required to run this program.  The brdInit()
	function in rcm33xx.lib enables the external I/O bus for LCD/Keypad
	operations.

	Description
	===========
	This sample program demonstrates the use of the external I/O bus with the
	controller using an LCD/Keypad Module.  When a switch press is detected the
	program lights up an LED on the prototyping board and on the LCD/Keypad
	Module and displays a message on the LCD.

	Instructions
	============
	1. Compile and run this program.
	2. Press and hold S2 or S3 on the prototyping board to see a message
	   displayed on the LCD and to light up a couple LEDs.  Here's the list of
	   how the switches are connected:

	     Switch    Proto-Board LED   Controller Keypad LED
	     ------    ---------------   ---------------------
	       S2        DS3               DS6
	       S3        DS4               DS7
*******************************************************************************/

#class auto
#memmap xmem
#use rcm39xx.lib	// sample library to use with this sample program

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6
#define USERLED 0

#define S2  2
#define S3  3

#define ON	1
#define OFF 0

// structure to hold font information
fontInfo fi6x8;

void main(void)
{
	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	dispInit();
	keypadDef();		// use the default keypad ASCII return values
	glBackLight(ON);	// turn on the LCD's backlight

	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);		// initialize 6x8 font

	for (;;) {
		glBlankScreen();
		glPrintf(0, 0 * LCD_YS / 3, &fi6x8, "Waiting for a switch");
		glPrintf(0, 1 * LCD_YS / 3, &fi6x8, "to be pressed on the");
		glPrintf(0, 2 * LCD_YS / 3, &fi6x8, "prototyping board...");

		while (switchIn(S2) && switchIn(S3));
		glBlankScreen();
		if (!switchIn(S2)) {
			glPrintf(0, 1 * LCD_YS / 3, &fi6x8, "Switch S2 is active!");
			dispLedOut(5, ON);			// turn on keypad LED DS6
			ledOut(DS3, ON);				// turn on proto-board LED DS3

			while (!switchIn(S2));
			dispLedOut(5, OFF);			// turn off keypad LED DS6
			ledOut(DS3, OFF);				// turn off proto-board LED DS3
		}

		if (!switchIn(S3)) {
			glPrintf(0, 2 * LCD_YS / 3, &fi6x8, "Switch S3 is active!");
			dispLedOut(6, ON);			// turn on keypad LED DS7
			ledOut(DS4, ON);				// turn on proto-board LED DS4

			while (!switchIn(S3));
			dispLedOut(6, OFF);			// turn off keypad LED DS7
			ledOut(DS4, OFF);				// turn off proto-board LED DS4
		}
	}
}


