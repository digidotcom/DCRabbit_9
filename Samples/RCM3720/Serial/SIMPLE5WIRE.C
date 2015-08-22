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
/********************************************************************
	simple5wire.c

	This program is used with RCM3700 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates basic initialization for a
	simple RS232 5-wire loopback displayed in STDIO window.

	Normally we would connect to another controller,

		 Tx <---> Rx
		RTS <---> CTS
		Gnd <---> Gnd

	RTS and CTS can be most any parallel port pin if properly
	configured. However, for this simple demonstration, make
	the following connections:

	Proto-board Connections
	=======================

			TxD <---> RxD
	(RTS)	TxC <---> RxC (CTS)

	Instructions
	============
	1.  Compile and run this program.
	2.  TxC and RxC become the flow control RTS and CTS.
	3.  To test flow control, disconnect RTS from CTS while
		 running	this program.  Characters should stop printing
		 in STDIO window and continue when RTS and CTS are
		 connected again.

********************************************************************/

///////
// PC2 and PC3 ar flow control RTS and CTS
///////
#define SERD_RTS_PORT PCDR
#define SERD_RTS_SHADOW PCDRShadow
#define SERD_RTS_BIT 2			//PC2; TxC on protoboard
#define SERD_CTS_PORT PCDR
#define SERD_CTS_BIT 3			//PC3; RxC on protoboard

// serial D buffer size
#define DINBUFSIZE  15
#define DOUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif

main()
{
	auto int nIn;
	auto char cOut;

	brdInit();				//initialize board for this demo

   BitWrPortI(PEDR, &PEDRShadow, 0, 5);	//set low to enable rs232 device

	serDopen(_232BAUD);		//initialize TxD, RxD
	serDflowcontrolOn();		//enable flow control
	serDwrFlush();				//clear buffers
	serDrdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serDputc (cOut);								//	Send Byte
			while ((nIn=serDgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
			}
	}
}

