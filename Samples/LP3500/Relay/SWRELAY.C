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

	swrelay.c

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

