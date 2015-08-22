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
	icomio.c

	This sample program is used with products such as
	Intellicom Series and Rabbit TCP/IP Development Kits.
   Please use the demo board for LEDs and switches.

	This program demonstrates digital I/O.  Outputs, O0-O3,
	will initially output low and then output high.

	Inputs, I0-I3, will read high or low depending on JP4
	setting and will be displayed in STDIO window.

	I/O is controlled by the following parallel port bits:

		Output  Port/bit
		O0  	  port D, bit 0
		O1      port D, bit 1
		O2	     port B, bit 6
		O3      port B, bit 7

		Input   Port/bit
		I0  	  port D, bit 2
		I1      port D, bit 3
		I2	     port E, bit 2
		I3      port E, bit 3

**********************************************************/
#class auto					/* Change local var storage default to "auto" */

///// Inputs
#define I0 0
#define I1 1
#define I2 2
#define I3 3

///// Outputs
#define O0 0
#define O1 1
#define O2 2
#define O3 3

#define OUTLO 0
#define OUTHI 1

void delay (unsigned cdelay)
{
	for ( ; cdelay>0; cdelay--);
}

//////////////////////////////////////////////////////////

void main()
{
	int line, reading;

	///// outputs all low
	for (line=O0; line<=O3; line++)
	{
		digOut(line, OUTLO);
	}

	///// outputs write high then low
	printf("Outputs O0-O3 going high and low  ");
	for (line=O0; line<=O3; line++)
	{
		putchar('.');
		digOut(line, OUTHI);
		delay (15000);
		putchar('.');
		digOut(line, OUTLO);
		delay (15000);
	}

	///// unconnected, inputs return the value of JP4
	printf("\nReading I0-I3  ");
	for (line=I0; line<=I3; line++)
	{
		reading = digIn(line);
		printf("%d ", reading);
	}
	putchar('\n');
}
//////////////////////////////////////////////////////////

