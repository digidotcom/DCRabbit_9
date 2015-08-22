/*******************************************************************************
        Samples\RabbitFLEX_SBC40\anain_avg_simple.c
        Rabbit Semiconductor, 2006

        Demonstrates very basic use of the flexAnaInAverage() and
        flexAnaInVoltsAverage() functions.  It simply continuously
        monitors the user-specified analog input pin and displays the
        current value to the stdio window.

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

// Simple delay function using MS_TIMER
nodebug
void msDelay(unsigned int delay)
{
	unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


// Set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   msDelay(100);
   printf ("\x1B=%c%c%s", x, y, s);
}

void main(void)
{
	int anain_raw, anain_raw_avg;
	float anain_volts, anain_volts_avg;
	char *name;
   char s[256];
   float avgvalue, test;
   int avgbuffer[10];
   int avgreading;


	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Get the name of the analog input pin
	name = flexPinName(&MY_ANAIN);

   // Set the discount averaging alpha value for the selected A/D channel
   flexAnaInAverageSetting(&MY_ANAIN, .1);

   // Loop forever
	while (1) {
		// Read the raw value for the specified analog input.  Note that the
		// parameter is a pointer to a Flex_IOPin (RabbitFlex pin) structure.
		anain_raw_avg = flexAnaInAverage(&MY_ANAIN);
      anain_raw = flexAnaIn(&MY_ANAIN);

      // This function reads the voltage for the specified analog input.  This
		// uses the calibration for the analog input to convert from the raw value
		// to a voltage.
      anain_volts = flexAnaInVolts(&MY_ANAIN);
		anain_volts_avg = flexAnaInVoltsAverage(&MY_ANAIN);

		// Print the name and values (raw and voltage) for the analog input.
		// Note that the "\x1b=\x20\x20" part simply places the cursor at the
		// home position in the stdio window.
      //printf("\x1b[1A");
      sprintf(s, "%s Rawdata= %4d  Avg rawdata= %4d   ", name, anain_raw,
              anain_raw_avg);
   	DispStr(0, 1, s);

      sprintf(s, "%s Volts= %6.3f Avg volts= %6.3f  ",  name, anain_volts,
              anain_volts_avg);
   	DispStr(0, 2, s);

   }
}

