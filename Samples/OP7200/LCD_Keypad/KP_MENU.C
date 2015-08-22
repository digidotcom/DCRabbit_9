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
/***************************************************************************
	kp_menu.c
	
	This sample program is for the OP7200 series controllers.


	Description
	-----------
  	This program demonstrates how to implement a menu system with using the
  	glMenu library.

  	
	Instructions
	------------
	1. Compile and run this program.
	
  	2. To select a menu option, use the UP/DOWN scroll keys to move the highlight
  	   bar to the option that you want to select and then press the ENTER key.

  	   If there's an arrow or square indicator in the right-bottom corner, then
  	   use the UP/DOWN scroll keys to view the additional menu options.
	   (Page-up/Page-down scroll keys also control the menu highlight bar)
  	
	   Once the option is selected the operation will be completed or you
	   will be prompted to do additional steps to complete the option
	   selected.
                 
**************************************************************************/
#class auto		// default storage class for variables in function: on stack.
#memmap xmem  // Required to reduce root memory usage

//----------------------------------------------------------
// Menu options........set as needed for your application
//----------------------------------------------------------
// Can insert/delete menu options. The highlight bar is setup
// to start with the first MENU option and stop at the last
// menu option in the MENU.
//
// When adding/deleting menu options you must match up the
// case statements to the menu option number.
//

const char *main_menu [] =
{		"1.Increase Menu size",
		"2.Decrease Menu size",
		"3.Backlight Menu",
		"4.Space holder opt1",
		"5.Space holder opt2",
		""
};

const char *backlight [] =
{		"1.Turn Backlight OFF",
		"2.Turn Backlight ON",
		"3.Exit Menu",
		""
};

const char *confirmation [] =
{		"  NO",
		"  YES",
		""
};
//----------------------------------------------------------
// END.....Macros and Structure for menu system
//----------------------------------------------------------

//---------------------------------------------------------
// Macro's
//---------------------------------------------------------
#define LEDOFF				0
#define TOGGLE				1
#define INCREMENT			2
#define OPERATE			3

#define ASCII				0
#define NUMBER				1

//----------------------------------------------------------
// Structures, arrays, variables
//----------------------------------------------------------
fontInfo fi8x12, fi10x16, fi12x16;
windowMenu winMenu1, winMenu2, winMenu3;

//------------------------------------------------------------------------
// Milli-sec delay function (not used)
//------------------------------------------------------------------------
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

void calc_XY(windowMenu *mPtr, int* x, int* y, int option_num)
{
	auto windowFrame *wPtr;

	wPtr = (windowFrame*) &mPtr->winframe;
	*x = wPtr->startwinx +  mPtr->pFont->pixWidth;
	*y = wPtr->startwiny + (mPtr->pFont->pixHeight * (option_num - mPtr->current_offset));
}

//------------------------------------------------------------------------
// Routine for sub_menu backlight options 
//------------------------------------------------------------------------
int confirm_menu( int* confirm_status, int x, int y )
{
	static int done;
	static int state;
	auto int option;
	
	costate
	{
		done = FALSE;
		state = MENU_INIT;
		while(!done)
		{
			// Display the MAIN MENU
			waitfor((option = glMenu(&winMenu3, &state, x, y )) != 0);

			// Get menu option from the user
			switch(option)
			{
					// Turn Backlight ON
				case 1:	*confirm_status = 0;
							done = TRUE;
							break; 	

			
				// Turn Backlight OFF
				case 2:	done = TRUE;
							*confirm_status = 1;
							break;	
			
				// User made invalid selection
				default:												
					break;
			}
		}
	}
	return(done);
}

//------------------------------------------------------------------------
// Routine for sub_menu backlight options 
//------------------------------------------------------------------------
int backlight_menu( void )
{

	static int done;
	static int state;
	auto int option, confirm_status;
	static int x, y;
		
	costate
	{
		done = FALSE;
		state = MENU_INIT;
		while(!done)
		{

			// Calculate where to place MENU2 based on MENU1 location
			calc_XY(&winMenu1, &x, &y, 3);

			// Display the MAIN MENU
			waitfor((option = glMenu(&winMenu2, &state, x, y)) != 0);

			// Get menu option from the user
			switch(option)
			{  
			
				// Turn Backlight OFF
				case 1:	// Calculate where to place MENU3 based on MENU2 location
							calc_XY(&winMenu2, &x, &y, 1);

							waitfor(confirm_menu(&confirm_status, x, y));
							if(confirm_status)
							{
								glBackLight(0);
								done = TRUE;
								state = MENU_NO_CHANGE;
							}
							else
							{
								glMenuClear(&winMenu3);
								state = MENU_REFRESH;	
							}
							
							break;	

				// Turn Backlight ON
				case 2:	// Calculate where to place MENU3 based on MENU2 location
							calc_XY(&winMenu2, &x, &y, 2);
							waitfor(confirm_menu(&confirm_status, x, y));
							if(confirm_status)
							{
								glBackLight(1);
								done = TRUE;
								state = MENU_NO_CHANGE;
							}
							else
							{
								glMenuClear(&winMenu3);
								state = MENU_REFRESH;
							}
							break; 	

				case 3:
					  	 	done = TRUE;
			       		break;

				// User made invalid selection
				default:												
					break;
			}
		}
		glBuffLock();
		glMenuClear(&winMenu2);
		glMenuClear(&winMenu3);		
		glRefreshMenu(&winMenu1);	
		glBuffUnlock();
	}
	return(done);
}

//------------------------------------------------------------------------
// Sample program to demonstrate the LCD and keypad
//------------------------------------------------------------------------
void main (	void	)		
{
	auto int option;
	static int state;
	
	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------	
	brdInit();			// Initialize the controller
	keyInit();			// Start-up the keypad driver
	keypadDef();		// Use the default keypad ASCII return values
	
	glInit();			// Initialize the graphic driver
	glBackLight(1);	// Turn-on the backlight
	glSetContrast(20);

	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);
	glXFontInit(&fi12x16, 12, 16, 32, 127, Font12x16);
	
	// Load structure with MENU information
	glMenuInit(&winMenu1, &fi10x16, DOUBLE_LINE, SHADOW, main_menu, " MENU ", 3);
	glMenuInit(&winMenu2, &fi10x16, DOUBLE_LINE, SHADOW, backlight, " BACKLIGHT ", -1);
	glMenuInit(&winMenu3, &fi10x16, DOUBLE_LINE, SHADOW, confirmation, " Are you sure? ", -1);
			
	// Set the state to display the MENU 
	state = MENU_INIT;

	//------------------------------------------------------------------------
	// Main program loop for the MENU system 
	//------------------------------------------------------------------------
	for (;;)
	{
		costate
		{
			keyProcess ();
			waitfor(DelayMs(10));
		}
		
		costate
		{
			// Display the MAIN MENU
			waitfor((option = glMenu(&winMenu1, &state, 0, 20)) != 0);

			// Get menu option from the user
			switch(option)
			{
				// Increase Menu size
				case 1:	glMenuClear(&winMenu1);
							glMenuInit(&winMenu1, &fi12x16, DOUBLE_LINE, SHADOW,
				                    main_menu, " MENU ", 3);
				         glMenuInit(&winMenu2, &fi12x16, DOUBLE_LINE, SHADOW,
				                    backlight, " BACKLIGHT ", -1);
							glMenuInit(&winMenu3, &fi12x16, DOUBLE_LINE, SHADOW,
											confirmation, " Are you sure? ", -1);           
				         state = MENU_INIT;
				         break;
				         
				// Decrease MENU size
				case 2:	glMenuClear(&winMenu1);
							glMenuInit(&winMenu1, &fi10x16, DOUBLE_LINE, SHADOW,
				                    main_menu, " MENU ", 3);
				         glMenuInit(&winMenu2, &fi10x16, DOUBLE_LINE, SHADOW,
				                    backlight, " BACKLIGHT ", -1);
							glMenuInit(&winMenu3, &fi10x16, DOUBLE_LINE, SHADOW,
											confirmation, " Are you sure? ", 3);           
				         state = MENU_INIT;
				         break;
				                       
				// Display backlight menu options
				case 3:	waitfor(backlight_menu()); 
							state = MENU_REFRESH;
							break;	

				// User made invalid selection
				default:
					// Check for menu sizing error during the development phase. If
					// it is a sizing error an error message will be displayed.
					if(option < 0)
					  for(;;);								
					break;
			}
		}
	}
}

