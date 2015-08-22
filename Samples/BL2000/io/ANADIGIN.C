/**************************************************************************

	Samples\BL2000\io\anadigIn.c

   Z-World, 2001
   
	This sample program is for the BL2000 series controllers.
	
	This program demonstrates the analog channels being used as
	digital inputs. You will be able to see a input channel toggle
	HIGH and LOW when pressing the push buttons on the DEMO board
	that is provided in your kit.

	Test Setup:
	-----------
	1. DEMO board jumper settings:
			- H1 remove all jumpers 
			- H2 jumper pins 3-5  
              jumper pins 4-6

	2. Connect a wire from the controller J3, AGND to the DEMO board
	   J1, GND.
	    
	3. Connect 5 volts from a power supply to the DEMO board.
	     a) Power supply GND to DEMO board J1, GND.
	     b) Power supply +5 volts to DEMO board J1, +5v.

	

	Test Instructions:
	------------------
	1. Connect SW1 - SW4 from the DEMO board to any four inputs on the
	   controller, choose from IN11 - IN21. (inputs IN20 and IN21 are 
	   only available on the BL2010 and BL2030 controllers)
	2. Compile and run this program.
	3. Press any one of the DEMO board switches SW1 - SW4 and you should
	   see the input go LOW on the channel the switch is connected to.
	4. Move connections from the DEMO board to other inputs and repeat
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

	auto char s[40];
	auto char display[80];
	auto int key, reading, channel;
	
	brdInit();
	
	//Display user instructions and channel headings
	DispStr(8, 1, "<<< A/D inputs being used as Digital inputs >>>");
	DispStr(8, 2, "IN11\tIN12\tIN13\tIN14\tIN15\tIN16\tIN17\tIN18");
	DispStr(8, 3, "----\t----\t----\t----\t----\t----\t----\t----");

	// channels 20 and 21 are only available on BL2010 and BL2030 controllers 
	if(_BOARD_TYPE_ == 0x0800 || _BOARD_TYPE_ == 0x0802)
	{ 
		DispStr(8, 6, "IN19");
		DispStr(8, 7, "----");
	}
	// channels 20 and 21 are only available on BL2010 and BL2030 controllers 
	if(_BOARD_TYPE_ == 0x0801 || _BOARD_TYPE_ == 0x0803)
	{ 
		DispStr(8, 6, "IN19\tIN20\tIN21");
		DispStr(8, 7, "----\t----\t----");
	}

	DispStr(8, 12, "Connect the Demo Bd. switches to the inputs that you what to toggle.");
	DispStr(8, 13, "(See instructions in sample program for complete details)");
	DispStr(8, 15, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// display the input status for all channels
		display[0] = '\0';							//initialize for strcat function
		for(channel=11;channel<19;channel++) 	//read and update channels 11 - 18
		{
			reading = digIn(channel);				//read channel
			sprintf(s, "%d\t", reading);			//format reading in memory				
			strcat(display,s);						//append reading to display string
		}
		DispStr(8, 4, display);						//update channels 11 - 18 input status 

		display[0] = '\0';							
		for(channel=19;channel<20;channel++)   //read and update channel 19
		{
			reading = digIn(channel);				
			sprintf(s, "%d\t", reading);			
			strcat(display,s);
		}
		DispStr(8, 8, display);

		// channels 20 and 21 are only available on BL2010 and BL2030 controllers 
		if(_BOARD_TYPE_ == 0x0801 || _BOARD_TYPE_ == 0x0803)
		{
			display[0] = '\0';
			for(channel = 20; channel < 22; channel++) //read and update channels 20 & 21
			{
				reading = digIn(channel);
				sprintf(s, "%d\t", reading);
				strcat(display,s);
			}
			DispStr(16, 8, display);
		}
			
		if(kbhit())
		{
			// check if the user is exiting the program
			key = getchar();
			if (key == 'Q' || key == 'q')		// Check if it's the q or Q key        
			{
				while(kbhit()) getchar();        
      		exit(0);               
     		}
     	}
   }
}
