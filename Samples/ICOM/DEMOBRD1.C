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

	demobrd1.c

 	This sample program is used with products such as
	Intellicom Series and TCP/IP Development Board with
	Z-World Demo Board.

   This program will flash the LED's attached to outputs
   O0 and O1 with 180 degree phase.

   Connect board outputs O0 and O1 to Demo Board LED's.

******************************************************/
#class auto					/* Change local var storage default to "auto" */

// C programs require a main() function
void main()
{
	int j;	// define an integer j to serve as a loop counter


	// set lowest 2 bits of port D as outputs
  	WrPortI(PDDDR, &PDDDRShadow, 0x03);
	// set all port D outputs to not be open drain
	WrPortI(PDDCR, &PDDCRShadow, 0x00);


	while(1) {  // begin an endless loop

		BitWrPortI(PDDR, &PDDRShadow, 0xFF, 0);	// turn led on output 0 on
		BitWrPortI(PDDR, &PDDRShadow, 0x00, 1);	// turn led on output 1 off

		for(j=0; j<20000; j++)
			; // time delay loop

		BitWrPortI(PDDR, &PDDRShadow, 0x00, 0);	// turn led on output 0 off
		BitWrPortI(PDDR, &PDDRShadow, 0xFF, 1);	// turn led on output 1 on

		for(j=0; j<20000; j++)
			; // time delay loop

	}	// end while loop

} // end program