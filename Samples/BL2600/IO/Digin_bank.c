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

	digin_bank.c

	This sample program is for the BL2600 series controllers.

	This program demonstrates using the digInBank API function to read
   digital inputs. With using the DEMO board provided in your kit, you
   will be able to see a digital input toggle from HIGH to LOW when a
   push button on the demo board is pressed.

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

	Test Instructions:
	------------------
	1. Connect SW1 - SW4 from the demo board to any four inputs on the
	   controller, choose from DIO00 - DIO15 and IN16 - IN31.
	2. Compile and run this program.
	3. Press any one of the DEMO board switches SW1 - SW4 and you should
	   see the input go LOW on the channel the switch is connected to.
	4. Move connections from the demo board to other inputs and repeat
	   steps 1 - 4.

**************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
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
	auto int data, reading;
   auto char mask;

   switch(start_channel)
   {
		case 0:
      	data = digInBank(0);
         break;

      case 8:
      	data = digInBank(1);
         break;

      case 16:
      	data = digInBank(2);
         break;

      case 24:
      	data = digInBank(3);
         break;

      default:
      	// Invalid bank channel number
      	exit(1);
   }

	// display the input status for the given channel range
   mask = 0x01;
  	display[0] = '\0';							//initialize for strcat function
	while(start_channel <= end_channel)		//update channel status
   {
   	reading = (data & mask) ? 1 : 0;
		sprintf(s, "%d\t", reading);			//format reading in memory
		strcat(display,s);						//append reading to display string
   	mask = mask << 1;
      start_channel++;
   }
	DispStr(col, row, display);				//update input status
}

///////////////////////////////////////////////////////////////////////////

void main()
{

	auto int key, reading, channel;

   // Initialize the controller
	brdInit();

	//Display user instructions and channel headings
	//Display user instructions and channel headings
	DispStr(8, 1, " <<< Digital inputs 00 - 31 >>>");
	DispStr(8, 3, "DIO00\tDIO01\tDIO02\tDIO03\tDIO04\tDIO05\tDIO06\tDIO07");
	DispStr(8, 4, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 7, "DIO08\tDIO09\tDIO10\tDIO11\tDIO12\tDIO13\tDIO14\tDIO15");
	DispStr(8, 8, "-----\t-----\t-----\t-----\t-----\t-----\t-----\t-----");

	DispStr(8, 11, "IN16\tIN17\tIN18\tIN19\tIN20\tIN21\tIN22\tIN23");
	DispStr(8, 12, "----\t----\t----\t----\t----\t----\t----\t----");


	DispStr(8, 16, "IN24\tIN25\tIN26\tIN27\tIN28\tIN29\tIN30\tIN31");
	DispStr(8, 17, "----\t----\t----\t----\t----\t----\t----\t----");



	DispStr(8, 22, "Connect the Demo Bd. switches to the inputs that you what to toggle.");
	DispStr(8, 23, "(See instructions in sample program for complete details)");
	DispStr(8, 24, "<-PRESS 'Q' TO QUIT->");

	//loop until user presses the "Q" key
	for(;;)
	{
		// update input channels 0 - 7, located at col = 8 row = 5.
		update_input(0, 7, 8, 5);

		// update input channels 8 - 15, located at col = 8 row = 9.
		update_input(8, 15, 8, 9);

		// update input channels 16 - 23, located at col = 8 row = 13.
		update_input(16, 23, 8, 13);

      // update input channels 24 - 31, located at col = 8 row = 18.
		update_input(24, 31, 8, 18);


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
///////////////////////////////////////////////////////////////////////////


