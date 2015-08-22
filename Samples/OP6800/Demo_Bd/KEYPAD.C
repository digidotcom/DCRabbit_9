/**************************************************************************
	keypad.c

   Z-World, 2001
	This sample program is for the OP6800 series controllers.
	
	This sample program demonstrates the use of the OP6800 Demo board
	with the controller. The program will light-up a LED and will display
	a message on the LCD when a keypress is detected.

	1. Connect a +12v power source to the OP6800 Demo Board connector
	   J2 as follows:

	   PS     	OP6800 Demo Board
	   --			-----------------
	   +12v		+RAW J2, pin 4 
	   GND      GND  J2, pin 1

	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press anyone of OP6800 keypad keys (continuously) to see a message
	   displayed on the LCD and light-up a LED. Here's a list of what key
	   controls what LED.      

	   keypad      	Controller Keypad LED	Demo Board LED  
	   ------			---------------------   --------------  
	   Scroll-Left 	     DS1             	DS1
	   Scroll-Up     	     DS3                DS3 
	   Scroll-Down         DS5                N/A
	   Scroll-Left SW4     DS7                N/A
		Page-Down           DS2                DS2
 		Page-Up             DS4                DS4
		Enter 		        DS6                N/A
**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.
#memmap xmem  // Required to reduce root memory usage


// Structure to hold font information
fontInfo fi6x8, fi8x10;

///////////////////////////////////////////////////////////////////////////

void main()
{
	static int led, channel, wKey, keypad_active, prompt_displayed;
	static int new_keypress_message, release_value, i;
	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------	
	brdInit();			// Initialize the controller

	// Start-up the keypad driver 
	keyInit();

	// Use default key values along with a key release code
	keyConfig (  3,'R', '1', 0, 0,  0, 0 );
	keyConfig (  6,'E', '2', 0, 0,  0, 0 );
	keyConfig (  2,'D', '3', 0, 0,  0, 0 );
	keyConfig (  5,'+', '4', 0, 0,  0, 0 );
	keyConfig (  1,'U', '5', 0, 0,  0, 0 );
	keyConfig (  4,'-', '6', 0, 0,  0, 0 );
	keyConfig (  0,'L', '7', 0, 0,  0, 0 );


	// Initialize the graphic driver
	glInit();

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
					digOut(7,1);
					digOut(8,1);
					digOut(9,1);
					digOut(10,1);
				}
				for(channel = 0; channel <= 6; channel++)
				{
					for(led = 0; led <=6; led++)
					{
						if(led != channel)
						ledOut(led, 0);
					}
					ledOut(channel, 1);
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
						ledOut(0, 1);
						digOut(7,0);
						channel = 0;
						break;

					case '-':
						glPrintf (0,  0, &fi8x10, "Page-Down key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						ledOut(1, 1);
						channel = 1;
						digOut(8,0);
						break;

					case 'U':
						glPrintf (0,  0, &fi8x10, "Scroll-Up key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						ledOut(2, 1);
						channel = 2;
						digOut(9,0);
						break;

					case '+':					
						glPrintf (0,  0, &fi8x10, "Page-Up key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						ledOut(3, 1);
						channel = 3;
						digOut(10,0);
						break;

					case 'D':					
						glPrintf (0,  0, &fi8x10, "Scroll-Down key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						ledOut(4, 1);
						channel = 4;
						break;

					case 'E':
						glPrintf (0,  0, &fi8x10, "Enter key");
						glPrintf (0,  16, &fi8x10, "is Active.");
						ledOut(5, 1);
						channel = 5;
						break;

					case 'R':
						glPrintf (0,  0, &fi8x10, "Scroll-Right");
						glPrintf (0,  16, &fi8x10, "key is Active.");
						ledOut(6, 1);
						channel = 6;
						break;
				}
				for(led=0; led <=6; led++)
				{
					if(led != channel)
					{
						ledOut(led, 0);
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
