/**************************************************************************

	digout_bank.c
   Z-World, 2004

	This sample program is for the BL2600 series controllers.

	This program demonstrates using the digOutBank API function to
   control digital sinking type outputs to toggle LED's ON and OFF
   on the demo board provided with your kit.

   Note: Configurable I/O channels DIO00 - DIO15 are automatically
    		defaulted to a digital input for a given channel if it's
         not configured as a digital output.


	Connections:
	------------
	1. DEMO board jumper settings:
			- H1 remove all jumpers
			- H2 jumper pins 3-5
              jumper pins 4-6

	2. Connect a wire from the controller J12 GND, to the DEMO board
	   J1 GND.

	3. Connect a wire from the controller J12 +PWR to the DEMO board
	   J1 +K.

   4. Connect the following wires from the controller J1 to the DEMO
	   board J1:

	   	From DIO00 to LED1
	   	From DIO01 to LED2
	   	From DIO02 to LED3
	   	From DIO03 to LED4

	Instructions:
	------------
	1. Compile and run this program.

	2. Select a output channel via the STDIO window (per the channels you
      connected to the LED's) to toggle a LED on the demo board.

	3. To check other outputs that are not connected to the demo board,
      you can use a voltmeter to see the output change states.

**************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Enable all digital outputs (sinking only)
#define DIGOUTCONFIG	0xFFFF

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
	while( (long) (MS_TIMER - done_time) < 0 );
}


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
	auto char channels[16];
	auto int output_status, channel;
	auto int output_level;
   auto int configuration;
   auto char mask, data;

   // Initialize the controller
	brdInit();

   // Enable digital outputs 0 - 15 (sinking type outputs only)
   digOutConfig(DIGOUTCONFIG);

	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Configurable I/O...Setup as Digital Outputs  >>>");
	DispStr(8, 3, "DIO00\tDIO01\tDIO02\tDIO03\tDIO04\tDIO05\tDIO06\tDIO07");
	DispStr(8, 4, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 7, "DIO08\tDIO09\tDIO10\tDIO11\tDIO12\tDIO13\tDIO14\tDIO15");
	DispStr(8, 8, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 12, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 13, "(See instructions in sample program for complete details)");
	DispStr(8, 19, "<-PRESS 'Q' TO QUIT->");

   // Set all digital outputs 0 - 7 OFF
   digOutBank(0, 0xFF);

   // Set all digital outputs 8 - 15 OFF
   digOutBank(1, 0xFF);

    // Initialize array that contains output logic level setting
   for(channel = 0; channel < 16; channel++)
	{
   	// Set all channels to indicate outputs are OFF
		channels[channel] = 1;
   }

	//loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// update high current outputs
      data = 0x00;
      mask = 0x01;
      display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel < 8; channel++)	//output to channels 0 - 7
		{
			if((output_level = channels[channel]) == 0)
         {
          	data &= ~mask;
         }
         else
         {
          	data |= mask;
         }
         mask = mask << 1;
			sprintf(s, "%d\t", output_level);		//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(8, 5, display);							//update output status
      digOutBank(0, data);

      data = 0x00;
      mask = 0x01;
		display[0] = '\0';
		for(channel = 8; channel < 16; channel++)	//output to channels 8 - 15
		{
      	if((output_level = channels[channel]) == 0)
         {
          	data &= ~mask;
         }
         else
         {
          	data |= mask;
         }
         mask = mask << 1;
			sprintf(s, "%d\t", output_level);
			strcat(display,s);
		}
		DispStr(8, 9, display);
      digOutBank(1, data);

		// Display the channel that ths user has selected
		sprintf(display, "Select output channel 0 - 15 (Input Hex 0-F) = ");
		DispStr(8, 15, display);
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
      printf("%d", channel);


		//wait for user to select logic level or exit program
		sprintf(display, "Select logic level = ");
		DispStr(8, 16, display);
		do
		{
			output_level = getchar();
			if (output_level == 'Q' || output_level == 'q')		// check if it's the q or Q key
			{
      		exit(0);
     		}
     		output_level = output_level - 0x30;

		}while(!((output_level >= 0) && (output_level <= 1)));
      printf("%d", output_level);
     	channels[channel] = output_level;
      msDelay(1000); // Delay to see logic level display

  		//clear channel and logic level selection prompts
  		DispStr(8, 15, "                                                     ");
  		DispStr(8, 16, "                                                     ");
   }
}