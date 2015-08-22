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
//  FlashLED2.c
//
//  Demonstration of using the Rabbit 2000 Core Module Prototyping Board
//
//  This program will repeatedly flash LED DS3 (PA1) on the Rabbit 2000 Core Module
//  Prototyping Board.  This program also shows the use of the runwatch() function
//  to allow Dynamic C to update watch expressions while running.  To test this:
//
//    1.  Add a watch expression for "k" under "Inspect:Add Watch Expression" dialog.
//    2.  Click "add to top" so that it will be in the watch list permanently.
//			 Under DC8, this button is "Add".
//    3.  Dismiss the dialog.
//    4.  While the program is running, hit Ctrl+U to update the watch window
//
////////////////////////////////////////////////////////////////////////////////
#class auto 			// Change default storage class for local variables: on the stack

// C programs require a main() function
void main()
{
	static int j;	// define an integer j to serve as a loop counter
	static int k;   // define a counter k with which to test the watch expression

	// Write 84 hex to slave port control register that initializes parallel
	// port A as an output port (port A drives the LEDs on the Prototyping Board).

	WrPortI(SPCR, &SPCRShadow, 0x84);

	k=0;     // initialize k to 0

	// now write all ones to port A which sets outputs high and LEDs off

	WrPortI(PADR, &PADRShadow, 0xff);

	while(1) {  // begin an endless loop

		BitWrPortI(PADR, &PADRShadow, 1, 1);  // turn LED DS3 off

		for(j=0; j<32000; j++)
			; // time delay loop

		BitWrPortI(PADR, &PADRShadow, 0, 1);  // turn LED DS3 on

		for(j=0; j<1000; j++)
			; // time delay loop

		k++;         // increment k
		runwatch();  // update watch expressions for Dynamic C to read

	}	// end while loop

} // end program

