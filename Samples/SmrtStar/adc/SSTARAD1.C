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
	sstarad1.c
	
	This sample program is used with Smart Star products.

	This program demonstrates how to recalibrate an ADC channel using
	two known voltages and defines the two coefficients, gain and offset,
	which will be rewritten into the ADC's EEPROM. 
	Computed equivalent voltage will be displayed.

	Instructions:
	Connect a power supply of 0-10 volts (or +/-10 volts depending on your
	board) to the input channel.
	Follow the prompted directions of this program during execution.
	Compile and run this program.
	Vary the voltage from 0-10 volts (or +/-10 volts depending on your
	board) to the input channel.
	
	This setup defaults for voltages of 0-10 volts.
	Change the macros below to match your board.
***************************************************************************/
#class auto

#define LOVOLT 1.0		//voltage for first reading,
								//for 0-10V enter 1.0V, for +/- 10V enter -9.0					
#define HIVOLT 9.0		//voltage for second reading


void main ()
{
	auto int rawdata, data1, data2,i;
	auto float voltequ, volt1, volt2;
	auto int inputnum, slotnum, msgcode;
	auto char buffer[64];

	brdInit();

	printf("Please enter ADC board slot position, 0 thru 6....");
	do {
		slotnum = getchar();
		} while ((slotnum < '0') || (slotnum > '6'));
	printf("Slot %d chosen.\n", slotnum-=0x30);

	if (msgcode = anaInEERd(ChanAddr(slotnum, 0)))
	{
		printf("Error %d: eeprom unreadable or empty slot\n", msgcode);
		exit(0);
	}
	
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

	/////Get two data points using known voltages
	printf("\nAdjust the voltage to approximately %.3fV and then ENTER the actual\n", LOVOLT);
	printf("voltage measurement, (floating point value) = ");
	gets(buffer);
	volt1 = atof(buffer);
	data1 = anaIn(ChanAddr(slotnum, inputnum));

	printf("\nAdjust the voltage to approximately %.3fV and then ENTER the actual\n", HIVOLT);
	printf("voltage measurement, (floating point value) = ");
	gets(buffer);
	volt2 = atof(buffer);
	data2 = anaIn(ChanAddr(slotnum, inputnum));
	printf("\n");
			
	/////calculate gains and offsets
	if	(anaInCalib(ChanAddr(slotnum, inputnum), data1, volt1, data2, volt2))
		printf("Cannot make coefficients\n");
	else
	{
		/////store coefficients into eeprom
		while (anaInEEWr(ChanAddr(slotnum, inputnum)));
		printf("Wrote coefficients to eeprom\n");
		
		/////read back coefficients from eeprom
		printf("Read coefficients from eeprom\n");
		if (msgcode = anaInEERd(ChanAddr(slotnum, inputnum)))
		{
			printf("Error %d: eeprom unreadable; channel %d\n", msgcode,inputnum);
			exit(0);
		}
		
		printf("Vary current on channel %d\n", inputnum);
		///// Display the current for a given channel
		while (1)
		{
			voltequ = anaInVolts(ChanAddr(slotnum, inputnum));
			printf("Voltage at CH%d is %.5f V\n", inputnum, voltequ);
		}
	}
}	//end main

