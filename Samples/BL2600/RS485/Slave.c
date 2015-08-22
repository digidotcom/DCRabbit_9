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

	slave.c

	This sample program is for the BL2600 series controllers.

	Connect circuitry as	described in the User Manual in the main
	hardware chapter under Serial Communication.

	This program demonstrates a simple RS485 transmission
	of alpha characters to a master controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	Use master.c to program the master controller.

	Make the following connections:

		Master to  Slave
		485+ <---> 485+
		485- <---> 485-
		GND  <---> GND

   Note: By default this program is setup to run on a BL2600 series
   		controller, if this program is ran on a different controller
         series, then the serMode function parameter may need to change
         for the given controller that you're using.
**************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// serial E buffer size
#define EINBUFSIZE  255
#define EOUTBUFSIZE 255


nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

///////////////////////////////////////////////////////////////////////////
void main()
{
	auto int nIn1;
	auto char cOut;

   // Initialize the controller
	brdInit();

	serEopen(19200);		// Set baud rate first
	serEwrFlush();			// Clear Rx and Tx data buffers
	serErdFlush();

   // Must use serial mode 1 0r 3 depending on the RS232 option you select
	serMode(1);

	while (1)
	{
		while ((nIn1 = serEgetc()) == -1);	//	Wait for lowercase ascii byte
		ser485Tx();									//	Enable transmitter
		serEputc ( toupper(nIn1) );			//	Echo uppercase byte
		while (serEgetc() == -1);				//	Wait for echo
		ser485Rx();									//	Disable transmitter
	}
}
///////////////////////////////////////////////////////////////////////////