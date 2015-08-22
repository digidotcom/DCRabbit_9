/**************************************************************************

	controlled.c
 	Z-World, 2002
 	
	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	The sample library, \Samples\RCM3000\rcm3000.lib, contains a
	brdInit() function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This sample program demonstrates the controlling port outputs
	from STDIO by toggling LED's on the prototyping board.  The LEDs
	are access by the "master" module.

	(Adapted from \Samples\BL2000\digout.c)

	Output control		LED on the proto-board
	--------------		----------------------
	Port G bit 6		DS1
	Port G bit 7		DS2
	
	Instructions
	============
	1. Compile and run this program.

	2. The program will prompt you for an LED, DS1 or DS2 selection.
		Make a selection from your PC keyboard.

	3. After you have made the LED selection you'll be prompted to
	   select an ON or OFF state. Logic LOW will light up the LED. 

**************************************************************************/
#class auto

#use rcm3000.lib 		//sample library used for this demo

#define DS1 1			//led, port G bit 6
#define DS2 2			//led, port G bit 7

////////
// output function
////////
void digOut(int channel, int onoff)
{
	// works for port G bits 6 and 7 only
	BitWrPortI(PGDR, &PGDRShadow, onoff, channel+5);
}

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
void main()
{

	auto char s[128];
	auto char display[128];
	auto char channels[8];
	auto int output_status, channel;
	auto int output_level;
	auto unsigned int outputChannel;	

	brdInit();				//initialize board for this demo
	
	// Display user instructions and channel headings
	DispStr(8, 2, "<<< Proto-board LED's   >>>");
	DispStr(8, 4, "DS1\tDS2");
	DispStr(8, 5, "-----\t-----");
	
	DispStr(8, 10, "From PC keyboard:");
	DispStr(8, 21, "< Press 'Q' To Quit >");
	
	for(channel = DS1; channel <=DS2 ; channel++)
	{
		channels[channel] = 1;		// Indicate output is OFF
		digOut(channel, 1);
	}
	
	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// Update high current outputs
		display[0] = '\0';								//initialize for strcat function
		for(channel = DS1; channel <= DS2; channel++)	//output to DS1 and DS2 only
		{
			output_level = channels[channel];		//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%s\t", output_level?"OFF":"ON");		//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(8, 6, display);							//update output status 

		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select 1=DS1 or 2=DS2 to toggle LED's");
		DispStr(8, 12, display);
     	do
		{
			channel = getchar();
			if (channel == 'Q' || channel == 'q')		// check if it's the q or Q key        
			{        
      		exit(0);               
     		}
     		channel = channel - 0x30;		// convert ascii to integer
		} while (!((channel >= DS1) && (channel <= DS2)));

		// Display the channel that the user has selected
		sprintf(display, "Selected DS%d to toggle               ", channel);
		DispStr(8, 12, display);

		// Wait for user to select logic level or exit program
		sprintf(display, "Select 1=OFF or 0=ON");
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
		sprintf(display, "Selected %s         ", output_level?"OFF":"ON");
     	DispStr(8, 13, display);
     	channels[channel] = output_level;

  		// Clear channel and logic level selection prompts 
  		DispStr(8, 12, "                                                  ");
  		DispStr(8, 13, "                                                  ");
   }

}
