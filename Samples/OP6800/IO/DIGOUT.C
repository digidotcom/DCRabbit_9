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

	This sample program is for the OP6800 series controllers.
	
	This sample program demonstrates the use of the sinking type high-
	current outputs by toggling LED's on the OP6800 Demo Board that is
	provided in your development kit.

	Here's a list of what digital outputs are being used to control the  
	LED's on the OP6800 demo board. 

	OP6800 Output		LED on the OP6800 Demo Board
	-------------		----------------------------
	OUT07 				DS1
	OUT08 				DS2
	OUT09 				DS3
	OUT10 				DS4
	
	Test Setup:
	-----------
	1. Connect a +12v power source to the OP6800 Demo Board connector J3/J5
	   as follows:

	   PS     	OP6800 Demo Board
	   --			-----------------
	   +12v		+RAW Terminal  
	   GND      GND Terminal
	

	Test Instructions:
	------------------
	1. Compile and run this program.

	2. The program will prompt you for your channel selection...select
	   Output Channel OUT07 using your PC keyboard.

	3. After you have made the channel selection you'll be prompted to
	   select the logic level....set channel OUT07 to a logic LOW which
	   will light the LED up. 

	4. At this point your ready to start toggling the other LEDS by
	   selecting channels OUT08 - OUT10, and changing the logic level.
	   (Use PC keyboard to do selections) 

	5. To check other outputs use a voltmeter to verify that the channels
	   have changed to the logic level that you have set it to.

**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.

// Set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////

void main()
{

	auto char s[128];
	auto char display[128];
	auto char channels[16];
	auto int output_status, channel;
	auto int output_level;
	auto unsigned int outputChannel;	

   brdInit();					// Required for OP6800 series boards
	
	// Display user instructions and channel headings
	DispStr(8, 2, " <<< Sinking output channels are OUT00-OUT10   >>>");
	DispStr(8, 4, "OUT00\tOUT01\tOUT02\tOUT03\tOUT04\tOUT05\tOUT06\tOUT07");
	DispStr(8, 5, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");
	
	DispStr(8, 9,  "OUT08\tOUT09\tOUT10");
	DispStr(8, 10, "-----\t-----\t-----");

	DispStr(8, 14, "Use digital outputs OUT07-OUT10 to toggle LED's on OP6800 demo board.");
	DispStr(8, 15, "(See instructions in sample program for complete details)");
	DispStr(8, 21, "<-PRESS 'Q' TO QUIT->");
	DispStr(8, 24, "Note: All key inputs are done from the PC keyboard");
	
	for(channel = 0; channel < 11; channel++)
	{
		channels[channel] = 1;	// Indicate sinking type output is OFF
		digOut(channel, 1);
	}
	
	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// Update high current outputs
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel <= 7; channel++)	//output to channels 0 - 7 
		{
			output_level = channels[channel];		//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);		//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(8, 6, display);							//update output status 

		display[0] = '\0';
		for(channel = 8; channel <= 10; channel++)	//output to channels 8 - 10
		{
			output_level = channels[channel];			//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);
			strcat(display,s);
		}
		DispStr(8, 11, display);

		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel 0 - 10 (Input Hex 0-A) = ");
		DispStr(8, 17, display);
     	do
		{
			channel = getchar();
			if (channel == 'Q' || channel == 'q')		// check if it's the q or Q key        
			{        
      		exit(0);               
     		}
		}while(!isxdigit(channel));

		// Convert the ascii hex value to a interger
		if( channel >= '0' && channel <='9')
		{
			channel = channel - 0x30;
		}
		else
		{
			channel = tolower(channel);
			channel = (channel - 'a') + 10;
		}

		// Display the channel that ths user has selected 
		sprintf(display, "Select output channel 0 - 10 (Input Hex 0-A) = %d", channel);
		DispStr(8, 17, display);


		// Wait for user to select logic level or exit program
		sprintf(display, "Select logic level = ");
		DispStr(8, 18, display);
		do
		{
			output_level = getchar();
			if (output_level == 'Q' || output_level == 'q')		// check if it's the q or Q key        
			{        
      		exit(0);               
     		}
     		output_level = output_level - 0x30;
				
		}while(!((output_level >= 0) && (output_level <= 1)));
		sprintf(display, "Select logic level = %d", output_level);
     	DispStr(8, 18, display);
     	channels[channel] = output_level;

  		// Clear channel and logic level selection prompts 
  		DispStr(8, 17, "                                                  ");
  		DispStr(8, 18, "                                                  ");
   }

}
