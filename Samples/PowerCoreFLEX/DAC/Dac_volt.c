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
/***************************************************************************
	dac_volt.c

  	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

   This program outputs a voltage that can be read with a voltmeter. The
   output voltage is computed with using the calibration constants that
   are read from EEPROM (simulated in Flash).

	!!!This program must be compiled to Flash.

	Instructions:
   -------------
	1.Connect a voltmeter to one of the following DAC output channel
     and GND located on the prototyping board.
   	DAC0 = J3, pin 1
      DAC1 = J3, pin 3
      DAC2 = J3, pin 5
      GND  = J3, pins 2, 4, 6 or 8.
   2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.

***************************************************************************/
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

#if RAM_COMPILE
#error "This program must be compiled to Flash."
#endif


// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

// clear the STDIO display for the range specified (start/end rows)
void clrscreen(int startrow, int endrow)
{
	auto char s[81];
	auto int i;

	for(i = 0; i < 80; i++)
	{
		s[i] = 0x20;
	}
	s[i] = '\0';
	while(startrow <= endrow)
	{
		 DispStr(0, startrow++, s);
	}
}

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int channel;
	auto float voltout, volt1, volt2;
	auto char tmpbuf[24];
	auto int key, done, cal_error;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

   DispStr(1, 1, "DAC0 - DAC2 Voltage Out Program");
	DispStr(1, 2, "-------------------------------");

	for(;;)
	{
		DispStr(1, 3, "Please enter an output channel (0 - 2) = ");
		do
		{
			channel = getchar();
		} while (!((channel >= '0') && (channel <= '2')));
		printf("%d", channel-=0x30);

		done = FALSE;
		while (!done)
		{
			// display DAC voltage message
			DispStr(1, 6, "Type a desired voltage (in Volts) =  ");

			// get user voltage value for the DAC thats being monitored
			voltout = atof(gets(tmpbuf));

			// send voltage value to DAC for it to output the voltage
			anaOutVolts(channel, voltout);
			DispStr(1, 7, "Observe voltage on meter.....");

			// display user options
			DispStr(1, 10, "User Options:");
			DispStr(1, 11, "-------------");
			DispStr(1, 12, "- Press the SPACEBAR to change the voltage");
			DispStr(1, 13, "- Press 'C' to change the DAC channel");
			DispStr(1, 14, "- Press the 'Q' key to quit");

			while(1)
			{
				// wait for a key to be pressed
				while(!kbhit());
				key = getchar();
				if(key == 'C' || key == 'c')
				{
					// exit while loop and clear previous calibration infor
					done = TRUE;
					clrscreen(3, 14);
					// empty keyboard buffer
					while(kbhit()) getchar();
					break;
				}
				if (key == 'Q' || key == 'q')		// check if it's the q or Q key
				{
					// exit sample program
     				exit(0);
   			}
				if(key == 0x20)
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
   				clrscreen(6, 14);
   				break;
   			}
			}
		}
	}
}