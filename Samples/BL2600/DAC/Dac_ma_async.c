/***************************************************************************
   dac_ma_async.c
   Z-World Inc 2004

   This sample program is intended for the BL2600 controller.

   Description
   ===========
   This program generates a 4-20ma current that can be monitored with a
   multimeter. The output current is computed with using the calibration
   constants that are located in reserved eeprom.

   The DAC circuit is setup for Asynchronously mode of operation which
   will update the DAC output at the time it's being written via the
   anaOut or anaOutmAmps functions.

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
      anaOutConfig(config, DAC_ASYNC);
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

         // Get current value for DAC to output
 			current = atof(gets(tmpbuf));

         // Write value to DAC to update output
     		anaOutmAmps(chSelected,  current);
			DispStr(1, 16, "Observe current on meter.....");

			// display user options
			DispStr(1, 19, "BL2600 DAC Options:");
			DispStr(1, 20, "-------------------");
			DispStr(1, 21, "1. Change the current on the selected DAC channel");
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