/***************************************************************************
	keylcd.c

	Use this sample with Intellicom Series products.

	This program demonstrates a simple setup for a user-defined 2x6 keypad
	and a 4x20 display.  Keypad characters will display when keys are
	pressed.  The output wraps from bottom back to the first line.

 	Backlight and contrast are not supported on VFD's.
	For this example to work properly, the display should be connected to
	parallel	port A as described here and in the example circuitry:

		PA.0	Out	LCD Backlight (if connected)
		PA.1	Out	Display CS (Active High)
		PA.2	Out	Display RS (High = Control, Low = Data) / LCD Contrast 0
		PA.3	Out	Display WR (Active Low) / LCD Contrast 1
		PA.4	Out	Display D4 / LCD Contrast 2
		PA.5	Out	Display D5 / LCD Contrast 3
		PA.6	Out	Display D6 / LCD Contrast 4
		PA.7	Out	Display D7 / LCD Contrast 5

	The keypad should be connected to parallel ports B, C and D
	as described here and in the example circuitry:

		PB.0	In		Keypad Col 0
		PC.1	In		Keypad Col 1
		PB.2	In		Keypad Col 2
		PB.3	In		Keypad Col 3
		PB.4	In		Keypad Col 4
		PB.5	In		Keypad Col 5
		PD.6	O.C.	Keypad Row 0
		PD.7	O.C.	Keypad Row 1

	This program will configure to the following keypad layout:

	[  1  ] [  2  ] [  3  ] [  4  ] [  5  ] [  .  ]
	[  6  ] [  7  ] [  8  ] [  9  ] [  0  ] [Enter]

***************************************************************************/
#class auto					/* Change local var storage default to "auto" */

#define BCONTRAST 20  //Adjust for best contrast a value from 4 to 252

void main (	void	)
{
	unsigned	wKey;		//	User Keypress

	brdInit();			// initialize display and keypad ports

	//setup characters on keypad
	keyConfig (  5,'1',0, 0, 0,  0, 0 );
	keyConfig (  4,'2',0, 0, 0,  0, 0 );
	keyConfig (  3,'3',0, 0, 0,  0, 0 );
	keyConfig (  2,'4',0, 0, 0,  0, 0 );
	keyConfig (  1,'5',0, 0, 0,  0, 0 );
	keyConfig (  0,'.',0, 0, 0,  0, 0 );

	keyConfig ( 13,'6',0, 0, 0,  0, 0 );
	keyConfig ( 12,'7',0, 0, 0,  0, 0 );
	keyConfig ( 11,'8',0, 0, 0,  0, 0 );
	keyConfig ( 10,'9',0, 0, 0,  0, 0 );
	keyConfig (  9,'0',0, 0, 0,  0, 0 );
	keyConfig (  8,'E',0, 0, 0,  0, 0 );

	dispBacklight(1);		//turn on backlight
	dispContrast(BCONTRAST);		//adjust contrast

	for (;;) {
		costate {								//	Process Keypad Press/Hold/Release
			keyProcess ();
			waitfor ( DelayMs(10) );
			}

		costate {								//	Process Keypad Press/Hold/Release
			waitfor ( wKey = keyGet() );	//	Wait for Keypress
			dispPrintf ("%c", wKey);
			}
		}
}