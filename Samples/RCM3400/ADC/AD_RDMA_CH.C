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

	This sample program is for the RCM3400 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates how to read an ADC milli-Amp channel using
	previously defined coefficients. It will also continuously display
	the current that is being monitored.

	Instructions
	============
	1.	Make sure pins 2-3 are connected on JP1, JP2, and JP9.  Connect pins 1-2,
		3-4, 5-6, 7-8 on JP5.

	2. Connect a power supply of 0-5 volts to one of the AIN channel 3-6
		on the controller.
	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Vary voltage (0-5v) on power supply to see the CURRENT meter track
	what is displayed by Dynamic C (4-20ma).

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/
#class auto

#define ADC_SCLKBAUD 115200ul
#define STARTCHAN 3
#define ENDCHAN 6


main ()
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


