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
/*******************************************************************
	parity.c

	This program is used with RCM3600 series controllers
	with prototyping boards.

	Description
	===========
	This program demonstrates use of parity modes by repeatedly
	sending byte values 0-127 from serial port D to serial port C.
	The program will switch between generating parity or not on
	port D. Port C will always be checking parity, so parity
	errors should occur during every other sequence.

	Prototyping Board Connections
	=============================

		On the RS232 connector
		-----------------------------

		(PC0) TXD <-------> RXC (PC3)


	Instructions
	============
	1.  Run this program and observe error sequence in STDIO.

*******************************************************************/
#class auto

#define DINBUFSIZE 31
#define DOUTBUFSIZE 15
#define CINBUFSIZE 31
#define COUTBUFSIZE 31

const long baud_rate = 9600L;

main()
{
	auto char received;
	auto int i;
	auto int txconfig;

	brdInit();				//initialize board for this demo

   BitWrPortI(PEDR, &PEDRShadow, 0, 5);	//set low to enable rs232 device

	serDopen(baud_rate);
	serCopen(baud_rate);

	serDparity(PARAM_OPARITY);
	serCparity(PARAM_OPARITY);
	txconfig = PARAM_OPARITY;

	printf("Starting...\n");

	while (1)
	{
		costate
		{
			//send as fast as we can
			for (i = 0; i < 128; i++)
			{
				waitfor(DelayMs(10)); 	//necessary if we are not using
											 	//flow control
				waitfordone{ cof_serDputc(i); }
			}
			// wait for data buffer, internal data and shift registers to become empty
  			waitfor(serDwrFree() == DOUTBUFSIZE);
  			waitfor(!((RdPortI(SDSR)&0x08) || (RdPortI(SDSR)&0x04)));
			yield;

			//toggle between sending parity bits, and not
			if (txconfig)
			{
				txconfig = PARAM_NOPARITY;
				printf("\nParity option set to no parity\n");
			}
			else
			{
				txconfig = PARAM_OPARITY;
				printf("\nParity option set to odd parity\n");
			}
			serDparity(txconfig);
		}

		costate
		{
			//receive characters in a leisurely fashion
			waitfordone
			{
				received = cof_serCgetc();
			}
			printf("received 0x%x\n", received);
			if (serCgetError() & SER_PARITY_ERROR)
			{
				printf("PARITY ERROR\n");
			}
	   }
	}
}

