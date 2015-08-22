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
	simple3wire.c

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This program demonstrates basic initialization for a simple RS-232 3-wire
	loopback displayed in the STDIO window.

	Proto-board Connections
	=======================
	Normally, we would connect to another controller as follows:
		 Tx <---> Rx
		 Rx <---> Tx
		Gnd <---> Gnd

	However, for this simple demonstration, make the following connections on a
	single prototyping board:
		TXE <---> RXF
		RXE <---> TXF

	Instructions
	============
	1. Compile and run this sample program in debug mode.

	2. Lower case characters are sent out TXE and received by RXF.  Each
	   character received by RXF is converted to upper case and then sent out
	   TXF to be received by RXE.

	3. Observe the sequence of transmitted characters as reported in the STDIO
	   window.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define BAUD_RATE 115200L

#define EINBUFSIZE  15
#define EOUTBUFSIZE 15
#define FINBUFSIZE  15
#define FOUTBUFSIZE 15

void main(void)
{
	static char cOut;
	static int nIn1, nIn2;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	serEopen(BAUD_RATE);
	serFopen(BAUD_RATE);

	// ensure there's no old serial data hanging about
	serEwrFlush();
	serErdFlush();
	serFwrFlush();
	serFrdFlush();

	cOut = 'a';	// our initial lower case character
	while (1) {
		serEputc(cOut);							//	send a lower case character
		while ((nIn1 = serFgetc()) == -1);	// wait for character received
		serFputc(toupper(nIn1));				//	send the converted upper case char
		while ((nIn2 = serEgetc()) == -1);	// wait for character received
		printf("Sent %c TXE --> RXF (got %c), sent %c TXF --> RXE (got %c).\n",
		       cOut, nIn1, toupper(nIn1), nIn2);
		if ('z' == cOut) {
			cOut = 'a';	// reset back to our initial lower case character
		} else {
			++cOut;	// increment to the next lower case character
		}
	}
}

