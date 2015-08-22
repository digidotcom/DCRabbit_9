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
/********************************************************************

	controlled.c

	This program is used with RCM3300 series controllers and
	prototyping boards.

	Description
	===========
	This sample program demonstrates controlling port outputs
	from STDIO by toggling LED's on the prototyping board.

	Instructions
	============
	1. Compile and run this program.

	2. The program will prompt you for an LED, DS3, DS4, DS5 or DS6
    	selection. Make a selection from your PC keyboard.

	3. After you have made the LED selection you'll be prompted to
	   select an ON or OFF state. Logic "1" will light up the LED.

*********************************************************************/
#class auto
#use rcm33xx.lib		//sample library to use with this application

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6

#define ON 1
#define OFF 0


////////
// Set the STDIO cursor location and display a string
////////
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////
main()
{

	auto char s[128];
	auto char display[128];
	auto char channels[8];
	auto int output_status, channel;
	auto int output_level;
	auto unsigned int outputChannel;

	brdInit();  //must do to initialize board

   //initialize led's to off state
	for(channel = DS3; channel <= DS6 ; channel++)
	{
		channels[channel] = OFF;
		ledOut(channel, OFF);
	}

	// Display user instructions and channel headings
	DispStr(8, 2, "<<< Proto-board LED's   >>>");
	DispStr(8, 4, "DS2\tDS3\tDS4\tDS5");
	DispStr(8, 5, "-----\t-----\t-----\t-----");

	DispStr(8, 10, "From PC keyboard:");
	DispStr(8, 21, "< Press 'Q' To Quit >");

	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// Update high current outputs
		display[0] = '\0';								//initialize for strcat function
		for(channel = DS3; channel <= DS6; channel++)	//output to DS1 and DS2 only
		{
			output_level = channels[channel];		//output logic level to channel
			ledOut(channel, output_level);
			sprintf(s, "%s\t", output_level?"ON ":"OFF");		//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(8, 6, display);							//update output status

		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select 3=DS3, 4=DS4, 5=DS5, 6=DS6 to toggle LED's");
		DispStr(8, 12, display);
     	do
		{
			channel = getchar();
			if (channel == 'Q' || channel == 'q')		// check if it's the q or Q key
			{
      		exit(0);
     		}
     		channel = channel - 0x30;		// convert ascii to integer
		} while (!((channel >= DS3) && (channel <= DS6)));

		// Display the channel that the user has selected
		sprintf(display, "Selected DS%d to toggle                          ", channel);
		DispStr(8, 12, display);

		// Wait for user to select logic level or exit program
		sprintf(display, "Select 1=ON or 0=OFF");
		DispStr(8, 13, display);
		do
		{
			output_level = getchar();
			if (output_level == 'Q' || output_level == 'q')		// check if it's the q or Q key
			{
      		exit(0);
     		}
     		output_level = output_level - 0x30;
		} while (!((output_level >= 0) && (output_level <= 1)));
		sprintf(display, "Selected %s         ", output_level?"OFF":"ON ");
     	DispStr(8, 13, display);
     	channels[channel] = output_level;

  		// Clear channel and logic level selection prompts
  		DispStr(8, 12, "                                                  ");
  		DispStr(8, 13, "                                                  ");
   }
}
///////////////////////////////////////////////////////////////////////////



