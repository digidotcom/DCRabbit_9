/***************************************************************************
	ad_rdse_all.c
   Z-World, 2003

	This sample program is for RCM3600 series controllers with
	prototyping boards.

	Description
	===========
	This program reads and displays voltage and equivalent values of each
	single-ended analog input channel. Coefficients are read from the
	simulated EEPROM (Flash)	to compute equivalent voltages and cannot
	be run in RAM.
	Computed raw data and equivalent voltages will be displayed.

	Prototyping board connections
	=============================
	Make sure pins 3-5 are connected on JP5, JP6, and JP7.  JP8 should
	not have any pins connected.

	Connect the power supply positive output to channels AIN0-AIN6 on the
	prototyping board and the negative output to GND on the controller.
	Connect a volt meter to monitor the voltage inputs.

	NOTE:	AIN7 is used as a thermistor input and therefore not used in
			this demonstration.

	Instructions
	============
	1. Connect a power supply of 0-20 volts to input channels.
	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.

***************************************************************************/
#class auto

#define STARTCHAN	0
#define ENDCHAN 6

#define GAINSET GAIN_1		//other gain macros
									//GAIN_1
									//GAIN_2
									//GAIN_4
									//GAIN_5
									//GAIN_8
									//GAIN_10
									//GAIN_16
									//GAIN_20

//---------------------------------------------------------
//	displays both the raw data count and voltage equivalent
//	_adsCalibS is address for single ended channels
//---------------------------------------------------------
void anaInInfo (unsigned int channel, unsigned int *rd, float *ve)
{
	auto unsigned value;
	auto float volt;

	value = anaIn(channel, SINGLE, GAINSET);
	volt = (value - _adcCalibS[channel][GAINSET].offset)*(_adcCalibS[channel][GAINSET].kconst);

	if (value == ADOVERFLOW)
	{
		*rd = ADOVERFLOW;
		*ve = ADOVERFLOW;
	}
	else
	{
		*rd = value;
		if (value <= 0.00)
			*ve = 0.000;
		else
			*ve = volt;
	}
}


main ()
{
	auto unsigned int rawdata;
	auto int inputnum, slotnum, keypress, msgcode;
	auto float voltequ;

	brdInit();			//reads calibration constants

	while (1)
	{
		printf("\nChoose:\n");
		printf("  1 to display raw data only\n");
		printf("  2 to display voltage only\n");
		printf("  3 to display both\n\n");

		switch (getchar())
		{
			case '1':
				for (inputnum=STARTCHAN; inputnum<=ENDCHAN; inputnum++)
				{
					rawdata = anaIn(inputnum, SINGLE, GAINSET);
					printf("CH%2d raw data %d\n", inputnum, rawdata);
				}
				break;
			case '2':
				for (inputnum=STARTCHAN; inputnum<=ENDCHAN; inputnum++)
				{
					voltequ = anaInVolts(inputnum, GAINSET);
					printf("CH%2d is %.5f V\n", inputnum, voltequ);
				}
				break;
			case '3':
				for (inputnum=STARTCHAN; inputnum<=ENDCHAN; inputnum++)
				{
					anaInInfo(inputnum, &rawdata, &voltequ);
					printf("CH%2d is %.5f V from raw data %d\n", inputnum, voltequ, rawdata);
				}
				break;
			default:
			break;
		}
	}
}	//end main

