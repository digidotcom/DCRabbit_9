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
        Samples\RabbitFLEX_SBC40\digout_simple.c

        Demonstrates very basic use of the flexDigOut() function.  It simply
        accepts a keypress from the user in the stdio window to toggle the
        output of the user-specified digital output.

        Note that you must change the macro at the beginning of the program
        to point to your preferred digital output.  Refer to your list of pins
        and software names for your specific RabbitFlex board for the
        appropriate information to put here.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct RabbitFlex IO pin
 * information structure.  These are the software names for each of your
 * RabbitFlex pins.  The pin that you specify here is the one that will be
 * used for this sample.
 */
#define MY_DIGOUT flex_digout1

void main(void)
{
	int output;

	// Start with the digital out low
	output = 0;

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Display a prompt to the user
	printf("\n\nPress any key to toggle the output...");

	// Loop forever
	while (1) {
		// Write the specified digital output.  Note that the parameter is a
		// pointer to a Flex_IOPin (RabbitFlex pin) structure.
		if (flexDigOut(&MY_DIGOUT, output) < 0) {
      	printf("\nNot a valid digital output!\n");
         return;
      }
		// Print the name of the pin (using the flexPinName() function) and the
		// current value to the stdio window.  Note that the "\x1b=\x20\x20" part
		// simply places the cursor at the home position in the stdio window.
		printf("\x1b=\x20\x20%s = %d", flexPinName(&MY_DIGOUT), output);
		// Wait for the user to press a key
		while (!kbhit());
		// Get (and discard) the key the user pressed
		getchar();
		// Toggle the output
		if (output == 0) {
			output = 1;
		}
		else {
			output = 0;
		}
	}
}

