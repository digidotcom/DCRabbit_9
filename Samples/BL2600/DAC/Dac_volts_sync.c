/***************************************************************************
   dac_volts_sync.c
   Z-World Inc 2004

   This sample program is intended for the BL2600 controller.

   Description
   ===========
   This program outputs a voltage that can be read with a voltmeter.
   The output voltage is computed with using the calibration constants
   that are located in reserved eeprom.

   The DAC circuit is setup for synchronously mode of operation which
   updates all DAC outputs at the same time when the anaOutStrobe
   function executes. The outputs all updated with values previously
   written with anaOutVolts and/or anaOut functions.

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
	auto int done, config;
   auto float voltout;
   auto int channel, selectChannel;
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
      anaOutConfig(config, DAC_SYNC);
      anaOutPwr(1);

     	done = FALSE;
      selectChannel = TRUE;
		while (!done)
		{
         if(selectChannel)
         {
         	DispStr(2, 11, "DAC0 - DAC3 Voltage Out Program");
				DispStr(2, 12, "-------------------------------");
				DispStr(2, 13, "Please enter an output channel (0 - 3) = ");
				do
				{
		  			channel = getchar();
				} while (!((channel >= '0') && (channel <= '3')));
				printf("%d", channel-=0x30);
            selectChannel = FALSE;
        	}

         // display DAC voltage message
			DispStr(2, 16, "Type a desired voltage (in Volts) =  ");

			// get user voltage value for the DAC thats being monitored
			voltout = atof(gets(tmpbuf));

 			// send voltage value to DAC for it to output the voltage
     		anaOutVolts(channel,  voltout);

 			// display user options
			DispStr(2, 19, "User Options:");
			DispStr(2, 20, "-------------");
			DispStr(2, 21, "1. Write voltage value to DAC channel (internal register)");
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
               blankScreen(16, 28);
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