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
/**************************************************************************

	kp_16key.c

	This sample is for use with the 9key keypad of the
	OP7200/7210 controller.

	The sample uses a 16 key data entry pad library created as a sample
	for customer use.  The user functions associated with the library are;

	glkeypadInit	--> Initializes the library.
	glkMsgBox	   --> Displays a Text box.
	glkGetInt		--> Display the 16 Keypad for integer entry.
	glkGetFloat		--> "                   " for float entry.
	glkGetLong     --> "                   " for long integer entry.
	glkGetString   --> "                   " for String entry.
		
	Instructions
	============
	Press one of the KEYPAD buttons below the Labels to invoke the
	16 Keypad.  See below for key details.
		
				  16 Key Data entry pad details
				  =============================
	
	|																		|
	|							LCD SCREEN								|
	|_____________________________________________________|
	
		^				^				^				^				^
		|				|				|				|				|
		Puts			Cursor		Move			Cursor		Puts Cursor in
		Cursor		To 			Cursor		to 			Bottom
		in top		bottom of	Up				top			Right
		Left   		Row			One			of same		Corner (RET KEY)
		corner										row
		(7 key)
						<-			  /\				->
					 move			  \/				move
					 cursor		 Enter			cursor				
					 left			selected			right One
					 One			  key

									  |
   								  V
									 Move
									Cursor
									 Down
									  One
										
  
**************************************************************************/
#class auto

#memmap xmem
#use "Terminal14.lib"		// Good size font for the keypad/keyboard labels
									// or the keyboard entry box 
#use "Terminal12.lib"		// Good size font for the heading box of the
									// keyboard/keypad
#use "Terminal9.lib"			// Good size font for the keypad/keyboard if
									// you plan on entering a long string.
#use "kpcust16key.lib"		// a custom 16 Key data entry pad.


///////////////////////////////////////////////////////////////////////

void main ()
{
	int key,iVal;
	float fVal;
	long lVal;
	char sVal[100];
	fontInfo fi8x10,fi12x18,fi14x16,fi10x12;				// font structures
		// Initialize Board
	brdInit();
		// Initialize Display						
	glInit();
		// Initialize the OP72xx 9 Key Keypad
	keyInit();
		// Initialize fonts
	glXFontInit(&fi8x10,  8,  10, 32, 127, Font8x10);
	glXFontInit ( &fi12x18,12,18,0x20,0x7D,Terminal14 );
	glXFontInit ( &fi14x16,14,16,0x20,0x7E,Terminal12 );
	glXFontInit ( &fi10x12,10,12,0x20,0x7E,Terminal9 );
		// Initialize the Custom 16 KEY data entry Pad used by the 9 KeyPad for
		// Data entry.  The Parameter -> 0 = No Buzzer, 1 = Beep on Press.
	glkeypadInit(0);
		// Set the Contrast
	glSetContrast(24);
		// Turn on the Backlight
	glBackLight(1);
		// Lock the Display Buffer
	glBuffLock();
		// The glkMsgBox functions are part of the kpcust16key.lib, and can be used to
		// display text boxes on the LCD.  The first 4 display small boxes with text
		// above a keypad button, giving the keypad button a "label" as to what it does
		// in this sample.
	glkMsgBox(0,200,70,40,&fi8x10,"ENTER\nINT",1,0);
	glkMsgBox(70,200,70,40,&fi8x10,"ENTER\nLONG",1,0);
	glkMsgBox(180,200,70,40,&fi8x10,"ENTER\nFLOAT",1,0);
	glkMsgBox(250,200,70,40,&fi8x10,"ENTER\nCHARS",1,0);
		// The next 2 functions display a 'USER Message'.  It can be an alarm message
		// or anything you wish.  
	glkMsgBox(0,0,320,60,&fi12x18,"KP_16KEY.C",1,0);
	glkMsgBox(0,60,320,120,&fi10x12,"Press keypad key below label\nto display the\n \n" \
				 "16 KEY DATA ENTRY PAD\n \nSee Sample description for\n16Key details",1,0);
		// Unlock the Display buffer.
	glBuffUnlock();
		// initialize the variables
	fVal = 0;
	iVal = 0;
	lVal = 0;
	sprintf(sVal,"");
	for (;;)
	{
			// Check for 9 key keypad presses 
		keyProcess();
		costate
		{
				// Wait for a valid keypress
			waitfor ( ( key = keyGet() ) > 0 );
			switch (key)
			{
				case 'B':		
					// The function will copy the contents of the LCD, clear the screen, and display the
					// custom 16 key data entry pad.  The result of the entry when the 'RET' key is pressed
					// will be stored in iVal.  The 16 key will then clear itself, and the contents  prior
					// to the function will be displayed.  There are 3 other functions related below.
					// See function description for parameter details.
					waitfor(glkGetInt(&iVal,-32767,32767,6,&fi12x18,&fi10x12,"ENTER INTEGER\n(-32767 -32767)") );
					printf("INT ENTERED = %d\n",iVal);
					break;
				case 'S':	  
					waitfor(glkGetString(sVal,25,&fi12x18,&fi10x12,"ENTER STRING\n(MAX 25 CHARS)") );
					printf("STRING ENTERED = %s\n",sVal);
					break;	
				case '-':
					waitfor(glkGetFloat(&fVal,-1000000,1000000,10,&fi12x18,&fi10x12,"ENTER FLOAT") );
					printf("FLOAT ENTERED = %f\n",fVal);
					break;
				case '+':
					waitfor(glkGetLong(&lVal,-1000000,1000000,10,&fi12x18,&fi10x12,"ENTER LONG") );
					printf("LONG ENTERED = %ld\n",lVal);
					break;
			}
		}
	}
}
