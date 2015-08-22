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

	This sample program is for the RCM3700 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates how to recalibrate one differential ADC channel
	using two known voltages to generate constants for that channel and
	rewritten into user block data area.  It will also continuously
	display the voltages being monitored.

	This program must be compiled to Flash.


	Power Supply to Proto-Board Connections
	=======================================
		+V			Gnd
		-----		-----
		AIN0 		AIN1
		AIN2 		AIN3
		AIN4 		AIN5

	Make sure pins 1-3 are connected on JP5, JP6, and JP7.  JP8 should
	not have any pins connected.

	Instructions
	============
	1. Connect 0-20 volt power supply positive terminal to AIN1 on the
		proto-board.
	2.	Connect power supply ground to AIN0 on the proto-board.
	3. Adjust power supply on to approximately 19.0 volts. The controller
		will see -19.0 volts across the two terminals.
	4. Compile and run this program.
	5. Enter 0 when prompted for gain code.
	6. Enter 0 when prompted for the input channel code.
	7. When prompted, switch power supply Gnd to AIN1 and positive terminal
		AIN0.

***************************************************************************/
#class auto

// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

const char vstr[][10] = {
	"20.0V\0",
	"10.0V\0",
	"5.0V\0",
	"3V\0",
	"2.25V\0",
	"2.0V\0",
	"1.25V\0",
	"1.0V\0"};

const float vmin[8] = { -19.0,  -9.0, -4.5, -3.6, -1.75, -1.8, -1.125, -0.9};
const float vmax[8] = { 19.0,  9.0, 4.5, 3.6, 1.75, 1.8, 1.125, 0.9};

void printrange()
{
	printf("\ngain_code\tVoltage range\n");
	printf("---------\t-------------\n");
	printf("\t0\t-20 to +20\n");
	printf("\t1\t-10 to +10\n");
	printf("\t2\t-5 to +5\n");
	printf("\t3\t-3 to +3\n");
	printf("\t4\t-2.25 to +2.25\n");
	printf("\t5\t-2 to +2\n");
	printf("\t6\t-1.25 to +1.25\n");
	printf("\t7\t-1 to +1\n");

	printf("\nchannel_code\tInputs\n");
	printf("------------\t-------------\n");
	printf("\t0\t+AIN0 -AIN1\n");
	printf("\t2\t+AIN2 -AIN3\n");
	printf("\t4\t+AIN4 -AIN5\n");
}

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


main ()
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
		printf("\nChoose the AD channel 0, 2, 4  .... ");
		gets(buffer);
		channel = atoi(buffer);

		printf("\nChoose gain code .... ");
		gets(buffer);
		gaincode = atoi(buffer);

		printf("\nAdjust voltage to %8.3f and enter actual =  ", vmin[gaincode]);
		gets(buffer);
		volts1 = atof(buffer);
		value1 = anaIn(channel, DIFF, gaincode);
		if (value1 == ADOVERFLOW)
			printf("lo:  channel=%d overflow\n", channel);
		else
			printf("lo:  channel=%d raw=%d\n", channel, value1);

		printf("\nSwitch the connections and enter voltage =  ");
		gets(buffer);
		volts2 = atof(buffer);
		value2 = anaIn(channel, DIFF, gaincode);
		if (value2 == ADOVERFLOW)
			printf("hi:  channel=%d overflow\n", channel);
		else
			printf("hi:  channel=%d raw=%d\n", channel, value2);

		anaInCalib(channel, DIFF, gaincode, value1, volts1, value2, volts2);

		printf("\nstore constants to flash\n");
		anaInEEWr(channel, DIFF, gaincode);

		printf("\nread calibration constants\n");
		anaInEERd(channel, DIFF, gaincode);

		printf("\nVary voltage from -%s to +%s  .... \n", vstr[gaincode], vstr[gaincode]);

		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			voltage = anaInDiff(channel, gaincode);
			printf("Ch %2d Volt=%.5f \n", channel, voltage);
			printf("press enter key to read values again or 'Q' to calibrate another channel\n\n");
			gets(buffer);
		}
	}
}
///////////////////////////////////////////////////////////////////////////

