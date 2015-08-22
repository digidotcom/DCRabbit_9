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
/*******************************************************************************
	simple5wire.c

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This program demonstrates basic hardware flow control for a simple RS-232
	5-wire loopback connection with results displayed in the STDIO window.

	Proto-board Connections
	=======================
	Normally, we would connect to another controller as follows:
		 Tx <---> Rx
		RTS <---> CTS
		Gnd <---> Gnd

	Note that RTS and CTS can be most any parallel port pin if properly
	configured.  However, for this simple demonstration, make the following
	connections on a single prototyping board:
		      TXE <---> RXE
		(RTS) TXF <---> RXF (CTS)

	Instructions
	============
	1. Compile and run this sample program in debug mode.

	2. A sequence of messages about sent and received characters is printed to
	   the STDIO window.

	3. If in free-running debug mode, notice that the first message indicates
	   that no character was received; this is expected because the program
	   doesn't wait for a received character to be ready.  If single-stepping in
	   debug mode, notice that the time between steps is sufficient for each
	   character to be sent and received in order.

	4. TXF and RXF are used as the hardware flow control lines, RTS and CTS.  To
	   test the hardware flow control, disconnect RTS (TXF) from CTS (RXF) while
	   the program is in free-running debug mode.  Notice that the next few
	   messages say characters are sent but that nothing has been received.  This
	   is really indicative of the serial output buffer filling up while the
	   disabled hardware flow control is being honored.  When the serial output
	   buffer is full the messages change again, to say that characters are being
	   neither sent nor received.

	5. To restore character transmission to normal, reconnect RTS and CTS again.
	   Note that the first few messages will almost certainly indicate that the
	   sent and the received characters are significantly out of step.  The
	   difference in step should then revert to the size of the output buffer and
	   thereafter remain constant until CTS and RTS are disconnected again.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define BAUD_RATE 115200L

// serial buffer size
#define EINBUFSIZE  15
#define EOUTBUFSIZE 15

// see serEflowcontrolOn() function description
#define SERE_RTS_PORT PGDR
#define SERE_RTS_SHADOW PGDRShadow
#define SERE_RTS_BIT 2	// PG2 is TxF on protoboard
#define SERE_CTS_PORT PGDR
#define SERE_CTS_BIT 3	// PG3 is RxF on protoboard

void main(void)
{
	static char cOut;
	static int cIn, nOut;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	// PG2 & PG3 is configured as input in brdInit; set PG2 to output
	BitWrPortI(PGDR, &PGDRShadow, 1, 2);	// preset PG2 to output high
	BitWrPortI(PGDDR, &PGDDRShadow, 1, 2);	// set PG2 to output

	serEopen(BAUD_RATE);

	serEflowcontrolOn();		// enable flow control
	serEwrFlush();				// clear buffers
	serErdFlush();

	cOut = 0x20;	// our initial output character is an ASCII space
	while (1) {
		nOut = serEputc(cOut);	//	attempt to send a character
		cIn = serEgetc();	// attempt to receive a character
		if (!nOut) {
			printf("Didn't send '%c' TXE --> RXE (got ", cOut);
		} else {
			printf("Sent '%c' TXE --> RXE (got ", cOut);
		}
		if (-1 == cIn) {
			printf("nothing");
		} else {
			printf("'%c'", cIn);
		}
		if (!nOut || -1 == cIn) {
			printf(").\n");
		} else {
			printf(": difference is %d).\n", cOut - cIn);
		}
		if (126 == cOut) {
			cOut = 0x20;	// reset back to our initial output character
		} else {
			++cOut;	// increment to the next output character
		}
	}
}

