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
	icom485m.c

	This sample program is used with products such as
	Intellicom Series and Rabbit TCP/IP Development Kits.

	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	Use icom485s.c to program slave controller.

	Serial port B on parallel port D's alternate function
	(ATxB) is used.

	Make the following connections on J7:

		Master to  Slave
		485+ <---> 485+
		485- <---> 485-
		GND  <---> GND

**********************************************************/
#class auto

#use icom.lib

// serial B buffer size
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

// serial baud rate
#ifndef _485BAUD
#define _485BAUD 115200
#endif


void delay (unsigned	wDelay)
{
	for ( ;wDelay>0;--wDelay);
}


//////////////////////////////////////////////////////////
/////	This portion to be run on a master controller.
//////////////////////////////////////////////////////////

void main()
{
	int nIn;
	char cOut;

	serBopen(_485BAUD);		//set baud rate first
	serMode(0);					//setup lines
	serBwrFlush();
	serBrdFlush();

	while (1) {
		for (cOut='a';cOut<='z';++cOut) {			//	Send Classified Sequence
			serBwrFlush();									//	Clear RS485 Echo Buffer
			serB485Tx();									//	Enable RS485 Transmitter
			serBputc ( cOut );							//	Send Byte
			while (serBgetc() == -1);					//	Wait for Echo
			serB485Rx();									// Disable RS485 Transmitter
			while ((nIn = serBgetc ()) == -1);		//	Wait for Reply
			serBrdFlush();
			printf ("Upper case %c is %c\n",cOut, nIn );		//	Display Byte vs. Echo
		}
	}
}
/////////////////////////////////////////////////////////*/



