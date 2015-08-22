/**************************************************************************
	switches.c

   Z-World, 2001
	This sample program is for the OP6800 series controllers.

	This sample program demonstrates the use of the OP6800 Demo board
	with the controller. The program will turn-on a LED and display a
	message on the LCD when a switch press is detected.

	1. Connect a +12v power source to the OP6800 Demo Board connector
	   J5 as follows:

	   PS     	OP6800 Demo Board
	   --			-----------------
	   +12v		+RAW J5, pin 2
	   GND      GND  J5, pin 1

	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press one of OP6800 Demo Board switches SW1 - SW4 (continuously) to
	   see a message displayed on the LCD and light-up a LED. Here's a list
	   of how the switches are connected.

	   Switch      Digital Input   Demo Board LED  Controller Keypad LED
	   ------		-------------   --------------  ---------------------
	    SW1         IN00           DS4             DS4
	    SW2         IN01           DS3             DS3
	    SW3         IN02           DS2             DS2
	    SW4         IN03           DS1             DS1

**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.
#memmap xmem  // Required to reduce root memory usage

// Structure to hold font information
fontInfo fi6x8;

///////////////////////////////////////////////////////////////////////////

void main()
{

	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------
	brdInit();			// Initialize the controller
	keyInit();			// Start-up the keypad driver
	keypadDef();		// Use the default keypad ASCII return values

	glInit();			// Initialize the graphic driver
	glBackLight(1);	// Turn-on the backlight
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);			//	Initialize 6x8 font
	glBlankScreen();
	for(;;)
	{
		glPrintf (0,  0, &fi6x8,  "Waiting for a Switch");
		glPrintf (0,  8, &fi6x8,  "to be pressed on the");
		glPrintf (0,  16, &fi6x8, "OP6800 Demo Board....");

		while(digIn(0) && digIn(1) && digIn(2) && digIn(3));
		glBlankScreen();
		if(!digIn(0))
		{
			glPrintf (0,  0, &fi6x8, "Switch 1 is Active!");
			digOut(10, 0);  // Turn-on Demo Board LED DS4
			ledOut(3, 1);   // Turn-on Keypad LED DS1

			while(!digIn(0));
			digOut(10, 1);	// Turn-OFF Demo Board LED DS4
			ledOut(3, 0); 	// Turn-OFF Keypad LED DS1
		}
		if(!digIn(1))
		{
			glPrintf (0,  0, &fi6x8, "Switch 2 is Active!");
			digOut(9, 0);	// Turn-on Demo Board LED DS3
			ledOut(2, 1); 	// Turn-on Keypad LED DS2

			while(!digIn(1));
			digOut(9, 1);	// Turn-OFF Demo Board LED DS3
			ledOut(2, 0); 	// Turn-OFF Keypad LED DS2
		}
		if(!digIn(2))
		{
			glPrintf (0,  0, &fi6x8, "Switch 3 is Active!");
			digOut(8, 0);	// Turn-on Demo Board LED DS2
			ledOut(1, 1); 	// Turn-on Keypad LED DS3

			while(!digIn(2));
			digOut(8, 1);	// Turn-OFF Demo Board LED DS2
			ledOut(1, 0); 	// Turn-OFF Keypad LED DS3
		}
		if(!digIn(3))
		{
			glPrintf (0,  0, &fi6x8, "Switch 4 is Active!");
			digOut(7, 0);	// Turn-on Demo Board LED DS1
			ledOut(0, 1); 	// Turn-on Keypad LED DS4

			while(!digIn(3));
			digOut(7, 1);	// Turn-OFF Demo Board LED DS1
			ledOut(0, 0); 	// Turn-OFF Keypad LED DS4
		}
		glBlankScreen();
	}
}