/***************************************************************************
	keymenu.c
	Z-World, 2002

	This sample program is for the LCD MSCG12232 Display Module and
	LP3500 series controllers.

	Description
	===========
  	This program demonstrates how to implement a menu system using a
  	highlight bar on a graphic LCD display.  It uses functions from
  	LCD122KEY7.LIB	and devPowerSet() to enable the module.

	The menu options for this sample	are as follows:

  	1.	Set Date & Time
  	2.	Display Date/Time
  	3.	Turn backlight OFF
  	4. Turn backlight ON
  	5. Toggle LEDS
  	6. Increment LEDS
  	7. Disable LEDS

  	To select a option use the scroll keys(Scroll and/or Page UP/DOWN keys)
  	to highlight the option that you want to select and then press the ENTER
  	key.

	Once the option is selected the operation will be completed or you
	will be prompted to do additional steps to complete the option
	selected.


	Development Note:
	-----------------
	Menu options can be added/deleted and the highlight bar will automatically
	adjust to the new menu list. This will also require that you add/delete case
	statements in main() to match your menu list.
**************************************************************************/
#class auto
#memmap xmem

//---------------------------------------------------------
// Bitmaps
//---------------------------------------------------------
// Bitmap : Zwbw5_bmp
// Buffer Size : 203
// Monochrome  : White Foreground, Black Background
// Mode   : Landscape
// Height : 29 pixels.
// Width  : 53 pixels.
// Init   : glXPutBitmap (leftedge,topedge,53,29,Zwbw5_bmp);

xdata Zwbw5_bmp {
'\x00','\x00','\x1F','\xF8','\x00','\x00','\x07',
'\x00','\x00','\xFF','\xFF','\x80','\x00','\x07',
'\x00','\x03','\xF3','\xF0','\xE0','\x00','\x07',
'\x00','\x0F','\xFF','\xE0','\x38','\x00','\x07',
'\x00','\x3F','\x0F','\xFC','\x0E','\x00','\x07',
'\x00','\x7E','\x1E','\x07','\x83','\x00','\x07',
'\x00','\xFC','\x38','\x01','\xE1','\x80','\x07',
'\x01','\xB8','\x30','\x00','\x38','\xC0','\x07',
'\x03','\x30','\x70','\x00','\x0C','\x60','\x07',
'\x06','\x60','\xE0','\x00','\x03','\x70','\x07',
'\x0E','\x60','\xC0','\x00','\x01','\xF0','\x07',
'\x0C','\xC1','\x80','\x00','\x00','\xE8','\x07',
'\x1F','\xFF','\xE0','\x00','\x00','\x2C','\x37',
'\x3F','\x83','\x7F','\x00','\x00','\x34','\x1F',
'\x31','\x03','\x01','\xE0','\x00','\x2A','\x3F',
'\x31','\x02','\x00','\x1C','\x00','\x26','\x67',
'\x62','\x06','\x00','\x07','\x00','\x27','\xC7',
'\x62','\x04','\x00','\x01','\x80','\x27','\x07',
'\x00','\x00','\x00','\x00','\x00','\x70','\x07',
'\x00','\x00','\x00','\x00','\x03','\x80','\x07',
'\x7C','\x21','\x0D','\xF9','\xF8','\x7C','\x07',
'\x08','\x31','\x8B','\x0F','\x84','\x63','\x07',
'\x18','\x11','\x8F','\xE1','\x14','\x61','\x07',
'\x11','\x93','\xF0','\x05','\x34','\x61','\x07',
'\x31','\xC0','\x54','\x07','\xE4','\x61','\x07',
'\x20','\x0A','\x56','\x05','\x44','\x61','\x07',
'\x40','\x0C','\x62','\x05','\x24','\x63','\x07',
'\x40','\x04','\x21','\x99','\x34','\x6E','\x07',
'\xFC','\x04','\x20','\xF1','\x17','\xF8','\x07'
};

//---------------------------------------------------------
// Macro's
//---------------------------------------------------------
#define MAXDISPLAYROWS	4
#define LEDOFF				0
#define TOGGLE				1
#define INCREMENT			2
#define OPERATE			3

#define ASCII				0
#define NUMBER				1

//----------------------------------------------------------
// Main_Menu options
//----------------------------------------------------------
// Can insert/delete menu options. The highlight bar is setup
// to start with the first MENU option and stop at the last
// menu option in the MENU.
//
// When adding/deleting menu options you must match up the
// case statements to the menu option number.
//
const char *main_menu [] =
{		" <<<<Main Menu>>>>",
		"1.Set Date & Time",
		"2.Display Date/Time",
		"3.Turn Backlight OFF",
		"4.Turn Backlight ON",
		"5.Toggle LED's",
		"6.Increment LED's",
		"7.Disable LED's",
		NULL
};

//----------------------------------------------------------
// Structures, arrays, variables
//----------------------------------------------------------
fontInfo fi6x8, fi8x10, fi12x16;
windowFrame textWindow;

typedef struct  {
	int data;
	char *ptr;
} fieldupdate;

struct tm CurTime;

char szTime[40];
char szString[20];
const char Days[] = {"SunMonTueWedThuFriSat"};
const char Months[] = {"JanFebMarAprMayJunJulAugSepOctNovDec"};


int ledCntrl;
int beeperTick, timerTick ;
int max_menu_options;
int max_cmds_options;
unsigned long ulTime;
char *keybuffer;


//------------------------------------------------------------------------
// Milli-sec delay function
//------------------------------------------------------------------------
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


//------------------------------------------------------------------------
// Process key to do number and ASCII field changes
//------------------------------------------------------------------------
int ProcessKeyField(int mode, fieldupdate *field)
{
	static int wKey;

	keyProcess();
	msDelay(100);
	if((wKey = keyGet()) != 0)
	{
		switch(wKey)
		{
			// Decrement number by 10 or pointer by 3
			case '-':
				if(mode == NUMBER)
					field->data -= 10;
				else
					field->ptr  -= 3;
				break;

			// Increment number by 10 or pointer by 3
			case '+':
				if(mode == NUMBER)
					field->data += 10;
				else
					field->ptr  += 3;
				break;

			// Increment number or pointer by 1
			case 'U':
				if(mode == NUMBER)
					field->data++;
				else
					field->ptr++;
				break;

			// Decrement number or pointer by 1
			case 'D':	// Decrement X1
				if(mode == NUMBER)
					field->data--;
				else
					field->ptr--;
				break;

			// Done Editing field
			case 'E':
				wKey = 'E';
				break;

			default:
				wKey = -1;
		  		break;
		}
	}
	return(wKey);
}


//------------------------------------------------------------------------
// Get and process the users MENU option
//------------------------------------------------------------------------
int GetKeypadOption( int *offset, int *highlight )
{
	static int wKey;

	if((wKey = keyGet()) != 0)
	{
		switch(wKey)
		{
			case '-':	// Page down
				if(*offset < ((sizeof(main_menu)/sizeof(int)) - 1))
				{
					if((*offset + MAXDISPLAYROWS) < ((sizeof(main_menu)/sizeof(int)) - 1))
						*offset += 4;
				}
				if(*offset == 0)
					*highlight = 1;
				else
					*highlight = 0;
				wKey = -1;
				break;

			case '+':	// Page up
				if(*offset > 3)
					*offset -=4;
				else
					*offset = 0;
				if(*offset == 0)
					*highlight = 1;
				else
					*highlight = 0;
				wKey = -1;
				break;

			case 'U':	// Scroll-up by one line
				*highlight -= 1;
				if(*highlight < 0)
				{
					*offset -= 1;
					*highlight = 0;
				}
				if(*offset == 0 && *highlight == 0)
					*highlight = 1;
				wKey = -1;
				break;

			case 'D':	// Scroll-down by one line
				if((*offset + (*highlight) + 1) < ((sizeof(main_menu)/sizeof(int)) - 1))
				{
					*highlight += 1;
					if(*highlight > 3)
					{
						*offset += 1;
						*highlight = 3;
					}
				}
				wKey = -1;
				break;

			case 'E':	// Select option
				wKey = *offset + *highlight;
				break;

			default:
				wKey = -1;
		  		break;
		}
	}
	return(wKey);
}

//------------------------------------------------------------------------
// Display a MENU on the LCD display and get the menu option from the user
//------------------------------------------------------------------------
int display_menu ( char **line, int initialize)
{
	static int offset, tmpoffset, i;
	static int menu_option;
	static int lasthighlight;
	static int scrolling;
	static int highlight;

	costate
	{
		if(initialize)
		{
			offset = 0;				// Initialize menu line index
			highlight = 1;			// Assumes all menus have a heading
			tmpoffset = ~offset;
		}

		menu_option = 0;			// Initially set to no option selected
		scrolling = FALSE;

		// Wait until you get a valid user option
		while(menu_option == 0)
		{
			// Display menu option
			if(tmpoffset != offset)
			{
				glBlankScreen();
				for(i=0; i < 4; i++)
				{	// Display up to 4 lines of menu options
					TextGotoXY(&textWindow, 0, i);
					TextPrintf(&textWindow, "%s", line[offset]);
					if(line[offset + 1] == NULL) {
			 			break;
					}
					offset++;
				}

				// Reset the offset back to the first option displayed
				offset = offset-i;
				tmpoffset = offset;
			}
			glSetBrushType(PIXXOR);
			glBlock (0, highlight*8, 122, 8);
			glSetBrushType(PIXBLACK);
			lasthighlight = highlight;

			// Get the user's option
			waitfor((menu_option = GetKeypadOption(&offset, &highlight)));
			// Check if user selected the scrolling option

			glSetBrushType(PIXXOR);
			glBlock (0, lasthighlight*8, 122, 8);
			glSetBrushType(PIXBLACK);
			if(menu_option == -1)
			{

				// Set menu option to zero due to scrolling operation
				menu_option = 0;
				scrolling = TRUE;
			}
		}
	}
	tmpoffset = offset;
	return(menu_option);
}

//------------------------------------------------------------------------
// Format the Date and Time for the LCD display
//------------------------------------------------------------------------
void FormatDateTime ( void )
{
	char Day[4], Mon[4];

	ulTime = read_rtc ();			// get the RTC value
	mktm( &CurTime, ulTime );		// convert seconds to date values

	strncpy ( Day, &Days[CurTime.tm_wday*3], 3 );
	strncpy ( Mon, &Months[(CurTime.tm_mon-1)*3], 3 );
	Day[3] = 0;
	Mon[3] = 0;

	sprintf ( szTime, "%s %s %d, %d \n%02d:%02d:%02d",
		Day, Mon, CurTime.tm_mday, CurTime.tm_year+1900,
		CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec );
}

//------------------------------------------------------------------------
// Display the Date and Time on the LCD display
//------------------------------------------------------------------------
int dispDate( void )
{
	static int status;
	auto int wKey;

	costate
	{
		// Get current Date/Time
		status = 0;
		ulTime = read_rtc ();			// get the RTC value
		mktm( &CurTime, ulTime );		// convert seconds to date values
		FormatDateTime();					// convert to text
		waitfor(DelayMs(5));

		// Display Date and Time
		glBuffLock();
		TextGotoXY(&textWindow, 0, 0);
		TextPrintf(&textWindow, "%s\n", szTime);
		waitfor(DelayMs(5));

		// Display user exit message
		TextGotoXY(&textWindow, 0, 3);
		TextPrintf(&textWindow, "Press Key to EXIT");
		waitfor(DelayMs(5));
		glBuffUnlock();

		// Wait for key to be pressed to exit
		waitfor(((wKey = keyGet()) != 0) || DelayMs(100));
		if(wKey != 0)
		{
			glBlankScreen();
			status = 1;
		}
	}
	return(status);
}


//------------------------------------------------------------------------
// LED control function
//------------------------------------------------------------------------
void leds( int mode )
{
	static int toggle, increment;
	auto int led, mask;

	#GLOBAL_INIT {toggle=0;}
	#GLOBAL_INIT {increment=0;}

	if(mode != OPERATE)
	{
		ledCntrl = mode;
		toggle = 0;
		increment = 0;
		return;
	}

	if(ledCntrl == TOGGLE)
	{
		toggle = (~toggle) & 0x01;
		for(led = 0; led <= 6; led++)
		{
			//Toggle the LED's
			dispLedOut(led, toggle);
		}
	}
	else if(ledCntrl == INCREMENT)
	{
		mask = 0x01;
		increment++;
		for(led = 0; led <= 6; led++)
		{

			if(increment & mask)
				dispLedOut(led, 1);
			else
				dispLedOut(led, 0);
			mask = mask << 1;
		}
	}
	else
	{	//Turn all LED'S OFF
		for(led = 0; led <= 6; led++)
			dispLedOut(led, 0);
	}
}


//------------------------------------------------------------------------
// Date and Time prompt message routine
//------------------------------------------------------------------------
void date_prompt(char *ptr, int *col, int *row)
{
	glBlankScreen();
	TextGotoXY(&textWindow, 0, 0);
	TextPrintf(&textWindow, "%s", ptr);
	TextCursorLocation(&textWindow, col, row);

	TextGotoXY(&textWindow, 0, 3);
	TextPrintf(&textWindow, "ENTER to Continue...");
}


//------------------------------------------------------------------------
// Set Date and Time
//  For some reason the starting date is always midnight Jan 1, 2001.
//------------------------------------------------------------------------
void SetDateTime( void )
{
	int wKey;
	int col, row;
	char buffer[256];
	fieldupdate dateTime;

	// Setup for FAST key repeat after holding down key for 12 ticks
	keyConfig (  6,'E',0, 12, 1,  1, 1 );
	keyConfig (  2,'D',0, 12, 1,  1, 1 );
	keyConfig (  5,'+',0, 12, 1,  1, 1 );
	keyConfig (  1,'U',0, 12, 1,  1, 1 );
	keyConfig (  4,'-',0, 12, 1,  1, 1 );

	date_prompt("Select \n4 digit year: ", &col, &row);
	dateTime.data = 2001;
	while(1)
	{
		sprintf(buffer, "%04d", dateTime.data);
		TextGotoXY(&textWindow, col, row);
		TextPrintf(&textWindow, "%s", buffer);
		while((wKey = ProcessKeyField(NUMBER, &dateTime)) == 0);
		if(dateTime.data < 1900 || dateTime.data > 2047)
		{
			dateTime.data = 2001;
		}
		if(wKey == 'E')
		{
			if( dateTime.data  >= 1900 && dateTime.data < 2048) {
				CurTime.tm_year = dateTime.data - 1900;	// offset from 1900
				break;
			}
		}
	}

	date_prompt("Enter month: ", &col, &row);
	dateTime.data = 1;
	while(1)
	{
		sprintf(buffer, "%02d", dateTime.data);
		TextGotoXY(&textWindow, col, row);
		TextPrintf(&textWindow, "%s", buffer);
		while((wKey = ProcessKeyField(NUMBER, &dateTime)) == 0);
		if(wKey == 'E')
		{
			if( dateTime.data >= 1 && dateTime.data < 13 )
			{
				CurTime.tm_mon = dateTime.data;
				break;
			}
		}
		if(dateTime.data < 1 || dateTime.data > 12)
		{
			dateTime.data  = (dateTime.data < 1) ? 12 : 1;
		}
	}

	date_prompt("Enter \nday of month: ", &col, &row);
	dateTime.data = 1;
	while(1)
	{
		sprintf(buffer, "%02d", dateTime.data);
		TextGotoXY(&textWindow, col, row);
		TextPrintf(&textWindow, "%s", buffer);
		while((wKey = ProcessKeyField(NUMBER, &dateTime))== 0);
		if(wKey == 'E')
		{
			if( dateTime.data  >= 1 && dateTime.data < 32) {
				CurTime.tm_mday = dateTime.data;
				break;
			}
		}
		if(dateTime.data < 1 || dateTime.data > 31)
		{
			dateTime.data  = (dateTime.data < 1) ? 31 : 1;
		}
	}


	date_prompt("Enter \nhour (24hr): ", &col, &row);
	dateTime.data = 0;
	while(1)
	{
		sprintf(buffer, "%02d", dateTime.data);
		TextGotoXY(&textWindow, col, row);
		TextPrintf(&textWindow, "%s", buffer);
		while((wKey = ProcessKeyField(NUMBER, &dateTime)) == 0);
		if(wKey == 'E')
		{
			if(dateTime.data >= 0 && dateTime.data < 24) {
				CurTime.tm_hour = dateTime.data;
				break;
			}
		}
		if(dateTime.data < 0 || dateTime.data > 23)
		{
			dateTime.data  = (dateTime.data < 0) ? 23 : 0;
		}
	}

	date_prompt("Enter minute: ", &col, &row);
	dateTime.data = 0;
	while(1)
	{
		sprintf(buffer, "%02d", dateTime.data);
		TextGotoXY(&textWindow, col, row);
		TextPrintf(&textWindow, "%s", buffer);
		while((wKey = ProcessKeyField(NUMBER, &dateTime)) == 0);
		if(wKey == 'E')
		{
			if( dateTime.data >= 0 && dateTime.data < 60) {
				CurTime.tm_min = dateTime.data;
				break;
			}
			if(wKey == 'E')
			{
				break;
			}
		}
		if(dateTime.data < 0 || dateTime.data > 59)
		{
			dateTime.data  = (dateTime.data < 0) ? 59 : 0;
		}
	}

	CurTime.tm_sec = 0;
	ulTime = mktime ( &CurTime );		// get seconds from 1/1/1980
	write_rtc ( ulTime );				// set the real time clock
	keypadDef();

	glBlankScreen();
	while(1)
	{
		// Get current Date/Time
		FormatDateTime();					// convert to text

		// Display Date and Time
		glBuffLock();
		TextGotoXY(&textWindow, 0, 0);
		TextPrintf(&textWindow, "%s\n", szTime);

		// Display user exit message
		TextGotoXY(&textWindow, 0, 3);
		TextPrintf(&textWindow, "Press Key to EXIT    ");
		glBuffUnlock();

		keyProcess ();
		if((wKey = keyGet()) != 0)
		{
			glBlankScreen();
			break;
		}
	}
}

//------------------------------------------------------------------------
// Display Sign-on message
//------------------------------------------------------------------------
void SignOnMessage(void)
{
	auto int signMesgDone, i, loop;
	auto char buffer[256];

	// Display Sign-on Message then wait for any key to continue
	glXPutBitmap (0,0,53,29,Zwbw5_bmp);
	msDelay(500);

	signMesgDone = FALSE;
	while(!signMesgDone)
	{
		i=0;
		sprintf(buffer, "Hello from Zworld!!!");
		strcat(buffer, "     Press any KEY to Continue...      ");
		while(buffer[i] != '\0' && !signMesgDone)
		{
			glHScroll(50, 0, LCD_XS, 16, -6);
			glPrintf (116,  4,   &fi6x8, "%c", buffer[i++]);
			for(loop=0; loop < 165; loop++)
			{
				msDelay(1);
				keyProcess ();
				if(keyGet() != 0)
				{
					signMesgDone = TRUE;
				}
			}
		}
	}
}

//------------------------------------------------------------------------
// Sample program to demonstrate the LCD and keypad
//------------------------------------------------------------------------
void main (	void	)
{
	auto int option, initialize;

	//------------------------------------------------------------------------
	// Initialize the controller
	//------------------------------------------------------------------------
	brdInit();							//Initialize the controller
	devPowerSet(DISPDEV, 1);		//enable display/keypad buffer
	dispInit();							//initialize module
	keypadDef();		// Use the default keypad ASCII return values

	//glBackLight(1);	// Turn-on the backlight
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);			//	Initialize 6x8 font
	glXFontInit(&fi8x10, 8, 10, 32, 127, Font8x10);		//	Initialize 10x16 font
	glXFontInit(&fi12x16, 12, 16, 32, 127, Font12x16);	//	Initialize 12x16 font

	// Setup and center text window to be the entire display
	TextWindowFrame(&textWindow, &fi6x8, 1, 0, 121, 32);

	// Set variables to known states
	initialize = TRUE; 	// Set flag for MENU options to be displayed
	ledCntrl = LEDOFF;	// Initially disable the LED's

	//------------------------------------------------------------------------
	// Display Sign-on message and wait for keypress
	//------------------------------------------------------------------------
	SignOnMessage();

	//------------------------------------------------------------------------
	// Main program loop for the MENU system
	//------------------------------------------------------------------------
	for (;;)
	{
		costate
		{
			keyProcess ();
			waitfor(DelayMs(10));
		}

		costate
		{
			leds(OPERATE);
			waitfor(DelayMs(50));
		}
		costate
		{
			// Display the MAIN MENU
			waitfor((option = display_menu(main_menu, initialize)) > 0);

			// Get menu option from the user
			switch(option)
			{
				// Change Date/Time
				case 1:	glBlankScreen();
							SetDateTime();
							initialize = TRUE;
							break;

				// Display current Date/Time
				case 2:	glBlankScreen();
							waitfor(dispDate());
							initialize = TRUE;
							break;

				// Turn Backlight OFF
				case 3:	glBackLight(0);
							initialize = FALSE;
							break;

				// Turn Backlight ON
				case 4:	glBackLight(1);
							initialize = FALSE;
							break;

				// Enable Toggle leds option
				case 5:	leds(TOGGLE);
							initialize = FALSE;
							break;

				// Enable Increment leds option
				case 6:	leds(INCREMENT);
							initialize = FALSE;
							break;

				// Disable LED's
				case 7:	leds(LEDOFF);
							initialize = FALSE;
							break;

				// User made invalid selection
				default:
					break;
			}
		}
	}
}