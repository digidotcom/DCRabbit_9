/*******************************************************************************
        Samples\RabbitFLEX_SBC40\anaout_calibrate.c
        Rabbit Semiconductor, 2006

        This program allows the user to calibrate the analog out pins on the
        RabbitFlex board.  Note that all RabbitFlex boards come precalibrated
        from the factory--this program will overwrite the factory calibration!

        First, a list of analog outputs is displayed, from which the user is
        allowed to select one output.  After selecting the output, a voltage
        of about 10% of the channel capacity is output.  The user is then
        prompted to measure the output voltage, and to enter the voltage in
        the stdio window.  Next, a voltage of about 90% of the channel
        capacity is output.  The user is again prompted to measure the output
        voltage, and to enter the voltage in the stdio window.

        After the calibration, the user is prompted to output a specific
        voltage on the newly calibrated channel.  That voltage is then
        generated, and the user is again prompted for a new voltage.

*******************************************************************************/

// The following function presents a menu to allow the user to select an
// analog output.  It returns a pointer to the Flex_IOPin structure
// corresponding to the selected pin.
Flex_IOPin *get_analog_output(void)
{
	int done;
	int i;
	int output;
	char buffer[20];

	done = 0;
	// Display the menu and prompt for selection until the user correctly
	// chooses an analog output
	while (!done) {
	   // Display a list of analog input pins
	   i = 0;
	   while (_flex_pins_anaout[i] != FLEX_GROUP_END) {
	      printf("%2d) %s\n", i+1, flexPinName(_flex_pins_anaout[i]));
	      i++;
	   }
	   // Prompt the user to select an analog output
	   printf("\nSelect an analog output (1 - %d):  ", i);
	   // Read the selected analog output
	   gets(buffer);
	   output = atoi(buffer);
	   // Make sure the analog input is in range
	   if ((output >= 1) && (output <= i)) {
	   	done = 1;
	   }
	   else {
	   	printf("\n\nERROR:  Not a valid analog output!\n\n");
	   }
	}
	// Return the selected analog output
	return _flex_pins_anaout[output-1];
}

void main(void)
{
	Flex_IOPin *pin;
	char buffer[20];
	float low_voltage;
	int low_value;
	float high_voltage;
	int high_value;
	int done;

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	while (1) {
		// Display a menu of analog outputs, and get a selection from the user
		pin = get_analog_output();

		// Supply a voltage of 10% of the channel capacity
		low_value = (int)(.10 * 1024);
		flexAnaOut(pin, low_value);
		// Prompt the user to measure the value and enter it
		printf("\nPlease measure the low voltage on %s and enter\n",
		       flexPinName(pin));
		printf("its value here:  ");
		gets(buffer);
		low_voltage = atof(buffer);

		// Supply a voltage of 90% of the channel capacity
		high_value = (int)(.90 * 1024);
		flexAnaOut(pin, high_value);
		// Prompt the user to measure the value and enter it
		printf("\nPlease measure the high voltage on %s and enter\n",
		       flexPinName(pin));
		printf("its value here:  ");
		gets(buffer);
		high_voltage = atof(buffer);

		// Apply the calibration
		flexAnaOutCalib(pin, low_value, low_voltage, high_value, high_voltage);
		printf("Calibration applied!\n");

		// Prompt the user for voltages to output until 'n' is entered (in which
		// case, we will calibrate another analog ouput)
		done = 0;
		while (!done) {
	      printf("\n\nEnter a voltage to generate, or 'n' to calibrate another ");
	      printf("pin:  ");
	      gets(buffer);
	      if (toupper(buffer[0]) == 'N') {
	      	// The user wants to calibrate another output
				done = 1;
	      }
	      else {
	      	// Output the requested voltage
	      	flexAnaOutVolts(pin, atof(buffer));
	      	printf("Generating requested voltage\n");
	      }
	   }
	}
}

