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

	tristate.c

	This sample program is for the OP7200 series controllers.
	
	This program demonstrates the use of the high-current outputs configured
	as SINKING, SOURCING and TRISTATE type outputs. The sample program requires
	the use the DEMO board that was provided in your development kit so you can
	see the LED's toggle ON/OFF via the high-current outputs.

	Attention!!!
	------------
	The high-current output OUT0 is being used to provide power to the DEMO
	board. And outputs OUT1-OUT4(configured as TRISTATE drivers) are being
	used to control the LEDS. 

	Test Setup:
	-----------
	1. DEMO board jumper settings:
			- H1 remove all jumpers 
			- H2 jumper pins 3-5  
              jumper pins 4-6             	

	2. Connect a wire from the controller J3 GND, to the DEMO board
	   J1 GND.

	   Note: Input -PWR isn't digital GND due to the bridge rectifier
            that's in the power input circuit. 
	   
	3. Connect a wire on the controller from +PWR to +K on connector J3.

	4. Connect a wire from the controller J3 OUT0, to the DEMO board
	   J1 +K.

	5. Connect the following wires from the controller J3 to the DEMO
	   board J1:
	   
	   	From OUT1 to LED1
	   	From OUT2 to LED2
	   	From OUT3 to LED3
	   	From OUT4 to LED4
	   
   Note: This is going to power the LED's on the DEMO board. With the 3.3k
         ohm resistor in series with the LED, the LED's can withstand the
         full input voltage range for a OP7200 series controller. 
	  
	
	Test Instructions:
	------------------
	1. Connect LED1 - LED4 from the DEMO board to any 4 outputs on the 
	   controller as in Test Setup above or choose from OUT1 - OUT7.

	2. Compile and run this program.

	3. The program will prompt you for your channel selection, select
	   Output Channel OUT00. After making the channel selection you'll
	   be prompted to select the logic level, set the channel OUT00 to
	   a logic high level to provide power to the DEMO Board.

	4. At this point your ready to start toggling the LEDS by selecting
	   selecting channels OUT1 - OUT4, and changing the logic level. 

	5. To check other outputs use a voltmeter to verify that the channels
	   have changed to the level that you have set it to.

**************************************************************************/
#class auto

#define DIGOUTCONFIG		0x01	
#define TRISTATECONFIG	0x7E

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
	auto int mask, valid;

   brdInit();							// Required for controllers

   //------------------------------------------------------------------------
	//
	// Set output OUT7 to be a sinking type driver.
	// Set output OUT6-OUT1 to be tristate type drivers.
	// Set output OUT0 to be a sourcing type driver.
	//
	// Note: The importance of setting the driver type, is to assure that
	//       when the high-current circuit is activated, your application
	//       circuit is initially OFF.
	//------------------------------------------------------------------------ 
	// Configure the high-current outputs to be as follows:
	// D7    Set for Sinking  bit = 0 	
	// D6    Set for TriState bit = X (Don't care) 
	// D5 	Set for TriState bit = X (Don't care)
	// D4 	Set for TriState bit = X (Don't care)
	// D3 	Set for TriState bit = X (Don't care)
	// D2 	Set for TriState bit = X (Don't care)
	// D1 	Set for TriState bit = X (Don't care)
	// D0 	Set for Sourcing bit = 1 	 
	digOutConfig(DIGOUTCONFIG);

	
	// Set bits D6-D1 high for Digital OUT6-OUT1 to be tristate outputs
	// Configure the high-current outputs to be as follows:
	// D7    Set for No Tristate bit = 0 	
	// D6    Set for TriState    bit = 1  
	// D5 	Set for TriState    bit = 1 
	// D4 	Set for TriState    bit = 1 
	// D3 	Set for TriState    bit = 1 
	// D2 	Set for TriState    bit = 1 
	// D1 	Set for TriState    bit = 1 
	// D0 	Set for No Tristate bit = 0 	 
	digTriStateConfig(TRISTATECONFIG);

	//--------------------------------------------------------------------------
	// Note: All High current outputs are in a high impedance state
	//       until the first digOut or digOutTriState function access,
	//       at which time it will be set to value that you have set
	//       it to.       
	//--------------------------------------------------------------------------
	// Display user instructions and channel headings
	DispStr(8, 1, " <<< Sourcing output channel  = OUT0      >>>");
	DispStr(8, 2, " <<< TriState output channels = OUT1-OUT6 >>>");
	DispStr(8, 3, " <<< Sinking output channel   = OUT7      >>>");
	DispStr(8, 5, "OUT0\tOUT1\tOUT2\tOUT3\tOUT4\tOUT5\tOUT6\tOUT7");
	DispStr(8, 6, "----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");
	
	DispStr(8, 9, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 10, "(See instructions in sample program for complete details)");
	DispStr(8, 12, "<-PRESS 'Q' TO QUIT->");

	// Initialize array for driver to be OFF for their given output configuration
	mask = 0x01;
	for(channel = 0; channel <= 7; channel++)	//output to channels 0 - 7 
	{
		if(mask & TRISTATECONFIG)
		{
			// Initialize tristate driver to be OFF
			channels[channel] = 2;		
		}
		else if(mask & DIGOUTCONFIG) 
		{
			// Initialize Sourcing driver to be OFF
			channels[channel] = 0;		
		}
		else
		{
			// Initialize Sinking driver to be OFF 
			channels[channel] = 1;		
		}
		mask = mask << 1;
	}
	
	// Loop until user presses the upper/lower case "Q" key
	for(;;)
	{
	
		// Update high current outputs
		mask = 0x01;
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel <= 7; channel++)	//output to channels 0 - 7 
		{
			if(mask & TRISTATECONFIG)
			{
				output_level = channels[channel];		//output logic level to channel
				digOutTriState(channel, output_level);
			}
			else
			{
				output_level = channels[channel];		//output logic level to channel
				digOut(channel, output_level);
			}
			sprintf(s, "%d\t", output_level);		//format logic level for display
			strcat(display,s);							//add to display string
			mask = mask << 1;
		}
		DispStr(8, 7, display);							//update output status 

		
		// Wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel 0 - 7 (Input 0-7) = ");
		DispStr(8, 14, display);
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
		sprintf(display, "Select output channel 0 - 7 (Input 0-7) = %d", channel);
		DispStr(8, 14, display);


		// Wait for user to select logic level or exit program
		sprintf(display, "Select logic level 0-2 (Opt 2 valid for tristate only) = ");
		DispStr(8, 15, display);
		valid = FALSE;	
		do
		{
			output_level = getchar();
			if (output_level == 'Q' || output_level == 'q')		// check if it's the q or Q key        
			{        
      		exit(0);               
     		}
     		output_level = output_level - 0x30;
     		if(output_level == 0 || output_level == 1)
     		{
				valid = TRUE;
     		}
     		else if(output_level == 2)
     		{
				mask = 0x01;
				mask = mask << channel;				
				if(mask & TRISTATECONFIG)
				{
					valid = TRUE;
				}
			}     		
				
		}while(!valid);
     	channels[channel] = output_level;

  		// Clear channel and logic level selection prompts 
  		DispStr(8, 14, "                                                         ");
  		DispStr(8, 15, "                                                         ");
   }
}
