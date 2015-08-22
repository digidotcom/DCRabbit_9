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

	This program is used with RCM3200 series controllers
	with prototyping boards.

	The sample library, rcm3200.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	An optional LCD/Keypad Module is required to run this
	program.  #define PORTA_AUX_IO at top of program to
	enable external I/O bus for LCD/Keypad operations.

	Description
	===========
	This sample program demonstrates the use of the external
	I/O bus with the controller using an LCD/Keypad Module.
	The program will light-up an LED and will display a message
	on the LCD when a switch press is detected.

	DS1 and DS2 LED's on the proto-board will also light-up
	on a switch press.

	Instructions
	============
	1. Compile and run this program.
	2. Press and hold DS1 or DS2 on the prototyping board to
	   see a message displayed on the LCD and light up an LED.
	   Here's a list of how the switches are connected.

	   Switch   Proto Board LED  Controller Keypad LED
	   ------   --------------  ---------------------
	    S2       DS1             DS6
	    S3       DS2             DS7

**********************************************************/
#class auto 			// Change default storage class for local variables: on the stack

#define PORTA_AUX_IO		//required to run LCD/Keypad for this demo
#memmap xmem
#use rcm3200.lib    		//sample library used for this demo

#define DS1 6		//led, port G bit 6
#define DS2 7		//led, port G bit 7
#define S2  1		//switch, port G bit 1
#define S3  0		//switch, port G bit 0

// Structure to hold font information
fontInfo fi6x8;

///////////////////////////////////////////////////////////////////////////

void main()
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

		while (BitRdPortI(PGDR, S2) && BitRdPortI(PGDR, S3));
		glBlankScreen();
		if (!BitRdPortI(PGDR, S2))
		{
			glPrintf (0,  0, &fi6x8, "Switch 2 is Active!");
			dispLedOut(5, 1);   									//Turn-on Keypad LED DS1
			BitWrPortI(PGDR, &PGDRShadow, 0, DS1);		//proto-board DS1 on

			while (!BitRdPortI(PGDR, S2));
			dispLedOut(5, 0); 	// Turn-OFF Keypad LED DS1
			BitWrPortI(PGDR, &PGDRShadow, 1, DS1);		//proto-board DS1 off
		}
		if (!BitRdPortI(PGDR, S3))
		{
			glPrintf (0,  0, &fi6x8, "Switch 3 is Active!");
			dispLedOut(6, 1); 										//Turn-on Keypad LED DS2
			BitWrPortI(PGDR, &PGDRShadow, 0, DS2);		//proto-board DS2 on

			while (!BitRdPortI(PGDR, S3));
			dispLedOut(6, 0); 	// Turn-OFF Keypad LED DS2
			BitWrPortI(PGDR, &PGDRShadow, 1, DS2);		//proto-board DS2 off
		}
		glBlankScreen();
	}

}
