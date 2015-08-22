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
	ad_cal_chan.c

	This sample program is for the LP3500 series controllers.

	Description
	===========
	This program demonstrates how to recalibrate one single-ended ADC channel
	using two known voltages to generate constants for that channel and
	rewritten into user block data area.  It will also continuously
	display the voltages being monitored.


	Connections
	===========
	Connect the power supply positive output to the AD channels and the
	negative output to GND on the controller.

	Connect a volt meter to monitor the voltage input.


	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
#class auto					/* Change local var storage default to "auto" */

const char vstr[][10] = {
	"0 - 20V\0",
	"0 - 10V\0",
	"0 -  5V\0",
	"0 -  4V\0",
	"0 - 2.5V\0",
	"0 -  2V\0",
	"0 -  1V\0",
	"0 -  1V\0"};

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
	auto int channel, value1, value2;
	auto float voltage, volts1, volts2;
	auto unsigned int rawdata, gaincode;
	auto char buffer[64];

	brdInit();

	while (1)
	{
		printf("\nChoose the AIN channel 0 - 7 .... ");
		gets(buffer);
		channel = atoi(buffer);

		printrange();
		printf("\nChoose gain code .... ");
		gets(buffer);
		gaincode = atoi(buffer);

		printf("\nAdjust to lower voltage of %s and enter actual =  ", vstr[gaincode]);
		gets(buffer);
		volts1 = atof(buffer);
		value1 = anaIn(channel, SINGLE, gaincode);
		if (value1 == ADOVERFLOW)
			printf("lo:  channel=%d overflow\n", channel);
		else
			printf("lo:  channel=%d raw=%d\n", channel, value1);

		printf("\nAdjust to higher voltage of %s and enter actual =  ", vstr[gaincode]);
		gets(buffer);
		volts2 = atof(buffer);
		value2 = anaIn(channel, SINGLE, gaincode);
		if (value2 == ADOVERFLOW)
			printf("hi:  channel=%d overflow\n", channel);
		else
			printf("hi:  channel=%d raw=%d\n", channel, value2);

		anaInCalib(channel, SINGLE, gaincode, value1, volts1, value2, volts2);

		printf("\nstore constants to flash\n");
		anaInEEWr(channel, SINGLE, gaincode);

		printf("\nread back constants\n");
		anaInEERd(channel, SINGLE, gaincode);

		printf("\nVary voltage from 0 -10  .... \n");

		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			voltage = anaInVolts(channel, gaincode);
			printf("Ch %2d Volt=%.5f \n", channel, voltage);
			printf("press enter key to read values again or 'Q' to calibrate another channel\n\n");
			gets(buffer);
		}
	}
}
///////////////////////////////////////////////////////////////////////////

