/*******************************************************************************
        Samples\RabbitFLEX_SBC40\keypad.c
        Rabbit Semiconductor, 2006

        This sample program demonstrates the use of crosswire keypad with the
        RabbitFlex SBC40.  Each key is configured to correspond with a specific
        character.  This is based on which output and input lines are used
        to read the specific key, as given by the following formula:

	         [output #] * [num outputs] + [input #]

	         where the output # and input # start from 0.  If these
	         correspond to rows and columns, then the formula would be:

	         [row #] * [num rows] + [column #]

        The user can also press the 'u' key in the stdio window to force an
        "unget" of a key--that is, placing it back on the queue.  In this
        program, that key will be immediately reread.  Additionally, the 'r'
        key will toggle whether or not a keypress repeats when the button is
        held down.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct pointer to a RabbitFlex
 * keypad information structure.  These are the software names for each of your
 * RabbitFlex keypads.  The keypad that you specify here is the one that will
 * be used for this sample.
 */
#define MY_KEYPAD flex_keypad

/*
 * This function configures the keycodes with the flexKeyConfig() function.
 * It simply assigns A-Z for the key presses, and a-z for key releases.  The
 * repeat parameter determines whether or not the key repeat options are
 * enabled.
 */
void config_keys(int repeat) {
	int numkeys;		// Number of keys in a keypad
	int i;				// Used to count through the keys in a keypad

	// Calculate the number of keys on the keypad.
	numkeys = MY_KEYPAD.num_outs * MY_KEYPAD.num_ins;

	// Configure the default keycodes.  This simply assigns A-Z for the key
	// presses, and a-z for key releases.
	for (i = 0; i < numkeys; i++) {
		if (!repeat) {
			// Configure the keys without repeat options
			flexKeyConfig(&MY_KEYPAD, i, 'A' + i, 0, 0, 0, 0, 0);
		}
		else {
			// Configure the keys with repeat options
			flexKeyConfig(&MY_KEYPAD, i, 'A' + i, 'a' + i, 255, 200, 255, 100);
		}
	}
}

void main(void) {
	char ch;				// Stores the keypress received
	char stdio_char;	// Stores a keypress from the stdio window
	char save_char;	// Saves the last received keypress
	int repeat;			// 0 = non-repeating mode; 1 = repeating mode

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Initialize the keypad
	flexKeyInit(&MY_KEYPAD);

	// Initially configure the keys without repeat options
	repeat = 0;
	config_keys(repeat);

	// Display some information to the user
	printf("Press a key on the keypad to see the corresponding keycode.\n");
	printf("When in repeating mode, the key release event will also be ");
	printf("displayed.\n\n");
	printf("The following commands can be issued:\n");
   printf("r) Toggle key repeating mode -- flexKeyConfig\n");
   printf("u) Unget a character         -- flexKeyUnget\n\n");

	while (1) {
		// This function must be called periodically to process keypresses on
		// the keypad
		flexKeyProcess(&MY_KEYPAD);

		// Get the next key from the keypress queue
		ch = flexKeyGet(&MY_KEYPAD);
		// Check that we actually received a character.  If so, print it out.
		if (ch != 0) {
			printf("%c\n", ch);
			// Save the last character we received, in case we want to put it
			// back on the keypad queue (flexKeyUnget)
			save_char = ch;
		}

		// Process commands from the stdio window
		if (kbhit()) {
			stdio_char = getchar();
			switch (stdio_char) {
			case 'r':
				// Toggle repeating of keys
				if (repeat) {
					printf("Repeating of keys turned OFF\n");
					repeat = 0;
				}
				else {
					printf("Repeating of keys turned ON\n");
					repeat = 1;
				}
				config_keys(repeat);
				break;
			case 'u':
				// Unget the last key received
				if (flexKeyUnget(&MY_KEYPAD, save_char) > 0) {
					printf("Keypress placed back in keypad queue\n");
				}
				else {
					printf("flexKeyUnget() failed!\n");
				}
				break;
			}
		}
	}
}

