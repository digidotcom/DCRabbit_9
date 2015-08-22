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
	ad_caldiff_ch.c

	This sample program is for the LP3500 series controllers.

	Description
	===========
	This program demonstrates how to recalibrate one differential ADC channel
	using two known voltages to generate constants for that channel and
	rewritten into user block data area.  It will also continuously
	display the voltages being monitored.

	Connections
	===========
	You will need two power supplies. For this demonstration,

	1. Connect 0-10 volt power supply 1 to AIN0 on the controller
		and power supply 2 to AIN1.
	2.	Connect power supply grounds to GND on the controller.


	Instructions
	============
	1. Adjust power supply (1) on AIN0 to approximately 9.0 volts.
	2. Adjust power supply (2) on AIN1 to approximately 1.0 volts.
	3. Compile and run this program.
	4. Enter 0 when prompted for gain code.
	5. Enter 0 when prompted for AD channel code.
	6. When prompted, switch power supply 1 connection to AIN1 and
		connect power supply 2 to AIN0.

***************************************************************************/
#class auto					/* Change local var storage default to "auto" */


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

	printf("\nchannel_code\tInputs\n");
	printf("------------\t-------------\n");
	printf("\t0\t+AIN0 -AIN1\n");
	printf("\t2\t+AIN2 -AIN3\n");
	printf("\t4\t+AIN4 -AIN5\n");
	printf("\t6\t+AIN6 -AIN7\n\n");
}


void main ()
{
	auto unsigned int channel, value1, value2;
	auto float voltage, volts1, volts2, v1, v2;
	auto unsigned int rawdata;
	auto char buffer[64];
	auto unsigned int gaincode;

	brdInit();

	while (1)
	{
		printrange();
		printf("\nChoose the AD channel 0,2,4, or 6 .... ");
		gets(buffer);
		channel = atoi(buffer);

		printf("\nChoose gain code .... ");
		gets(buffer);
		gaincode = atoi(buffer);

		printf("\nEnter actual + voltage =  ");
		gets(buffer);
		v1 = atof(buffer);
		printf("\nEnter actual - voltage =  ");
		gets(buffer);
		v2 = atof(buffer);
		volts2=v1-v2;
		value2 = anaIn(channel, DIFF, gaincode);
		if (value2 == -1)
			printf("channel=%d overflow\n", channel);
		else
			printf("channel=%d raw=%d difference=%.3fV\n", channel, value2, volts2);

		printf("\nSwitch power supply connections and hit return key\n");
		gets(buffer);

		volts1=-volts2;
		value1 = anaIn(channel, DIFF, gaincode);
		if (value1 == -1)
			printf("channel=%d overflow\n", channel);
		else
			printf("channel=%d raw=%d difference=%.3fV\n", channel, value1, volts1);

		if (anaInCalib(channel, DIFF, gaincode, value1, volts1, value2, volts2))
		{
			printf("cannot make calibrations\n");
			exit(0);
		}

		printf("\nstore constants to flash\n");
		anaInEEWr(channel, DIFF, gaincode);

		printf("\nread back constants\n");
		anaInEERd(channel, DIFF, gaincode);

		printf("\nVary voltage from 0 - 15V  .... \n\n");

		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			voltage = anaInDiff(channel, gaincode);
			if (voltage != ADOVERFLOW)
				printf("Ch %2d Volt=%.5f \n", channel, voltage);

			printf("press enter key to read value again or 'Q' to calibrate another\n\n");
			gets(buffer);
		}
	}

}
///////////////////////////////////////////////////////////////////////////

