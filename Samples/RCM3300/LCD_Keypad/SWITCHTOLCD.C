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
/**********************************************************
	switchtolcd.c

	This program is used with RCM3300 series controllers
	with prototyping boards.

	An optional LCD/Keypad Module is required to run this
	program.  brdInit() in rcm33xx.lib automatically
   enables external I/O bus for LCD/Keypad operations.

	Description
	===========
	This sample program demonstrates the use of the external
	I/O bus with the controller using an LCD/Keypad Module.
	The program will light-up an LED and will display a message
	on the LCD when a switch press is detected.

	DS3 and DS4 LED's on the proto-board will also light-up
	on a switch press.


	Instructions
	============
	1. Compile and run this program.
	2. Press and hold S2 or S3 on the prototyping board to
	   see a message displayed on the LCD and light up an LED.
	   Here's a list of how the switches are connected.

	   Switch   Proto Board LED  Controller Keypad LED
	   ------   --------------  ---------------------
	    S2       DS3             DS6
	    S3       DS4             DS7

**********************************************************/
#class auto
#memmap xmem

#use rcm33xx.lib		//sample library to use with this application

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6
#define USERLED 0
#define S2  2
#define S3  3

#define ON	1
#define OFF 0

// Structure to hold font information
fontInfo fi6x8;

///////////////////////////////////////////////////////////////////////////

main()
{

	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------
	brdInit();				//initialize board for this demo

	dispInit();

	keypadDef();		// Use the default keypad ASCII return values

	glBackLight(1);	// Turn-on the backlight
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);			//	Initialize 6x8 font
	glBlankScreen();
	for(;;)
	{
		glPrintf (0,  0, &fi6x8,  "Waiting for a Switch");
		glPrintf (0,  8, &fi6x8,  "to be pressed on the");
		glPrintf (0,  16, &fi6x8, "prototyping board...");

		while (switchIn(S2) && (switchIn(S3)));
		glBlankScreen();
		if (!(switchIn(S2)))
		{
			glPrintf (0,  0, &fi6x8, "Switch 2 is Active!");
			dispLedOut(5, 1);   			//Turn-on Keypad LED DS1
			ledOut(DS3, ON);				//proto-board led on

			while (!(switchIn(S2)));
			dispLedOut(5, 0); 			// Turn-OFF Keypad LED DS1
			ledOut(DS3, OFF);				//proto-board led off
		}

		if (!(switchIn(S3)))
		{
			glPrintf (0,  0, &fi6x8, "Switch 3 is Active!");
			dispLedOut(6, 1);   			//Turn-on Keypad LED
			ledOut(DS4, ON);				//proto-board led on

			while (!(switchIn(S3)));
			dispLedOut(6, 0); 			// Turn-OFF Keypad LED
			ledOut(DS4, OFF);				//proto-board led off
		}

		glBlankScreen();
	}
}


