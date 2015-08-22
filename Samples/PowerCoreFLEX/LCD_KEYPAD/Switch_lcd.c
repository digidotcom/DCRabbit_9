/**************************************************************************
	switch_lcd.c
   Z-World, 2004

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	Description
	===========
	This sample program demonstrates the use of the external
	I/O bus with the controller using an LCD/Keypad Module.
	The program will light-up an LED and will display a message
	on the LCD when a switch press is detected.

	DS5 and DS6 LED's on the proto-board will also light-up
	on a switch press.


	Instructions
	============
	1. Compile and run this program.
	2. Press and hold S2 or S3 on the prototyping board to
	   see a message displayed on the LCD and light up an LED.
	   Here's a list of how the switches are connected.

	   Switch   Proto Board LED  Controller Keypad LED
	   ------   --------------  ---------------------
	    S2       DS5             DS0
	    S3       DS6             DS1

***************************************************************************/
#class auto
#memmap xmem

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

///////
// The following macros are used with LCD/Keypad Modules and values
// default here according to the prototyping board. They must be defined
// before using graphic libraries.
// Make modifications as necessary to match your application.
///////
#define LCDCSREGISTER	IB6CR				// Use Port E bit 6 for LCD strobe
#define LCDCSSHADOW		IB6CRShadow		// define shadow register
#define LCDCSCONFIG		0x78				// set for 7 waits, I/O (rd and wr) data strobe, allow writes
#define LCDSTROBE			0x40				// bit 6 mask

#define KEYCSREGISTER	IB6CR 			// Use Port E bit 6 for keypad strobe
#define KEYCSSHADOW		IB6CRShadow		// define shadow register
#define KEYCSCONFIG		0x78				// set for 7 waits, I/O (rd and wr) data strobe, allow writes
#define KEYSTROBE			0x40				// bit 6 mask

///////
// change base address here to match above strobe address
///////
#define LCDBASEADDR		0xC000			// IB6CR strobe I/O address
#define KEYBASEADDR		0xC000			// IB6CR strobe I/O address


// Macro's to control the data buffer I/O direction for external I/O accesses
#define SET_EXTBUFFER_RD BitWrPortI( PEDR, &PEDRShadow, 0, 7);
#define SET_EXTBUFFER_WR BitWrPortI( PEDR, &PEDRShadow, 1, 7);
// End driver configuration


#use "lcd122key7.lib"

#define ON	0
#define OFF 1

// Structure to hold font information
fontInfo fi6x8;

///////////////////////////////////////////////////////////////////////////

main()
{

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	// Start-up the keypad driver and
	// Initialize the graphic driver
	dispInit();

   // Use the default keypad ASCII return values
	keypadDef();

   // Turn-on the backlight
   glBackLight(1);

   //	Initialize 6x8 font
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);
	for(;;)
	{
		glPrintf (0,  0, &fi6x8,  "Waiting for a Switch");
		glPrintf (0,  8, &fi6x8,  "to be pressed on the");
		glPrintf (0,  16, &fi6x8, "prototyping board...");

		while(digIn(0) && digIn(1));
		glBlankScreen();
		if(!digIn(0))
		{
			glPrintf (0,  0, &fi6x8, "Switch S2 is Active!");
			dispLedOut(0, 1);   		//Turn-on Keypad LED DS5
			ledOut(0, ON);				//proto-board led on

			while (!digIn(0));
			dispLedOut(0, 0); 		// Turn-OFF Keypad LED DS5
			ledOut(0, OFF);			//proto-board led off
		}

		if(!digIn(1))
		{
			glPrintf (0,  0, &fi6x8, "Switch S3 is Active!");
			dispLedOut(1, 1);   		//Turn-on Keypad LED DS6
			ledOut(1, ON);				//proto-board led on

			while(!digIn(1));
			dispLedOut(1, 0); 		// Turn-OFF Keypad LED DS6
			ledOut(1, OFF);			//proto-board led off
		}

		glBlankScreen();
	}
}


