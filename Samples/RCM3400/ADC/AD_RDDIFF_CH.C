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
	ad_rddiff_ch.c

	This sample program is for the RCM3400 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates reading one differential ADC channel
	using two known voltages and constants for that channel.
	It will also continuously display the voltage being monitored.

	This program must be compiled to Flash.

	Prototyping board connections
	=============================
	Make sure pins 1-2 are connected on JP1, JP2, and JP9.  JP5 should
	not have any pins connected.

	1. Connect 0-10 volt power supply positive terminal to AIN0 and
		the Gnd terminal to AIN1 on the proto-board.

	OR

	You will need two power supplies.
	1. Connect 0-10 volt power supply (1) to AIN0 on the controller
		and power supply (2) to AIN1.
	2.	Connect power supply grounds to GND on the controller.


	Instructions
	============
	1. Adjust power supply (1) on AIN0 to approximately 9.0 volts.
	2. If you are using a second power supply, adjust AIN1 to approximately
		1.0 volts.
	3. Compile and run this program.
	4. Enter 0 when prompted for AD channel code.

***************************************************************************/
#class auto

#define ADC_SCLKBAUD 115200ul
const char vstr[][10] = {
	"0 - 20V \0",
	"0 - 10V \0",
	"0 - 5V  \0",
	"0 - 4V  \0",
	"0 - 2.5V\0",
	"0 - 2V  \0",
	"0 - 1V  \0",
	"0 - 1V  \0"};

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
}


main ()
{
	auto unsigned int channel, gaincode;
	auto float voltage, volts1, volts2, v1, v2;
	auto char buffer[64];

	brdInit();

	while (1)
	{
		printrange();
		printf("\nChoose the AD channel 0, 2, 4  .... ");
		gets(buffer);
		channel = atoi(buffer);

		printf("\nVary voltage from -10 +10 V  .... \n\n");

		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			for (gaincode=0; gaincode<=7; gaincode++)
			{
				voltage = anaInDiff(channel, gaincode);
				printf("Ch %2d at gain range %s Volt=%.3f \n", channel, vstr[gaincode], voltage);
			}
			printf("\npress enter key to read value again or 'Q' or read another channel\n\n");
			gets(buffer);
		}
	}

}
///////////////////////////////////////////////////////////////////////////

