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
	ssdac1.c
	
	This sample program is used with Smart Star products, specifically
	for 0-10V and +/-10V DAC boards.

	!!!Caution this will overwrite the calibration constants set at the
	   factory.
	   
	This program demonstrates how to recalibrate an DAC channel using
	two known voltages and defines the two coefficients, gain and offset,
	which will be rewritten into the DAC's EEPROM. 

	Instructions:
	Connect a voltage meter to an output channel.
	Compile and run this program.
	Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto

#define LOCOUNT 400			//gives highest voltage
#define HICOUNT 3695			//gives lowest voltage

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
	
	printf("Please enter an output channel, 0 thru 7....");
	do
	{
		outputnum = getchar();
	} while (!((outputnum >= '0') && (outputnum <= '7')));
	printf("channel %d chosen.\n", outputnum-=0x30);

	/////get voltages from two known raw data
	anaOut(ChanAddr(slotnum, outputnum), HICOUNT);
	printf("Type first voltage reading (in Volts) from meter and press Enter   ");
	volt1 = atof(gets(tmpbuf));
	
	anaOut(ChanAddr(slotnum, outputnum), LOCOUNT);
	printf("Type second voltage reading (in Volts) from meter and press Enter  ");
	volt2 = atof(gets(tmpbuf));

	if (anaOutCalib(ChanAddr(slotnum, outputnum), HICOUNT, volt1, LOCOUNT, volt2))
		printf("Cannot make coefficients\n");
	else
	{
		/////store coefficients into eeprom
		while (anaOutEEWr(ChanAddr(slotnum, outputnum)));
		printf("Wrote coefficients to eeprom\n");
		
		printf("Read coefficients from eeprom\n");
		if (msgcode = anaOutEERd(ChanAddr(slotnum, outputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,outputnum);
			exit(0);
		}
				
		while (1)
		{
			printf("\nType a desired voltage (in Volts)  ");
			voltout = atof(gets(tmpbuf));
			printf("Observe voltage on meter.....\n");
			anaOutVolts(ChanAddr(slotnum, outputnum), voltout);
		}
	}
}
