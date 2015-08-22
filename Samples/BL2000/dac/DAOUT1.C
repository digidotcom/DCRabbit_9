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
	daout1.c
	
   This sample program is used with the BL20XX series controllers.

   This program outputs a voltage that can be read with a voltmeter. The 
   output voltage is computed with using the calibration constants that
   are read from EEPROM (simulated in Flash).
   	
	!!!This program must be compiled to Flash.
   
	Instructions:
	Connect a voltage meter to an output channel DAC0 or DAC1.
	Compile and run this program.
	Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int channel;
	auto float voltout;
	auto char tmpbuf[32];
	
	brdInit();
	printf("Please enter an output channel, 0 or 1....");
	do
	{
		channel = getchar();
	} while (!((channel >= '0') && (channel <= '1')));
	printf("channel %d chosen.\n", channel-=0x30);
		
	while (1)
	{
		printf("\nType a desired voltage (in Volts)  ");
		voltout = atof(gets(tmpbuf));
		printf("Observe voltage on meter.....\n");
		anaOutVolts(channel, voltout);
	}
}
