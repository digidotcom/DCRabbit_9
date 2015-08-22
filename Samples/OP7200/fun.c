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
/***************************************************************************
	fun.c

 	Demo program for use with the OP72xx 'eDisplay' operator
	interface.  It is designed to demonstrate some of the
	features associated with the device such as graphics, keypad,
	digital and analog inputs, and the new menuing system
	functions.

	For demonstrating the analog input section, a variable
	power supply of 0-10vdc is recommended.
					
**************************************************************************/ 

#class auto
#memmap xmem

#use "kpcust16key.lib"

#ifdef TOUCHSCREEN
const static char btype[] = "OP7200 eDisplay";
#endif
#ifndef TOUCHSCREEN
const static char btype[] = "OP7210 eDisplay";
#endif            

//-----------------------------------------------
// Demo control options Macro's 
//-----------------------------------------------
// Press/Release Keypad codes for menu options  
#define MENUKEY0_PRESS	0x01
#define MENUKEY1_PRESS	0x02
#define MENUKEY2_PRESS	0x03
#define MENUKEY3_PRESS	0x04
#define MENUKEY4_PRESS	0x05
#define MENUKEY0_REL		0x11
#define MENUKEY1_REL		0x12
#define MENUKEY2_REL		0x13
#define MENUKEY3_REL		0x14
#define MENUKEY4_REL		0x15

// Menu selection macros 			
#define  DEMO_POPUPMENU		1
#define  DEMO_GRAPHICS     2
#define  DEMO_IO           3
#define  DEMO_SPEC         4
#define  DEMO_EXIT_MENU    5

//-----------------------------------------------
// Contrast control Macro's
//-----------------------------------------------	
// Contrast control codes for the keypad 
#define CONTRAST_PRESS0		0x01
#define CONTRAST_PRESS1		0x02
#define CONTRAST_REL0 		0x11
#define CONTRAST_REL1		0x12
#define CONTRAST_RIGHT		'R'
#define CONTRAST_LEFT		'L'
#define CONTRAST_EXIT      'E'

// Menu selection options
#define CONTRAST_DEFAULT	1
#define CONTRAST_SAVE      2
#define CONTRAST_LOWER   	3
#define CONTRAST_HIGHER 	4
#define CONTRAST_EXIT_MENU 5

// Misc contrast control macro's
#define DEFAULT_CONTRAST		22 // User defined
#define KEY_BEEP_PERIOD       2  // mSec's

//-----------------------------------------------
// Application Macro's
//-----------------------------------------------	
#define	CNT_TEXT		174
#define	CNT_BAR		191

// Misc Macros 
#define	INVERT		1
#define	NOINVERT		0

// Time/Date macros 
#define	DAY			0
#define	MONTH			1
#define	YEAR			2			
#define	HOUR			3
#define	MINUTE		4
#define	SECOND		5

// Max number of buttons to be used for top level menu control
#define MAX_MAIN_BUTTONS 	5


// Max number of buttons to be used for contrast menu control
#define MAX_CONTRAST_BUTTONS 	2


//------------------------------------------------------------------------
// WingDing Font info data
//------------------------------------------------------------------------
// Font   : Wingdings (22pt)
// Monochrome  : Black Foreground, White Background
// Mode   : Landscape
// Width  : 28 Pixels
// Height : 28 Pixels
// Range  : 0xF0..0xF8
// Init   : glXFontInit ( &fontdesc,28,28,0xF0,0xF8,Wingdings22 );

xdata Wingdings22 {
// Char = 0xF0
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x04','\x00','\x00',
'\x00','\x06','\x00','\x00',
'\x00','\x05','\x00','\x00',
'\x3F','\xFC','\x80','\x00',
'\x20','\x00','\x40','\x00',
'\x20','\x00','\x20','\x00',
'\x20','\x00','\x10','\x00',
'\x20','\x00','\x10','\x00',
'\x20','\x00','\x20','\x00',
'\x20','\x00','\x40','\x00',
'\x3F','\xFC','\x80','\x00',
'\x00','\x05','\x00','\x00',
'\x00','\x06','\x00','\x00',
'\x00','\x04','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF1
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x60','\x00','\x00',
'\x00','\x90','\x00','\x00',
'\x01','\x08','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x04','\x02','\x00','\x00',
'\x08','\x01','\x00','\x00',
'\x10','\x00','\x80','\x00',
'\x3E','\x07','\xC0','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x03','\xFC','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF2
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x01','\xFE','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x1F','\x03','\xE0','\x00',
'\x08','\x00','\x40','\x00',
'\x04','\x00','\x80','\x00',
'\x02','\x01','\x00','\x00',
'\x01','\x02','\x00','\x00',
'\x00','\x84','\x00','\x00',
'\x00','\x48','\x00','\x00',
'\x00','\x30','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF3
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x40','\x20','\x00',
'\x00','\xC0','\x30','\x00',
'\x01','\x40','\x28','\x00',
'\x02','\x40','\x24','\x00',
'\x04','\x7F','\xE2','\x00',
'\x08','\x00','\x01','\x00',
'\x10','\x00','\x00','\x80',
'\x20','\x00','\x00','\x40',
'\x20','\x00','\x00','\x40',
'\x10','\x00','\x00','\x80',
'\x08','\x00','\x01','\x00',
'\x04','\x7F','\xE2','\x00',
'\x02','\x40','\x24','\x00',
'\x01','\x40','\x28','\x00',
'\x00','\xC0','\x30','\x00',
'\x00','\x40','\x20','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF4
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x60','\x00','\x00',
'\x00','\x90','\x00','\x00',
'\x01','\x08','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x04','\x02','\x00','\x00',
'\x08','\x01','\x00','\x00',
'\x10','\x00','\x80','\x00',
'\x3E','\x07','\xC0','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x3E','\x07','\xC0','\x00',
'\x10','\x00','\x80','\x00',
'\x08','\x01','\x00','\x00',
'\x04','\x02','\x00','\x00',
'\x02','\x04','\x00','\x00',
'\x01','\x08','\x00','\x00',
'\x00','\x90','\x00','\x00',
'\x00','\x60','\x00','\x00',
// Char = 0xF5
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x3F','\xF0','\x00','\x00',
'\x20','\x20','\x00','\x00',
'\x20','\x40','\x00','\x00',
'\x20','\x20','\x00','\x00',
'\x20','\x10','\x00','\x00',
'\x20','\x08','\x00','\x00',
'\x20','\x04','\x00','\x00',
'\x28','\x02','\x00','\x00',
'\x34','\x01','\x00','\x00',
'\x22','\x00','\x80','\x00',
'\x01','\x00','\x40','\x00',
'\x00','\x80','\x80','\x00',
'\x00','\x41','\x00','\x00',
'\x00','\x22','\x00','\x00',
'\x00','\x14','\x00','\x00',
'\x00','\x08','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF6
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\xFF','\xC0','\x00',
'\x00','\x40','\x40','\x00',
'\x00','\x20','\x40','\x00',
'\x00','\x40','\x40','\x00',
'\x00','\x80','\x40','\x00',
'\x01','\x00','\x40','\x00',
'\x02','\x00','\x40','\x00',
'\x04','\x01','\x40','\x00',
'\x08','\x02','\xC0','\x00',
'\x10','\x04','\x40','\x00',
'\x20','\x08','\x00','\x00',
'\x10','\x10','\x00','\x00',
'\x08','\x20','\x00','\x00',
'\x04','\x40','\x00','\x00',
'\x02','\x80','\x00','\x00',
'\x01','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF7
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x18','\x00','\x00',
'\x00','\x26','\x00','\x00',
'\x00','\x41','\x00','\x00',
'\x00','\x80','\x80','\x00',
'\x31','\x00','\x80','\x00',
'\x3A','\x01','\x00','\x00',
'\x2C','\x06','\x00','\x00',
'\x20','\x08','\x00','\x00',
'\x20','\x10','\x00','\x00',
'\x20','\x20','\x00','\x00',
'\x20','\x40','\x00','\x00',
'\x20','\x60','\x00','\x00',
'\x20','\x20','\x00','\x00',
'\x3F','\xF0','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
// Char = 0xF8
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x03','\x80','\x00','\x00',
'\x04','\x40','\x00','\x00',
'\x08','\x20','\x00','\x00',
'\x10','\x10','\x00','\x00',
'\x10','\x08','\xC0','\x00',
'\x08','\x05','\xC0','\x00',
'\x04','\x03','\x40','\x00',
'\x03','\x00','\x40','\x00',
'\x00','\x80','\x40','\x00',
'\x00','\x40','\x40','\x00',
'\x00','\x20','\x40','\x00',
'\x00','\x60','\x40','\x00',
'\x00','\x40','\x40','\x00',
'\x00','\xFF','\xC0','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00',
'\x00','\x00','\x00','\x00'
};


// Font structures
fontInfo	fi6x8, fi8x10, fi10x16, fi12x16, fi17x35, fi14x16, fi10x12, fiWingDing; 

// Structures for the popup menus
windowMenu MenuStart, MenuBackLight;

// Time Structure
struct tm Time;				

/****** Constants for displaying the Date ********/
const char * const Days[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
const char * const Months[] = {"","JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};

// XMEM pointer for touchscreen button structures
unsigned long xmemContrastBtn, xmemMainBtn;  

//------------------------------------------------------------------------
// Below are the menu options that are used in the popup menu demo 
//------------------------------------------------------------------------

/**** System Menu *************/
const char * const startmenu [] =
{
	"Set Contrast",
	"Set Backlight",
	"Increase Size",
	"Decrease Size",
	"Set Time",
	"Set Date",
	"Exit Demo",
	""
};

/***** BackLight Menu *******/
const char * const backlight [] =
{
	"Backlight On",
	"Backlight Off",
	"System Menu",
	""
};

//------------------------------------------------------------------------
// Write the contrast setting to userblock area in flash for use
// on subsequent power cycles. 
//------------------------------------------------------------------------
void glSaveContrast( int level )
{
	auto char buffer[5];
	auto int checksum;
	auto int i;
	auto int *ptr;
	
	buffer[0] = 0x55;
	buffer[1] = 0xAA;
	buffer[2] = level;
	checksum = 0;
	for(i=0; i < 3; i++)
	{
		checksum += buffer[i];
	}
	ptr = (int*)(&buffer[3]);
	*ptr = checksum; 
	writeUserBlock(0, buffer, sizeof(buffer));
}


//------------------------------------------------------------------------
// Check if there's a valid contrast setting in Flash userblock
//------------------------------------------------------------------------
int glGetContrast( void )
{
	auto char buffer[5];
	auto int checksum;
	auto int i;
	auto int *ptr;
		
	readUserBlock(buffer, 0, sizeof(buffer));

	checksum = 0;	
	for(i=0; i < 3; i++)
	{
		checksum += buffer[i];
	}
	ptr = (int*) &buffer[3];
	if(checksum  == *ptr && buffer[0] == 0x55 && buffer[1] == 0xAA)
		return (buffer[2]);
	else
		return(DEFAULT_CONTRAST);
}

//------------------------------------------------------------------------
// Support function to update the contrast bar graph
//------------------------------------------------------------------------
void update_bar( float value, int initialize )
{
	static int barGraph, pixel;
	static int cursor_location;

	if(initialize)
	  cursor_location = 0;
	
	if(value >= 32)
		value = 31;

	if(value < 0)
		value = 0;

	pixel = (int) (value * 9.9375) + 1;
	if(pixel > 320-1) pixel = 320-1;			

	barGraph = 0;
	if(pixel > cursor_location)
	{
		glSetBrushType(PIXBLACK);
		while(cursor_location+barGraph < pixel)
		{
			barGraph++;
		}
		glBlock(cursor_location, CNT_BAR + 7, barGraph,  9);
		cursor_location += barGraph;
	}
	else if(pixel < cursor_location)
	{
		glSetBrushType(PIXWHITE);
		while(cursor_location-barGraph > pixel)
		{
			barGraph++;
		}
		glBlock(cursor_location-barGraph, CNT_BAR + 7 , barGraph,  9);
		cursor_location -= barGraph;
	}
	glSetBrushType(PIXBLACK);
}

//------------------------------------------------------------------------
// Function to display/update the contrast bar graph
//------------------------------------------------------------------------
void dispContrastBar(int contrast, int initialize)
{	
	auto int brushtype;
	
	// Save-off current brushtype 
	brushtype = glGetBrushType();
	glSetBrushType(PIXBLACK);
	if(initialize)
	{
		glBuffLock();
						
		glPrintf(-2,   CNT_TEXT, &fi10x16, "0");
		glPrintf(32,   CNT_TEXT, &fi10x16, "4");
		glPrintf(74,   CNT_TEXT, &fi10x16, "8");
		glPrintf(105,  CNT_TEXT, &fi10x16, "12");
		glPrintf(145,  CNT_TEXT, &fi10x16, "16");
		glPrintf(185,  CNT_TEXT, &fi10x16, "20");
		glPrintf(225,  CNT_TEXT, &fi10x16, "24");
		glPrintf(265,  CNT_TEXT ,&fi10x16, "28");
		glPrintf(300,  CNT_TEXT, &fi10x16, "32");
	
		glPlotLine(    0, CNT_BAR,     0,  CNT_BAR + 4 );
		glPlotLine( 40-1, CNT_BAR,  40-1,  CNT_BAR + 4);
		glPlotLine( 80-1, CNT_BAR,  80-1,  CNT_BAR + 4);
		glPlotLine(120-1, CNT_BAR, 120-1,  CNT_BAR + 4);
		glPlotLine(160-1, CNT_BAR, 160-1,  CNT_BAR + 4);
		glPlotLine(200-1, CNT_BAR, 200-1,  CNT_BAR + 4);
		glPlotLine(240-1, CNT_BAR, 240-1,  CNT_BAR + 4);
		glPlotLine(280-1, CNT_BAR, 280-1,  CNT_BAR + 4);
		glPlotLine(320-1, CNT_BAR, 320-1,  CNT_BAR + 4);
				
		glPlotPolygon(4, 0,CNT_BAR + 6,  320-1,CNT_BAR + 6,  320-1,CNT_BAR + 16,  0,CNT_BAR + 16);
		glBuffUnlock();
			
		update_bar(contrast, 1);
	}
	else
	{
		update_bar(contrast, 0);
	}
	// Restore original brush setting
	glSetBrushType(brushtype);
}

//------------------------------------------------------------------------
// Check for menu button selection from touchscreen
//------------------------------------------------------------------------
void CreateContrastButtons( void )
{	
	// Allocate XMEM for creation of menu buttons

	xmemContrastBtn = btnInit(MAX_CONTRAST_BUTTONS);

	// Define buttons to be used for program control
	btnCreateText(xmemContrastBtn, 0, 0,    210, 80, 30, 1, 0, &fi8x10, "DEFAULT\nCONTRAST");
	btnCreateText(xmemContrastBtn, 1, 240,  210, 80, 30, 1, 0, &fi8x10, "SAVE\nCONTRAST");	
	btnAttributes(xmemContrastBtn, 0, 0,   0,   0, 1);
	btnAttributes(xmemContrastBtn, 1, 0,   0,   0, 1);
}

//------------------------------------------------------------------------
// Check for menu button selection from touchscreen
//------------------------------------------------------------------------
int contrast_ts_cntrl( void )
{
	auto int btn;

	btn = 0;
	if((btn = btnGet(xmemContrastBtn)) >= 0)
	{
		switch(btn)
		{
			case 0:	// Contrast BTNKEY0 
				btn = CONTRAST_DEFAULT; 
				break;
						
			case 1:	// Contrast BTNKEY1
				btn = CONTRAST_SAVE; 
				break;
				
			default:
				btn = 0;	// No Option selected 
				break;
		}
	}
	return(btn);
}

//------------------------------------------------------------------------
// Check for menu button selection from keypad
//------------------------------------------------------------------------
int contrast_keypad_cntrl( void )
{
	static int state;
	static unsigned long debounce_period;
	auto unsigned long buzzer_period;
	auto int key;
	auto int brushtype;
	
	#GLOBAL_INIT{state = 0;}
	brushtype = glGetBrushType();
	key = 0;
	switch(state)
	{
		case 0:
	
			keyProcess();				// scan for key presses
			debounce_period = MS_TIMER + 25;
			state = 1;
			break;
					
		case 1:
			if((long) (MS_TIMER-debounce_period) >= 0 )
			{
				state = 2;
			}
			break;
		case 2:
			if((key = keyGet()) > 0)
			{
				switch(key)
				{
					// Default Contrast key being pressed
					case CONTRAST_PRESS0:  
						btnRecall(xmemContrastBtn, 0);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;

					// Save Contrast key being pressed 	
					case CONTRAST_PRESS1:	
						btnRecall(xmemContrastBtn, 1);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;

					// Set the contrast to its default value and write it to flash.
					case CONTRAST_REL0:      
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = CONTRAST_DEFAULT;
						break;

					// Save the current contrast value to flash, so it can be used on subsequent power cycles.
					case CONTRAST_REL1:
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = CONTRAST_SAVE;
						break;

					// Set Contrast higher 		
					case CONTRAST_LEFT:	
						key = CONTRAST_LOWER;
						break;

					// Set Contrast lower 
					case CONTRAST_RIGHT:  
						key = CONTRAST_HIGHER;
						break;

					case CONTRAST_EXIT:
						key = CONTRAST_EXIT_MENU;
						break;
		
					default:   // All other keypresses are invalid for this menu option
						state = 0;
			  			key = 0;
			  			break;
				}
			}
			state = 0;
			break;
		default:
			key = 0;
			state = 0;
			break;
	}
	if((KEY_BEEP_PERIOD && key == 1) || (KEY_BEEP_PERIOD && key == 2))
	{
		buzzer_period = MS_TIMER + KEY_BEEP_PERIOD-1;
		buzzer(1);
		while(!((long) (MS_TIMER-buzzer_period) >= 0 ));
		buzzer(0);
	}
	glSetBrushType(brushtype);
	return(key);
}

//------------------------------------------------------------------------
// Check if a screen key was pressed, the key press control will be the 
// the touchscreen if its installed, otherwise it will be the keypad.  
//------------------------------------------------------------------------
int contrast_user_selection( void )
{
	auto int BtnKey;

	// Check if touchscreen menu button is selected
	// Note: If the touchscreen option not installed, the function below will bascially
	//       do nothing because the touchscreen status line will never go active. Which 
	//       allows this program to run on OP7200 or OP7210 without any code changes.   
	if((BtnKey = contrast_ts_cntrl()) > 0)
	{
		return BtnKey;
	}
	// Check and return keypad status 
	return(contrast_keypad_cntrl());
}

//------------------------------------------------------------------------
// Check for menu button selection from touchscreen
//------------------------------------------------------------------------
int main_ts_cntrl( void )
{
	auto int btn;

	btn = 0;
	if((btn = btnGet(xmemMainBtn)) >= 0)
	{
		switch(btn)
		{
			case 0: 	// BTNKEY0
				btn = DEMO_POPUPMENU; 
				break;
						
			case 1:	// BTNKEY1
				btn = DEMO_GRAPHICS; 
				break;
				
			case 2:	// BTNKEY2
				btn = DEMO_IO; 
				break;
				
			case 3:	// BTNKEY3
				btn = DEMO_SPEC;	
				break;
			
			case 4:	// BTNKEY4
				btn = DEMO_EXIT_MENU; 
				break;
				
			default:
				btn = 0;
				break;
		}
	}
	return(btn);
}

//------------------------------------------------------------------------
// Check for menu button selection from keypad
//------------------------------------------------------------------------
int main_keypad_cntrl( void )
{
	static int state;
	static unsigned long debounce_period;
	auto unsigned long buzzer_period;
	auto int key;
	auto int brushtype;
	
	#GLOBAL_INIT{state = 0;}

	brushtype = glGetBrushType();
	key = 0;
	switch(state)
	{
		case 0:
			keyProcess();				// scan for key presses
			debounce_period = MS_TIMER + 25;
			state = 1;
			break;
					
		case 1:			
			if((long) (MS_TIMER-debounce_period) >= 0 )
			{
				state = 2;
			}
			break;
			
		case 2:
			if((key = keyGet()) > 0)
			{
				switch(key)
				{
					case MENUKEY0_PRESS:	// BTNKEY0 was Pressed
						btnRecall(xmemMainBtn, 0);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;
						
					case MENUKEY1_PRESS:	// BTNKEY1 was Pressed
						btnRecall(xmemMainBtn, 1);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;
						
					case MENUKEY2_PRESS: // BTNKEY2 was Pressed
						btnRecall(xmemMainBtn, 2);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;
						
					case MENUKEY3_PRESS:	// BTNKEY3 was Pressed
						btnRecall(xmemMainBtn, 3);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;
						
					case MENUKEY4_PRESS:	// BTNKEY4 was Pressed
						btnRecall(xmemMainBtn, 4);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;
						
					case MENUKEY0_REL: 	// BTNKEY0 was released
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = DEMO_POPUPMENU; 
						break;
						
					case MENUKEY1_REL: 	// BTNKEY1 was released
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = DEMO_GRAPHICS; 
						break;
						
					case MENUKEY2_REL:	// BTNKEY2 was released
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = DEMO_IO; 
						break;
						
					case MENUKEY3_REL:	// BTNKEY3 was released
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = DEMO_SPEC; 
						break;
						
					case MENUKEY4_REL:	// BTNKEY4 was released
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = DEMO_EXIT_MENU;	
						break;
							
					default:
						key = 0;
						break;
				}
			}
			state = 0;
			break;

		default:
			key = 0;
			state = 0;
			break;
	}
	if((KEY_BEEP_PERIOD && key == 1) || (KEY_BEEP_PERIOD && key == 2) ||
	   (KEY_BEEP_PERIOD && key == 3) || (KEY_BEEP_PERIOD && key == 4) ||
		(KEY_BEEP_PERIOD && key == 5))
	{
		buzzer_period = MS_TIMER + KEY_BEEP_PERIOD-1;
		buzzer(1);
		while(!((long) (MS_TIMER-buzzer_period) >= 0 ));
		buzzer(0);
	}
	glSetBrushType(brushtype);
	return(key);
}

//------------------------------------------------------------------------
// Check if a screen key was pressed, the key press control will be the 
// the touchscreen if its installed, otherwise it will be the keypad.  
//------------------------------------------------------------------------
int getBtnKey( void )
{
	auto int BtnKey;

	// Check if touchscreen menu button is selected 
	if((BtnKey = main_ts_cntrl()) > 0)
		return(BtnKey);

	// Check and return keypad status 
	return(main_keypad_cntrl());
}

//------------------------------------------------------------------------
// Create main Menu screen buttons to be activated by the touchscreen
// or keypad.
//------------------------------------------------------------------------
void CreateMenuButtons( void )
{
	// Initialize virtual 16 keypad for use by data/time demo functions
	glkeypadInit(0);
		
	// Allocate XMEM for creation of menu buttons
	xmemMainBtn = btnInit(MAX_MAIN_BUTTONS);

	// Create buttons to be displayed which can be used by the touchscreen and/or the
	// keypad. Note....Button 4 is being used to return back to the main menu from the
	// various demo functions.
	btnCreateText(xmemMainBtn, 0, (0 * 65),  210, 60, 30, 1, 0, &fi8x10, "MENU\nDEMO");
	btnCreateText(xmemMainBtn, 1, (1 * 65),  210, 60, 30, 1, 0, &fi8x10, "GRAPH\nDEMO");
	btnCreateText(xmemMainBtn, 2, (3 * 65),  210, 60, 30, 1, 0, &fi8x10, "INPUT\nDEMO");
	btnCreateText(xmemMainBtn, 3, (4 * 65),  210, 60, 30, 1, 0, &fi8x10, "SPECS\nSCRN");	
	btnCreateText(xmemMainBtn, 4, (4 * 65),  210, 60, 30, 1, 0, &fi8x10, "MAIN\nMENU");

	// The following initialization only applies when the touchscreen is being used.
	// Button attributes are audio when option selected and no auto repeat.
	btnAttributes(xmemMainBtn, 0, 0,   0,   0, 1);
	btnAttributes(xmemMainBtn, 1, 0,   0,   0, 1);
	btnAttributes(xmemMainBtn, 2, 0,   0,   0, 1);
	btnAttributes(xmemMainBtn, 3, 0,   0,   0, 1);
	btnAttributes(xmemMainBtn, 4, 0,   0,   0, 1);
}

//------------------------------------------------------------------------
// BacklightMenu displays a popmenu and controls the backlight
//------------------------------------------------------------------------
int BacklightMenu( void )
{
	static int state, done;
	auto int options;

	done = FALSE;
	state = MENU_INIT;
	while (!done)
	{
		costate
		{
			keyProcess();				// scan for key presses
			waitfor(DelayMs(20));	// key debounce period
		}		
		costate
		{
			// waitfor the enter key
			waitfor ( (options = glMenu(&MenuBackLight, &state, 30, 60)) != 0 ); 
			switch (options)
			{
				case 1:
					glBackLight(1);	// turn on the Backlight
					break;
					
				case 2:
					glBackLight(0);	// turn off the backlight
					break;
					
				/**** Exit this menu and return to the start menu ****/
				case 3:
					done = TRUE;
					break;
			}
		}
	}
	glBuffLock();
	glMenuClear(&MenuBackLight);	// Clear this menu
	glRefreshMenu(&MenuStart);		// Refresh the Start Menu
	glBuffUnlock();					
	return 1;
}

//------------------------------------------------------------------------
// SetContrastMenu displays a screen to control the contrast
//------------------------------------------------------------------------
int SetContrastScreen( void )
{
	static int done, wKey;
	static int CursorLocation;
	auto int selection;
	auto int level;

	// Get current contrast setting from the userblock area
	level = glGetContrast();
	
	// Initialize the following keys for 50 tick auto repeat
	keyConfig (5,'L',0, 50, 1,  1, 1);
	keyConfig (7,'R',0, 50, 1,  1, 1);

	// Space and Backspace...setup to only respond to the keypress when its released
	keyConfig (  0, CONTRAST_PRESS0, CONTRAST_REL0, 0, 0,  0, 0 );
	keyConfig (  4, CONTRAST_PRESS1, CONTRAST_REL1, 0, 0,  0, 0 );

	// Display menu buttons across the bottom of the screen
	btnDisplay(xmemContrastBtn,0);
	btnDisplay(xmemContrastBtn,1);
	
	done = FALSE;
	glBuffLock();
	glMenuClear(&MenuStart);
	CursorLocation = 0;
	dispContrastBar(level, 1);
	glPrintf(40,   60, &fi12x16, "PRESS -> TO INCREASE");
	glPrintf(40,   80, &fi12x16, "PRESS <- TO DECREASE");
	glPrintf(40,   100, &fi12x16, "PRESS ENTER TO EXIT");
	glPrintf(16,   CNT_TEXT - 25, &fi12x16, "CURRENT CONTRAST SETTING");
	glBuffUnlock();
	while (!done)
	{
		while(!(selection = contrast_user_selection()));
		switch(selection)
		{						
			case CONTRAST_LOWER:
				if(level >= 0)
					glSetContrast(--level);
				dispContrastBar(level, 0);
				break;
					
			case CONTRAST_HIGHER:
				if(level < 31)
					glSetContrast(++level);
				dispContrastBar(level, 0);
				break;

			case CONTRAST_SAVE:
				glSaveContrast(level); 
				break;

			case CONTRAST_DEFAULT:
				level = DEFAULT_CONTRAST;
				glSetContrast(level);
				dispContrastBar(level, 0);
				glSaveContrast(level); 
				break;
					
			case CONTRAST_EXIT_MENU:
				done = TRUE;
				break;
		}							
	}

	// Set keys (all) back to default setting
	keypadDef();
	return 1;
}		
		
//------------------------------------------------------------------------
// This will allow the user to choose what aspect of time to set hours,
// minutes or seconds. upon selection it will call the 16 key virtual
// keypad for the values to entered.
//------------------------------------------------------------------------
int SetTimeMenu( void )
{
	static int done;
	static unsigned int Hour, Minute, Second;
	static char RetStr[9],*p1, *e1;
	static unsigned long time;
	
	done = FALSE;
	while (!done)
	{
		costate
		{
			keyProcess();				// scan for key presses
			waitfor(DelayMs(20));	// key debounce period
		}
		costate
		{
			time = read_rtc();			// Get current time
			mktm (&Time,time);			// convert long time to structure tm time
			sprintf(RetStr,"%02d-%02d-%02d",Time.tm_hour,Time.tm_min,Time.tm_sec);
			waitfor (glkGetString(RetStr,8,&Glk14x16,&Glk10x12,"ENTER NEW TIME\n(HH-MM-SS)"));

			// Check if a new time was entered....could do range and syntax checking at this point
			if(strlen(RetStr) > 0)
			{
				p1 = RetStr;
				Hour = (unsigned int)strtod(p1,&e1);
				p1 = e1 + 1;
				Minute = (unsigned int)strtod(p1,&e1);
				p1 = e1 + 1;
				Second = (unsigned int)strtod(p1,&e1);
				Time.tm_hour = Hour;
				Time.tm_min  = Minute;
				Time.tm_sec  = Second;
				tm_wr(&Time);
			}
			done = TRUE;
		}
	}
	return 1;
}

//------------------------------------------------------------------------
// SetDateMenu will allow the user to set the proper Date
//------------------------------------------------------------------------
int SetDateMenu()
{
	static int state,done;
	static unsigned int Day, Month, Year;
	static char RetStr[12],*p1, *e1;
	static unsigned long time;

	done = FALSE;
	while (!done)
	{
		costate
		{
			keyProcess();				// scan for key presses
			waitfor(DelayMs(20));	// key debounce period
		}
		costate
		{
			time = read_rtc();			// Get current time
			mktm (&Time,time);			// convert long time to structure tm time
			sprintf(RetStr,"%02d-%02d-%04d",Time.tm_mday,Time.tm_mon,(Time.tm_year + 1900));
			waitfor (glkGetString(RetStr,10,&Glk14x16,&Glk10x12,"ENTER NEW DATE\n(DD-MM-YYYY)"));

			// Check if a new date was entered....could do range and syntax checking at this point
			if(strlen(RetStr) > 0)
			{
				p1 = RetStr;
				Day = (unsigned int)strtod(p1,&e1);
				p1 = e1 + 1;
				Month = (unsigned int)strtod(p1,&e1);
				p1 = e1 + 1;
				Year = (unsigned int)strtod(p1,&e1);
				Time.tm_mday = Day;
				Time.tm_mon  = Month;
				Time.tm_year = Year - 1900;
				tm_wr(&Time);
			}
			done = TRUE;
		}
	}
	return 1;
}

//------------------------------------------------------------------------
// PopupMenu demonstates the usage of the popup menu system
//------------------------------------------------------------------------
int PopupMenuDemo( void )
{
	static int state, done;
	static unsigned long time;
	static char Sec;
	static int popupInit;
	auto int options;
	auto char *ptr;

	// Initial variable during premain execution period
	#GLOBAL_INIT{popupInit=FALSE;} 	
	if(!popupInit)
	{
		// Initializes the popup Menu's used in this demo 
		glMenuInit(&MenuStart, &fi10x16, DOUBLE_LINE, SHADOW, startmenu, " SYSTEM MENU ", -1);
		glMenuInit(&MenuBackLight, &fi10x16, DOUBLE_LINE, SHADOW, backlight, " BACKLIGHT MENU ", -1);
		popupInit = TRUE;
	}

	// Set keypad to the default setting
	keypadDef();
	
	// Description of MACRO's to control how the Menu is displayed: 
	// 0 = MENU_INIT..........Initialize and Display Menu. 
	// 1 = MENU_NO_CHANGE.....Return to selected option, no
	//                        change's to menu or highlightbar.	                                    	 		
   // 2 = MENU_REFRESH.......Display the last image of the menu,
   //                        including location of the highlight
   //                        bar.
	state = MENU_INIT;				

	// Display the screen title ****/
	ptr = "POPUP MENU DEMO";
	glBuffLock();						// Lock Display
	glBlankScreen();					// Blank Screen
	btnClear(xmemMainBtn,0);
	btnClear(xmemMainBtn,1);
	btnClear(xmemMainBtn,2);
	btnClear(xmemMainBtn,3);
	glPrintf(160  -( (strlen(ptr) * 17) / 2),0,&fi17x35,ptr);
	glBuffUnlock();					// Unlock display

	done = FALSE;
	while (!done)
	{
		
		costate
		{
			keyProcess();				// scan for key presses
			waitfor(DelayMs(20));	// key debounce period
		}		
		costate
		{
			time = read_rtc();			// Get current time
			mktm (&Time,time);			// convert long time to structure tm time
			if (Sec != Time.tm_sec)		// check for time update
			{
				/**** Display the time/date on the display ****/
				glPrintf(30,210,&fi10x16,"%s %02d/%s/%04d   %02d:%02d:%02d",
							Days[Time.tm_wday], Time.tm_mday, Months[Time.tm_mon],
							Time.tm_year + 1900, Time.tm_hour,Time.tm_min,Time.tm_sec);
			}
			waitfor (DelayMs(500));
		}
		costate
		{
			// Wait for highlighted option to be selected with the ENTER key   
			waitfor (( options = glMenu(&MenuStart, &state, 0 , 30) )!= 0); //

			//  Each case corresponds to the menu option that was selected 
			switch (options)
			{
				// This calls the contrast control screen	
				case 1:
					glBuffLock();						// Lock Display
					glBlankScreen();
					glPrintf(160  -( (strlen(ptr) * 17) / 2),0,&fi17x35, ptr);
					glBuffUnlock();
					waitfor (SetContrastScreen());
					glBuffLock();						// Lock Display
					glBlankScreen();					// Blank Screen
					glPrintf(160  -( (strlen(ptr) * 17) / 2),0,&fi17x35, ptr);
					state = MENU_INIT;
					glBuffUnlock();
					break;

				// This calls the BackLight menu 
				case 2:
					waitfor (BacklightMenu());
					state = MENU_REFRESH;
					break;	

				// This will increases the font size from 10x16 to 12x16 
				case 3: 
					glMenuClear (&MenuStart);
					glMenuInit(&MenuStart, &fi12x16, DOUBLE_LINE, SHADOW, startmenu, " START MENU ", -1);
					glMenuInit(&MenuBackLight, &fi12x16, DOUBLE_LINE, SHADOW, backlight, " BACKLIGHT MENU ", -1);
					state = MENU_INIT;
					break;

				// This will decrease the font size from 12x16 to 10x16
				case 4:
					glMenuClear (&MenuStart);
					glMenuInit(&MenuStart, &fi10x16, DOUBLE_LINE, SHADOW, startmenu, " START MENU ", -1);
					glMenuInit(&MenuBackLight, &fi10x16, DOUBLE_LINE, SHADOW, backlight, " BACKLIGHT MENU ", -1);
					state = MENU_INIT;
					break;
				
				// This Calls the SetTime Menu
				case 5:
					waitfor (SetTimeMenu());
					state = MENU_REFRESH;
					break;
					
				// This calls the SetDate Menu
				case 6:
					waitfor (SetDateMenu());
					state = MENU_REFRESH;
					break;
					
				// This will turn off the menu and return to the Main Splash Screen 
				case 7:
					done = TRUE;
					break;
			}
		}
	}
	return 1;
}

//------------------------------------------------------------------------
// GraphicsDemo displays Bitmaps, Circles, Squares, and fonts, continually
// updating, until the screen key MAIN MENU is pressed.
//------------------------------------------------------------------------
int GraphicsDemo( void )
{
	auto int HotKey;
	static int done,x,y,z,j,i;
	static char DispStr[20];
	static CoData	BitMaps, KeyScan;
	static const char* const Text[] = {	"DISPLAY TEXT WITH VARIABLE FONTS",
											"6 X 8 FONT",
											"8 X 10 FONT",
											"10 X 16 FONT",
											"12 X 16 FONT",
											"17 X 35 FONT",
											"( WINGDINGS )"};

	done = FALSE;
	keyConfig ( 4, MENUKEY4_PRESS, MENUKEY4_REL, 0, 0,  0, 0 );
	glBuffLock();
	glBlankScreen();
	btnClear(xmemMainBtn,0);
	btnClear(xmemMainBtn,1);
	btnClear(xmemMainBtn,2);
	btnClear(xmemMainBtn,3);
	btnDisplay(xmemMainBtn,4);
	glBuffUnlock();

	CoBegin(&KeyScan);
	CoBegin(&BitMaps);
	while (!done)
	{
		TsScanState();				// scan touchscreen for keypress
		costate KeyScan
		{
			keyProcess();				// scan for key presses	
			waitfor(DelayMs(20));	// key debounce period
			CoBegin(&KeyScan);      // do Keyscan costate again
		}		
		costate BitMaps 
		{
			glSetBrushType(PIXWHITE);
			glBlock(0,0,320,209);
			glSetBrushType(PIXBLACK);
			waitfor(DelayMs(2));
			for (z  = 0 ; z < 2 ; z++)
			{
				glSetBrushType(PIXBLACK);
				glPrintf(25,230,&fi8x10,"DISPLAYING BITMAPS");
				for (x  = 0 ; x < 3 ; x++)
				{
					for (y = 0 ; y < 3 ; y++)
					{
						glXPutBitmap(y * 107,x * 68,106,58,zwLogo106);
						waitfor(DelayMs(2));
					}
					waitfor(DelayMs(10));
				}
				glPrintf(25,230,&fi8x10,"INVERTING BITMAPS ");
				glSetBrushType(PIXXOR);
				for (x  = 0 ; x < 3 ; x++)
				{
					for (y = 0 ; y < 3 ; y++)
					{
						glBlock(y * 107,x * 68,106,58);
						waitfor(DelayMs(2));
					}
					waitfor(DelayMs(2));
				}
				waitfor(DelayMs(10));
			}
			glSetBrushType(PIXBLACK);
			glPrintf(25,230,&fi8x10,"DISPLAYING CIRCLES");
			for (z = 0 ;z < 1 ; z++)
			{
				glSetBrushType(PIXWHITE);
				glBlock(0,0,320,209);
				glSetBrushType(PIXBLACK);
				for (x = 0 ; x < 44 ; x++)
				{
					glPlotCircle(160,120,x*2);
					waitfor(DelayMs(2));
				}
				waitfor(DelayMs(1000));
			}
			glSetBrushType(PIXBLACK);
			glPrintf(25,230,&fi8x10,"DISPLAYING SQUARES");
			for (z = 0 ;z < 1 ; z++)
			{
				glSetBrushType(PIXWHITE);
				glBlock(0,0,320,209);
				glSetBrushType(PIXBLACK);
				for (x = 0 ; x < 40 ; x++)
				{
					glBuffLock();
					glPlotLine(150-(x*2),120-(x*2),170 + (x*2),120 - (x*2));
					glPlotLine(150-(x*2),130+(x*2),170 + (x*2),130 + (x*2));
					glPlotLine(150-(x*2),120-(x*2),150 - (x*2),130 + (x*2));
					glPlotLine(170+(x*2),120-(x*2),170 + (x*2),130 + (x*2));
					glBuffUnlock();	
					waitfor(DelayMs(2));
				}
				waitfor(DelayMs(1000));
			}
			glSetBrushType(PIXWHITE);
			glBlock(0,0,320,209);
			glSetBrushType(PIXBLACK);
			glBuffLock();
			glPrintf(160  -( (strlen(Text[0]) * 10) / 2),30,&fi10x16,Text[0]);
			glPrintf(160  -( (strlen(Text[1]) * 6) / 2),60,&fi6x8,Text[1]);
			glPrintf(160  -( (strlen(Text[2]) * 8) / 2),75,&fi8x10,Text[2]);
			glPrintf(160  -( (strlen(Text[3]) * 10) / 2),90,&fi10x16,Text[3]);
			glPrintf(160  -( (strlen(Text[4]) * 12) / 2),110,&fi12x16,Text[4]);
			glPrintf(160  -( (strlen(Text[5]) * 17) / 2),130,&fi17x35,Text[5]);
			glPrintf(25,230,&fi8x10,"DISPLAYING TEXT   ");
			waitfor(DelayMs(2));
			j = 0;										
			for (i = 0xf0 ; i<0xf9 ; i++)
			{
				DispStr[j] = i;
				j++;	
			}
			waitfor(DelayMs(2));
			DispStr[j] = '\0';
			glPrintf(140  -( (strlen(DispStr) * 22) / 2),160,&fiWingDing,DispStr);
			glPrintf(160  -( (strlen(Text[6]) * 12) / 2),193,&fi12x16,Text[6]);
			glBuffUnlock();
			waitfor(DelayMs(4500) );
			CoBegin(&BitMaps); // do Bitmaps costate again
		}

		// Should be the last costate so that when done flag is set, the loop gets
		// exited with no additional code (within the above costates) being executed.             
		costate
		{
			waitfor ( getBtnKey() == DEMO_EXIT_MENU);
			// Return keys back to the default setting
			keypadDef();
			
			// Clear btn from LCD and active button list
			btnClear(xmemMainBtn,4);

			// Reset costates for proper exit
			CoReset(&BitMaps);	
			CoReset(&KeyScan);   
			done = TRUE;
		}		
	}
	glSetBrushType(PIXBLACK);
	return 1;
}

//------------------------------------------------------------------------
// IODemo displays the first digital inputs (DIN0 -DIN5), and the
// first analog inputs (AIN0 - AIN5).
//
// The digital inputs will be filled circles when logic 1, or
// empty circles when logic 0.
//
// The analog inputs will display a bar graph between 0-10vdc, and
// the actual float vdc value xx.x above the graph.
//
// It will continually update 1 every 500 ms. and finish when the
// MAIN MENU screen key is pressed.
//------------------------------------------------------------------------
int IODemo( void )
{
	static int done,loop;
	static unsigned int Dinval[8];
	static float Ainval[8];
	auto int HotKey;
	auto char *ptr;

	ptr = "IO DEMO";
	keyConfig ( 4, MENUKEY4_PRESS, MENUKEY4_REL, 0, 0,  0, 0 );
	done = FALSE;
	
	glBuffLock();
	glBlankScreen();
	btnClear(xmemMainBtn,0);
	btnClear(xmemMainBtn,1);
	btnClear(xmemMainBtn,2);
	btnClear(xmemMainBtn,3);	
	glPrintf(160  -( (strlen(ptr) * 17) / 2),0,&fi17x35,ptr);
	btnDisplay(xmemMainBtn,4);
	
	// This displays the input headings 
	glPrintf(10 , 40, &fi8x10,  "DIN0 DIN1 DIN2 DIN3 DIN4 DIN5");
	glPrintf(10 , 50, &fi8x10,  "-----------------------------");
	
	#ifdef ANALOG		
	glPrintf(10 , 100, &fi8x10,  "AIN0 AIN1 AIN2 AIN3 AIN4 AIN5");
	glPrintf(10 , 110, &fi8x10,  "-----------------------------");
	#endif
	glBuffUnlock();

	while (!done)
	{
		
		costate
		{
			waitfor (IntervalMs(500));				// the scan time of the inputs
			for (loop = 0  ; loop < 6 ; loop++) // loop for getting the 6 input values
			{
				Dinval[loop] = digIn(loop);
				Ainval[loop] = anaInVolts(loop,1);
			}
			glBuffLock();

			// This section will clear out the old data
			glSetBrushType(PIXWHITE);		
			glBlock(0,55,260,40);
			glBlock(0,135,260,240);
			glSetBrushType(PIXBLACK);

			// This section will display the new data
			for (loop = 0 ;  loop < 6 ; loop++)
			{
				// This checks the logic level, this displays the appropriate filled, empty circle 
				if (Dinval[loop])
					glFillCircle(25 + (loop * 40) ,75,15);
				else
					glPlotCircle(25 + (loop * 40) ,75,15);
					
				#ifdef ANALOG					
					// This display the initial analog bar 
					glPrintf(10 + (loop * 40), 120, &fi8x10, "%04.01f",Ainval[loop]);
					glPlotLine(10 + (loop * 40),239,10 + (loop * 40),139);
					glPlotLine(40+ (loop * 40),239,40+ (loop * 40),139);
					glPlotLine(10+ (loop * 40),239,40+ (loop * 40),239);
					glPlotLine(10+ (loop * 40),139,40+ (loop * 40),139);
				
					// This is so that the bar does not go beyond the 10vdc limit, or below the 0vdc limit 
					if (Ainval[loop] > 10.0) Ainval[loop] = 10.0;
					if (Ainval[loop] < 0.0) Ainval[loop] = 0.0;
					// This fills in the section of the bar graph depending on the vdc value 
					glBlock(10 + (loop * 40), 240 - (int)(Ainval[loop] * 10.0), 30, (int)(Ainval[loop] * 10.0));
				#endif			
			}
			glBuffUnlock();
		}		
		
		// This costate waits for the MAIN MENU button to be pressed to exit this demo
		costate
		{
			waitfor ( (HotKey = getBtnKey()) == DEMO_EXIT_MENU);
			done = TRUE;
			// Return keys back to the default setting
			keypadDef();

			// Clear btn from LCD and active button list
			btnClear(xmemMainBtn,4);
		}
	}
	return 1;
}

//------------------------------------------------------------------------
// SpecDemo displays information about the Controller like crystal speed,
// flash size, sram size, mac address, etc.
//
// Can exit this demo by pressing the MAIN MENU screen key.
//------------------------------------------------------------------------
int SpecDemo()
{
	static int done;
	auto int HotKey;
	auto char *ptr;
	
	ptr = "SPECIFICATIONS";
	
	// Map the physical key to return the required menu button number 
	keyConfig ( 4, MENUKEY4_PRESS, MENUKEY4_REL, 0, 0,  0, 0 );
	done = FALSE;
	glBuffLock();
	glBlankScreen();
	btnClear(xmemMainBtn,0);
	btnClear(xmemMainBtn,1);
	btnClear(xmemMainBtn,2);
	btnClear(xmemMainBtn,3);
	glkMsgBox(4,40,312,160,&fi8x10,"",1,0);	
	
	glPrintf(160  -( (strlen(ptr) * 17) / 2),0,&fi17x35,ptr);
	btnDisplay(xmemMainBtn,4);
	glPrintf(10,50,&fi10x12, "BOARD :   %s",btype);
	glPrintf(10,65,&fi10x12, "STAMP :   %02d/%02d/%02d%02d  %02d:%02d",
	         SysIDBlock.timestamp[2], SysIDBlock.timestamp[3],
				SysIDBlock.timestamp[0], SysIDBlock.timestamp[1],
				SysIDBlock.timestamp[4], SysIDBlock.timestamp[5]);
	glPrintf(10,80,&fi10x12, "DC VER:   %x",CC_VER);
	glPrintf(10,115,&fi10x12,"CPU   :   RABBIT 2000");
	glPrintf(10,130,&fi10x12,"SPEED :   %7.4f MHZ",(SysIDBlock.crystalFreq/1.0e6)*2.0);
	glPrintf(10,145,&fi10x12,"FLASH :   %ld Bytes",SysIDBlock.flashSize * 4096ul);
	glPrintf(10,160,&fi10x12,"SRAM  :   %ld Bytes",SysIDBlock.ramSize * 4096ul);
	glPrintf(10,175,&fi10x12,"MAC   :   %02X:%02X:%02X:%02X:%02X:%02X",
				SysIDBlock.macAddr[0], SysIDBlock.macAddr[1],
        		SysIDBlock.macAddr[2], SysIDBlock.macAddr[3],
           	SysIDBlock.macAddr[4], SysIDBlock.macAddr[5]);
	glBuffUnlock();

	while (!done)
	{
		costate
		{
			waitfor ( (HotKey = getBtnKey()) == DEMO_EXIT_MENU);
			done = TRUE;
			// Return keys back to the default setting
			keypadDef();

			// Clear btn from LCD and active button list
			btnClear(xmemMainBtn,4);
		}
	}
	return 1;
	
}

//------------------------------------------------------------------------
// Function to display initial menu
//------------------------------------------------------------------------
int InitialMenu( void )
{
	const static char title[] = "Operator Interface/HMI";
	static int Key;
	
	costate
	{	
		// Map the physical key to return the required menu button number
		keyConfig (  0, MENUKEY0_PRESS, MENUKEY0_REL, 0, 0,  0, 0 );  	// Menu Button 0
		keyConfig (  1, MENUKEY1_PRESS, MENUKEY1_REL, 0, 0,  0, 0 );	// Menu Button 1
		keyConfig (  3, MENUKEY2_PRESS, MENUKEY2_REL, 0, 0,  0, 0 );	// Menu Button 2
		keyConfig (  4, MENUKEY3_PRESS, MENUKEY3_REL, 0, 0,  0, 0 );	// Menu Button 3
	
		// Lock display buffer and clear the display 
		glBuffLock();					
		glBlankScreen();
	
		// Display the zworld bitmap
		glXPutBitmap(170 - (220 / 2),5,220,120,zwLogo220 );	

		// Display controller type and demo title
		glPrintf(160  -( (strlen(btype) * 17) / 2),135,&fi17x35,btype); 
		glPrintf(160  -( (strlen(title) * 12) / 2),175,&fi12x16,title);	 	 

		// Display menu buttons across the bottom of the screen
		btnDisplay(xmemMainBtn,0);
		btnDisplay(xmemMainBtn,1);
		btnDisplay(xmemMainBtn,2);
		btnDisplay(xmemMainBtn,3);

		// Unlock the display buffer and display the screen 
		glBuffUnlock();
		waitfor ((Key = getBtnKey()) > 0);
		
		// Return keys back to the default setting
		keypadDef();
	}
	return(Key);
}

//------------------------------------------------------------------------
// Main...shows overall control for the sample program    
//------------------------------------------------------------------------
void main ()
{
	auto int Key;
	
	brdInit();					// Initialize the controller
	glInit();					// Initialize LCD interface 

	glBackLight(1);			// Turn on the Backlight
	glSetContrast(glGetContrast());		// Set the contrast

	keyInit();					// Initialize keypad, use default key setting 
	keypadDef();
	
	// Initializes fonts used in this demo 
	glXFontInit(&fi6x8,   6,  8,  32, 255, Font6x8);
	glXFontInit(&fi8x10,  8,  10, 32, 127, Font8x10);
	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);
	glXFontInit(&fi12x16, 12, 16, 32, 127, Font12x16);
	glXFontInit(&fi17x35, 17, 35, 32, 127, Font17x35);
	glXFontInit (&fiWingDing,28,28,0xF0,0xF8,Wingdings22 );
	glXFontInit ( &fi14x16,14,16,0x20,0x7E,Terminal12 );
	glXFontInit ( &fi10x12,10,12,0x20,0x7E,Terminal9 );

	// Main Menu Buttons...create an array of button descriptors in XMEM. 
	CreateMenuButtons();

	// Contrast Control Buttons....create an array of button descriptors in XMEM.  
	CreateContrastButtons();
			
	for (;;)						// Loop forever
	{
		costate
		{
			// Display main menu and wait for a menu button to be pressed.
			// (menu button options are across bottom of LCD screen)
			waitfor (( Key = InitialMenu()) > 0); 
			switch(Key)
			{
				case DEMO_POPUPMENU:
					waitfor (PopupMenuDemo());
					break;

				case DEMO_GRAPHICS:
					waitfor (GraphicsDemo());	
					break;

				case DEMO_IO:
					waitfor (IODemo());
					break;
					
				case DEMO_SPEC:
					waitfor (SpecDemo());
					break;
					
				default:
					break;
			}				
		}	
	}
}					


