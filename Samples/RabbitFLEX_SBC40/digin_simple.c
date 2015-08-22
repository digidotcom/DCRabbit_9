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
/*******************************************************************************
        Samples\RabbitFLEX_SBC40\digin_simple.c

        Demonstrates very basic use of the flexDigIn() function.  It simply
        monitors the user-specified digital input and displays the current
        state in the stdio window.

        Note that you must change the macro at the beginning of the program
        to point to your preferred digital input.  Refer to your list of pins
        and software names for your specific RabbitFlex board for the appropriate
        information to put here.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct pointer to a RabbitFlex
 * IO pin information structure.  These are the software names for each of your
 * flex pins.  The pin that you specify here is the one that will be used for
 * this sample.
 */
#define MY_DIGIN flex_digin1

void main(void)
{
	int result;

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Loop forever
	while (1) {
		// Read the specified digital input.  Note that the parameter is a pointer
		// to a Flex_IOPin (RabbitFlex pin) structure.  If this were not specified
		// as the MY_DIGIN macro, then the call would look like:
		// flexDig(&flex_digin1);
		result = flexDigIn(&MY_DIGIN);
      if (result < 0) {
      	printf("Not a valid digital input!\n");
         return;
      }

		// Print the name of the pin (using the flexPinName() function) and the
		// current value to the stdio window.  Note that the "\x1b=\x20\x20" part
		// simply places the cursor at the home position in the stdio window.
		printf("\x1b=\x20\x20%s = %d\n", flexPinName(&MY_DIGIN), result);
	}
}

