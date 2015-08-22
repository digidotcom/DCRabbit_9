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
/*****************************************
Demonstration of parity modes

This demonstration will repeatedly send byte values 0-127 from serial port B
to serial port C. Each time the program will switch between generating
parity or not on port B. Port C will always be checking parity,
so parity errors should occur during every other sequence.

To perform this test you will need to connect PC4 to PC3

*******************************************/
#class auto 			// Change default storage class for local variables: on the stack

#define BINBUFSIZE 31
#define BOUTBUFSIZE 15
#define CINBUFSIZE 31
#define COUTBUFSIZE 31

const long BAUD_RATE = 9600L;

void main()
{
	auto char received;
	auto int i;
	auto int txconfig;

	serBopen(BAUD_RATE);
	serCopen(BAUD_RATE);

	serBparity(PARAM_OPARITY);
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
				waitfor(DelayMs(10)); //necessary if we are not using
											 //flow control
				waitfordone{ cof_serBputc(i); }
			}
			//toggle between sending parity bits, and not
			if (txconfig)
			{
				txconfig = 0;
			}
			else
			{
				txconfig = PARAM_OPARITY;
			}
			serBparity(txconfig);
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

