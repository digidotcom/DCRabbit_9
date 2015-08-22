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

	This program is used with RCM3600 series controllers
	with prototyping boards.

	An optional LCD/Keypad Module is required to run this
	program.  #define PORTA_AUX_IO at top of program to
	enable external I/O bus for LCD/Keypad operations.

	Description
	===========
	This sample program demonstrates the use of the external
	I/O bus with the controller using an LCD/Keypad Module.
	The program will light-up an LED and will display a message
	on the LCD when an S1 switch press is detected.

	DS1 LED on the proto-board will also light-up
	on a switch press.

   Note:  Port pin PB7 is connected as both switch S2 and
   external I/O bus on the proto-board.  S2 should not be used
   with LCD/Keypad operations.


	Instructions
	============
	1. Compile and run this program.
	2. Press and hold S1 on the prototyping board to
	   see a message displayed on the LCD and light up an LED.
	   Here's a list of how the switches are connected.

	   Switch   Proto Board LED  Controller Keypad LED
	   ------   --------------  ---------------------
	    S1       DS1             DS6

**********************************************************/
#class auto
#define PORTA_AUX_IO		//required to run LCD/Keypad for this demo
#memmap xmem

#define DS1(x) (BitWrPortI(PFDR, &PFDRShadow, x, 6))	//led, port F bit 6
#define DS2(x) (BitWrPortI(PFDR, &PFDRShadow, x, 7))	//led, port F bit 7
#define S1  (BitRdPortI(PFDR, 4))							//switch, port F bit 4
#define ON	0
#define OFF 1

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

		while (S1);
		glBlankScreen();
		if (!S1)
		{
			glPrintf (0,  0, &fi6x8, "Switch 1 is Active!");
			dispLedOut(5, 1);   			//Turn-on Keypad LED DS1
			DS1(ON);							//proto-board DS1 on

			while (!S1);
			dispLedOut(5, 0); 			// Turn-OFF Keypad LED DS1
			DS1(OFF);						//proto-board DS1 off
		}
		glBlankScreen();
	}
}


