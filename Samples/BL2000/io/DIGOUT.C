/**************************************************************************

	Samples\BL2000\IO\digOut.c

   Z-World, 2001
	This sample program is for the BL2000 series controllers.
	
	This program demonstrates the use of the high current outputs. With
	using the provided DEMO board in your kit you can see a LED Toggle 
	ON/OFF via a high current output.
	
	Test Setup:
	-----------
	1. On the controller add a wire between J1, +RAW and +K.
	
	2. Add a wire from the controller J1, GND to the DEMO board J1, GND.
	
	3. Add a wire from the controller J1, +RAW to the DEMO board J1, +5V.
	   
   This is going to power the LED's on the DEMO board. And with a 3.3k
   ohm resistor in series with the LED, the LED's can withstand the full
   input voltage range for a BL2000 series controller. 
	  
	
	
	Test Instructions:
	------------------
	1. Connect LED1 - LED4 from the DEMO board to any 4 outputs on the 
	   controller, choose from OUT0 - OUT9.
	2. Compile and run this program.
	3. The program will prompt you to select the output channel that you
	   want to toggle. Select the desired channel and then the logic level
	   you want to set it to. When done you'll see the LED connected to
	   the output selected change states.  
	4. Move connections from the DEMO board to other outputs and repeat
	   steps 1 - 4.

**************************************************************************/
#class auto


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
	

	brdInit();
		
	//Display user instructions and channel headings
	DispStr(8, 1, " <<< High Current Outputs >>>");
	DispStr(8, 2, "OUT0\tOUT1\tOUT2\tOUT3\tOUT4\tOUT5\tOUT6\tOUT7");
	DispStr(8, 3, "----\t----\t----\t----\t----\t----\t----\t----");
	
	DispStr(8, 6, "OUT8\tOUT9");
	DispStr(8, 7, "----\t----");

	DispStr(8, 12, "Connect the Demo Bd. LED's to the outputs that you want to demo.");
	DispStr(8, 13, "(See instructions in sample program for complete details)");
	DispStr(8, 19, "<-PRESS 'Q' TO QUIT->");

	//Preset all high current outputs to a logic high
	for(channel = 0; channel < 10; channel++)
	{
		channels[channel] = 1;
		digOut(channel, 1);
	}

	
	//loop until user presses the upper/lower case "Q" key
	for(;;)
	{
		// update high current outputs
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel < 8; channel++)	//output to channels 0 - 7 
		{
			output_level = channels[channel];		//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);			//format logic level for display
			strcat(display,s);							//add to display string
		}
		DispStr(8, 4, display);							//update output status 

		
		display[0] = '\0';
		for(channel = 8; channel < 10; channel++)	//output to channels 8 & 9
		{
			output_level = channels[channel];		//output logic level to channel
			digOut(channel, output_level);
			sprintf(s, "%d\t", output_level);
			strcat(display,s);
		}
		DispStr(8, 8, display);

		// wait for user to make output channel selection or exit program
		sprintf(display, "Select output channel(0 - 9) = ");
		DispStr(8, 15, display);
     	do
		{
			channel = getchar();
			if (channel == 'Q' || channel == 'q')		// check if it's the q or Q key        
			{        
      		exit(0);               
     		}
     		channel = channel - 0x30;
			
		}while(!((channel >= 0) && (channel <= 9)));
		sprintf(display, "Select output channel(0 - 9) = %d", channel);
		DispStr(8, 15, display);

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
		sprintf(display, "Select logic level = %d", output_level);
     	DispStr(8, 16, display);
     	channels[channel] = output_level;

  		//clear channel and logic level selection prompts 
  		DispStr(8, 15, "                                                ");
  		DispStr(8, 16, "                                                ");
   }
}
