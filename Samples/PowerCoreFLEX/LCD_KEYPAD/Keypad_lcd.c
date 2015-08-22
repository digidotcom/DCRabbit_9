/**************************************************************************
	keypad_lcd.c
   Z-World, 2004

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	Description
	===========
	This sample program demonstrates the use of the external
	I/O bus with the controller using an LCD/Keypad Module.
	The program will light-up an LED and will display a message
	on the LCD when a key press is detected.

	Instructions
	============
	1. Compile and run this program.
	2. Press and hold a key on the LCD/Keypad Module to see a
		message displayed on the LCD and light-up a LED. Here's
		a list of what key controls what LED.

	   keypad      	LCD/Keypad LED
	   ------			--------------
	   Scroll-Left 	     DS1
	   Scroll-Up     	     DS3
	   Scroll-Down         DS5
	   Scroll-Right     	  DS7
		Page-Down           DS2
 		Page-Up             DS4
		Enter 		        DS6
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

#define ON	1
#define OFF 0

// Structure to hold font information
fontInfo fi6x8, fi8x10;

///////////////////////////////////////////////////////////////////////////

main()
{
	static int led, channel, wKey, keypad_active, prompt_displayed;
	static int new_keypress_message, release_value, i;
	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------
	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	// Start-up the keypad driver and
	// Initialize the graphic driver
	dispInit();

	// Use default key values, along with a key release code
	keyConfig (  3,'R', '1', 0, 0,  0, 0 );
	keyConfig (  6,'E', '2', 0, 0,  0, 0 );
	keyConfig (  2,'D', '3', 0, 0,  0, 0 );
	keyConfig (  5,'+', '4', 0, 0,  0, 0 );
	keyConfig (  1,'U', '5', 0, 0,  0, 0 );
	keyConfig (  4,'-', '6', 0, 0,  0, 0 );
	keyConfig (  0,'L', '7', 0, 0,  0, 0 );

	// Initialize fonts
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);          // Initialize 6x8 font
	glXFontInit(&fi8x10, 8, 10, 32, 127, Font8x10);			//	initialize 8x10 font

   // Initialize control flags
	keypad_active = FALSE;
	prompt_displayed = FALSE;
	new_keypress_message = FALSE;
	for(;;)
	{
		costate
		{
			keyProcess ();
			waitfor ( DelayMs(10) );

		}

		costate
		{
			// Wait for any key to be pressed
			waitfor((wKey = keyGet()) != 0);
			release_value = -1;
			switch(wKey)
			{
				case 'L': 	release_value = '7'; break;
				case '-': 	release_value = '6'; break;
				case 'U':   release_value = '5'; break;
				case '+': 	release_value = '4'; break;
				case 'D':	release_value = '3'; break;
				case 'E':	release_value = '2'; break;
				case 'R':	release_value = '1'; break;
			}
			if(release_value != -1)
			{
				keypad_active = TRUE;
				// Wait for the key to be released
				waitfor(keyGet() == release_value);
				keypad_active = FALSE;
			}
		}

		costate
		{
			if(!keypad_active)
			{
				if(!prompt_displayed)
				{
					glBlankScreen();
					glPrintf (0,  0, &fi6x8,  "Waiting for a Key to");
					glPrintf (0,  8, &fi6x8,  "be pressed on the ");
					glPrintf (0,  16, &fi6x8, "LCD Keypad....");
					glFillPolygon(4, 115, 26,  121,26,   121,31,   115, 31);
					prompt_displayed = TRUE;
					new_keypress_message = FALSE;

				}

				for(channel = 0; channel <= 6; channel++)
				{
					for(led = 0; led <=6; led++)
					{
						if(led != channel)
						dispLedOut(led, 0);
					}
					dispLedOut(channel, 1);
					waitfor(DelayMs(100));
					if(keypad_active)
					{

						break;
					}
				}
			}
		}

		costate
		{
			if(keypad_active && !new_keypress_message)
			{
				glBlankScreen();
				glFillPolygon(4, 113, 26,  121,26,   121,31,   113, 31);
				switch(wKey)
				{
					case 'L':
						glPrintf (0,  0, &fi8x10, "Scroll-Left key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						dispLedOut(0, 1);
						channel = 0;
						break;

					case '-':
						glPrintf (0,  0, &fi8x10, "Page-Down key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						dispLedOut(1, 1);
						channel = 1;
						break;

					case 'U':
						glPrintf (0,  0, &fi8x10, "Scroll-Up key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						dispLedOut(2, 1);
						channel = 2;
						break;

					case '+':
						glPrintf (0,  0, &fi8x10, "Page-Up key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						dispLedOut(3, 1);
						channel = 3;
						break;

					case 'D':
						glPrintf (0,  0, &fi8x10, "Scroll-Down key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						dispLedOut(4, 1);
						channel = 4;
						break;

					case 'E':
						glPrintf (0,  0, &fi8x10, "Enter key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						dispLedOut(5, 1);
						channel = 5;
						break;

					case 'R':
						glPrintf (0,  0, &fi8x10, "Scroll-Right");
						glPrintf (0,  16, &fi8x10, "key is Active.");
						dispLedOut(6, 1);
						channel = 6;
						break;
				}
				for(led=0; led <=6; led++)
				{
					if(led != channel)
					{
						dispLedOut(led, 0);
					}
				}
				prompt_displayed = FALSE;
				new_keypress_message = TRUE;
			}
		}
		costate
		{
			if(keypad_active)
			{
				for(i=0; i<(LCD_XS-8); i+=4)
				{
					glHScroll(0, 26, LCD_XS, 6, -4);
					waitfor(DelayMs(5));
					if(!keypad_active)
						abort;
				}
				for(i=0; i<(LCD_XS-8); i+=4)
				{
					glHScroll(0, 26, LCD_XS, 6, 4);
					waitfor(DelayMs(5));
					if(!keypad_active)
						abort;
				}
			}
		}
	}
}


