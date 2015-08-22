/***************************************************************************
	ad_cal_chan.c
	Z-World, 2002

	This sample program is for the RCM3400 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates how to recalibrate one single-ended analog
	input channel with one gain using two known voltages to generate
	constants for that channel and	rewritten into user block data area.
	It will also continuously display the voltages being monitored.

	This program cannot run in RAM.

	Prototyping board connections
	=============================
	Make sure pins 2-3 are connected on JP1, JP2, and JP9.  JP5 should
	not have any pins connected.

	Connect the power supply positive output to an analog input channel
	and the negative output to GND on the controller.

	Connect a volt meter to monitor the voltage input.

	NOTE:	AIN7 is used as a thermistor input and therefore not used in
			this demonstration.

	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
#class auto

#define ADC_SCLKBAUD 115200ul
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


main ()
{
	auto int channel, value1, value2;
	auto float voltage, volts1, volts2;
	auto unsigned int rawdata, gaincode;
	auto char buffer[64];

	brdInit();

	while (1)
	{
		printf("\nChoose AIN channel 0 - 6 .... ");
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

