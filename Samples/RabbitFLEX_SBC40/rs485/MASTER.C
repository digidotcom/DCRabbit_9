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

	Samples\RabbitFLEX_SBC40\rs485\master.c

	This sample program is for the RabbitFLEX SBC40 series controllers.
   You must have the RS485 option installed to use this sample
   program.


	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	Use slave.c to program the slave controller.

	Make the following connections:

		Master to  Slave
		485+ <---> 485+
		485- <---> 485-
		GND  <---> GND

**************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// serial C buffer size
#define CINBUFSIZE  255
#define COUTBUFSIZE 255

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
	auto int sync;

   // Initialize the controller
	brdInit();

	serCopen(19200);	// Set baud rate first
	serCwrFlush();		// Clear Rx and Tx data buffers
	serCrdFlush();

	printf ("Waiting to sync up to the other controller\r");
	// Sync up to the slave controller
	sync = FALSE;
	while(!sync)
	{
		cOut = 0x55;
		ser485Tx();							//	enable transmitter
		serCputc ( cOut );				//	send lowercase byte
		while (serCgetc() == -1);		//	wait for echo
		ser485Rx();							// disable transmitter
		msDelay(5);
		if((nIn1 = serCgetc ()) == -1)	//	check for reply
		{
			printf ("Waiting to sync up to the other controller\r");
		}
		else
		{
			if(nIn1 == cOut)
			{
				sync = TRUE;
			}
		}
	}

	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{
			ser485Tx();										//	enable transmitter
			serCputc ( cOut );							//	send lowercase byte
			while (serCgetc() == -1);					//	wait for echo
			ser485Rx();										// disable transmitter

			while ((nIn1 = serCgetc ()) == -1);		//	wait for reply
			if (nIn1 == (toupper(cOut)))
				printf ("\n\rUpper case %c is %c", cOut, nIn1 );
		}
	}
}
///////////////////////////////////////////////////////////////////////////