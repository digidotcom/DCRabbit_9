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
////////////////////////////////////////////////////////////////////////////////
//
//  CountLEDs.c
//
//  Demonstration of using the onboard LEDs of the BL2000
//
//  This program will count from 0 to 31 in binary, using the four general-
//  purpose LEDs DS4-DS7, and the Processor Bad LED DS8.
//
//  The LEDs are used in reverse logical order to minimize the cycling of
//  the relay, which is slaved to the same output as DS4.
//  
//
////////////////////////////////////////////////////////////////////////////////
#class auto


// C programs require a main() function
void main()
{
	int j, k;	// define integers j and k to serve as loop counters

	// Write 84 hex to slave port control register which initializes parallel
	// port A as an output port (port A drives the LED's on the development board).

	WrPortI(SPCR, &SPCRShadow, 0x84); 

	// Now write all zeroes to port A, which sets the outputs low and LEDs off.
	// (This step is optional, but it's always good practice to be sure a port
	// is properly initialized before use.)
  
	WrPortI(PADR, &PADRShadow, 0x00);

	while (1) {  // start endless loop

		for ( k=0;k<32;k++ ) {  // start loop which counts from 0 to 31

      // Note that LED DS8 is connected to Port B, and that its action is reversed from the other
      // four LEDs attached to Port A. (A look at the BL2000 schematic will show why this is so.)
		
			if (k & 0x01) WrPortI(PBDR, NULL, 0x00);  			 // turn LED DS8 on if bit 0 of the count is logic 1
				else WrPortI(PBDR, NULL, 0x80);           		 // else turn it off if bit 0 is logic 0
			
			if (k & 0x02) BitWrPortI(PADR, &PADRShadow, 1, 3);  // turn LED DS7 on if bit 1 of the count is logic 1
				else BitWrPortI(PADR, &PADRShadow, 0, 3);        // else turn it off if bit 0 is logic 0
			
			if (k & 0x04) BitWrPortI(PADR, &PADRShadow, 1, 2);  // turn LED DS6 on if bit 2 of the count is logic 1
				else BitWrPortI(PADR, &PADRShadow, 0, 2);        // else turn it off if bit 0 is logic 0
			
			if (k & 0x08) BitWrPortI(PADR, &PADRShadow, 1, 1);  // turn LED DS5 on if bit 3 of the count is logic 1
				else BitWrPortI(PADR, &PADRShadow, 0, 1);        // else turn it off if bit 0 is logic 0
			
			if (k & 0x10) BitWrPortI(PADR, &PADRShadow, 1, 0);  // turn LED DS4 on if bit 4 of the count is logic 1
				else BitWrPortI(PADR, &PADRShadow, 0, 0);        // else turn it off if bit 0 is logic 0
			
			for ( j=0;j<32000;j++ );  // empty loop inserts delay between count increments
		}	

	}	// end while loop
  
} // end program
