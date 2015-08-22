/***************************************************************************
	sstarad4.c
	
   Z-World, 2001
	This sample program is used with Smart Star Analog to Digital cards.
	
	This program demonstrates how to compute coefficients for ADC channels
	using	two known voltages.
	Coefficients are stored and read back from Flash to compute voltages
	and are displayed.
	
	!!!This program does not write coefficients to the ADC's eeprom and
	   must be compiled to Flash.

	Instructions:
	Connect a power supply of 0-10 volts (or +/-10 volts depending on your
	board) to input channels.
	Follow the prompted directions of this program during execution.
	Compile and run this program.

	This setup defaults for voltages of 0-10 volts.
	Change the macros below to match your board.	
***************************************************************************/
#class auto

#if FLASH_COMPILE == 0
#error "MUST COMPILE INTO FLASH MEMORY."
#endif

#define ADC_CARD ADCBOARD	//use ADC board for parameter

#define LOVOLT 1.0			//voltage for first reading, for +/- 10V enter -9.0
#define HIVOLT 9.0			//voltage for second reading


void main ()
{
	auto int inputnum, slotnum, status;
	auto int data1[11], data2[11];
	auto unsigned int rawdata;
	auto float voltequ, volt1, volt2;
	auto char buffer[64];

	brdInit();

	printf("Please enter ADC board slot position, 0 thru 6....");
	do {
		slotnum = getchar();
		} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	/////get raw data values
	printf("\nAdjust the voltage to approximately %.3fV and then ENTER the actual\n", LOVOLT);
	printf("voltage measurement, (floating point value) = ");
	gets(buffer);
	volt1 = atof(buffer);
	for (inputnum=0; inputnum<=10; inputnum++)
	{
		data1[inputnum] = anaIn(ChanAddr(slotnum, inputnum));
	}
	printf("\nAdjust the voltage to approximately %.3fV and then ENTER the actual\n", HIVOLT);
	printf("voltage measurement, (floating point value) = ");
	gets(buffer);
	volt2 = atof(buffer);
	for (inputnum=0; inputnum<=10; inputnum++)
	{
		data2[inputnum] = anaIn(ChanAddr(slotnum, inputnum));
	}

	for (inputnum=0, status=0; inputnum<=10; inputnum++)
	{
		/////calculate gains and offsets
		if	(anaInCalib(ChanAddr(slotnum, inputnum), data1[inputnum], LOVOLT, data2[inputnum], HIVOLT))
		{
			printf("Cannot make coefficients for channel %d\n", inputnum);
			status++;
		}
	}
	
	if (!status)
	{
		/////store coefficients into flash and read back
		anaSaveCalib(ADC_CARD);
		memset(_adcCalib, 0, sizeof(_adcCalib));		//clear table
		anaLoadCalib(ADC_CARD);
		printf("Save and read coefficients from flash\n");
	
		for (inputnum=0; inputnum<=10; inputnum++)
		{
			voltequ = anaInVolts(ChanAddr(slotnum, inputnum));
			printf("Voltage at CH%d is %.5f V \n", inputnum, voltequ);
		}
	}
	
}	//end main
