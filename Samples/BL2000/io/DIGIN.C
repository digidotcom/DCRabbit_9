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

	Samples\BL2000\io\digIn.c

	This sample program is for the BL2000 series controllers.
	
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
              
	2. Connect a wire from the controller J1, GND to the DEMO board
	   J1, GND.
	   
	3. On the controller add a wire between J1, +RAW and +K.

	4. Connect a wire from the controller J1, +RAW to the DEMO board
	   J1, +5 volts.

	The DEMO board has been designed to withstand the full input voltage
	range of a BL2000 series controller.

	Test Instructions:
	------------------
	1. Connect SW1 - SW4 from the demo board to any four inputs on the
	   controller, choose from IN0 - IN9.
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

///////////////////////////////////////////////////////////////////////////

void main()
{

	auto char s[40];
	auto char display[80];
	auto int key, reading, channel;

	brdInit();
		
	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Digital inputs: >>>");
	DispStr(8, 2, "IN0\tIN1\tIN2\tIN3\tIN4\tIN5\tIN6\tIN7");
	DispStr(8, 3, "---\t---\t---\t---\t---\t---\t---\t---");
	
	DispStr(8, 6, "IN8\tIN9\tIN10");
	DispStr(8, 7, "---\t---\t----");

	DispStr(8, 12, "Connect the Demo Bd. switches to the inputs that you what to toggle.");
	DispStr(8, 13, "(See instructions in sample program for complete details)");
	DispStr(8, 15, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// display the input status for all channels
		display[0] = '\0';								//initialize for strcat function
		for(channel = 0; channel < 8; channel++)	//read channels 0 - 7 
		{
			reading = digIn(channel);					//read channel
			sprintf(s, "%d\t", reading);				//format reading in memory
			strcat(display,s);							//append reading to display string
		}
		DispStr(8, 4, display);						//update input status 

		
		display[0] = '\0';
		for(channel = 8; channel < 11; channel++)	//read channels 8 - 10
		{
			reading = digIn(channel);
			sprintf(s, "%d\t", reading);
			strcat(display,s);
		}
		DispStr(8, 8, display);

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
