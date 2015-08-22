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

	This program is used with RCM3200 series controllers with
	prototyping boards.

	The sample library, rcm3200.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

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

			TxB <---> RxB
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
#class auto 			// Change default storage class for local variables: on the stack

#use rcm3200.lib    		//sample library used for this demo

///////
// PC2 and PC3 were for flow control RTS and CTS
///////
#define SERB_RTS_PORT PCDR
#define SERB_RTS_SHADOW PCDRShadow
#define SERB_RTS_BIT 2			//PC2; TxC on protoboard
#define SERB_CTS_PORT PCDR
#define SERB_CTS_BIT 3			//PC3; RxC on protoboard

// serial B buffer size
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200L
#endif

void main()
{
	auto int nIn;
	auto char cOut;

	brdInit();				//initialize board for this demo

	serBopen(_232BAUD);		//initialize TxB, RxB
	serBflowcontrolOn();		//enable flow control
	serBwrFlush();				//clear buffers
	serBrdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serBputc (cOut);								//	Send Byte
			while ((nIn=serBgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
			}
	}
}
