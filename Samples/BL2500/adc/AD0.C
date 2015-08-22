/********************************************************************
	ad0.c
	Z-World, 2002

	This sample program is intended for the BL2500 series controllers.
	
	Description
	===========
	This program reads and displays voltage and equivalent value of
	channel AD0 to STDIO window.
	
	Instructions
	============
	1. Connect a power supply of 0-3.1 volts to AD0.
	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.

*********************************************************************/
#class auto

//////
// this function returns the count and its converted voltage value
//////
void anaInInfo (unsigned int channel, unsigned int *rd, float *ve)
{
	auto int count;
	auto float value;
	
	count = anaIn(channel);
	if (count == ADOVERFLOW)
		value = ADOVERFLOW;
	else
		value = (count - _adcCalibS[channel].offset)*(_adcCalibS[channel].kconst);

	*rd = count;
	*ve = value;
}


main ()
{
	auto unsigned int rawdata, inputnum;
	auto float voltequ;

	brdInit();			//sets pwm frequency
	inputnum=0;
	
	while (1)
	{	
		printf("\nChoose:\n");
		printf("  1 to display raw data only\n");
		printf("  2 to display voltage only\n");
		printf("  3 to display both\n\n");

		switch (getchar())
		{
			case '1': 
					rawdata = anaIn(inputnum);
					printf("CH%2d raw data %d\n", inputnum, rawdata);
				break;
			case '2':
					voltequ = anaInVolts(inputnum);
					printf("CH%2d is %.5f V\n", inputnum, voltequ);
				break;
			case '3':
					anaInInfo(inputnum, &rawdata, &voltequ);
					printf("CH%2d is %.5f V from raw data %d\n", inputnum, voltequ, rawdata);
				break;
			default:
			break;
		}
	}		
}	//end main
