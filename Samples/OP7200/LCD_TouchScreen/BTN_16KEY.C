/**************************************************************************

	btn_16key.c

   Z-World, 2001
	This sample program is for the OP7200 series controllers.
	
	Description
	===========
	This program demonstrates the the usage of the virtual keypad
	for user data entry.  The keypad keyset is setup and controlled by
	the library tsCustKeypad.lib.  The library is intended to be a sample
	and can be modified as needed.  The library is located in the
	samples\op7200\lcd_touchscreen directory and uses the gltouchscreen.lib
	functions.  

	Instructions
	============
	1. Compile and run program.
	2. Press  one of the 4 labeled buttons. The virtual keypad
		will appear, enter some data. and press
		the 'RET' key.  The data entered will be displayed in the STDIO
		window.
	3. Try this with all four buttons.


	
	Function Key Listing used in the tscustkeypad.lib
	=================================================
	CLR	will clear the text entry window.
	DEL	will delete the last entry.
	ESC	will escape from the /keypad with no changes to the
			entry.
	RET	will store the text that was entered and close the /
			keypad.
**************************************************************************/
#class auto
#memmap xmem					// 
#use "tsCust16Key.lib"	// custom keyset 
#use "Terminal12.lib"		// Good size font for the text entry box
#use "Terminal9.lib"			// Good size font for the heading box.


// These unsigned long values are asssigned by the Init functions
// (btnInit for the userX and tscVKPInit for the keypad functions)
// They are memory locations in xmem where the BtnData values are
// stored for there respective keysets.

unsigned  long userX;		// starting memory location where the user
									//	buttons will be stored.
unsigned  long vkpX;			// starting memory location where the keypad
									//	buttons will be stored.		
									
									
void main ()
{
	// Setup some variables for extracting data out of the /keypad.
	int btn,iVal;
	long lVal;
	float fVal;
	char sVal[100];
	
	// Setup up some font structures
	fontInfo fi14x16,fi10x12;
		// initial the the board and touchscreen.
	brdInit();			// sbc required initialization
		// initialize the LCD
	glInit();
	
		// Turn on the Backlight, setup the contrast.
	glBackLight(1);
		// Set the Contrast
	glSetContrast(24);
		// Initialize the fonts
	glXFontInit ( &fi14x16,14,16,0x20,0x7E,Terminal12 );
	glXFontInit ( &fi10x12,10,12,0x20,0x7E,Terminal9 );

		// Initialize the gltouchscreen.lib library
	userX = btnInit( (int)100 );

		//  Initialize the Virtual Keypad keyset.
	vkpX = tscVKPInit( (int)16, &fi14x16,&fi10x12);
		//  Set the Button Attributes (since the function is non-blocking,
		//  it will need to be repeatedly called until a 1 is returned).
	while (!tscVKPAttributes(vkpX,1,500,100,1));
		
		// Create 5 buttons to be displayed and used on the LCD
	btnCreateText(userX,1,70,10,180,40,1,1,&fi10x12,"ENTER A STRING\nVALUE");
	btnCreateText(userX,2,65,55,190,40,1,1,&fi10x12, "ENTER A FLOAT\nVALUE");
	btnCreateText(userX,3,60,100,200,40,1,1,&fi10x12, "ENTER A NEW\nLONG  VALUE");
	btnCreateText(userX,4,55,145,210,40,1,1,&fi10x12,"ENTER A NEW\nINTEGER VALUE");

		// Setup the button attributes
	btnAttributes(userX,1,0,0,0,1);
	btnAttributes(userX,2,0,0,0,1);
	btnAttributes(userX,3,0,0,0,1);
	btnAttributes(userX,4,0,0,0,1);

	
		// Display the buttons, with a boarder around the outside of the LCD
	
	btnMsgBox(0,0,320,240,NULL,"",1,0);	// will display a border
	
	while (!btnDisplayLevel(userX,1)); // Will display all buttons of Level 1
	fVal = 0;
	lVal = 0;
	iVal = 0;
	sprintf(sVal,"");
	for (;;)
	{
		costate
		{
				// Wait for a button to be pressed 
			waitfor ( (	btn = btnGet(userX) ) >= 0 );
			switch (btn)
			{
				case 1:
							// Display the Virtual keypad in 'STRING' mode
					waitfor ( tscVKPGetString(vkpX,sVal,100,&fi14x16,&fi10x12,
													"ENTER NEW STRING\nVARIABLE") );
					printf("New String Entered = %s\n",sVal);
					break;
			 	case 2:
			 			// Display the Virtual Keypad for use with floats
					waitfor ( tscVKPGetFloat(vkpX,&fVal,-1000000,1000000,25,&fi14x16,&fi10x12,
												"ENTER NEW\nFLOAT VALUE") );
					printf ( "New Float Entered = %f\n",fVal );
					break;
			 	case 3:
			 			// // Display the Virtual Keypad For use with Longs
					waitfor ( tscVKPGetLong(vkpX,&lVal,-1000000,1000000,25,&fi14x16,&fi10x12,
												"ENTER NEW\nLONG VALUE") );
					printf ( "New Long Entered = %ld\n",lVal );
					break;
			 	case 4:
			 			// // Display the Virtual Keypad For use with ints
					waitfor ( tscVKPGetInt(vkpX,&iVal,-32000,32000,6,&fi14x16,&fi10x12,
								 			  "ENTER NEW\nINTEGER VALUE") );
					printf ( "New Integer Entered = %d\n",iVal );
					break;
					
			}
		}
	}
}