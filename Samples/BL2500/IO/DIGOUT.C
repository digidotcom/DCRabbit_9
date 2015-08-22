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

	digOut.c
   
	This sample program is intended for the BL2500 series controllers
	and demo boards.
	
	Description
	===========
	This program demonstrates the use of the digital outputs and the
	function call digOut().
	Using the demo board in provided your kit you will see the logic
	levels of output channels on STDIO and the state of corresponding
	LED's on the demo board.

	
	Connections
	===========
	Make the following connections:
		
	Controller		Demo Board
	----------		----------
		  OUT00 <->	LED1
		  OUT01 <->	LED2
		  OUT02 <->	LED3
		  OUT03 <->	LED4
	  J7 pin 3 <-> +K (not to exceed 18V)

	  		  
	Instructions
	============
	1. Compile and run this program.
	2. You will be prompted to choose an output channel and a logic level.
		Choose channel 0 and 0 for logic level.
	3. LED1 should be lit.  Choose logic levels for the rest of the channels.
	   
**************************************************************************/
#class auto

// screen foreground colors
#define	BLACK		"\x1b[30m"
#define	RED		"\x1b[31m"	
#define	GREEN		"\x1b[32m"
#define	BLUE		"\x1b[34m"

#define STARTCHAN 0
#define ENDCHAN 7

#define OUTCONFIG	0xFF		//logic highs disables led's
char output[ENDCHAN+1];

/////
// Set the STDIO cursor location and display a string
/////
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

/////
// updates screen
/////
void update_outputs(int start_channel, int end_channel, int col, int row)
{
	auto char s[40];
	auto char display[80];
	auto int reading;
	
	// display the input status for the given channel range
	display[0] = '\0';								//initialize for strcat function
	while (start_channel <= end_channel)		//read channels  
	{	
		sprintf(s, "%d\t", output[start_channel++]);		//format logic level for display
		strcat(display,s);										//add to display string
	}
	DispStr(col, row, display);					//update input status
}
		
///////////////////////////////////////////////////////////////////////////
main()
{

	auto char s[128];
	auto char display[128];
	auto char tmpbuf[64];
	auto int channum;

   brdInit();
	printf("%s", BLACK);		// screen foreground
   
   /// initialize outputs to the output configuration
   for (channum=STARTCHAN; channum <= ENDCHAN; channum++)
   {
   	output[channum] = (OUTCONFIG >> channum) & 1;
		digOut(channum, output[channum]);
	}
	update_outputs(STARTCHAN, ENDCHAN, 8, 6);   //send to screen
	
	/// Display user instructions and channel headings
	DispStr(8, 1, " <<< Sinking output channels  = OUT1-OUT7   >>>");
	DispStr(8, 4, "OUT0\tOUT1\tOUT2\tOUT3\tOUT4\tOUT5\tOUT6\tOUT7");
	DispStr(8, 5, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");
	
	DispStr(8, 14, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 15, "(See instructions in sample program for complete details)");
	DispStr(8, 21, "<-PRESS 'Q' TO QUIT->");

	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		do
		{
			sprintf(display, "%sSelect output channel %d - %d and press Enter   ", BLUE, STARTCHAN, ENDCHAN);
			DispStr(8, 17, display);
			gets(tmpbuf);
			if (!strcmp(tmpbuf,"q") || !strcmp(tmpbuf,"Q"))	// check if it's the q or Q key        
			{        
  				exit(0);               
  			}
			channum = atoi(tmpbuf);
		}	while (channum < STARTCHAN && channum > ENDCHAN);

		do
		{
			sprintf(display, "%sSelect logic level 0 or 1 and press Enter   ", BLUE);
   	  	DispStr(8, 18, display);
			gets(tmpbuf);
			if (!strcmp(tmpbuf,"q") && !strcmp(tmpbuf,"Q"))	// check if it's the q or Q key        
			{        
  				exit(0);               
  			}
			output[channum] = atoi(tmpbuf);
		}	while (output[channum] != 0 && output[channum] != 1);

		/// update screen at col 8 row 6
		update_outputs(STARTCHAN, ENDCHAN, 8, 6);
		
		/// send state to output channel 
		digOut(channum, output[channum]);
		
		/// clears selection prompts
  	  	DispStr(8, 17, "                                                 ");
  	  	DispStr(8, 18, "                                                 ");
   }
}
///////////////////////////////////////////////////////////////////////////
