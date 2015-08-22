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
	Simple3wire.c

 	This program is used with RabbitFLEX SBC40 boards.

	Description
	===========
	This program demonstrates basic initialization for a
	simple RS232 3-wire loopback displayed in STDIO window.

   Here's the typical connections for a 3 wire interface that
   would be made between controllers.

       TX <---> RX
       RX <---> TX
		Gnd <---> Gnd

	Connections
	===========
	1. Connect TXF and RXF together.

	Instructions
	============
	1. Choose which serial port to use with the macro USE_SERIALX below.
	2. Compile and run this program.
 	3. View STDIO window for sample program results.

***************************************************************************/

// Define which serial port to use.  This can be USE_SERIALF, USE_SERIALD, or
// USE_SERIALE.  Make sure that the serial port you select is actually
// available on your RabbitFLEX design.
#define USE_SERIALF

// Configure the correct serial functions based on the user's selection.

#ifdef USE_SERIALF
	#define FINBUFSIZE 15
	#define FOUTBUFSIZE 15
	#define serXopen serFopen
	#define serXwrFlush serFwrFlush
	#define serXrdFlush serFrdFlush
	#define serXputc serFputc
	#define serXgetc serFgetc
#endif

#ifdef USE_SERIALD
	#define DINBUFSIZE 15
	#define DOUTBUFSIZE 15
	#define serXopen serDopen
	#define serXwrFlush serDwrFlush
	#define serXrdFlush serDrdFlush
	#define serXputc serDputc
	#define serXgetc serDgetc
#endif

#ifdef USE_SERIALE
	#define EINBUFSIZE 15
	#define EOUTBUFSIZE 15
	#define serXopen serEopen
	#define serXwrFlush serEwrFlush
	#define serXrdFlush serErdFlush
	#define serXputc serEputc
	#define serXgetc serEgetc
#endif

//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// serial buffer size
#define SERXINBUFSIZE  15
#define SERXOUTBUFSIZE 15


// serial baud rate
#define BAUD232 115200

main()
{
	auto int nIn;
	auto char cOut;

	// Initialize I/O for the RabbitFLEX SBC40
	brdInit();

   // Open serial port
   serXopen(BAUD232);

   // Can use serial mode 0 for 3 wire RS232 operation
   // and serMode must be executed after the serXopen
   // function(s).
   serMode(0);

   // Clear serial buffers
   serXwrFlush();
	serXrdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serXputc (cOut);								//	Send Byte
			while ((nIn=serXgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
		}
	}
}


