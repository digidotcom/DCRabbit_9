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
	Samples\BL2000\io\led.c

	This sample program is used with BL20XX series controllers.
	
	This program demonstrates how to toggle the LED's on
	on the controller.
	
	Use led.c to program the controller.

	The program will toggle the following LED's:

	0 = OUT0_LED
	1 = OUT1_LED
	2 = OUT2_LED
	3 = OUT3_LED
	4 = BAD_INDICATOR

	Note: When turning LED0 ON and OFF, RELAY0 will also be toggling because
	      its tied to the same control signal.


**************************************************************************/
#class auto


#define LEDOFF 0  //Value used to turn the LED OFF
#define LEDON	1	//Value used to turn the LED ON

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto unsigned int i;	 	// variable for the loop counter
	auto unsigned int led; 	// variable used to cycle thru all the LED's
	
	// Initialize all ports on the controller
	brdInit();

	for(;;)	// begin an endless loop
	{  

		for(led = 0; led < 5; led++)
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
