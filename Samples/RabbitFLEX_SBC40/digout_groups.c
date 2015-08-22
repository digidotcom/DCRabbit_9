/*******************************************************************************
        Samples\RabbitFLEX_SBC40\digout_groups.c
        Rabbit Semiconductor, 2006

        This sample demonstrates how to use the grouping functionality for
        digital outputs.  It first displays a list of all digital outputs, and
        allows the user to select which digital outputs are desired for the
        group.  Once the group has been selected, the digital output values
        are cycled continuously.

*******************************************************************************/

/*
 * Skip past any whitespace in the string
 */
char *gobble_spaces(char *string)
{
	int num;
	num = strspn(string, " \t");
	// Return a pointer past the whitespace
	return string + num;
}

/*
 * Parse through a number or range expression (e.g., 1-4).  Return the low
 * index and the high index (if only a single number, then high index = low
 * index).
 */
char *get_range(char *string, int *low_index, int *high_index)
{
	int is_range;
	int num_chars;
	char *p;

   // Parse out the pin number--use ' ' (space) and '\t' (tab) as separators
	num_chars = strcspn(string, " \t");
	string[num_chars] = '\0';

   // Split the low and high index by the '-' character (if it exists)
	p = strchr(string, '-');
	if (p) {
		*p = '\0';
		is_range = 1;
	}
	else {
		is_range = 0;
	}

   // Get the low index
   *low_index = atoi(string) - 1;
   string += strlen(string) + 1;
   // If this is a range expression, then get the high index
   if (is_range) {
   	*high_index = atoi(string) - 1;
   	string += strlen(string) + 1;
   }
   else {
   	*high_index = *low_index;
   }
   // Return a pointer past the range expression
   return string;
}

/*
 * Prompts the user with a list of digital outputs available on the RabbitFlex
 * board.  Then allows the user to input a list of pins to add to a digital
 * output group.  This function returns whether the group was formed correctly or
 * not.
 */
int get_group(Flex_IOPin *group[])
{
	int i,j;
	char buffer[128];
	char *p, *endp;
	int count;
	int low_index;
	int high_index;
	int max_pin;

	// Display the list of digital outputs
	printf("Please select from the following list of digital outputs:\n");
	i = 0;
	while (_flex_pins_digout[i] != FLEX_GROUP_END) {
		printf("%2d) %s\n", i+1, flexPinName(_flex_pins_digout[i]));
		i++;
	}
	// Remember the maximum pin index;
	max_pin = i - 1;

	// Prompt the user for the digital output group
	printf("Enter the pins you want in the group as a space-separated list.\n");
	printf("You can also select ranges, such as 3-6.\n");
	printf("  Example:  1 3-6 10 11\n");
	// Get the user's input
	gets(buffer);
	// Initialize variables for the loop
	p = buffer;
	endp = p + strlen(p) + 1;
	// Keep track of number of pins in the group
	i = 0;
	// Gobble up initial spaces
	p = gobble_spaces(p);
	// Loop until we are out of pin numbers to process
	while (p < endp) {
		// Get the first range of pins (if low_index == high_index, then there
		// was only a single pin)
		p = get_range(p, &low_index, &high_index);

		// Make sure that the high pin index is at least as high as the low pin
		// index.  Also make sure that the high pin index isn't higher than the
		// highest possible index.
		if (high_index < low_index || high_index > max_pin || low_index < 0) {
			// Error in the pin group specification
			return -1;
		}

		// Assign the requested pin(s) to the group
		for (j = low_index; j <= high_index; j++) {
			// Check if there are too many pins for the group
			if (i >= 16) {
	         printf("Too many pins in the group!\n");
	         return -1;
	      }
			group[i] = _flex_pins_digout[j];
			i++;
		}

		// Parse out more whitespace
		p = gobble_spaces(p);
	}
	// Check that we actually got something in the group
	if (!i) {
		printf("No pins specified!\n");
		return -1;
	}
	// Terminate the group
	group[i] = FLEX_GROUP_END;
	// Indicate that we acquired the group successfully--return the number of
	// members in the group.
	return i;
}

/*
 * Create a timeout
 */
unsigned long set_timeout(unsigned int seconds)
{
	return (MS_TIMER + seconds*1000L);
}

/*
 * Check the timeout
 */
int chk_timeout(unsigned long timeout)
{
	return ((long)(MS_TIMER-timeout))>=0;
}

void main(void)
{
	int num_members;
	unsigned int output;
	int i;
	unsigned long timeout;

	// Groups can contain up to 16 pins.  The 17th entry is needed for the
	// FLEX_GROUP_END marker.
	Flex_IOPin *group[17];

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Prompt the user to create the group of digital outputs
	num_members = get_group(group);
	while (num_members < 0) {
		printf("Error in pin group specification!  Please try again...\n\n");
		num_members = get_group(group);
	}
	// Skip a couple of lines
	printf("\n\n");

	printf("Cycling outputs...\n");

	// Continuously toggle each output in turn
	while (1) {
		// Check if there is only one member of the group
		if (num_members == 1) {
			// Just blink the light if there is only one member of the group
			flexDigOutGroup16(group, 0x00);
         timeout = set_timeout(1);
			while (!chk_timeout(timeout));
			flexDigOutGroup16(group, 0x01);
         timeout = set_timeout(1);
			while (!chk_timeout(timeout));
		}
		else {
	      // Do one complete cycle of the outputs
	      i = 0;
	      output = 0x01;
	      while (group[i] != FLEX_GROUP_END) {
	         flexDigOutGroup16(group, output);
	         i++;
	         output <<= 1;
	         // Set the timeout to 1 second
	         timeout = set_timeout(1);
	         while (!chk_timeout(timeout));
	      }
	   }
   }
}

