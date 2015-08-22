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

	digin.c
   
	This sample program is intended for the BL2500 series controllers
	and demo boards.
	
	Description
	===========
	This program demonstrates the use of the digital inputs and the
	function call digIn().
	Using the demo board in provided your kit you will see an input
	channel toggle from HIGH to LOW when pressing a push button on
	the demo board.

	
	Connections
	===========
	Make the following connections:
			
	Controller		Demo Board
	----------		----------
						Jumper H2 pins 3-5						
						Jumper H2 pins 4-6						
			IN00 <->	SW1
			IN01 <->	SW2
			IN02 <->	SW3
			IN03 <->	SW4
	  J11 pin 9 <-> GND
	  J7 pin 3 <-> +K (not to exceed 18V)

	  
	Instructions
	============
	1. Compile and run this program.
	2. Press and hold any one of the DEMO board switches SW1 - SW4 and
		you should see the input go LOW on the channel the switch is
		connected to.
	3. IN04 to IN15 can be viewed by moving the initial connecting the
		input lines to demo board or by touching the line with a GND signal.
	   
**************************************************************************/
#class auto

#define STARTCHAN 0
#define ENDCHAN 15


// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

// update digital inputs for the given channel range
void update_input(int start_channel, int end_channel, int col, int row)
{
	auto char s[40];
	auto char display[80];
	auto int reading;
	
	// display the input status for the given channel range
	display[0] = '\0';							//initialize for strcat function
	while(start_channel <= end_channel)		//read channels  
	{	
		reading = digIn(start_channel++);	//read channel
		sprintf(s, "%d\t", reading);			//format reading in memory
		strcat(display,s);						//append reading to display string
	}
	DispStr(col, row, display);				//update input status
	
}
		
///////////////////////////////////////////////////////////////////////////
main()
{

	auto int key, reading, channel;

	brdInit();	
		
	//Display user instructions and channel headings
	DispStr(8, 1, "\t<<< Protected digital inputs 0 - 7: >>>");
	DispStr(8, 3, "IN00\tIN01\tIN02\tIN03\tIN04\tIN05\tIN06\tIN07");
	DispStr(8, 4, "----\t----\t----\t----\t----\t----\t----\t----");
	
	DispStr(8, 7, "\t<<< Unprotected digital inputs 8 - 15: >>>");
	DispStr(8, 9, "IN08\tIN09\tIN10\tIN11\tIN12\tIN13\tIN14\tIN15");
	DispStr(8, 10, "----\t----\t----\t----\t----\t----\t----\t----");
	
	DispStr(8, 16, "Connect the Demo Bd. switches to the inputs that you what to toggle.");
	DispStr(8, 17, "(See instructions in sample program for complete details)");
	DispStr(8, 19, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// update input channels 0 - 7 at col 8 row 5
		update_input(STARTCHAN, 7, 8, 5);
		
		// update input channels 8 - 15 at col 8 row 11
		update_input(8, ENDCHAN, 8, 11);

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


