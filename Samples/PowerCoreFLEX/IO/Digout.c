/**************************************************************************

	digOut.c
   Z-World, 2004

   This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	This program demonstrates controlling sinking and sourcing type
   digital outputs.

   Test setup for digital outputs:
   -------------------------------

   Out0...1. Connect a load resistor (~100 ohms) from +5v (J4 pin 6) to
          digital sinking output0 (J2, pin 3).

   Out1...1. Connect a load resistor (~100 ohms) from +5v (J4 pin 6) to
   		 digital sinking output1 (J2, pin 4).

   Out2...1. Connect a load resistor (~100 ohms) from GND (J2 pin 2) to
          digital sourcing output2 (J2, pin 5).
          2. Connect a wire from +K (J2 pin 1) to +5v (J4 pin 6).

   Out3...1. Connect a load resistor (~100 ohms) from (J2 pin 2) to
          digital sourcing output3 (J2, pin 6).
          2. Connect a wire from +K (J2 pin 1) to +5v (J4 pin 6).

	Instructions:
	------------
	1. Compile and run this program.
	2. Select a output channel and logic level via the STDIO window to
      toggle the digital output.
   3. Verify logic level with voltmeter.

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

	auto char s[128];
	auto char display[128];
	auto char channels[10];
	auto int output_status, channel;
	auto int output_level;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	//Display user instructions and channel headings
	DispStr(8, 1, " <<< High Current Outputs >>>");
	DispStr(10, 3, "OUT0\tOUT1\tOUT2\tOUT3");
	DispStr(10, 4, "----\t----\t----\t----");

	DispStr(8, 11, "<-PRESS 'Q' TO QUIT->");

	//Preset the digital outputs to a logic high
	for(channel = 0; channel < 4; channel++)
	{
   	if(channel < 2)
		{
      	// Set sinking channel to logic 1, channel = OFF
      	channels[channel] = 1;
			digOut(channel, 1);
		}
      else
      {
      	// Set sourcing channel to logic 0, channel = OFF
         channels[channel] = 0;
			digOut(channel, 0);
      }
   }


	//loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// update digital outputs
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel < 4; channel++)	//output to channels 0 - 3
		{
			output_level = channels[channel];		//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);			//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(10, 5, display);							//update output status

		// wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel(0 - 3) = ");
		DispStr(8, 8, display);
     	do
		{
			channel = getchar();
			if (channel == 'Q' || channel == 'q')		// check if it's the q or Q key
			{
      		exit(0);
     		}
     		channel = channel - 0x30;

		}while(!((channel >= 0) && (channel <= 3)));
		sprintf(display, "Select output channel(0 - 3) = %d", channel);
		DispStr(8, 8, display);

		//wait for user to select logic level or exit program
		sprintf(display, "Select logic level = ");
		DispStr(8, 9, display);
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
     	DispStr(8, 9, display);
     	channels[channel] = output_level;

  		//clear channel and logic level selection prompts
  		DispStr(8, 8, "                                                ");
  		DispStr(8, 9, "                                                ");
   }
}