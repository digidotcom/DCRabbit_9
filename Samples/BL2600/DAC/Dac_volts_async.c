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
   dac_volts_async.c

   This sample program is intended for the BL2600 controller.

   Description
   ===========
   This program outputs a voltage that can be read with a voltmeter.
   The output voltage is computed with using the calibration constants
   that are located in reserved eeprom.

   The DAC circuit is setup for Asynchronously mode of operation which
   will update the DAC output at the time it's being written via the
   anaOut or anaOutVolts functions.

	Instructions:
   -------------
	1. Connect a voltmeter to one of the DAC output channels labeled
      AV0 - AV3 on PCB.
	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


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

void  blankScreen(int start, int end)
{
	auto char buffer[256];
   auto int i;

   memset(buffer, 0x00, sizeof(buffer));
 	memset(buffer, ' ', sizeof(buffer));
   buffer[sizeof(buffer)-1] = '\0';
   for(i=start; i < end; i++)
   {
   	DispStr(start, i, buffer);
   }
}

void main()
{
 	auto char tmpbuf[128];
   auto char recbyte;
   auto int done, config;
   auto float voltout;
   auto int channel, chSelected, selectChannel;
   auto int key;

   // Initialize controller
	brdInit();

	for(;;)
	{

		DispStr(1, 2, "DAC output voltage configuration");
		DispStr(1, 3, "--------------------------------");
		DispStr(1, 4, "0 = Unipolar  0  to +10v");
		DispStr(1, 5, "1 = Bipolar  -10 to +10v");
		DispStr(1, 7, "Please enter the DAC configuration 0 - 1 = ");
		do
		{
			config = getchar();
		} while (!((config >= '0') && (config <= '1')));
		printf("%d", config-=0x30);

      // Configure the DAC for given configuration
      anaOutConfig(config, DAC_ASYNC);
      anaOutPwr(1);

     	done = FALSE;
      selectChannel = TRUE;
		while (!done)
		{
         if(selectChannel)
         {
         	DispStr(1, 10, "DAC0 - DAC3 Voltage Out Program");
				DispStr(1, 11, "-------------------------------");
				DispStr(1, 12, "Please enter an output channel (0 - 3) = ");
				do
				{
		  			channel = getchar();
				} while (!((channel >= '0') && (channel <= '3')));
				printf("%d", channel-=0x30);
            chSelected = channel;
            selectChannel = FALSE;
         }

         // display DAC voltage message
			DispStr(1, 15, "Type a desired voltage (in Volts) =  ");

			// get user voltage value for the DAC thats being monitored
			voltout = atof(gets(tmpbuf));

         // send voltage value to DAC for it to output the voltage
     		anaOutVolts(chSelected,  voltout);
			DispStr(1, 16, "Observe voltage on meter.....");

			// display user options
			DispStr(1, 19, "BL2600 ADC User Options:");
			DispStr(1, 20, "------------------------");
			DispStr(1, 21, "1. Change the voltage on the current DAC channel");
  			DispStr(1, 22, "2. Change to another DAC channel");
			DispStr(1, 23, "3. Change overall DAC output configuration");

			while(1)
			{
				// wait for a key to be pressed
				while(!kbhit());
				key = getchar();
            if(key == '1')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               blankScreen(15, 30);
   				break;
   			}
            if(key == '2')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               blankScreen(10, 30);
               selectChannel = TRUE;
   				break;
   			}

				if(key == '3')
				{
					// exit while loop and clear previous calibration infor
					done = TRUE;
  					blankScreen(0, 30);
					// empty keyboard buffer
					while(kbhit()) getchar();
					break;
				}
			}
		}
	}
}