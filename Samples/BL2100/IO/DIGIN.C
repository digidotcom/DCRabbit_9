/**************************************************************************

	Samples\BL2100\IO\digIn.c

   Z-World, 2001
	This sample program is for the BL2100 series controllers.
	
	This program demonstrates the use of the digital inputs. With
	using the provided DEMO board in your kit you can see a input
	channel toggle from HIGH to LOW when pressing a push button on
	the DEMO board.
	
	Test Setup:
	-----------
	1. DEMO board jumper settings:
			- H1 remove all jumpers 
			- H2 jumper pins 3-5  
              jumper pins 4-6
              
	2. Connect a wire from the controller J4 GND, to the DEMO board
	   J1 GND.

	3. Connect a wire from the controller J4 +RAW to the DEMO board
	   J1 +K.

	Test Instructions:
	------------------
	1. Connect SW1 - SW4 from the demo board to any four inputs on the
	   controller, choose from IN0 - IN23.
	2. Compile and run this program.
	3. Press any one of the DEMO board switches SW1 - SW4 and you should
	   see the input go LOW on the channel the switch is connected to.
	4. Move connections from the demo board to other inputs and repeat
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

// update digital inputs for the given channel range
void update_input(int start_channel, int end_channel, int col, int row)
{
	auto char s[40];
	auto char display[80];
	auto int reading;
	
	// display the input status for the given channel range
	display[0] = '\0';							//initialize for strcat function
	while(start_channel <= end_channel)		//read channels  
	{
		reading = digIn(start_channel++);	//read channel
		sprintf(s, "%d\t", reading);			//format reading in memory
		strcat(display,s);						//append reading to display string
	}
	DispStr(col, row, display);				//update input status 
}
		
///////////////////////////////////////////////////////////////////////////

void main()
{

	auto int key, reading, channel;

	brdInit();		// Required for BL2100 series boards
		
	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Digital inputs 0 - 23: >>>");
	DispStr(8, 3, "IN0\tIN1\tIN2\tIN3\tIN4\tIN5\tIN6\tIN7");
	DispStr(8, 4, "----\t----\t----\t----\t----\t----\t----\t----");
	
	DispStr(8, 7, "IN8\tIN9\tIN10\tIN11\tIN12\tIN13\tIN14\tIN15");
	DispStr(8, 8, "----\t----\t----\t----\t----\t----\t----\t----");

	DispStr(8, 11, "IN16\tIN17\tIN18\tIN19\tIN20\tIN21\tIN22\tIN23");
	DispStr(8, 12, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 16, "Connect the Demo Bd. switches to the inputs that you what to toggle.");
	DispStr(8, 17, "(See instructions in sample program for complete details)");
	DispStr(8, 19, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// update input channels 0 - 7   (display at col = 8 row = 4)
		update_input(0, 7, 8, 5);

		// update input channels 8 - 15  (display at col = 8 row = 8)
		update_input(8, 15, 8, 9);

		// update input channels 16 - 23 (display at col = 8 row = 12)
		update_input(16, 23, 8, 13);
			
		if(kbhit())
		{
			key = getchar();
			if (key == 'Q' || key == 'q')		// check if it's the q or Q key        
			{
				while(kbhit()) getchar();        
      		exit(0);               
     		}       
		}
   }
}
