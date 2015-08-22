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
        Samples\RabbitFLEX_SBC40\anaout_simple.c

        Demonstrates very basic use of the flexAnaOut() and flexAnaVolts()
        functions.  It simply accepts a raw value or a voltage from the user
        through the stdio window and outputs the requested value on the analog
        output.

        Note that you must change the macro at the beginning of the program
        to point to your preferred analog output.  Refer to your list of pins
        and software names for your specific RabbitFlex board for the
        appropriate information to put here.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct pointer to a RabbitFlex
 * IO pin information structure.  These are the software names for each of your
 * RabbitFlex pins.  The pin that you specify here is the one that will be used
 * for this sample.
 */
#define MY_ANAOUT flex_anaout1

void main(void)
{
	int anaout_raw;		// Used for the analog output raw value
	float anaout_volts;	// Used for the analog output voltage value
	char *name;				// Used to point to the name of the analog output
	char c;					// Uset to get a keypress from the user
	char input[20];		// Get input from the user

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Get the name of the analog output pin
	name = flexPinName(&MY_ANAOUT);

	// Loop forever
	while (1) {
		// Prompt for raw or voltage user input for analog output value
		printf("Press 'r' to input a raw value or 'v' to input a voltage...\n\n");
		// Wait for the user to press a key
		while (!kbhit());
		// Get the key that the user pressed
		c = getchar();
		// Accept a raw value if 'r' or 'R' was pressed, or a voltage if 'v' or
		// 'V' was pressed
		switch (c) {
		case 'r':
		case 'R':
			// Prompt for a raw value
			printf("Enter a raw value (0 - 1023): ");
			// Read the value from the stdio window
			gets(input);
			// Convert the value to an integer
			anaout_raw = atoi(input);
			// Output the value on the analog output.  Note that the parameter is a
			// pointer to a Flex_IOPin (RabbitFlex pin) structure.  If this were
			// not specified as the MY_ANAOUT macro, then the call would look like:
  			// flexAnaOut(&flex_anaout1, output);
			if (flexAnaOut(&MY_ANAOUT, anaout_raw) < 0) {
         	printf("\nNot a valid analog output!\n");
            return;
         }
			// Display the new raw value to the user
			printf("\n%s raw value is %d\n\n", name, anaout_raw);
			break;
		case 'v':
		case 'V':
			// Prompt for a voltage
			printf("Enter a voltage: ");
			// Read the value from the stdio window
			gets(input);
			// Convert the value to a float
			anaout_volts = atof(input);
			// Output the voltage on the analog output
			if (flexAnaOutVolts(&MY_ANAOUT, anaout_volts) < 0) {
         	printf("\nNot a valid analog output!\n");
            return;
         }
			// Display the new raw value to the user
			printf("\n%s voltage is %f\n\n", name, anaout_volts);
			break;
		}
	}
}

