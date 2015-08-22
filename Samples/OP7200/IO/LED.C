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

	This sample program toggles the LED on the OP7200. 
	
**************************************************************************/
#class auto


#memmap xmem  // Required to reduce root memory usage 

#define LEDOFF 0  //Value used to turn the LED OFF
#define LEDON	1	//Value used to turn the LED ON

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto unsigned int i;	 	// variable for the loop counter
	auto unsigned int led; 	// variable used to cycle thru all the LED's
	
	brdInit();	// Initialize the controller

	for(;;)	// begin an endless loop
	{  		
		//Turn-on the LED indicated by the led variable
		ledOut(0, LEDON);
		for(i=0; i<20000; i++); // time delay loop

		//Turn-off the LED indicated by the led variable
		ledOut(0, LEDOFF);
		for(i=0; i<20000; i++); // time delay loop
	}	
} 
