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

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

   Description
	===========
	This demonstration will repeatedly send byte values 0-127 from serial
	port E to serial port F. The program will switch between generating
   parity or not on port E. Port F will always be checking parity, so
   parity errors should occur during every other sequence.

   Note: For the sequence that does get parity errors, the errors won't
	occur for each byte received. This is because certain byte patterns
   along with the stop bit will appear to generate the proper parity for
   the UART.

   Connections
	===========
   1. Jumper TXE to RXF together located on J1 pins 3 and 4 with
   	using the 0.1 spacing shunt.

   Instructions
	============
   1. Power-on the controller.
	2. Compile and run this program.
   3. Stop program.
	4. View STDIO window to see the test results of the sample program.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

#define EINBUFSIZE 255
#define EOUTBUFSIZE 255


#define FINBUFSIZE 255
#define FOUTBUFSIZE 255

const long baud_rate = 9600L;

void main()
{
	auto int received, receive_count;
	auto int i;
	auto int txconfig;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	serEopen(baud_rate);
   serFopen(baud_rate);

   // Serial mode must be done after opening the serial ports
   serMode(0);

   // Clear serial data buffers
   serErdFlush();
   serFrdFlush();
   serEwrFlush();
   serFwrFlush();

	printf("Start with the parity option set properly\n");
	serEparity(PARAM_OPARITY);
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
				serEputc(i);
			}
         // Wait for data buffer, internal data and shift registers to become empty
   		waitfor(serEwrFree() == EOUTBUFSIZE);
   		waitfor(!((RdPortI(SESR)&0x08) || (RdPortI(SESR)&0x04)));

         // Wait for entire 128 bytes to be processed
         waitfor(receive_count == 128);

			// Toggle between parity options
			if (txconfig)
			{
				txconfig = PARAM_NOPARITY;
				printf("\n\nInproperly set parity option\n");
			}
			else
			{
				txconfig = PARAM_OPARITY;
				printf("\n\nProperly set parity option\n");
			}
			serEparity(txconfig);
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