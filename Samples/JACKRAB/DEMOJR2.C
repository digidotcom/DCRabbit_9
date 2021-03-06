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

     demojr2.c

     Demonstration of using the Jackrabbit Development Board

     This program will repeatedly flash LED DS3 on the Jackrabbit Development Board
     (bit 2 on port A).  This program also shows the use of the runwatch()
     function to allow Dynamic C to update watch expressions while running.
     To test this:

		 1. 	add a watch expression for "k" under "Inspect:Add/Del Watch Expression"
		 2.	click "Add to top" so that it will be permanently in the watch list
		 3.	while the program is running, type Ctrl+U to update the watch window

******************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


// C programs require a main() function
void main()
{
	static int  j; 	// define an integer j to serve as a loop counter
	static int  k; 	// define a counter k to test the watch expression with

	// Write 84 hex to slave port control register which initializes parallel
	// port A as an output port (port A drives the LED's on the development board).

	WrPortI(SPCR, &SPCRShadow, 0x84); 

	k=0;		// initialize k to zero
  
	// now write all ones to port A which sets outputs high and LED's off
  
	WrPortI(PADR, &PADRShadow, 0xff);
  
	while(1) {  // begin an endless loop

		BitWrPortI(PADR, &PADRShadow, 1, 2);  // turn LED DS3 off

		for(j=0; j<25000; j++)
			; // time delay loop
			
		BitWrPortI(PADR, &PADRShadow, 0, 2);  // turn LED DS3 on

		for(j=0; j<1000; j++)
			; // time delay loop

		k++;					// increment k
		runwatch();			// update watch expressions for Dynamic C to read

	}	// end while loop
  
} // end program

