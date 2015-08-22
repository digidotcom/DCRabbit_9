/*******************************************************************************
        Samples\RabbitFLEX_SBC40\lcd.c
        Rabbit Semiconductor, 2006

        This sample program demonstrates the use of an LCD display with the
        RabbitFLEX SBC40.  The user is presented with a menu of commands.  Each
        command performs a specific function on the LCD.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct pointer to a RabbitFlex
 * LCD information structure.  These are the software names for each of your
 * RabbitFlex LCDs.  The LCD that you specify here is the one that will be used
 * for this sample.
 */
#define MY_LCD flex_lcd

/*
 * This is a utility function that homes the cursor in the stdio window and
 * clears the status line that is located there.
 */
void stdio_home(void)
{
	printf("\x1b[u\x1b[K");
}

/*
 * This is another utility function that both deletes a given number of lines
 * in the stdio window from the current cursor position and up, and then homes
 * the cursor and clears the status line that is located there.
 */
void stdio_clear_and_home(int num_lines)
{
	int i;

	printf("\n");
	for (i = 0; i < num_lines; i++) {
		printf("\x1b[1A\x1b[K");
	}
	printf("\x1b[u\x1b[K");
}

void main(void)
{
	int backlight;					// Backlight state
	char contrast;					// Contrast value
	unsigned int num_rows;		// Number of rows on the LCD
	unsigned int num_cols;		// Number of columns on the LCD
	unsigned int cur_row;		// Current row
	unsigned int cur_col;		// Current column
	int disp_onoff;				// Is the display on or off?
	unsigned int cursormode;	// Mode for the LCD cursor
	char c;							// Used to hold a character
	char buffer[128];				// Used to get a message to display
	int temp;						// Used as a temporary value
// The following can be uncommented to customize some aspects of the attached
// LCD
/*
	Flex_LCDConf my_lcd;

	// Number of columns on the display
	my_lcd.num_cols = 20;
	// Number of rows on the display
	my_lcd.num_rows = 4;
	// Starting address of each row (up to 4 rows)
	my_lcd.row_addresses[0] = 0x80;
	my_lcd.row_addresses[1] = 0xc0;
	my_lcd.row_addresses[2] = 0x94;
	my_lcd.row_addresses[3] = 0xd4;
	// Minimum duty cycle for contrast control
	my_lcd.pwm_contrast_min = 0;
	// Maximum duty cycle for contrast control
	my_lcd.pwm_contrast_max = 1024;
	// Default scaled value for contrast control (0-255)
	my_lcd.def_contrast = 0;
*/

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// Initialize the display.  Note that the second parameter allows you to
	// customize certain aspects of the LCD display, such as the number of rows
	// and columns, default contrast setting, etc.  See the commented-out
	// section above.
	flexDispInit(&MY_LCD, NULL);

	// Uncomment the following line (and comment out the line above) to use the
	// LCD customization structure
//	 flexDispInit(&MY_LCD, &my_lcd);

	// Initialize some LCD parameters
	// Backlight defaults to off
	backlight = 0;
	flexDispBacklight(&MY_LCD, backlight);
	// Get the default contrast
	contrast = flexDispGetContrast(&MY_LCD);
	// Get the number of rows and columns on the display
	flexDispGetDimensions(&MY_LCD, &num_cols, &num_rows);
	// Set the current row and column to 0,0
	cur_row = 0;
	cur_col = 0;
	flexDispGoto(&MY_LCD, cur_row, cur_col);
	// The display defaults to being on
	disp_onoff = 1;
	flexDispOnoff(&MY_LCD, disp_onoff);
	// Set the default mode for the LCD cursor to on
	cursormode = DISP_CURON;
	flexDispCursor(&MY_LCD, cursormode);

	// Skip a couple of lines (top line will be used for feedback)
	printf("\x1b[s\n\n");
   // Display a menu of choices to the user
   printf("LCD Menu\n--------\n");
   printf("b) Toggle the backlight         -- flexDispBacklight\n");
   printf("c) Display a character          -- flexDispPutc\n");
   printf("m) Change the cursor mode       -- flexDispCursor\n");
   printf("o) Toggle the display on or off -- flexDispOnoff\n");
   printf("p) Print a message              -- flexDispPrintf\n");
   printf("r) Reset (clear) the display    -- flexDispClear\n");
   printf("+) Increase the contrast value  -- flexDispContrast\n");
   printf("-) Decrease the contrast value  -- flexDispContrast\n");
   printf("h) Move the cursor left         -- flexDispGoto\n");
   printf("j) Move the cursor down         -- flexDispGoto\n");
   printf("k) Move the cursor up           -- flexDispGoto\n");
   printf("l) Move the cursor right        -- flexDispGoto\n");
   printf("\n\nPlease press the key corresponding to your selection...\n");
   stdio_home();
   printf("\n");

	while (1) {
		// Skip past the menu
		printf("\x1b[20B");
		// Get the user's menu selection
	   c = getchar();
	   switch (c) {
	   // Toggle the backlight
	   case 'b':
	   	stdio_home();
	   	if (backlight == 0) {
	   		backlight = 1;
	   		printf("Backlight turned on\n");
	   	}
	   	else {
	   		backlight = 0;
	   		printf("Backlight turned off\n");
	   	}
	   	flexDispBacklight(&MY_LCD, backlight);
	      break;
	   // Accept a character to display
	   case 'c':
	   	// Display a character to the display
	   	printf("Enter a character to display:  ");
	   	// Get the character
	   	c = getchar();
	   	// Send the character to the display
	   	flexDispPutc(&MY_LCD, c);
	   	stdio_clear_and_home(1);
	   	printf("'%c' has been written to the display\n", c);
	      break;
	   // Cycle the cursor mode
	   case 'm':
	   	switch (cursormode) {
	   	case DISP_CUROFF:
	   		cursormode = DISP_CURON;
		   	stdio_home();
		   	printf("Cursor mode set to \"on\" (DISP_CURON)\n");
	   		break;
	   	case DISP_CURON:
	   		cursormode = DISP_CURBLINK;
		   	stdio_home();
		   	printf("Cursor mode set to \"blink\" (DISP_CURBLINK)\n");
	   		break;
	   	case DISP_CURBLINK:
	   		cursormode = DISP_CUROFF;
		   	stdio_home();
		   	printf("Cursor mode set to \"off\" (DISP_CUROFF)\n");
	   		break;
	   	}
	   	flexDispCursor(&MY_LCD, cursormode);
	      break;
	   // Toggle the display on and off
	   case 'o':
	   	stdio_home();
	   	if (disp_onoff == 0) {
	   		disp_onoff = 1;
	   		printf("Display turned on\n");
	   	}
	   	else {
	   		disp_onoff = 0;
	   		printf("Display turned off\n");
	   	}
	   	flexDispOnoff(&MY_LCD, disp_onoff);
	   	if (disp_onoff == 1) {
	   		// Turning the display off and then back on happens to set the
	   		// cursor mode to off.  So here we reset the cursor mode explicitly.
	   		flexDispCursor(&MY_LCD, cursormode);
	   	}
	      break;
	   // Accept a text message to send to the display
	   case 'p':
	   	printf("Enter the text to display:  ");
	   	gets(buffer);
	   	flexDispPrintf(&MY_LCD, buffer);
	   	stdio_clear_and_home(2);
	   	printf("Message displayed\n");
	      break;
	   // Clear the display and home the cursor
	   case 'r':
	   	flexDispClear(&MY_LCD);
	   	stdio_home();
	   	printf("Display cleared\n");
	      break;
	   // Increase the contrast value
	   case '+':
	   	// We are increasing in steps of 5, since that already provides more
	   	// than enough contrast resolution
	   	temp = contrast;
	   	temp += 5;
	   	if (temp > 255) {
				temp = 255;
			}
			contrast = (char)temp;
			flexDispContrast(&MY_LCD, contrast);
	   	stdio_home();
	   	printf("Contrast set to %d\n", (int)contrast);
	      break;
	   // Decrease the contrast value
	   case '-':
	   	// We are decreasing in steps of 5, since that already provides more
	   	// than enough contrast resolution
	   	temp = contrast;
	   	temp -= 5;
	   	if (temp < 0) {
	   		temp = 0;
	   	}
	   	contrast = (char)temp;
	   	flexDispContrast(&MY_LCD, contrast);
	   	stdio_home();
	   	printf("Contrast set to %d\n", (int)contrast);
	      break;
	   // Move the cursor to the left
	   case 'h':
	   	// Get the current position
	   	flexDispGetPosition(&MY_LCD, &cur_col, &cur_row);
	   	// Check if we are moving to the left of the first column
	   	if (cur_col == 0) {
	   		// We are, so set to the last column
	   		cur_col = num_cols - 1;
	   		// Check if we are moving above the first row
	   		if (cur_row == 0) {
	   			// We are, so set to the last row
	   			cur_row = num_rows - 1;
	   		}
	   		else {
	   			// We aren't, so just decrement the row
		   		cur_row--;
		   	}
	   	}
	   	else {
		   	cur_col--;
		   }
		   // Move the cursor
	   	flexDispGoto(&MY_LCD, cur_col, cur_row);
	   	stdio_home();
	   	printf("Cursor moved to the left\n");
	      break;
		// Move the cursor down
	   case 'j':
	   	// Get the current position
	   	flexDispGetPosition(&MY_LCD, &cur_col, &cur_row);
			cur_row++;
			// Check if we are moving below the last row.  If so, set to the
			// first row.
			if (cur_row >= num_rows) {
				cur_row = 0;
			}
		   // Move the cursor
	   	flexDispGoto(&MY_LCD, cur_col, cur_row);
	   	stdio_home();
	   	printf("Cursor moved down\n");
	      break;
	   // Move the cursor up
	   case 'k':
	   	// Get the current position
	   	flexDispGetPosition(&MY_LCD, &cur_col, &cur_row);
			// Check if we are moving above the first row.  If so, set to the last
			// row.
	   	if (cur_row == 0) {
	   		cur_row = num_rows - 1;
	   	}
	   	else {
		   	cur_row--;
		   }
		   // Move the cursor
	   	flexDispGoto(&MY_LCD, cur_col, cur_row);
	   	stdio_home();
	   	printf("Cursor moved up\n");
	      break;
	   case 'l':
	   	// Get the current position
	   	flexDispGetPosition(&MY_LCD, &cur_col, &cur_row);
	   	cur_col++;
	   	// Check if we are moving past the last column.
	   	if (cur_col > num_cols - 1) {
	   		// We are, so be sure to increment the row as well as setting the
	   		// cursor to the first column.
	   		cur_col = 0;
	   		cur_row++;
	   		// Check if we are moving below the last row.  If so, set to the
	   		// first row.
	   		if (cur_row >= num_rows) {
	   			cur_row = 0;
	   		}
	   	}
		   // Move the cursor
	   	flexDispGoto(&MY_LCD, cur_col, cur_row);
	   	stdio_home();
	   	printf("Cursor moved to the right\n");
	      break;
	   // Invalid command
	   default:
	   	stdio_home();
	   	printf("ERROR:  Invalid command\n");
	      break;
	   }
	}
}

