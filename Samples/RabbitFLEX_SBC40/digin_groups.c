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
        Samples\RabbitFLEX_SBC40\digin_groups.c

        This sample demonstrates how to use the grouping functionality for
        digital inputs.  It first displays a list of all digital inputs, and
        allows the user to select which digital inputs are desired for the
        group.  Once the group has been selected, the status of all digital
        inputs in the group is displayed and updated continuously.

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
 * Prompts the user with a list of digital inputs available on the RabbitFlex
 * board.  Then allows the user to input a list of pins to add to a digital
 * input group.  This function returns whether the group was formed correctly or
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

	// Display the list of digital inputs
	printf("Please select from the following list of digital inputs:\n");
	i = 0;
	while (_flex_pins_digin[i] != FLEX_GROUP_END) {
		printf("%2d) %s\n", i+1, flexPinName(_flex_pins_digin[i]));
		i++;
	}
	// Remember the maximum pin index;
	max_pin = i - 1;

	// Prompt the user for the digital input group
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
			group[i] = _flex_pins_digin[j];
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

void main(void)
{
	int num_members;
	unsigned int result;
	int i;

	// Groups can contain up to 16 pins.  The 17th entry is needed for the
	// FLEX_GROUP_END marker.
	Flex_IOPin *group[17];

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Prompt the user to create the group of digital inputs
	while (num_members = get_group(group) < 0) {
		printf("Error in pin group specification!  Please try again...\n\n");
	}
	// Skip a couple of lines
	printf("\n\n");

	// Continuously display the group of digital inputs
	while (1) {
		// Save the current cursor position
		printf("\x1b[s");
		// Read the digital input group.  The first parameter is the group that
		// the program constructed.  The second parameter is a pointer to an
		// unsigned integer, which is where the digital input readings will be
		// placed.  Note that the first pin in the group will have its reading
		// in the lowest bit of result, and so on.
	   flexDigInGroup16(group, &result);
	   // Go through the group and the result
	   i = 0;
	   while (group[i] != FLEX_GROUP_END) {
	   	// Print the name of this pin and the digital input state
	      printf("%-20s = %d\n", flexPinName(group[i]), result & 0x01);
	      // Right-shift the result to get to the next digital input
	      result >>= 1;
	      i++;
	   }
		// Go back to the saved cursor position
		printf("\x1b[u");
	}
}

