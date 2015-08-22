/***************************************************************************
	adc_cal_se_bipolar.c
	Z-World, 2004

	This sample program is for the BL2600 series controller.

	Description
	===========
	This program demonstrates how to recalibrate a single-ended
   bipolar ADC channel using two known voltages to generate
   calibration constants for the given channel and will write
   the data into reserved eeprom.

   It will also continuously display the voltages being monitored.

	Connections
	===========
	For this calibration procedure you will need a power supply
	that has a floating output.

	1. Before doing the following steps, set the power supply to
      zero volts and then turn it OFF.

	2. Initially connect the positive side of the power supply to
	   the channel (0 -7) being calibrated.

   3. Connect the negative output of the power supply to AGND on
      the controller.


	Instructions
	============
	1. Power-on the controller.
	2. Compile and run this program.
	3. Turn ON the power supply for the ADC calibration.
	4. Follow the instructions as displayed.
***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

const float vmax[] = {
	10.0,
	5.0,
	4.0,
	2.5,
	2.0,
	1.25,
	1.00,
   0.5
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
	printf("\n gain_code\tVoltage range\n");
	printf("-----------\t-------------\n");
	printf("\t0\t +- 10.0v\n");
	printf("\t1\t +- 5.00v\n");
 	printf("\t2\t +- 2.50v\n");
	printf("\t3\t +- 2.00v\n");
	printf("\t4\t +- 1.25v\n");
	printf("\t5\t +- 1.00v\n");

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

   // Configure channel pair 0 & 1 for Single-Ended bipolar mode of operation.
   // (Max voltage range is ±10V)
   anaInConfig(0, SE1_MODE);

   // Configure channel pair 2 & 3 for Single-Ended bipolar mode of operation
   // (Max voltage range is ±10V)
   anaInConfig(1, SE1_MODE);

   // Configure channel pair 4 & 5 for Single-Ended bipolar mode of operation
   // (Max voltage range is ±10V)
   anaInConfig(2, SE1_MODE);

   // Configure channel pair 6 & 7 for Single-Ended bipolar mode of operation
   // (Max voltage range is ±10V)
   anaInConfig(3, SE1_MODE);

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
		} while (!( (gaincode >= '0') && (gaincode <= '5')) );
		gaincode = gaincode - 0x30;
		printf("%d", gaincode);
		while(kbhit()) getchar();

     	cal_voltage = vmax[gaincode]*.8;
		printf("\nAdjust Power connected to AIN%d to approx. %.2f\n", channel, cal_voltage);
		printf("and then enter actual voltage = ");
		gets(buffer);

		volts1 = atof(buffer);
		value1 = 0;
		for(i=0; i<10; i++)
			value1 += anaIn(channel, gaincode);
		value1 = value1/10;
      printf("Hi:  channel=%d raw=%d\n", channel, value1);


		printf("\nSwap power supply connections and then PRESS any key\n");
		while(!kbhit());
		while(kbhit()) getchar();

		volts2 = -volts1;
		value2 = 0;
		for(i=0; i<10; i++)
			value2 += anaIn(channel, gaincode);
		value2 = value2/10;
      printf("Lo:  channel=%d raw=%d\n", channel, value2);

		anaInCalib(channel, SE1_MODE, gaincode,
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

