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
   dac_cal_ma.c

   This sample program is intended for the BL2600 controller.

   Description
   ===========
	This program demonstrates how to recalibrate a DAC channel
   using a known current to generate calibration constants for
   the given channel and then will write the data into reserved
   eeprom.

	Instructions:
   -------------
	1. For the DAC channel to be calibrated connect the current meter
      from the DAC output in series thru a resistor < 500 ohms to GND.

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


#define BIPOLAR_HI 	1848		// Sets up a low current calibration point
#define BIPOLAR_LO 	200    	// Sets up a high current calibration point

#define UNIPOLAR_HI	3695		// Sets up a low current calibration point
#define UNIPOLAR_LO 	400      // Sets up a high current calibration point


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
   auto float current1, current2, currentout;
   auto hicount, locount;
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
			DispStr(1, 5, "DAC output Configuration");
			DispStr(1, 6, "------------------------");
			DispStr(1, 7, "0 = Unipolar 4-20ma full resolution (0 - 4095)");
			DispStr(1, 8, "1 = Bipolar  4-20ma half resolution (0 - 2047)");
			DispStr(1, 9, "Please enter the DAC configuration 0 - 1 = ");
			do
			{
				config = getchar();
			} while (!((config >= '0') && (config <= '1')));
			printf("%d", config-=0x30);

         if(config == 0)
         {
         	locount = UNIPOLAR_LO;
            hicount = UNIPOLAR_HI;
         }
         else
         {
         	locount = BIPOLAR_LO;
            hicount = BIPOLAR_HI;
         }
      	// Configure the DAC for given configuration
      	anaOutConfig(config, DAC_ASYNC);
         anaOutPwr(1);
         cal_index = config + 2;
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
      anaOut(channel, hicount);
		DispStr(1, 17, "ENTER the 1st current reading from meter ~5.5ma = ");
		current1 = atof(gets(tmpbuf));

		anaOut(channel, locount);
		DispStr(1, 18, "ENTER the 2nd current reading from meter ~19.5  = ");
		current2 = atof(gets(tmpbuf));

     	anaOutCalib(channel, cal_index, hicount, current1, locount, current2);

      DispStr(1, 20, "Channel now calibrated...");
 		done = FALSE;
      cal_error = FALSE;
		while (!done && !cal_error)
		{
			// display DAC voltage message
			DispStr(1, 22, "Type a desired current (in milli-amps) =  ");

			// get user voltage value for the DAC thats being monitored
			currentout = atof(gets(tmpbuf));

			// send voltage value to DAC for it to output the voltage
     		anaOutmAmps(channel,  currentout);
			DispStr(1, 23, "Observe current on meter...");

			// display user options
			DispStr(1, 26, "User Options:");
			DispStr(1, 27, "-------------");
			DispStr(1, 28, "1. Change the current on selected channel");
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