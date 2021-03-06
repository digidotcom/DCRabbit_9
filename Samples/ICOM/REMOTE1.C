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
	remote1.c

	This sample program is used with products such as
	Intellicom Series.

	This program demonstrates a simple serial data
	communications using a remote ANSI terminal such as
	Hyperterminal from an available com port connection.

	Unless otherwise defined, serial baud rate is set
	for 115200 bps, hardware flow control, 8 data bits,
	no parity, one stop bit and ANSI emulation.


	Make the following cable connections:

   To PC
   Serial Port			To Controller J7
	==================================

   DCD	1-----*
   			   |
   DSR	2-----*
   			   |
   RX		3-----|------TX
   			   |
   RTS	4--*  |
   			|  |
   TX		5--|--|------RX
   			|  |
   CTS	6--*  |
   			   |
   DTR	7-----*

   RI		8---NC

   GND	9------------GND


   Run this program and type from remote terminal.
   Characters will display on controller.  Press keys on
   controller and characters will display on remote
   terminal.

**********************************************************/
#class auto					/* Change local var storage default to "auto" */

// serial C buffer size
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif


//////////////////////////////////////////////////////////

void main()
{
	auto int nIn;
	auto char cOut;
	auto char inkey;

	brdInit();
	keypadDef();

	serCopen(_232BAUD);
	serMode(0);
	serCwrFlush();
	serCrdFlush();


	while (1) {
		costate {									//	Process Keypad Press/Hold/Release
			keyProcess();
			waitfor(DelayMs(10));
			}

		costate {
			if ((inkey = keyGet()) != 0) {	//	Wait for Keypress
				dispPrintf ("%c", inkey);		//	Display Byte
				serCputc(inkey);					// Transmit byte
				}
			waitfor(DelayMs(10));
			}

		costate {
			if ((nIn=serCgetc()) != -1)		// Wait for receive byte
				dispPrintf ("%c", nIn);			//	Display Byte
			waitfor(DelayMs(10));
			}
		}
}
//////////////////////////////////////////////////////////

