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
/*******************************************************************************

	swrelay.c

	This sample program is for RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This program demonstrates the relay switching function call.  Keyboard
	control selects relay common contacts as either normally open or normally
	closed contacts.

	Normally closed positions are NC1 and NC2.
	Normally open positions are NO1 and NO2.
	Common contacts are COM1 and COM2.

	Instructions
	============
	1. Compile and run this program.
	2. Follow the directions given in the STDIO window.
	3. Observe the DS7 (RELAY) LED on the RCM3300 prototyping board and notice
	   the relay "click" when the User choice toggles the relay's output state.

*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
	x += 0x20;
	y += 0x20;
	printf ("\x1B=%c%c%s", x, y,
	        "                                                                 ");
	printf ("\x1B=%c%c%s", x, y, s);
}

void main(void)
{
	auto char keypress;
	auto int msg;
	static char display[128];

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	DispStr(5, 1, "Press 0 for Normally Closed contact.");
	DispStr(5, 2, "Press 1 for Normally Open contact.");

	while (1) {
		do {
			keypress = getchar();
			sprintf(display, "You chose '%c' (%s).", keypress,
			        '0' == keypress ? "NC" : '1' == keypress ? "NO" : "ignored");
			DispStr(5, 4, display);
		}	while (keypress != '1' && keypress != '0');
		relayOut(0, keypress - '0');	// update relay contact choice
	}
}

