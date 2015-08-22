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
	adrd_ma_ch.c
	
	This sample program is used with OP7200 series controllers

	Description
	===========
	This program demonstrates how to read an ADC milli-Amp channel using
	previously defined coefficients. It will also continuously display
	the current that is being monitored.

	Important Note:
	---------------
	You must calibrate the A/D channel for 4-20ma mode of operation
	before running this program.  
	
	
	Instructions
	============
	1. Connect a 100 ohm resistor from AGND to the A/D channel that you
	   want to convert to a 4-20ma input channel.
	
	2. Connect a power supply of 0-3 volts with a series 100 ohm resistor
	 	to one of the AIN channel 0 - 7 on the controller

                         
	-----------------|									 |---------------------------
						  |									 | 4-20ma board	
	Power supply	  |          	|-------|		 | 
	0 - 4v		 POS |------------|current|-------| AIN channels 0 - 7
	                 |	         | meter |	|	 |
	                 |            ---------   _   |
	                 |    		              | |  |		
                    |                       | | <-- This external resistor  
                    |             100 Ohm   | |  |  is required for 4-20ma 
	                 |             1/4 Watt   -   |  operation.
 	                 |                        |   |  
	             NEG |----------------------------| AGND
					     |									 |
	-----------------|									 |---------------------------


	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Vary voltage (0-4v) on power supply to see the CURRENT meter track
	   what is displayed by Dynamic C (4-20ma). 

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/
#class auto


void main ()
{
	auto int channel;
	auto float currentequ;
	auto int key;
	auto char buffer[64];
	
	brdInit();
	while (1)
	{
		printf("\n\nPlease enter an ADC channel (0-7) = ");
		do
		{
			channel = getchar();
		} while (!( (channel >= '0') && (channel <= '7')) );
		channel = channel - 0x30;
		printf("%d", channel);
		while(kbhit()) getchar();

		printf("\n");
		printf("Vary current on channel %d\n", channel);
		do
		{
			currentequ = anaInmAmps(channel);
			printf("Current at CH%d is %.2fma\n", channel, currentequ);
			printf("\nPress ENTER key to read value again or 'Q' or read another channel\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();

		}while(key != 'q' && key != 'Q');
	}

	
}	//end main

