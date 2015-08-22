/***************************************************************************

	read4_20.c
   Z-World, 2001
   
	This sample program is used with Smart Star 4-20 milli-Amp Analog
	to Digital cards.
	
	This program reads and displays current and equivalent values of each
	ADC channel.  Coefficients are read from the ADC's EEPROM to compute
	equivalent current.
	Computed raw data and equivalent current will be displayed.
	
	Instructions:
	Connect a 4 to 20 mA power supply to input channels.
	Compile and run this program.
	Follow the prompted directions of this program during execution.

***************************************************************************/
#class auto

//---------------------------------------------------------
//	displays both the raw data count and current equivalent
//---------------------------------------------------------
void anaInInfo (unsigned int channel, unsigned int *rd, float *ve)
{
	auto unsigned int chan, value;
	auto float volt;

	chan = ((channel>>7) & 0x0007)*ADCHANNELS + (channel & 0x000F);
	
	value = anaIn(channel);
	volt = (_adcCalib[chan][0] * (_adcCalib[chan][1] - value));
	*rd = value;
	*ve = volt;
}


main ()
{
	auto unsigned int rawdata;
	auto int inputnum, slotnum, keypress, msgcode;
	auto float currentequ;

	brdInit();

	printf("Please enter ADC board slot position, 0 thru 6....");
	do {
		slotnum = getchar();
		} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	/////must read coefficients from eeprom to calibrate
	for (inputnum=0; inputnum<=10; inputnum++)
	{
		if (msgcode = anaInEERd(ChanAddr(slotnum, inputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,inputnum);
			exit(0);
		}
	}

	while (1)
	{	
		printf("\nChoose:\n");
		printf("  1 to display raw data only\n");
		printf("  2 to display current only\n");
		printf("  3 to display both\n\n");

		switch (getchar())
		{
			case '1': 
				for (inputnum=0; inputnum<=10; inputnum++)
				{
					rawdata = anaIn(ChanAddr(slotnum, inputnum));
					printf("CH%2d raw data %d\n", inputnum, rawdata);
				}	
				break;
			case '2':
				for (inputnum=0; inputnum<=10; inputnum++)
				{
					currentequ = anaInmAmps(ChanAddr(slotnum, inputnum));
					if (currentequ < 4.00)
						printf("CH%2d is %.6f A\n", inputnum, currentequ);
					else
						printf("CH%2d is %.6f mA\n", inputnum, currentequ);
				}	
				break;
			case '3':
				for (inputnum=0; inputnum<=10; inputnum++)
				{
					anaInInfo(ChanAddr(slotnum, inputnum), &rawdata, &currentequ);
					if (currentequ < 4.00)
						printf("CH%2d is %.6f A from raw data %d\n", inputnum, currentequ, rawdata);
					else
						printf("CH%2d is %.6f mA from raw data %d\n", inputnum, currentequ, rawdata);
				}	
				break;
			default:
			break;
		}
	}		
}	//end main

