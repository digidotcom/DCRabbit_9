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

   slice01.c

   This example demonstrate the basic functionality of a slice statement
   It alternates incrementing two counters in a tight loop.  The first
   tight loop is allowed to run for 25 ticks and the second for 50 ticks.
   After each has had a chance to run the values of the counters are
   outputed to the stdio window and the process is repeated by the outside
   loop.

   The first number in the slice statement is the size of the stack.  It
   needs to be at least large enough for the worst case stack usage by
   the user program and interrupt routines.

   Notice the use of shared variables for counters.  This is _IMPORTANT_
   to insure the atomic updating of the values at the end of the
   increment operations.  If they were not shared variables it is possible
   that half of the long would be updated with the other half not updated.

   The expected behavior is that x is incremented about 1/2 as fast as y.
   This will not be exact because y has less overhead because it has 1/2
   as many context switches.  The lower the ticks the more overhead will
   influence the amount of work you can get done in each pass.

******************************************************/
#class auto


shared long x,y;
 
void main()
{
	x=y=0; 					// initialize the counters

	for(;;) { 				// outside loop
	
		slice(200,25) { 	// run this section for 25 ticks
			for(;;) {
				x++;
			}
		}
		
		slice(200,50) {	// run this section for 50 ticks
			for(;;) {
				y++;
			}
		}
		
		printf("x=%ld, y=%ld\n",x,y); // print the results
	}
}

