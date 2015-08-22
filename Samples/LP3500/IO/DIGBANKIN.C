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

	digbankin.c

	This sample program is for LP3500 series controllers.

	Description
	===========
	This program demonstrates the use of the digital inputs.
	Using the provided DEMO board in your kit to toggle a bank of
	input	channels from HIGH to LOW when pressing a push button on
	the DEMO board.

	Connections
	===========
	Connect +K to external power source.

	When the controller is plugged into to the demo board the
	following connections are readily available.

	Controller		Demo Board
	----------		----------
			IN00 <->	S1
			IN01 <->	S2
			IN02 <->	S3
			IN03 <->	S4


	Instructions
	============
	1. Compile and run this program.
	2. Press any one of the DEMO board switches S1 or S2 and you should
	   see the inputs go LOW on the bank of channels the switch is connected to.

**************************************************************************/
#class auto

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

// update digital inputs for the given channel range
void update_input(int bank, int col, int row)
{
	auto char s[40];
	auto char display[80];
	auto int reading, i;

	// display the input status for the given channel range
	display[0] = '\0';							//initialize for strcat function

	reading = digBankIn(bank);				//read channels
	for (i=0; i<=7; i++)
	{
		sprintf(s, "%d\t", reading&1);			//format reading in memory
		strcat(display,s);						//append reading to display string
		reading = reading >> 1;
	}

	DispStr(col, row, display);			//update input status
}

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int key, reading, channel;

	brdInit();

	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Digital inputs 0 - 15: >>>");
	DispStr(8, 3, "IN0\tIN1\tIN2\tIN3\tIN4\tIN5\tIN6\tIN7");
	DispStr(8, 4, "----\t----\t----\t----\t----\t----\t----\t----");

	DispStr(8, 7, "IN8\tIN9\tIN10\tIN11\tIN12\tIN13\tIN14\tIN15");
	DispStr(8, 8, "----\t----\t----\t----\t----\t----\t----\t----");

	DispStr(8, 16, "Connect the Demo Bd. switches to the inputs that you what to toggle.");
	DispStr(8, 17, "(See instructions in sample program for complete details)");
	DispStr(8, 19, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// update input channels 0 - 7   (display at col = 8 row = 5)
		update_input(0, 8, 5);

		// update input channels 8 - 15  (display at col = 8 row = 9)
		update_input(1, 8, 9);

		if(kbhit())
		{
			key = getchar();
			if (key == 'Q' || key == 'q')		// check if it's the q or Q key
			{
				while(kbhit()) getchar();
      		exit(0);
     		}
		}
   }
}
///////////////////////////////////////////////////////////////////////////


