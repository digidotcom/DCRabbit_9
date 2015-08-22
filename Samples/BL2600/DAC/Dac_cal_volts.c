/***************************************************************************
   dac_cal_volts.c
   Z-World Inc 2004

   This sample program is intended for the BL2600 controller.

   Description
   ===========
	This program demonstrates how to recalibrate a DAC channel
   using a known voltage to generate calibration constants for
   the given channel and then will write the data into reserved
   eeprom.

	Instructions:
   -------------
	1. Connect a voltmeter to one of the DAC output channels labeled
      AV0 - AV3 on PCB.
	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Note: The DAC output goes thru a inverting amplifier, the higher
// rawdata value the lower the output voltage.
#define HICOUNT 3695		// Sets up a low voltage calibration point
#define LOCOUNT 400     // Sets up a high voltage calibration point


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

///////////////////////////////////////////////////////////////////////////

void main()
{
   auto float volt1, volt2, voltout;
   auto int data1, data2;
   auto int channel, selectChannel, configureDAC;
   auto char tmpbuf[128];
   auto char config;
 	auto int key, done, cal_error;
   auto cal_index;

   // Initialize controller
	brdInit();

	while(1)
	{
		DispStr(1, 1,"!!!Caution this will overwrite the calibration constants set at the factory.");
		DispStr(1, 2,"Do you want to continue(Y/N)?");

		while(!kbhit());
		key = getchar();
		if(key == 'Y' || key == 'y')
		{
			break;
		}
		else if(key == 'N' || key == 'n')
		{
			exit(0);
		}
	}

   configureDAC = TRUE;
  	selectChannel = TRUE;

 	for(;;)
	{
      if(configureDAC)
      {
			DispStr(1, 5, "DAC output configuration");
			DispStr(1, 6, "------------------------");
			DispStr(1, 7, "0. Unipolar 0 to 10v");
	  		DispStr(1, 8, "1. Bipolar -10v to +10v");
			DispStr(1, 10, "Enter the DAC configuration 0 - 1 = ");
			do
			{
				config = getchar();
			} while (!((config >= '0') && (config <= '1')));
			printf("%d", config-=0x30);

      	// Configure the DAC for given configuration
      	anaOutConfig(config, DAC_ASYNC);
         anaOutPwr(1);
         cal_index = config;
         configureDAC = FALSE;
      }

      if(selectChannel)
      {
      	blankScreen(13, 32);
 			DispStr(1, 13, "DAC0 - DAC3 Channel Selection");
			DispStr(1, 14, "-----------------------------");
			DispStr(1, 15, "Please enter an output channel (0 - 3) = ");
			do
			{
				channel = getchar();
			} while (!((channel >= '0') && (channel <= '3')));
			printf("%d", channel-=0x30);
         selectChannel = FALSE;
      }

      // set two known voltage points using rawdata values, the
		// user will then type in the actual voltage for each point
      anaOut(channel, HICOUNT);
		DispStr(1, 17, "ENTER the voltage reading from meter(7-10% of max voltage)  = ");
		volt1 = atof(gets(tmpbuf));

		anaOut(channel, LOCOUNT);
		DispStr(1, 18, "ENTER the voltage reading from meter(90-93% of max voltage) = ");
		volt2 = atof(gets(tmpbuf));

      // Create calibration data and store into BL2600 reserved eeprom
     	anaOutCalib(channel, cal_index, HICOUNT, volt1, LOCOUNT, volt2);

      DispStr(1, 20, "Channel now calibrated...");
 		done = FALSE;
      cal_error = FALSE;
		while (!done && !cal_error)
		{
			// display DAC voltage message
			DispStr(1, 22, "Type a desired voltage (in Volts) =  ");

			// get user voltage value for the DAC thats being monitored
			voltout = atof(gets(tmpbuf));

			// send voltage value to DAC for it to output the voltage
     		anaOutVolts(channel,  voltout);
			DispStr(1, 23, "Observe voltage on meter...");

			// display user options
			DispStr(1, 26, "User Options:");
			DispStr(1, 27, "-------------");
			DispStr(1, 28, "1. Change the voltage on current channel");
			DispStr(1, 29, "2  Calibrate another DAC channel");
			DispStr(1, 30, "3. Change overall DAC output configuration");
         DispStr(1, 31, "4. Exit Program");

         // Clear input buffer
         while(kbhit()) getchar();
         while(1)
			{
         	// wait for a key to be pressed
				while(!kbhit());
				key = getchar();
				if(key == '1')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               blankScreen(22, 32);
 					break;
				}
            if(key == '2')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               done = TRUE;
               selectChannel = TRUE;
               break;
   			}
            if(key == '3')
				{
   				// empty the keyboard buffer and clear user options
   				while(kbhit()) getchar();
               configureDAC = TRUE;
               selectChannel = TRUE;
               done = TRUE;
               blankScreen(5, 32);
               break;
   			}
				if(key == '4')
				{
					// exit sample program
     				exit(0);
   			}
 			}
      }
	}
}