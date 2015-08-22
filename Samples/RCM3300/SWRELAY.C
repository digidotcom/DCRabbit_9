/**************************************************************************

	swrelay.c
   Z-World, 2002

	This sample program is for RCM3300 series controllers and
   prototyping boards.

	Description
	===========
	This program demonstrates relay switching function call.
   Keyboard control switches relay common contacts to either
   normally open or normally closed contacts.

	Normally closed positions = NC1 and NC2
	Normally open positions = NO1 and NO2
   Common contacts = COM1 and COM2

	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

**************************************************************************/
#class auto					/* Change local var storage default to "auto" */
#use rcm33xx.lib		//sample library to use with this application

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

// set cursor and display character
void DispCh(int x, int y, char s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%c", x, y, s);
}


void main()
{
	auto int msg;
	auto char keypress;

	brdInit();  //must do to initialize board

	DispStr(5, 1, "Press 0 for Normally closed contact");
	DispStr(5, 2, "Press 1 for Normally open contact");

	while (1)
	{
		do {
			keypress = getchar();
			DispStr(5, 4, "You chose ");
   		DispCh (16, 4, keypress);
		}	while (keypress != '1' && keypress != '0');

		relayOut(0, keypress-0x30);	//relay contact
	}

}

