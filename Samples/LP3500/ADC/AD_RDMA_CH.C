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
	ad_rdma_ch.c

	This sample program is used with LP3500 series controllers

	Description
	===========
	This program demonstrates how to read an ADC milli-Amp channel using
	previously defined coefficients. It will also continuously display
	the current that is being monitored.

	Instructions
	============
	1. Jumpers must be installed on J3 for each input to be read.

	2. Connect a power supply of 0-3 volts with a series 0 ohm resistor
	to one of the AIN channel 0 - 3 on the controller


	----------------|									|---------------------------
						 |									| 4-20ma board
	Power supply	 |  100 ohm	 |-------|		|
	0 - 4v		  + |-/\/\/\----|current|-----| AIN channels 0 - 3
	                |	1/4 watt  | meter |		|
	                |				 ---------		|
	              - |-------------------------| AGND
					    |									|
	----------------|									|---------------------------

	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Vary voltage (0-4v) on power supply to see the CURRENT meter track
	what is displayed by Dynamic C (4-20ma).

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/
#class auto					/* Change local var storage default to "auto" */

#define STARTCHAN 0
#define ENDCHAN 3


void main ()
{
	auto int inputnum;
	auto char buffer[64];
	auto float currentequ;

	brdInit();

	while (1)
	{
		printf("\nChoose the AD mAmp channel %d to %d .... ", STARTCHAN, ENDCHAN);
		gets(buffer);
		inputnum = atoi(buffer);

		printf("Vary current on channel %d\n", inputnum);
		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			currentequ = anaInmAmps(inputnum);
			printf("Current at CH%d is %.2fma\n", inputnum, currentequ);
			printf("\npress enter key to read value again or 'Q' to view another channel\n\n");
			gets(buffer);
		}
	}


}	//end main


