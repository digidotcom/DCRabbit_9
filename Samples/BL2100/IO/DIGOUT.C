/**************************************************************************

	digOut.c

   Z-World, 2001
	This sample program is for the BL2100 series controllers.

	This program demonstrates the use of the high-current outputs configured
	as SINKING and SOURCING type outputs. The sample program requires the use
	the DEMO board that was provided in your development kit so you can see
	the LED's toggle ON/OFF via the high-current outputs.

	Attention!!!
	------------
	The high-current output OUT00 is being use to provide the power source to
	the DEMO board. And the sinking type outputs OUT01-OUT04 are being used to
	control the LEDS.

	Test Setup:
	-----------
	1. DEMO board jumper settings:
			- H1 remove all jumpers
			- H2 jumper pins 3-5
              jumper pins 4-6

	2. Connect a wire from the controller J4 GND, to the DEMO board
	   J1 GND.

	3. Connect a wire on the controller from J4 +RAW to J7 +K1 and +K2.

	4. Connect a wire from the controller J7 OUT00, to the DEMO board
	   J1 +K.

	5. Connect the following wires from the controller J7 to the DEMO
	   board J1:

	   	From OUT01 to LED1
	   	From OUT02 to LED2
	   	From OUT03 to LED3
	   	From OUT04 to LED4



   Note: This is going to power the LED's on the DEMO board. With the 3.3k
         ohm resistor in series with the LED, the LED's can withstand the
         full input voltage range for a BL2100 series controller.


	Test Instructions:
	------------------
	1. Connect LED1 - LED4 from the DEMO board to any 4 outputs on the
	   controller as in Test Setup above or choose from OUT01 - OUT15.

	2. Compile and run this program.

	3. The program will prompt you for your channel selection, select
	   Output Channel OUT00.

	4. After you have made the channel selection you'll be prompted to
	   select the logic level, set the OUT00 channel to a high logic
	   level.

	5. At this point your ready to start toggling the LEDS by selecting
	   selecting channels OUT01 - OUT04, and changing the logic level.

	6. To check other outputs use a voltmeter to verify that the channels
	   have changed to the level that you have set it to.

**************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

//------------------------------------------------------------------------
//
// Set output OUT15-OUT01 to be sinking type drivers.
// Set output OUT00 to be a sourcing type driver.
//
// Note: The importance of setting the driver type, is to assure that
//       when the high-current circuit is activated, your application
//       circuit is initially OFF.
//------------------------------------------------------------------------
#define HICONFIG	0x0001

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

   brdInit();					// Required for BL2100 series boards
	digOutConfig(HICONFIG); // Configure high-current outputs

	// Display user instructions and channel headings
	DispStr(8, 1, " <<< Sourcing output channel  = OUT00         >>>");
	DispStr(8, 2, " <<< Sinking output channels  = OUT01-OUT15   >>>");
	DispStr(8, 4, "OUT00\tOUT01\tOUT02\tOUT03\tOUT04\tOUT05\tOUT06\tOUT07");
	DispStr(8, 5, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 9, "OUT08\tOUT09\tOUT10\tOUT11\tOUT12\tOUT13\tOUT14\tOUT15");
	DispStr(8, 10, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 14, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 15, "(See instructions in sample program for complete details)");
	DispStr(8, 21, "<-PRESS 'Q' TO QUIT->");

	// Set the channel array to reflect the output channel default value
	outputChannel = HICONFIG;
	for(channel = 0; channel < 16; channel++)
	{
		// Set the high-current outputs to be OFF, for both sinking
		// and sourcing type outputs.
		if(outputChannel & 0x0001)
		{
			channels[channel] = 0;	// Indicate sourcing type output is OFF
		}
		else
		{
			channels[channel] = 1;	// Indicate sinking type output is OFF
		}
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
		for(channel = 8; channel <= 15; channel++)	//output to channels 8 - 15
		{
			output_level = channels[channel];			//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);
			strcat(display,s);
		}
		DispStr(8, 11, display);

		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel 0 - 15 (Input Hex 0-F) = ");
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
		sprintf(display, "Select output channel 0 - 15 (Input Hex 0-F) = %d", channel);
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

