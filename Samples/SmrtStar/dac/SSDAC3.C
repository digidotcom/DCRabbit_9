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
	ssdac3.c
	
	This sample program is used with Smart Star products, specifically
	for the 4-20mA DAC board.

	!!!Caution this will overwrite the calibration constants set at the
	   factory.
	   
	This program demonstrates how to recalibrate an DAC channel using
	two known currents and defines the two coefficients, gain and offset,
	which will be rewritten into the DAC's EEPROM. 

	Instructions:
	Connect a current meter to an output channel.
	Compile and run this program.
	Follow the prompted directions of this program during execution.
	
	Note:  When entering current values, milli-Amp values will produce
	a milli-Amp value. Similarly, Amp values will produce an Amp value.
	
***************************************************************************/
#class auto

#define LOCOUNT 400				//gives highest current
#define HICOUNT 3695				//gives lowest current


void main()
{
	auto int slotnum, outputnum, i, msgcode;
	static float curout, cur1, cur2;
	auto char tmpbuf[24];
	
	brdInit();
	
	printf("Please enter DAC board slot position, 0 thru 6....");

	do
	{
		slotnum = getchar();
	} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	///// configure all outputs to 4 mA and enable output
	for (outputnum=0; outputnum<=7; outputnum++)
	{
		if (msgcode = anaOutEERd(ChanAddr(slotnum, outputnum)))
		{
			printf("Error %d: eeprom unreadable or empty slot; channel %d\n", msgcode,outputnum);
			exit(0);
		}
		else
			anaOutmAmps(ChanAddr(slotnum, outputnum), 0.0);
	}
	anaOutEnable();
	
	printf("Please enter an output channel, 0 thru 7....");
	do
	{
		outputnum = getchar();
	} while (!((outputnum >= '0') && (outputnum <= '7')));
	printf("channel %d chosen.\n", outputnum-=0x30);

	/////get currents from two known raw data
	anaOut(ChanAddr(slotnum, outputnum), HICOUNT);
	printf("Type first current reading from meter and press Enter   ");
	cur1 = atof(gets(tmpbuf));
	
	anaOut(ChanAddr(slotnum, outputnum), LOCOUNT);
	printf("Type second current reading from meter and press Enter  ");
	cur2 = atof(gets(tmpbuf));
	
	if (anaOutCalib(ChanAddr(slotnum, outputnum), HICOUNT, cur1, LOCOUNT, cur2))
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
			printf("\nType the desired current and press Enter  ");
			curout = atof(gets(tmpbuf));
			printf("Observe current on meter.....\n");
			anaOutmAmps(ChanAddr(slotnum, outputnum), curout);
		}
	}
}
