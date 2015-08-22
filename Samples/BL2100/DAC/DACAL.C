/***************************************************************************
	Dacal.c
	
   Z-World, 2001
	This sample program is used with the BL2100 series controllers.

	!!!Caution this will overwrite the calibration constants set at
	   the factory. This program must be compiled to Flash.
	
	This program demonstrates how to recalibrate an DAC channel using
	two known voltages and defines the two coefficients, gain and offset,
	which will be rewritten into the DAC's EEPROM (simulated in Flash). 

	Instructions:
	1.Connect a voltage meter to an output channel.
	2.Compile and run this program.
	3.Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif

#define ESC		 27

#define LOCOUNT 400		//sets up a high voltage calibration point 
#define HICOUNT 3695    //sets up a low voltage calibration point

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

	brdInit();		// Required for BL2100 series boards

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

	clrscreen(0, 3);
	DispStr(1, 1, "DAC0 - DAC3 Calibration Program");
	DispStr(1, 2, "-------------------------------");

	for(;;)
	{
		DispStr(1, 3, "Please enter an output channel (0 - 3) = ");
		do
		{
			channel = getchar();
		} while (!((channel >= '0') && (channel <= '3')));
		printf("%d", channel-=0x30);
		
		// set two known voltage points using rawdata values, the
		// user will then type in the actual voltage for each point
		
		anaOut(channel, LOCOUNT);
		DispStr(1, 5, "ENTER the first voltage reading from meter(appox +9.0v)  = ");
		volt2 = atof(gets(tmpbuf));

		anaOut(channel, HICOUNT);
		DispStr(1, 6, "ENTER the second voltage reading from meter(appox +1.0v) = ");
		volt1 = atof(gets(tmpbuf));
		
		cal_error = FALSE;
		if (anaOutCalib(channel, HICOUNT, volt1, LOCOUNT, volt2))
		{
			cal_error = TRUE;
			DispStr(1, 8, "Cannot make coefficients...Press any key to retry");
			while(!kbhit());
			clrscreen(3, 16);
		}
		else
		{
			//store coefficients into eeprom
			while (anaOutEEWr(channel));
			//read back coefficients from eeprom
			while (anaOutEERd(channel));
			DispStr(1, 8, "Calibration constants has been written to the eeprom");
		}
		done = FALSE;
		while (!done && !cal_error)
		{
			// display DAC voltage message
			DispStr(1, 10, "Type a desired voltage (in Volts) =  ");

			// get user voltage value for the DAC thats being monitored
			voltout = atof(gets(tmpbuf));

			// send voltage value to DAC for it to output the voltage   
			anaOutVolts(channel, voltout);
			DispStr(1, 11, "Observe voltage on meter.....");

			// display user options
			DispStr(1, 13, "User Options:");
			DispStr(1, 14, "-------------");
			DispStr(1, 15, "- Press the SPACEBAR to change the voltage");
			DispStr(1, 16, "- Press the 'C' key to calibrate another DAC channel");
			DispStr(1, 17, "- Press the 'Q' key to quit");

			// wait for a key to be pressed
			while(1)
			{
				while(!kbhit());
				key = getchar();
				if(key == 'C' || key == 'c')
				{
					// exit while loop and clear previous calibration infor
					done = TRUE;
					clrscreen(3, 17);
				
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
   				clrscreen(8, 17);
   				break;
   			}
   		}
		}
	}
}
