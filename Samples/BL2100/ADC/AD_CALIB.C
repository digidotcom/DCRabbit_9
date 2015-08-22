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
	ad_calib.c

	This sample program is used with the BL2100 series controllers.

	This program demonstrates how to recalibrate an ADC channel using two
	known	voltages to generate two coefficients, gain and offset, which 
	will be rewritten	into user block data area. It will also continuously
	display the voltage that's being monitored.

	!!! Caution !!!
	1. Make sure that you don't exceed the voltage range of the A/D input channel.
	2. This program will overwrite the calibration constants set at the factory.
	3. This program must be compiled to Flash.
                      
	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that your using.

	2. Preset the voltage on the power supply to be within the voltage range
	of the A/D converter channel that your going to calibrate. For A/D
	channels 0 - 10 the input voltage range is -10 to +10 volts.

	3. Power-on the controller.
	
	4. Connect output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Follow the calibration instructions	prompted from this program
	during execution.
	
	7. Vary the voltage (see above for voltage range) on a A/D channel,
	the voltage displayed in the STDIO window and the voltmeter should
	track each other. 
	   
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif

#define MAXVOLT 9.0		
//#define MINVOLT -9.0		//enter -9.0 for -10V to 10V; 1.0 for 0 to 10V 
#define MINVOLT 1.0		//enter -9.0 for -10V to 10V; 1.0 for 0 to 10V 

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

void main ()
{
	auto int rawdata, datapoint1, datapoint2;
	auto float voltequ, lovoltage, hivoltage;
	auto int channel, cal_error, done, key;
	auto char buffer[128];

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
	DispStr(1, 1, "ADC0 - ADC10 Calibration Program");
	DispStr(1, 2, "--------------------------------");

	for(;;)
	{
		DispStr(1, 3, "Please enter an input channel, 0 thru A (10)....");
		do
		{
			channel = getchar();
		} while (!(isdigit(channel)) && (channel != 'a') && (channel != 'A'));

		// convert the ascii hex value to a interger
		if( channel >= '0' && channel <='9')
		{
			channel = channel - 0x30;
		}
		else
		{
			channel = tolower(channel);
			channel = (channel - 'a') + 10;
		}
		printf("%d", channel);
		
		/////get raw data from two known voltage points
		sprintf(buffer, "Adjust the voltage to appox %.2f volts and then ENTER the actual", MINVOLT);
		DispStr(1, 5, buffer);
		DispStr(1, 6, "voltage being measured, (floating point value) = ");
		gets(buffer);
		datapoint1 = anaIn(channel);
		lovoltage = atof(buffer);

		sprintf(buffer, "Adjust the voltage to appox +%.2f volts and then ENTER the actual", MAXVOLT);
		DispStr(1, 8, buffer);
		DispStr(1, 9, "voltage being measured, (floating point value) = ");
		gets(buffer);
		datapoint2 = anaIn(channel);
		hivoltage = atof(buffer);
		
		/////calculate gains and offsets
		cal_error = FALSE;
		if	(anaInCalib(channel, datapoint1, lovoltage, datapoint2, hivoltage))
		{
			cal_error = TRUE;
			DispStr(1, 12, "Cannot make coefficients...Press any key to retry");
			while(!kbhit());
			clrscreen(3, 20);
		}
		else
		{
			/////store coefficients into eeprom (simulated in flash memory)
			while (anaInEEWr(channel));
		
			/////read back coefficients from eeprom
			while (anaInEERd(channel));
			DispStr(1, 11, "Calibration constants has been written to the eeprom");

			// display user options
			DispStr(1, 17, "User Options:");
			DispStr(1, 18, "-------------");
			DispStr(1, 19, "- Press the 'C' key to calibrate another ADC channel");
			DispStr(1, 20, "- Press the 'Q' key to quit");
		}

		done = FALSE;
		while (!done && !cal_error)
		{
			sprintf(buffer, "Vary voltage on channel %d  (voltage range -10 to +10 volts)", channel);
			DispStr(1, 13, buffer);

			voltequ = anaInVolts(channel);
			sprintf(buffer, "Voltage at CH%d is %.4f ", channel, voltequ);
			DispStr(1, 14, buffer);
			
			// check if a key was pressed
			if(kbhit())
			{
				key = getchar();
				if(key == 'C' || key == 'c')
				{
					// exit and prompt user for next channel to be calibrated
					done = TRUE;
					clrscreen(3, 20);
			
					// empty keyboard buffer
					while(kbhit()) getchar();       
				}
				if (key == 'Q' || key == 'q')		// check if it's the q or Q key        
				{
					// exit sample program     
  					exit(0);               
  				}
  			}
   	}
   }
}	
