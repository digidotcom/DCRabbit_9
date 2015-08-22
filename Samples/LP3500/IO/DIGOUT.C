/**************************************************************************

	digOut.c
   Z-World, 2002

	This sample program is for the LP3500 series controllers.

	This program demonstrates the use of the outputs configured	as SINKING
	and SOURCING type outputs. The sample program requires the use
	the DEMO board that was provided in your development kit so you can see
	the LED's toggle ON/OFF via the high-current outputs.

	Connections
	===========
	Connect +K to external power source.

	When the controller is plugged into to the demo board the
	following connections are readily available.

	Controller		Demo Board
	----------		----------
			OUT0 <->	DS1
			OUT1 <->	DS2
			OUT2 <->	DS3
			OUT3 <->	DS4

	Instructions
	============
	1. Compile and run this program.

	2. The program will prompt you for your channel selection, select
	   Output Channel OUT0.

	3. After you have made the channel selection you'll be prompted to
	   select the logic level, set the OUT0 channel to a high logic
	   level.

	4. At this point your ready to start toggling the LEDS, DS1-DS4, by
		selecting channels OUT1 - OUT3, and changing the logic level.

	5. To check other outputs OUT8 and OUT9, use a voltmeter to verify
		that the channels have changed to the level that you have set it to.

**************************************************************************/
#class auto


//------------------------------------------------------------------------
// Set to initially disable outputs OUT9-OUT0.
//------------------------------------------------------------------------
#define OUTCONFIG	0x00FF

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

   brdInit();

	// Display user instructions and channel headings
	DispStr(8, 1, " <<< Sinking output channels  = OUT1-OUT7   >>>");
	DispStr(8, 2, " <<< Sourcing output channel  = OUT8-OUT9   >>>");
	DispStr(8, 4, "OUT0\tOUT1\tOUT2\tOUT3\tOUT4\tOUT5\tOUT6\tOUT7");
	DispStr(8, 5, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 9, "OUT8\tOUT9");
	DispStr(8, 10, "-----\t-----");

	DispStr(8, 14, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 15, "(See instructions in sample program for complete details)");
	DispStr(8, 21, "<-PRESS 'Q' TO QUIT->");

	// Set the channel array to reflect the output channel default value
	outputChannel = OUTCONFIG;
	for(channel = 0; channel <=9 ; channel++)
	{
		// Set outputs to be OFF, for both sinking
		// and sourcing type outputs.
		channels[channel] = outputChannel & 0x0001;
		outputChannel = outputChannel >> 1;
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
		for(channel = 8; channel <= 9; channel++)	//output to channels 8 - 9
		{
			output_level = channels[channel];			//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);
			strcat(display,s);
		}
		DispStr(8, 11, display);

		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel 0 - 9 (Input Hex 0-F) = ");
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

		// Display the channel that ths user has selected
		sprintf(display, "Select output channel 0 - 9  = %d", channel);
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

		} while(!((output_level >= 0) && (output_level <= 1)));

		sprintf(display, "Select logic level = %d", output_level);
     	DispStr(8, 18, display);
     	channels[channel] = output_level;

  		// Clear channel and logic level selection prompts
  		DispStr(8, 17, "                                                  ");
  		DispStr(8, 18, "                                                  ");
   }
}
///////////////////////////////////////////////////////////////////////////

