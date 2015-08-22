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
        Samples\RabbitFLEX_SBC40\anain_calibrate.c

        This program allows the user to calibrate the analog in pins on the
        RabbitFlex board.  Note that all RabbitFlex boards come precalibrated
        from the factory--this program will overwrite the factory calibration!

        First, a list of analog inputs is displayed, from which the user is
        allowed to select one input.  After selecting the input, the user is
        then prompted to supply a low voltage to the selected analog input.
        Then the user is prompted for a high voltage.  It is suggested that
        these inputs should be about 10% and 90% of the voltage range for the
        selected analog input.

        After the calibration, the measured voltage is displayed to allow the
        user to adjust the input and verify that the calibration is correct.

        Note that it is possible to modify this sample to calibrate 4-20 mA
        analog inputs.  Simply change all references to flexAnaInVolts to
        flexAnaInmAmps.  The flexAnaInCalib function works just as well with
        4-20 mA analog inputs as the regular analog inputs.  Instead of
        specifying voltages, the user should specify the current reading in
        milliamps.

*******************************************************************************/

// The following function presents a menu to allow the user to select an
// analog input.  It returns a pointer to the Flex_IOPin structure corresponding
// to the selected pin.
Flex_IOPin *get_analog_input(void)
{
	int done;
	int i;
	int input;
	char buffer[20];

	done = 0;
	// Display the menu and prompt for selection until the user correctly
	// chooses an analog input
	while (!done) {
	   // Display a list of analog input pins
	   i = 0;
	   while (_flex_pins_anain[i] != FLEX_GROUP_END) {
	      printf("%2d) %s\n", i+1, flexPinName(_flex_pins_anain[i]));
	      i++;
	   }
	   // Prompt the user to select an analog input
	   printf("\nSelect an analog input (1 - %d):  ", i);
	   // Read the selected analog input
	   gets(buffer);
	   input = atoi(buffer);
	   // Make sure the analog input is in range
	   if ((input >= 1) && (input <= i)) {
	   	done = 1;
	   }
	   else {
	   	printf("\n\nERROR:  Not a valid analog input!\n\n");
	   }
	}
	// Return the selected analog input
	return _flex_pins_anain[input-1];
}

void main(void)
{
	Flex_IOPin *pin;
	char buffer[20];
	float low_voltage;
	int low_value;
	float high_voltage;
	int high_value;

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	while (1) {
		// Display a menu of analog inputs, and get a selection from the user
		pin = get_analog_input();

		// Prompt the user to supply a low voltage, and read in that voltage
		printf("\nApply low voltage, and enter voltage value here:  ");
		gets(buffer);
		low_voltage = atof(buffer);
		// Read the analog input to get the raw value.  Note that since pin is
		// already a pointer, then we do not need to take the address of pin
		// (&pin) to pass to the flexAnaIn() function.
		low_value = flexAnaIn(pin);
		printf("Low voltage set to %f\n\n", low_voltage);

		// Prompt the user to supply a high voltage, and read in that voltage
		printf("Apply high voltage, and enter voltage value here:  ");
		gets(buffer);
		high_voltage = atof(buffer);
		// Read the analog input to get the raw value
		high_value = flexAnaIn(pin);
		printf("High voltage set to %f\n\n", high_voltage);

		// Apply the calibration
		flexAnaInCalib(pin, low_value, low_voltage, high_value, high_voltage);
		printf("Calibration applied!\n");

		// Display the voltage from the newly calibrated channel
		printf("\n\n\nPress any key to configure another analog input channel\n");
		// Move the cursor up 3 lines, print the pin name, and display the
		// analog input value.
		printf("\x1b[3A%s value:  %7.4f", flexPinName(pin),
		       flexAnaInVolts(pin));
		// Continue displaying the analog input value until a key is pressed
		while (!kbhit()) {
			// Note that "\x1b[7D" moves the cursor 7 positions back, so that
			// we can print over the previous value
			printf("\x1b[7D%7.4f", flexAnaInVolts(pin));
		}
		// Get (and throw away) the key that was pressed
		getchar();
		printf("\n\n\n\n");
	}
}

