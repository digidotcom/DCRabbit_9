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

	This program is used with RCM3300 series controllers and
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

			TxE <---> RxE
	(RTS)	TxF <---> RxF (CTS)

	Instructions
	============
	1.  Compile and run this program.
	2.  TxF and RxF become the flow control RTS and CTS.
	3.  To test flow control, disconnect RTS from CTS while
		 running	this program.  Characters should stop printing
		 in STDIO window and continue when RTS and CTS are
		 connected again.

********************************************************************/

#use rcm33xx.lib

///////
// flow control RTS and CTS
///////
#define SERE_RTS_PORT PGDR
#define SERE_RTS_SHADOW PGDRShadow
#define SERE_RTS_BIT 2			//PG2; TxF on protoboard
#define SERE_CTS_PORT PGDR
#define SERE_CTS_BIT 3			//PG3; RxF on protoboard

// serial buffer size
#define EINBUFSIZE  15
#define EOUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif

main()
{
	auto int nIn;
	auto char cOut;

   #if _USER
   #warns "This sample permanantly disables the RabbitSys serial console."
   #warns "Remove power and the battery to restore serial console operation."   
	if (_sys_con_disable_serial() != 0) {
   	printf("Error: Cannot disable RabbitSys serial port.");
   }
   #endif
	brdInit();				//initialize board for this demo

	// PG2 & PG3 is configured as input in brdInit, set PG2 as output
	BitWrPortI(PGDR, &PGDRShadow, 1, 2);		//set PG2 to output high
	BitWrPortI(PGDDR, &PGDDRShadow, 1, 2);		//set PG2 to output

	serEopen(_232BAUD);		//initialize
	serEflowcontrolOn();		//enable flow control
	serEwrFlush();				//clear buffers
	serErdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serEputc (cOut);								//	Send Byte
			while ((nIn=serEgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
			}
	}
}









