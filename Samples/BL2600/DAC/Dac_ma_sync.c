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
   dac_ma_sync.c

   This sample program is intended for the BL2600 controller.

   Description
   ===========
   This program generates a 4-20ma current that can be monitored with a
   multimeter. The output current is computed with using the calibration
   constants that are located in reserved eeprom.

   The DAC circuit is setup for synchronously mode of operation which
   updates all DAC outputs at the same time when the anaOutStrobe
   function executes. The outputs all updated with values previously
   written with anaOutmAmps and/or anaOut functions.



	Instructions:
   -------------
	1. Connect a current meter in series to the DAC output to be
      monitored thru a resistor < 500 ohms to GND.

                         |-----------------|       50 - 400 Ohms
      AI0 - AI3 ---------|  Current Meter  |-------/\/\/\/\------
                         |-----------------|                    |
                                                                |
                                                               ---
                                                               GND
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
   auto float current;
   auto int channel, chSelected, selectChannel;
   auto int key;

   // Initialize controller
	brdInit();

   // Main loop to control 4-20ma DAC outputs
	for(;;)
	{

		DispStr(1, 2, "DAC output Configuration");
		DispStr(1, 3, "------------------------");
		DispStr(1, 4, "0 = Unipolar 4-20ma full resolution (0 - 4095)");
		DispStr(1, 5, "1 = Bipolar  4-20ma half resolution (0 - 2047)");
		DispStr(1, 7, "Please enter the DAC configuration 0 - 1 = ");
		do
		{
			config = getchar();
		} while (!((config >= '0') && (config <= '1')));
		printf("%d", config-=0x30);

      // Configure the DAC for desired output configuration.
      // Note:  If you are not using +-10 voltage outputs in your application,
      // then recommend you configure the DAC circuit for unipolar operation
      // for full DAC resolution.
      anaOutConfig(config, DAC_SYNC);
      anaOutPwr(1);

     	done = FALSE;
      selectChannel = TRUE;
		while (!done)
		{
         if(selectChannel)
         {
         	DispStr(1, 10, "Select 4-20ma Output Channel");
				DispStr(1, 11, "----------------------------");
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
			DispStr(1, 15, "Type a desired current (in milli-amps) =  ");

			// Get value for DAC to write to output
			current = atof(gets(tmpbuf));

         // Write value to internal register of DAC, output will be updated
         // when the anaOutStrobe function is executed.
     		anaOutmAmps(chSelected,  current);
			DispStr(1, 16, "Observe current on meter.....");

			// display user options
			DispStr(2, 19, "User Options:");
			DispStr(2, 20, "-------------");
			DispStr(2, 21, "1. Write current value to DAC channel (internal register)");
         DispStr(2, 22, "2. Strobe DAC chip, all DAC channels will be updated");
         DispStr(2, 23, "3. Change to another DAC channel");
			DispStr(2, 24, "4. Change overall DAC output configuration");
			DispStr(2, 25, "5. Exit Program");

         DispStr(2, 27, "Note: Must strobe DAC for outputs to be updated!");
         while(1)
			{
				// wait for a key to be pressed
				while(!kbhit());
				key = getchar();
            if(key == '1')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               blankScreen(14, 28);
   				break;
   			}

            if(key == '2')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               anaOutStrobe();
               DispStr(2, 30, "DAC outputs have been updated...");
               msDelay(1000);
               DispStr(2, 30, "                                ");
   			}

            if(key == '3')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               blankScreen(11, 28);
               selectChannel = TRUE;
   				break;
   			}

				if(key == '4')
				{
					// exit while loop and clear previous calibration infor
					done = TRUE;
  					blankScreen(0, 28);
					// empty keyboard buffer
					while(kbhit()) getchar();
					break;
				}
				if (key == '5')		// check if it's the q or Q key
				{
					// exit sample program
     				exit(0);
   			}
			}
		}
	}
}