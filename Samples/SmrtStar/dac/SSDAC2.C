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
	ssdac2.c
	
	This sample program is used with Smart Star products, specifically
	with 0-10V and +/-10V DAC and ADC boards.

	!!!Caution this will overwrite the calibration constants set at the
	   factory.
	   
	This program demonstrates how to recalibrate a DAC channel using an
	an ADC board.	Two known voltages and defines the two coefficients,
	gain and offset, which will be rewritten into the DAC's EEPROM. 

	Instructions:
	This setup assumes that both DAC and ADC boards are the same range.
	Connect a DAC output channel to an ADC input channel.
	Compile this and run this program.
	
***************************************************************************/
#class auto

#define LOCOUNT 400			//gives highest voltage
#define HICOUNT 3695			//gives lowest voltage


void msdelay (long sd)
{
	auto unsigned long t1;

	t1 = MS_TIMER + sd;
	while ((long)(MS_TIMER-t1) < 0);
}

void main()
{
	auto int dacslot, adcslot, inputnum, outputnum, msgcode;
	static float voltout, volt1, volt2;
	
	brdInit();
	
	printf("Please enter DAC board slot position, 0 thru 6....");
	do
	{
		dacslot = getchar();
	} while ((dacslot < '0') || (dacslot > '6'));
	printf("Slot %d chosen.\n", dacslot-=0x30);

	///// configure all outputs to zero volts and enable output
	for (outputnum=0; outputnum<=7; outputnum++)
	{
		if (msgcode = anaOutEERd(ChanAddr(dacslot, outputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,outputnum);
			exit(0);
		}
		else
			anaOutVolts(ChanAddr(dacslot, outputnum), 0.0);
	}
	
	anaOutEnable();
	
	printf("Please enter an output channel, 0 thru 7....");
	do
	{
		outputnum = getchar();
	} while (!((outputnum >= '0') && (outputnum <= '7')));
	printf("channel %d chosen.\n", outputnum-=0x30);

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
	
	/////get voltages from two known raw data
	anaOut(ChanAddr(dacslot, outputnum), HICOUNT);
	msdelay(10);			
	volt1 = anaInVolts(ChanAddr(adcslot, inputnum));
	
	anaOut(ChanAddr(dacslot, outputnum), LOCOUNT);
	msdelay(10);			
	volt2 = anaInVolts(ChanAddr(adcslot, inputnum));

	if (anaOutCalib(ChanAddr(dacslot, outputnum), HICOUNT, volt1, LOCOUNT, volt2))
		printf("Cannot make coefficients\n");
	else
	{
		/////store coefficients into eeprom
		while (anaOutEEWr(ChanAddr(dacslot, outputnum)));
		printf("Wrote coefficients to dac eeprom\n");
		
		printf("Read coefficients from dac eeprom\n");
		if (msgcode = anaOutEERd(ChanAddr(dacslot, outputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,outputnum);
			exit(0);
		}
		
		for (voltout=0; voltout <=10; voltout+=1.25)
		{
			anaOutVolts(ChanAddr(dacslot, outputnum), voltout);
			msdelay(10);					// variable delay needed for large voltage change
			volt1 = anaInVolts(ChanAddr(adcslot, inputnum));
			printf("DAC channel %d output %.3fV, ADC channel %2d input %.3fV\n", outputnum,
					voltout, inputnum, volt1);
		}
	}
}
