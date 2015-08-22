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
/**************************************************************************
	led.c

	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	This program toggles LED0 and LED1 (DS5 and DS6) on the prototyping
   board.

   Instructions:
   -------------
   1. Compile and run this program.
   2. View LED's toggling on the prototyping board.

***************************************************************************/
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

#define LEDOFF 1  //Value used to turn the LED OFF
#define LEDON	0	//Value used to turn the LED ON

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto unsigned int i;	 	// variable for the loop counter
	auto unsigned int led; 	// variable used to cycle thru all the LED's

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	for(;;)	// begin an endless loop
	{

		for(led = 0; led < 2; led++)
		{
			//Turn-on the LED indicated by the led variable
			ledOut(led, LEDON);
			for(i=0; i<30000; i++); // time delay loop

			//Turn-off the LED indicated by the led variable
			ledOut(led, LEDOFF);
			for(i=0; i<30000; i++); // time delay loop
		}
	}
}