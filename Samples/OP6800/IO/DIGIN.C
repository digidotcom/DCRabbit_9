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

	This sample program is for the OP6800 series controllers.
	
	This program demonstrates the use of the digital inputs. With
	using the provided OP6800 Demo Board in your kit you can toggle
	an input channel from HIGH to LOW by pressing a push button on
	the OP6800 Demo Board.

	OP6800 Demo Board Setup:
	------------------------
	1. Connect a +12v power source to the OP6800 Demo Board connector
	   J3/J5 as follows:

	   PS     	OP6800 Demo Board
	   --			-----------------
	   +12v		+RAW Terminal  
	   GND      GND Terminal

	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press one of OP6800 Demo Board switches SW1 - SW4 (continuously) to
	   see an input go LOW. Here's a list of how the switches are connected.      

	   Switch      Digital Input
	   ------		-------------
	    SW1         IN00
	    SW2         IN01
	    SW3         IN02
	    SW4         IN03
	    	 
	NOTE: You can also toggle the other digital inputs(IN04-IN12) by
	      connecting/dis-connecting a wire momentarily from the channel
	      to GND. For best results do only one channel at a time.    
	      	      
**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.


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

void main()
{
	auto int key, reading, channel;

	brdInit();	// Required for OP6800 series boards
		
	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Digital Inputs = IN00-IN12: >>>");
	DispStr(8, 3, "IN00\tIN01\tIN02\tIN03\tIN04\tIN05\tIN06\tIN07");
	DispStr(8, 4, "----\t----\t----\t----\t----\t----\t----\t----");
	
	DispStr(8, 7, "IN08\tIN09\tIN10\tIN11\tIN12");
	DispStr(8, 8, "----\t----\t----\t----\t----");

	DispStr(8, 16, "Toggle IN00-IN03 by pressing switches SW1-SW4 on the OP6800 Demo Bd.");
	DispStr(8, 17, "(See instructions in sample program for complete details)");
	DispStr(8, 19, "<-PRESS 'Q' ON PC KEYBOARD TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// update input channels 0 - 7   
		update_input(0, 7, 8, 5);

		// update input channels 8 - 12  
		update_input(8, 12, 8, 9);
			
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
