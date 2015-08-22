/***************************************************************************
	ssdac4.c
	
   Z-World, 2001
	This sample program is used with Smart Star products, specifically
	with 4-20mA DAC and ADC boards.

	!!!Caution this will overwrite the calibration constants set at the
	   factory.
	   
	This program demonstrates how to recalibrate a DAC channel using an
	an ADC board.	Two known currents and defines the two coefficients,
	gain and offset, which will be rewritten into the DAC's EEPROM. 

	Instructions:
	This setup assumes that both DAC and ADC boards are the same
	current range.
	Connect a DAC output channel to an ADC input channel.
	Compile this and run this program.
	Follow the prompted directions of this program during execution.
	
	Note:  When entering current values, milli-Amp values will produce
	a milli-Amp value. Similarly, Amp values will produce an Amp value.
	
***************************************************************************/
#class auto

#define LOCOUNT 400			//gives highest current
#define HICOUNT 3695			//gives lowest current


void msdelay (long sd)
{
	auto unsigned long t1;

	t1 = MS_TIMER + sd;
	while ((long)(MS_TIMER-t1) < 0);
}

void main()
{
	auto int dacslot, adcslot, inputnum, outputnum, i, msgcode;
	static float curout, cur1, cur2, curin;
	
	brdInit();
	
	printf("Please enter DAC board slot position, 0 thru 6....");
	do
	{
		dacslot = getchar();
	} while ((dacslot < '0') || (dacslot > '6'));
	printf("Slot %d chosen.\n", dacslot-=0x30);

	printf("Please enter an output channel, 0 thru 7....");
	do
	{
		outputnum = getchar();
	} while (!((outputnum >= '0') && (outputnum <= '7')));
	printf("channel %d chosen.\n", outputnum-=0x30);

	///// configure all outputs to 4 mA and enable output
	for (i=0; i<=7; i++)
	{
		if (msgcode = anaOutEERd(ChanAddr(dacslot, outputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,outputnum);
			exit(0);
		}
		else
			anaOutmAmps(ChanAddr(dacslot, outputnum), 0.0);
	}
	anaOutEnable();
	
	printf("Please enter ADC board slot position, 0 thru 6....");
	do {
		adcslot = getchar();
		} while ((adcslot < '0') || (adcslot > '6'));
	printf("Slot %d chosen.\n", adcslot-=0x30);

	printf("Please enter an input channel, 0 thru A (10)....");
	do {
		inputnum = getchar();
		} while (!((inputnum >= '0') && (inputnum <= '9')) && (inputnum != 'a') && (inputnum != 'A'));
	if ((inputnum == 'a') || (inputnum == 'A'))
	{
		printf("channel 10 chosen.\n");
		inputnum=10;
	}
	else
		printf("channel %d chosen.\n", inputnum-=0x30);

	/////read coefficients from adc eeprom
	printf("Read coefficients from adc eeprom\n");
	if (msgcode = anaInEERd(ChanAddr(adcslot, inputnum)))
	{
		printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,inputnum);
		exit(0);
	}
	
	/////get currents from two known raw data
	anaOut(ChanAddr(dacslot, outputnum), HICOUNT);
	msdelay(10);			
	cur1 = anaInmAmps(ChanAddr(adcslot, inputnum));
	
	anaOut(ChanAddr(dacslot, outputnum), LOCOUNT);
	msdelay(10);			
	cur2 = anaInmAmps(ChanAddr(adcslot, inputnum));

	if (anaOutCalib(ChanAddr(dacslot, outputnum), HICOUNT, cur1, LOCOUNT, cur2))
		printf("Cannot make coefficients\n");
	else
	{
		/////store coefficients into eeprom
		while (anaOutEEWr(ChanAddr(dacslot, outputnum)));
		printf("Wrote coefficients to dac eeprom\n");
		
		printf("Read coefficients from dac eeprom\n");
		if (msgcode = anaOutEERd(ChanAddr(dacslot, outputnum)))
		{
			printf("Error %d: eeprom unreadable; channel %d\n", msgcode,outputnum);
			exit(0);
		}
		
		if (cur1 < 1.0)
		{
			curout = 0.004;
			cur1 = 0.020;
			cur2 = 0.0015;
		}
		else
		{
			curout = 4.00;
			cur1 = 20.0;
			cur2 = 1.5;
		}
		
		for (curout; curout <= cur1; curout+=cur2)
		{
			anaOutmAmps(ChanAddr(dacslot, outputnum), curout);
			msdelay(10);						// variable delay needed for large voltage change
			curin = anaInmAmps(ChanAddr(adcslot, inputnum));
			if (curin < 1.0)
				printf("DAC channel %d output %.5f A, ADC channel %d input %.5f A\n", outputnum,
					curout, inputnum, curin);
			else
				printf("DAC channel %d output %.4f mA, ADC channel %d input %.4f mA\n", outputnum,
					curout, inputnum, curin);
		}
	}
}
