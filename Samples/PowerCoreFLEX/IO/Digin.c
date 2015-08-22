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

	digIn.c

  	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	This program demonstrates how to read digital inputs with using
	high-level API library function (digIn).

   Instructions:
	------------
	1. Compile and run this program.

	2. Press switch S2 and/or S3 on the prototyping board to see the
      displayed logic level change.

***************************************************************************/
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////

void main()
{

	auto char s[40];
	auto char display[80];
	auto int key, reading, channel;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Digital inputs: >>>");
	DispStr(10, 3, "IN0\tIN1");
	DispStr(10, 4, "---\t---");

	DispStr(8, 8, "Press S2 or S3 on the prototyping bd to see the inputs change states.");
 	DispStr(8, 10, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// display the input status for all channels
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel < 2; channel++)	//read channels 0 - 1
		{
			reading = digIn(channel);					//read channel
			sprintf(s, "%d \t", reading);				//format reading in memory
			strcat(display,s);							//append reading to display string
		}
		DispStr(10, 5, display);						//update input status

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