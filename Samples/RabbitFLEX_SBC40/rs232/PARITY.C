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
/**************************************************************************

	parity.c

 	This program is used with RabbitFLEX SBC40 boards.  Note that you must
   have 2 serial ports available--either Ports D and F or Ports E and F.


   Description
	===========
	This demonstration will repeatedly send byte values 0-127 from serial
	port D or E to serial port F. The program will switch between generating
   parity or not on port D or E. Port F will always be checking parity, so
   parity errors should occur during every other sequence.

   Note: For the sequence that does get parity errors, the errors won't
	occur for each byte received. This is because certain byte patterns
   along with the stop bit will appear to generate the proper parity for
   the UART.

   Connections
	===========
   1. Jumper TXD/E to RXF together located on J6 pins 1 and 2.

   Instructions
	============
   1. Power-on the controller.
	2. Compile and run this program.
   3. Stop program.
	4. View STDIO window to see the test results of the sample program.

***************************************************************************/
// Comment this out if you are using serial port E
#define USE_SERIALD

//  Set a default of declaring all local variables "auto" (on stack)
#class auto


#define DINBUFSIZE 255
#define DOUTBUFSIZE 255


#define EINBUFSIZE 255
#define EOUTBUFSIZE 255


#define FINBUFSIZE 255
#define FOUTBUFSIZE 255

#ifdef USE_SERIALD
	#define serXopen serDopen
	#define serXwrFlush serDwrFlush
	#define serXrdFlush serDrdFlush
	#define serXputc serDputc
	#define serXgetc serDgetc
   #define serXparity serDparity
   #define serXwrFree serDwrFree
   #define SXSR SDSR
#else
	#define serXopen serEopen
	#define serXwrFlush serEwrFlush
	#define serXrdFlush serErdFlush
	#define serXputc serEputc
	#define serXgetc serEgetc
   #define serXparity serEparity
   #define serXwrFree serEwrFree
   #define SXSR SESR
#endif

#define SERXOUTBUFSIZE 255

const long baud_rate = 9600L;

void main()
{
	auto int received, receive_count;
	auto int i;
	auto int txconfig;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	serXopen(baud_rate);
   serFopen(baud_rate);

   // Serial mode must be done after opening the serial ports
   serMode(0);

   // Clear serial data buffers
   serXrdFlush();
   serFrdFlush();
   serXwrFlush();
   serFwrFlush();

	printf("Start with the parity option set properly\n");
	serXparity(PARAM_OPARITY);
   serFparity(PARAM_OPARITY);

	txconfig = PARAM_OPARITY;

	while (1)
	{
		costate
		{
      	receive_count = 0;
			// Send data value 0 - 127
			for (i = 0; i < 128; i++)
			{
         	yield; // Yield so data can be read from serial port C
				serXputc(i);
			}
         // Wait for data buffer, internal data and shift registers to become empty
   		waitfor(serXwrFree() == SERXOUTBUFSIZE);
   		waitfor(!((RdPortI(SXSR)&0x08) || (RdPortI(SXSR)&0x04)));

         // Wait for entire 128 bytes to be processed
         waitfor(receive_count == 128);

			// Toggle between parity options
			if (txconfig)
			{
				txconfig = PARAM_NOPARITY;
				printf("\n\nImproperly set parity option\n");
			}
			else
			{
				txconfig = PARAM_OPARITY;
				printf("\n\nProperly set parity option\n");
			}
			serXparity(txconfig);
		}
		costate
		{
      	// Receive characters in a leisurely fashion
 			if((received = serFgetc()) != -1)
         {
         	receive_count++;
				printf("received 0x%x\n", received);
				if (serFgetError() & SER_PARITY_ERROR)
				{
					printf("PARITY ERROR\n");
				}
	   	}
      }
	}
}