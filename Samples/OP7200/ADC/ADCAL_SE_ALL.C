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
	adcal_se_all.c

	This sample program is for the OP7200 series controllers.
	
	Description
	===========
	This program demonstrates how to recalibrate all single-ended ADC
	channels for a given gain, with using two known voltages to generate
	the constants for each channel. Only the constants for the given gain
	will be rewritten into the userblock calibration data area. The program
	will also display the voltage's that are present on the A/D inputs.
	
	Connections
	===========
	Connect the power supply positive output to one of the A/D channels
	AIN0-AIN7 and the negative output to AGND on the controller.

	Connect a voltmeter to monitor the A/D inputs.
	
	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
#class auto

#define STARTCHAN	0
#define ENDCHAN 7

const float vmax[] = {
	20.0,
	10.0,
	5.0,
	4.0,
	2.5,
	2.0,
	1.25,
	1.00
};


typedef struct {
	int value1, value2;			// keeps track of data for calibrations 
	float volts1, volts2;		// keeps track of data for calibrations
	} _line;

_line ln[16];


// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

void printrange()
{
	printf("\ngain_code\tVoltage range\n");
	printf("---------\t-------------\n");
	printf("\t0\t0 - 20 \n");
	printf("\t1\t0 - 10\n");
	printf("\t2\t0 - 5\n");
	printf("\t3\t0 - 4\n");
	printf("\t4\t0 - 2.5\n");
	printf("\t5\t0 - 2\n");
	printf("\t6\t0 - 1.25\n");
	printf("\t7\t0 - 1\n\n");
}


void main ()
{
	auto long average;
	auto unsigned int rawdata;
	auto int channel, gaincode;
	auto int key, i;
	auto float voltage, cal_voltage;
	auto char buffer[64];

	
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
	
	while (1)
	{
		printrange();
		printf("\nChoose gain code .... ");
		do
		{
			gaincode = getchar();
		} while (!( (gaincode >= '0') && (gaincode <= '7')) );
		gaincode = gaincode - 0x30;
		printf("%d", gaincode);
		while(kbhit()) getchar();
		
		cal_voltage = .1*vmax[gaincode];
		printf("\nAdjust to approx. %.4f and then enter actual voltage = ", cal_voltage);
		gets(buffer);
		for (channel=STARTCHAN; channel<=ENDCHAN; channel++)
		{
			ln[channel].volts1 = atof(buffer);
			average = 0;
			for(i=0; i<10; i++)
				average += anaIn(channel, SE_MODE, gaincode);
			ln[channel].value1 = (int)average/10;
			printf("lo:  channel=%d raw=%d\n", channel, ln[channel].value1);
		}
		while(kbhit()) getchar();
	
		cal_voltage = .9*vmax[gaincode];
		printf("\nAdjust to approx. %.4f and then enter actual voltage = ", cal_voltage);
		gets(buffer);
		for (channel=STARTCHAN; channel<=ENDCHAN; channel++)
		{
			ln[channel].volts2 = atof(buffer);
			average = 0;
			for(i=0; i<10; i++)
				average += anaIn(channel, SE_MODE, gaincode);
			ln[channel].value2 = (int)average/10;
			printf("hi:  channel=%d raw=%d\n", channel, ln[channel].value2);
		}
		while(kbhit()) getchar();
			
		for (channel=STARTCHAN; channel<=ENDCHAN; channel++)
		{
 			anaInCalib(channel, SE_MODE, gaincode, ln[channel].value1, ln[channel].volts1,
 			                                       ln[channel].value2, ln[channel].volts2);
		}
		
		printf("\nstore constants to flash\n");
		for (channel=STARTCHAN; channel<=ENDCHAN; channel++)
		{
			anaInEEWr(channel, SE_MODE, gaincode);				//store all channels
		}
		
		printf("\nread back constants\n");
		anaInEERd(ALL_CHANNELS, SE_MODE, gaincode);				//read all channels
		
		printf("\nVary voltage within the range selected\n");
		
		do 
		{
			for (channel=STARTCHAN; channel<=ENDCHAN; channel++)
			{
				voltage = anaInVolts(channel, gaincode);
				printf("Ch %2d Volt=%.5f \n", channel, voltage);
			}
			printf("Press ENTER key to read values again or 'Q' to calibrate another gain\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();
			
		}while(key != 'q' && key != 'Q');
	}            
}
///////////////////////////////////////////////////////////////////////////
