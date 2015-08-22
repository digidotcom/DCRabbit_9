/**************************************************************************

	swrelay.c
   Z-World, 2002

	This sample program is for LP3500 series controllers.

	Description
	===========
	This program demonstrates relay switching function call operating
	on normal power source.  Keyboard control switches the relay to
	Set (NO) and Reset (NC) positions.  Uses ANSI terminal sequences.


	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

**************************************************************************/
#class auto					/* Change local var storage default to "auto" */

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

	brdInit();

	DispStr(5, 1, "Press 1 for Normally open (set) contact");
	DispStr(5, 2, "Press 0 for Normally closed (reset) contact");

	while (1)
	{
		do {
			keypress = getchar();
			DispStr(5, 4, "You chose ");
   		DispCh (16, 4, keypress);
		}	while (keypress != '1' && keypress != '0');

		msg=relayOut(0, keypress-0x30);	//relay contact
		if (msg)
		{
			DispStr(5, 6, "Cannot switch; normal power source is unavailable");
		}
	}

}

