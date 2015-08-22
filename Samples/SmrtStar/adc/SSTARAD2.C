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
	sstarad2.c
	
	This sample program is used with Smart Star Analog to Digital cards.
	
	This program reads and displays voltage and equivalent values of each
	ADC channel.  Coefficients are read from the ADC's EEPROM to compute
	equivalent voltages.
	Computed raw data and equivalent voltages will be displayed.
	
	Instructions:
	Connect a power supply of 0-10 volts (or +/-10 volts depending on your
	board) to input channels.
	Compile and run this program.
	Follow the prompted directions of this program during execution.

***************************************************************************/
#class auto

//---------------------------------------------------------
//	displays both the raw data count and voltage equivalent
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


void main ()
{
	auto unsigned int rawdata;
	auto int inputnum, slotnum, keypress, msgcode;
	auto float voltequ;

	brdInit();

	printf("Please enter ADC board slot position, 0 thru 6....");
	do {
		slotnum = getchar();
		} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	/////read coefficients from eeprom
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
		printf("  2 to display voltage only\n");
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
					voltequ = anaInVolts(ChanAddr(slotnum, inputnum));
					printf("CH%2d is %.5f V\n", inputnum, voltequ);
				}	
				break;
			case '3':
				for (inputnum=0; inputnum<=10; inputnum++)
				{
					anaInInfo(ChanAddr(slotnum, inputnum), &rawdata, &voltequ);
					printf("CH%2d is %.5f V from raw data %d\n", inputnum, voltequ, rawdata);
				}	
				break;
			default:
			break;
		}
	}		
}	//end main

