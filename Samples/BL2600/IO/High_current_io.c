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

	high_current_io.c

	This sample program is for the BL2600 series controllers.

	This program demonstrates the use of the high-current outputs by
   toggling LED's ON and OFF on the demo board provided with your kit.

	Attention!!!
	------------
	1. The high-current output HOUT0 is configured for sourcing to
      provide power to the DEMO board.

   2. Outputs HOUT1-HOUT2 are configured to demonstrate tristate
      operation for control of LEDS on the Demo board.

   3. Output HOUT3 is configured to demonstrate sinking operation
      for control of a LED on the Demo board.


   High-Current +HK0, +HK1, +HK2 and + HK3 Power Connections
   -----------------------------------------------------
   1. Connect J12 pin 1 (+DIN) to +HK0, +HK1, +HK2 and +HK3
      located on J16 pins 2, 5, 8, and 11, respectively.

   Note: Power connections (HK0 - HK3) are only needed for sourcing
         and trisate output operation, outputs configured for sinking
         operation don't require its corresponding +HK power terminal
         connected.


   Connections from BL2600 to Demo Board:
	--------------------------------------
	1. DEMO board jumper settings:
			- H1 remove all jumpers
			- H2 jumper pins 3-5
              jumper pins 4-6

	2. Connect a wire from the controller J12 GND, to the DEMO board
	   J1 GND.

	3. Connect LED1 - LED4 from the DEMO board to digital outputs
   	HOUT1 - HOUT3.

   4. Connect HOUT0 to +K on the Demo board. (this is going to be used
   	for the power supply on the demo board)

	Instructions:
	------------
	1. Compile and run this program.

	2. The program will prompt you for your channel selection, select
	   Output Channel HOUT0.

	3. After you have made the channel selection you'll be prompted to
	   select the logic level, set the HOUT0 channel to a high logic
	   level.

	4. Select output channel HOUT1 - HOUT3 via the STDIO window to
      toggle a LED on the demo board.

**************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////


nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

void main()
{
	auto char s[128];
	auto char display[128];
	auto char channels[16];
	auto int output_status, channel;
	auto int output_level;
	auto unsigned int outputChannel;

   // Initialize the controller
	brdInit();

   // Configure the high-current outputs for the following operation:
   // HOUT0............Sourcing operation
   // HOUT1-HOUT2......Tristate operation
   // HOUT3............Sinking operation
   digHoutConfig(0x01);			// Set Hout0 Sourcing and Hout3 Sinking
   digHTriStateConfig(0x06);  // Set Hout1 & Hout2 for Tristate

	// Display user instructions and channel headings
	DispStr(8, 1, " <<< Sourcing output channel   = HOUT0         >>>");
	DispStr(8, 2, " <<< Tristate output channels  = HOUT1-HOUT2   >>>");
   DispStr(8, 3, " <<< Sinking output channel    = HOUT3         >>>");
	DispStr(8, 5, "HOUT0\tHOUT1\tHOUT2\tHOUT3");
	DispStr(8, 6, "-----\t-----\t-----\t-----");

	DispStr(8, 9, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 10, "(See instructions in sample program for complete details)");
	DispStr(8, 16, "<-PRESS 'Q' TO QUIT->");

	// Set the channel array to desired default values.
   // HOUT0.............Set to 0 for sourcing transistor to be OFF.
   // HOUT1-HOUT2.......Set to 2 for both sinking/sourcing transistors to be OFF.
   // HOUT3.............Set to 1 for sinking transistor to be OFF.

   channels[0] = 0;	// Set value for Hout0 Sourcing Output to be OFF
   channels[1] = 2;  // Set value for Hout1 Tristate Output to be OFF
   channels[2] = 2;  // Set value for Hout2 Tristate Output to be OFF
   channels[3] = 1;  // Set value for Hout3 Sinking  Output to be OFF

	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// Update outputs
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel <= 3; channel++)	//output to channels 0 - 4
		{
			output_level = channels[channel];		//output logic level to channel
         if(channel == 1 || channel == 2)
         	digHoutTriState(channel, output_level);
         else
         	digHout(channel, output_level);
			sprintf(s, "%d\t", output_level);		//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(8, 7, display);							//update output status


		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel 0 - 3 = ");
		DispStr(8, 12, display);
     	do
		{
			channel = getchar();
			if (channel == 'Q' || channel == 'q')		// check if it's the q or Q key
			{
      		exit(0);
     		}
		}while(!isxdigit(channel));

		// Convert the ascii hex value to a interger
		if( channel >= '0' && channel <='7')
		{
			channel = channel - 0x30;
		}

      // Display the channel that ths user has selected
      printf("%d", channel);

      // Display proper logic level range for given ouput channel
      // configuration.
      if(channel == 0 || channel == 3)
      {
         // Sinking or Soucring logic level selection
			sprintf(display, "Select logic level (0 - 1) = ");
      }
      else
      {
         // Tristate logic level selection
			sprintf(display, "Select logic level (0 - 2) = ");
     	}
      DispStr(8, 13, display);
		while(1)
		{
			output_level = getchar();
			if (output_level == 'Q' || output_level == 'q')		// check if it's the q or Q key
			{
      		exit(0);
     		}
     		output_level = output_level - 0x30;

         // Check for valid logic level for given output type
         if(channel == 0 || channel == 3)
         {
				if((output_level >= 0) && (output_level <= 1))
            	break;
         }
         else if(channel == 1 || channel == 2)
         {
            if((output_level >= 0) && (output_level <= 2))
            	break;
        	}
      }
     	channels[channel] = output_level;

      printf("%d", output_level);
      msDelay(1000); // Delay to see logic level display

  		// Clear channel and logic level selection prompts
  		DispStr(8, 12, "                                                     ");
  		DispStr(8, 13, "                                                     ");
   }
}