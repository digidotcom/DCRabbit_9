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
/*****************************************************

     puts.c

     This program writes a null terminated string over serial port B.
     It must be run with a serial utility such as Hyperterminal.

     Connect RS232 cable from PC to Rabbit:
     	 Connect PC's RS232 GND to Rabbit GND
       Connect PC's RS232 TD  to Rabbit RXB
       Connect PC's RS232 RD  to Rabbit TXB

     Configure the serial utility for port connected to RS232, 19200 8N1.

     Run Hyperterminal.
     Run this program.

******************************************************/
#class auto


/*****************************************************
     The input and output buffers sizes are defined here. If these
     are not defined to be (2^n)-1, where n = 1...15, or they are
     not defined at all, they will default to 31 and a compiler
     warning will be displayed.
******************************************************/
#define BINBUFSIZE 15
#define BOUTBUFSIZE 15

void main()
{
	static const char s[] = "Hello Z-World\r\n";

   serBopen(19200);
   serBputs(s);
   // first, wait until the serial buffer is empty
   while (serBwrFree() != BOUTBUFSIZE) ;
   // then, wait until the Tx data register and the Tx shift register
	//  are both empty
	while (BitRdPortI(SBSR, 3) || BitRdPortI(SBSR, 2));

	// now we can close the serial port without cutting off Tx data
   serBclose();
}