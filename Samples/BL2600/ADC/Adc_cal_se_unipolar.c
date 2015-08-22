/***************************************************************************
	adc_cal_se_unipolar.c
	Z-World, 2004

	This sample program is for the BL2600 series controller.

	Description
	===========
	This program demonstrates how to recalibrate a single-ended
   unipolar ADC channel using two known voltages to generate
   calibration constants for the given channel and will write
   the data into reserved eeprom.

   It will also continuously display the voltages being monitored.

	Connections
	===========
	1. Connect the positive output of the power supply to the AD channel
   that you want to calibrate.
   2. Connect the negative output of the power supply to AGND on the
   controller.
   3. Connect a voltmeter to monitor the ADC channel being calibrated
   at the same points you connected the power supply in steps 1 and 2.
	(For best results use a 4 1/2 digit voltmeter)

	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

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

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

void printrange( void )
{
	printf("\n\n");
	printf("Gain code\tVoltage range\n");
	printf("-----------\t-------------\n");
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
	auto long value1, value2;
	auto unsigned int rawdata;
	auto int channel, gaincode;
	auto int key, i;
	auto float voltage, volts1, volts2, cal_voltage;
	auto char buffer[64];

   // Initialize the controller
	brdInit();

   // Configure channel pair 0 & 1 for Single-Ended unipolar mode of operation.
   // (Max voltage range is 0 - 20v)
   anaInConfig(0, SE0_MODE);

   // Configure channel pair 2 & 3 for Single-Ended unipolar mode of operation
   // (Max voltage range is 0 - 20v)
   anaInConfig(1, SE0_MODE);

   // Configure channel pair 4 & 5 for Single-Ended unipolar mode of operation
   // (Max voltage range is 0 - 20v)
   anaInConfig(2, SE0_MODE);

   // Configure channel pair 6 & 7 for Single-Ended unipolar mode of operation
   // (Max voltage range is 0 - 20v)
   anaInConfig(3, SE0_MODE);

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
	while(kbhit()) getchar();

	while (1)
	{
		printf("\n\nPlease enter an ADC channel, 0 thru 7....");
		do
		{
			channel = getchar();
		} while (!( (channel >= '0') && (channel <= '7')) );
		channel = channel - 0x30;
		printf("%d", channel);
		while(kbhit()) getchar();


		printrange();
		printf("\nChoose gain code .... ");
		do
		{
			gaincode = getchar();
		} while (!( (gaincode >= '0') && (gaincode <= '7')) );
		gaincode = gaincode - 0x30;
		printf("%d", gaincode);
		while(kbhit()) getchar();

		cal_voltage = .2*vmax[gaincode];
		printf("\nAdjust to approx. %.4f and then enter actual voltage = ", cal_voltage);
		gets(buffer);
		while(kbhit()) getchar();

		volts1 = atof(buffer);
		value1 = 0;
		for(i=0; i<10; i++)
			value1 += anaIn(channel, gaincode);
		value1 = value1/10;

		printf("lo:  channel=%d raw=%d\n", channel, value1);
		cal_voltage = .8*vmax[gaincode];
		printf("\nAdjust to approx. %.4f and then enter actual voltage = ", cal_voltage );
		gets(buffer);
		while(kbhit()) getchar();

		volts2 = atof(buffer);
		value2 = 0;
		for(i=0; i<10; i++)
			value2 += anaIn(channel, gaincode);
		value2 = value2/10;
		printf("hi:  channel=%d raw=%d\n", channel, value2);

		anaInCalib(channel, SE0_MODE, gaincode,
		           (int) value1, volts1, (int) value2, volts2);

		printf("\nVary voltage within the range selected.... \n");
		do
		{
			voltage = anaInVolts(channel, gaincode);
			printf("Ch %2d Volt=%.4f \n", channel, voltage);
			printf("Press ENTER key to read values again or 'Q' to calibrate another channel\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();
		}while(key != 'q' && key != 'Q');
	}
}
///////////////////////////////////////////////////////////////////////////

