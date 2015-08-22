/***************************************************************************
	anavout.c
   Z-World, 2002
   
	This sample program is used with Smart Star Digital to Analog cards,
	specifically for 0-10V and +/-10V DAC boards.
	
	This program outputs equivalent voltage value of a DAC channel.
	Coefficients are read once from the DAC's EEPROM to compute equivalent
	voltages.
	
	Instructions
	------------
	Connect a voltage meter to an output channel.
	Compile and run this program.
	Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto


void main()
{
	auto int slotnum, outputnum, msgcode;
	static float voltout, volt1, volt2;
	auto char tmpbuf[24];
	
	brdInit();
	
	printf("Please enter DAC board slot position, 0 thru 6....");
	do
	{
		slotnum = getchar();
	} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);
		
	///// configure all outputs to zero volts and enable output
	for (outputnum=0; outputnum<=7; outputnum++)
	{
		if (msgcode = anaOutEERd(ChanAddr(slotnum, outputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,outputnum);
			exit(0);
		}
		else
			anaOutVolts(ChanAddr(slotnum, outputnum), 0.0);
	}
	
	anaOutEnable();
	
	while (1)
	{
		printf("\nPlease enter an output channel, 0 thru 7....");
		do
		{
			outputnum = getchar();
		} while (!((outputnum >= '0') && (outputnum <= '7')));
		printf("channel %d chosen.\n", outputnum-=0x30);

		printf("Type a desired voltage (in Volts)  ");
		voltout = atof(gets(tmpbuf));
		printf("Observe voltage on meter.....\n");
		anaOutVolts(ChanAddr(slotnum, outputnum), voltout);
	}

}
