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
	keypadtoled.c

	This program is used with RCM3700 series controllers
	with prototyping boards.

	An optional LCD/Keypad Module is required to run this
	program.  #define PORTA_AUX_IO at top of program to
	enable external I/O bus for LCD/Keypad operations.

	Description
	===========
	This sample program demonstrates the use of the external
	I/O bus with the controller using an LCD/Keypad Module.
	The program will light-up an LED and will display a message
	on the LCD when a key press is detected.

	DS1 and DS2 LED's on the proto-board will also light-up.

	Instructions
	============
	1. Compile and run this program.
	2. Press and hold a key on the LCD/Keypad Module to see a
		message displayed on the LCD and light-up a LED. Here's
		a list of what key controls what LED.

	   keypad      	Controller Keypad LED	Proto Board LED
	   ------			---------------------   --------------
	   Scroll-Left 	     DS1             	---
	   Scroll-Up     	     DS3                ---
	   Scroll-Down         DS5                ---
	   Scroll-Left SW4     DS7                ---
		Page-Down           DS2                DS2
 		Page-Up             DS4                DS1
		Enter 		        DS6                ---
*******************************************************************/
#class auto
#define PORTA_AUX_IO		//required to run LCD/Keypad for this demo

#memmap xmem

// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

#define DS1 6				//led, port F bit 6
#define DS2 7				//led, port F bit 7
#define ON	0
#define OFF 1

// Structure to hold font information
fontInfo fi6x8, fi8x10;

////////
// output function to control protoboard LED's
////////
void pbLedOut(int channel, int onoff)
{
	// works for port F bits 6 and 7 only
	BitWrPortI(PFDR, &PFDRShadow, onoff, channel);
}

///////////////////////////////////////////////////////////////////////////

main()
{
	static int led, channel, wKey, keypad_active, prompt_displayed;
	static int new_keypress_message, release_value, i;
	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------
	brdInit();				// Initialize the controller for this demo

	// Start-up the keypad driver and
	// Initialize the graphic driver
	dispInit();

	// Use default key values along with a key release code
	keyConfig (  3,'R', '1', 0, 0,  0, 0 );
	keyConfig (  6,'E', '2', 0, 0,  0, 0 );
	keyConfig (  2,'D', '3', 0, 0,  0, 0 );
	keyConfig (  5,'+', '4', 0, 0,  0, 0 );
	keyConfig (  1,'U', '5', 0, 0,  0, 0 );
	keyConfig (  4,'-', '6', 0, 0,  0, 0 );
	keyConfig (  0,'L', '7', 0, 0,  0, 0 );

	// Initialize 6x8 font
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);          // Initialize 6x8 font
	glXFontInit(&fi8x10, 8, 10, 32, 127, Font8x10);			//	initialize 8x10 font
	glBlankScreen();

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

					// Turn-Off leds on the Demo board
					pbLedOut(DS1, OFF); 		//DS1 off
					pbLedOut(DS2, OFF);		//DS2 off
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
						pbLedOut(DS1, ON); 		//DS1 on
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
						pbLedOut(DS2, ON); 		//DS2 on
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


