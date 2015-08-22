/***************************************************************************
	keymenu.c
	Z-World, 2002
	
   This program demonstrates a basic Smart Star system using ADC Voltage
   and Digital I/O cards with a Display/Keypad Module.  Additional display
   samples are located in \Samples\LCD_Keypad directory.

  	This program demonstrates how to implement a menu system using a
  	highlight bar on a graphic LCD display. The menu options for this sample
  	are as follows:

	1. Assign Card Slots
	2. Read Analog Inputs
	3. Read Digital Inputs
  	4.	Set Date & Time
  	5.	Display Date/Time
  	6.	Turn backlight OFF
  	7. Turn backlight ON
  	8. Toggle LEDS
  	9. Increment LEDS
  	10. Disable LEDS

  	To select a option use the scroll keys(Scroll and/or Page UP/DOWN keys)
  	to highlight the option that you want to select and then press the ENTER
  	key.

	Once the option is selected the operation will be completed or you
	will be prompted to do additional steps to complete the option
	selected.

	Hardware Notes:
	---------------
	Be sure to assign board types to each slot before using the "Read ADC"
	and "Read DIO" menu options.

	If the board resets on certain demos, your power supply might not be
	strong enough.  For instance, if all slots are loaded and the you select
	"blink LEDs" the board can reset itself if the power supply cannot suppy
	enough amps for all the LEDs.  200mA is not enough, 500mA is good.

	Development Note:
	-----------------
	Menu options can be added/deleted and the highlight bar will automatically
	adjust to the new menu list. This will also require that you add/delete case
	statements in main() to match your menu list.

**************************************************************************/
#class auto

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
#memmap xmem

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
char * const main_menu[] =
{		" <<<<Main Menu>>>>",
		"1)Assign Card Slots",
		"2)Read Analog Inputs",
		"3)Read Digital Inputs",
		"4)Set Date & Time",
		"5)Display Date/Time",
		"6)Turn Backlight OFF",
		"7)Turn Backlight ON",
		"8)Toggle LED's",
		"9)Increment LED's",
		"10)Disable LED's",
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
void msDelay(long sd)
{
	auto unsigned long t1;
	
	t1 = MS_TIMER + sd;
	while ((long)(MS_TIMER-t1) < 0);
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
int GetKeypadOption( int *offset, int *highlight, int msize )
{
	static int wKey;

	if((wKey = keyGet()) != 0)
	{
		switch(wKey)
		{
			case '-':	// Page down
				if(*offset < (((msize)/sizeof(int)) - 1))
				{
					if((*offset + MAXDISPLAYROWS) < (((msize)/sizeof(int)) - 1))
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
				if((*offset + (*highlight) + 1) < (((msize)/sizeof(int)) - 1))
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
int display_menu ( char **line, int initialize, int menusize)
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
		while (menu_option == 0)
		{    
			// Display menu option
			if(tmpoffset != offset)
			{
				glBlankScreen();
				for(i=0; i < 4; i++)
				{	// Display up to 4 lines of menu options
					TextGotoXY(&textWindow, 0, i);	   			
					TextPrintf(&textWindow, "%s", line[offset]); 
					if (line[offset + 1] == NULL) {	   			
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
			waitfor((menu_option = GetKeypadOption(&offset, &highlight, menusize))); 		
			// Check if user selected the scrolling option

			glSetBrushType(PIXXOR);
			glBlock (0, lasthighlight*8, 122, 8);
			glSetBrushType(PIXBLACK);
			if (menu_option == -1)
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
{	char Day[4], Mon[4];

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
			ledOut(led, toggle);
		}
	}
	else if(ledCntrl == INCREMENT)
	{
		mask = 0x01;
		increment++;
		for(led = 0; led <= 6; led++)
		{
			
			if(increment & mask)
				ledOut(led, 1);
			else
				ledOut(led, 0);
			mask = mask << 1;
		}
	}
	else
	{	//Turn all LED'S OFF
		for(led = 0; led <= 6; led++)
			ledOut(led, 0);
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

int adcslot, dioslot;

char * const slotmenu[] =
{	"<<Slots>>",
	"Slot 0",
	"Slot 1",
	"Slot 2",
	NULL
};

char * const cardmenu[] =
{
	"<<Cards>>",
	"Dig I/O",
	"ADC",
	"None",
	NULL
};

//------------------------------------------------------------------------
// Assigns slots for ADC and Digital I/O cards
//------------------------------------------------------------------------
void AssignSlots()
{
	auto int slotpick, cardpick, initialize, done;

	initialize = TRUE;
	done = 1;
			
	while (done)
	{
		costate
		{
			keyProcess ();
			waitfor(DelayMs(10));
		}
		
		costate
		{
			waitfor((slotpick = display_menu(slotmenu, initialize,sizeof(slotmenu))) > 0);
		
			if (slotpick)
			{
				waitfor((cardpick = display_menu(cardmenu, initialize,sizeof(cardmenu))) > 0);
			
				// Get menu option from the user
				switch (cardpick)
				{
					// Assign card slots
					case 1:	dioslot = slotpick-1;
								break;
					case 2:	adcslot = slotpick-1;
								break;
					default:												
						break;
				}
				done = 0;
			}
		}
	}
}

//---------------------------------------------------------
//	displays both the raw data count and voltage equivalent
//---------------------------------------------------------
void anaInInfo (unsigned int channel, unsigned int *rd, float *ve)
{
	auto unsigned int chan, value;
	auto float volt;

	chan = ((channel>>7) & 0x0007)*ADCHANNELS + (channel & 0x000F);
	
	value = anaIn(channel);
	volt = (_adcCalib[chan][0] * (_adcCalib[chan][1] - value));
	*rd = value;
	*ve = volt;
}

//------------------------------------------------------------------------
// Read analog input channels 0 thru 5
//------------------------------------------------------------------------
void ReadAI()
{
	auto unsigned int rawdata;
	auto int inputnum, slotnum, x, i;
	auto float voltequ;
	char str[122];

	slotnum = adcslot;
	glPrintf(0,0, &fi6x8, "ADC Slot %d chosen", slotnum);

	/////read coefficients from eeprom
	for (inputnum=0; inputnum<=10; inputnum++)
	{
		while (anaInEERd(ChanAddr(slotnum, inputnum)));
	}

	
	for (inputnum=0; inputnum<=5; inputnum++)
	{
		anaInInfo(ChanAddr(slotnum, inputnum), &rawdata, &voltequ);
		glPrintf(0, 16, &fi6x8, "CH%2d is %.5f V  ", inputnum, voltequ);
		sprintf(str, " raw data %d  ", rawdata);
		for (i=0; str[i]!='\0'; i++)
		{
			glHScroll(0, 24, 122, 16, -8);
			glPrintf (116, 24,  &fi6x8, "%c", str[i]);
			msDelay(30);
		}
		msDelay(800);
	}
	
	while (keyGet() == 0)
	{
   	keyProcess ();
	}
}

//------------------------------------------------------------------------
// Read digital input channels 0 thru 7
//------------------------------------------------------------------------
void ReadDI()
{
	auto char slotnum, ionum, i;
	auto int x, y;
	
	slotnum = dioslot;
	TextGotoXY(&textWindow,0,0);
	TextPrintf(&textWindow, "I/O Slot %d chosen", slotnum);
		
	///// read inputs
	TextGotoXY(&textWindow,0,2);
	TextPrintf(&textWindow, "Read channels 0 to 7");
	for (i=0; i<8; i++)
	{
		ionum = digIn(ChanAddr(slotnum, i));				//inputs individually
		TextGotoXY(&textWindow,i,3);
		TextPrintf(&textWindow, "%d", ionum);
	}

	while (keyGet() == 0)
	{
   	keyProcess ();
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
	brdInit();			// Initialize the controller
	keyInit();			// Start-up the keypad driver
	keypadDef();		// Use the default keypad ASCII return values

	
	glInit();			// Initialize the graphic driver
	glBackLight(1);	// Turn-on the backlight
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
			waitfor((option = display_menu(main_menu, initialize,sizeof(main_menu))) > 0);

			// Get menu option from the user
			switch(option)
			{
				// Assign card slots
				case 1:	glBlankScreen();
							AssignSlots();
							initialize = TRUE;
							break; 			

				// Read analog inputs			
				case 2:	glBlankScreen();
							ReadAI();
							initialize = TRUE;
							break; 			
		
				// Read digital inputs			
				case 3:	glBlankScreen();
							ReadDI();
							initialize = TRUE;
							break; 			
		
				// Change Date/Time
				case 4:	glBlankScreen();
							SetDateTime();						
							initialize = TRUE;
							break; 			

				// Display current Date/Time
				case 5:	glBlankScreen();
							waitfor(dispDate());
							initialize = TRUE;
							break;	

				// Turn Backlight OFF
				case 6:	glBackLight(0);
							initialize = FALSE;
							break;	

				// Turn Backlight ON
				case 7:	glBackLight(1);
							initialize = FALSE;
							break; 	

				// Enable Toggle leds option	
				case 8:	leds(TOGGLE);
							initialize = FALSE;
							break;	

				// Enable Increment leds option	
				case 9:	leds(INCREMENT);
							initialize = FALSE;
							break;	

				// Disable LED's
				case 10:	leds(LEDOFF);
							initialize = FALSE;
							break;	

				// User made invalid selection
				default:												
					break;
			}
		}
	}

}
