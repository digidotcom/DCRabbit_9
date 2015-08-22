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
/**********************************************************
	simple3wire.c

	This program is used with LP3500 series controllers.

	Description
	===========
	This program demonstrates basic initialization for a
	simple RS232 3-wire loopback displayed in STDIO window.

	Proto-board Connections
	=======================
	For this simple demonstration, make the following
	connections:

		TxB <---> RxC
		RxB <---> TxC

	Instructions
	============
	1.  Compile and run this program.
	2.  Lower case characters are sent by TxC. RxB receives
	    the character.  TxB sends the converted uppercase
	    character to RxC and displays in STDIO.

**********************************************************/
#class auto					/* Change local var storage default to "auto" */

///////
// change serial buffer name and size here
// for example:  CINBUFSIZE to DINBUFSIZE
///////
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

///////
// change serial baud rate here
///////
#ifndef _232BAUD
#define _232BAUD 19200
#endif

///////////////////////////////////////////////////////////////////////

void main()
{
	auto int nIn1, nIn2;
	auto char cOut;

	brdInit();				//initialize board for this demo

	serCopen(_232BAUD);
	serBopen(_232BAUD);
	serCwrFlush();
	serCrdFlush();
	serBwrFlush();
	serBrdFlush();
	serMode(0);			//enable serial device

	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{
			serCputc (cOut);								//	Send lowercase byte
			while ((nIn1=serBgetc()) == -1);			// Wait for echo
			serBputc (toupper(nIn1));					//	Send the converted upper case byte
			while ((nIn2=serCgetc()) == -1);			// Wait for echo
			printf ("Serial C sent %c, serial B sent %c, serial C received %c\n",
						cOut, toupper(nIn1), nIn2);
		}
	}
}

