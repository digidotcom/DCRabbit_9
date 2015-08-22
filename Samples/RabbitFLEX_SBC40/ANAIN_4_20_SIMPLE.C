/*******************************************************************************
        Samples\RabbitFLEX_SBC40\anain_4_20_simple.c
        Rabbit Semiconductor, 2006

        Demonstrates very basic use of the flexAnaIn() and flexAnaInmAmps()
        functions.  It simply continuously monitors the user-specified analog
        input pin and displays the current value to the stdio window.

        Note that you must change the macro at the beginning of the program
        to point to your preferred analog input.  Refer to your list of pins
        and software names for your specific RabbitFlex board for the
        appropriate information to put here.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct pointer to a RabbitFlex
 * IO pin information structure.  These are the software names for each of your
 * RabbitFlex pins.  The pin that you specify here is the one that will be used
 * for this sample.
 */
#define MY_ANAIN flex_anain1

void main(void)
{
	int anain_raw;			// Used for the analog input raw value
	float anain_mamps;	// Used for the analog input current value
	char *name;				// Used to point to the name of the analog input
   int newvalue;			// Stores whether or not a new analog input value is
   							// available

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Get the name of the analog input pin
	name = flexPinName(&MY_ANAIN);

	// Loop forever
	while (1) {
   	// Check if a new analog input reading has occurred.  Note that the
      // second parameter indicates whether the new value status is cleared
      // for the given pin.  If the second parameter is cleared, then the new
      // value status is cleared; otherwise, the new value status is left
      // unchanged.
      newvalue = flexAnaInNewValue(&MY_ANAIN, 1);
   	if (newvalue > 0) {
	      // Read the raw value for the specified analog input.  Note that the
	      // parameter is a pointer to a Flex_IOPin (RabbitFlex pin) structure.
	      // If this were not specified as the MY_ANAIN macro, then the call
	      // would look like:
	      // flexAnaIn(&flex_anain1);
	      anain_raw = flexAnaIn(&MY_ANAIN);
	      // This function reads the current for the specified analog input.
         // This uses the calibration for the analog input to convert from the
         // raw value to a current.
	      anain_mamps = flexAnaInmAmps(&MY_ANAIN);

	      // Print the name and values (raw and current) for the analog input.
	      // Note that the "\x1b=\x20\x20" part simply places the cursor at the
	      // home position in the stdio window.
	      printf("\x1b=\x20\x20%s (raw)   = %5d\n", name, anain_raw);
	      printf("%s (mAmps) = %7.4f", name, anain_mamps);
      }
      // Check if flexAnaInNewValue() indicated that this was not an analog
      // input
      else if (newvalue < 0) {
      	printf("Not a valid analog input!\n");
         return;
      }
	}
}

