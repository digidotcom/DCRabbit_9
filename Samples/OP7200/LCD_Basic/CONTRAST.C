/***************************************************************************
	contrast.c

	Z-World, 2001
	Sample program to demonstrate how to adjust the contrast on the LCD
	display and to save/retrieve the contrast value in the flash userblock
	area for the display to power-up with the users contrast setting. 
		
	Instructions:
	1. Compile and run this program.
	2. Use the Scroll Right/Left keys to adjust the contrast.
	3. Use top right button to save contrast setting into the userblock.
	3. Use top left button to restore the default contrast setting.  

***************************************************************************/
#class auto
#memmap xmem  // Required to reduce root memory usage

// XMEM pointer for touchscreen button structures
unsigned long xmemArea;  

// Font structures
fontInfo	fi6x8, fi8x10, fi10x16; 

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

#define DEFAULT_CONTRAST		22
#define KEY_BEEP_PERIOD       2

#define NUM_BUTTONS           2

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
	buffer[1] = 0xaa;
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
		glBlock(cursor_location, 196, barGraph, 5);
		cursor_location += barGraph;
	}
	else if(pixel < cursor_location)
	{
		glSetBrushType(PIXWHITE);
		while(cursor_location-barGraph > pixel)
		{
			barGraph++;
		}
		glBlock(cursor_location-barGraph, 196, barGraph, 5);
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
		glPrintf(-2,   175, &fi10x16, "0");
		glPrintf(32,   175, &fi10x16, "4");
		glPrintf(74,   175, &fi10x16, "8");
		glPrintf(105,  175, &fi10x16, "12");
		glPrintf(145,  175, &fi10x16, "16");
		glPrintf(185,  175, &fi10x16, "20");
		glPrintf(225,  175, &fi10x16, "24");
		glPrintf(265,  175, &fi10x16, "28");
		glPrintf(300,  175, &fi10x16, "32");
	
		glPlotLine(    0, 192,     0,  194);
		glPlotLine( 40-1, 192,  40-1,  194);
		glPlotLine( 80-1, 192,  80-1,  194);
		glPlotLine(120-1, 192, 120-1,  194);
		glPlotLine(160-1, 192, 160-1,  194);
		glPlotLine(200-1, 192, 200-1,  194);
		glPlotLine(240-1, 192, 240-1,  194);
		glPlotLine(280-1, 192, 280-1,  194);
		glPlotLine(320-1, 192, 320-1,  194);
				
		glPlotPolygon(4,  0,195,  320-1, 195,  320-1,201,  0, 201);
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
int contrast_ts_cntrl( void )
{
	auto int btn;

	btn = 0;
	if((btn = btnGet(xmemArea)) >= 0)
	{
		switch(btn)
		{
			case 0:
				btn = 1; // BTNKEY0
				break;
						
			case 1:
				btn = 2; // BTNKEY1
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
int contrast_keypad_cntrl( void )
{
	static int state;
	static unsigned long debounce_period;
	auto int key;
	auto unsigned long buzzer_period;
	
	#GLOBAL_INIT{state = 0;}
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
			state = 0;
			if((key = keyGet()) > 0)
			{
				switch(key)
				{
					// Default Contrast key being pressed
					case CONTRAST_PRESS0:  
						btnRecall(xmemArea, 0);
						glSetBrushType(PIXXOR);
						glBlock(BtnData.xStart, BtnData.yStart, BtnData.xSize, BtnData.ySize);
						key = 0;
						break;

					// Save Contrast key being pressed 	
					case CONTRAST_PRESS1:	
						btnRecall(xmemArea, 1);
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
						
					default:   // All other keypresses are invalid for this application
						state = 0;
					  	key = 0;
					  	break;
				}
			}
			break;
			
		default:
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
	glSetBrushType(PIXBLACK);
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

void main()
{
	int selection, keydata;
	int xkey, ykey, i, wKey;
	int option_valid, level;

	brdInit();
	
	keyInit();		// Start-up the keypad driver
	keypadDef();	// Use the default keypad ASCII return values

	// Setup for FAST key repeat after holding down key for 50 ticks
	keyConfig ( 5,CONTRAST_LEFT ,0, 50, 1,  1, 1 );
	keyConfig ( 7,CONTRAST_RIGHT,0, 50, 1,  1, 1 );

	// Space and Backspace...setup to only respond to the keypress when its released
	keyConfig ( 0, CONTRAST_PRESS0, CONTRAST_REL0, 0, 0,  0, 0 );
	keyConfig ( 4, CONTRAST_PRESS1, CONTRAST_REL1, 0, 0,  0, 0 );

	glInit();			// Initialize the graphic driver
	glBackLight(1);
	glSetContrast(level = glGetContrast());
	
	glXPutBitmap(0, 0, 317,172,zwLogo317);
	glXFontInit(&fi8x10,  8,  10, 32, 127, Font8x10);
	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);		//	Initialize basic font


	// Allocate XMEM for creation of menu buttons
	xmemArea = btnInit(NUM_BUTTONS);

	// Define buttons to be used for program control
	btnCreateText(xmemArea, 0, 0,    210, 90, 30, 1, 0, &fi8x10, "DEFAULT\nCONTRAST");
	btnCreateText(xmemArea, 1, 230,  210, 90, 30, 1, 0, &fi8x10, "SAVE\nCONTRAST");	

	btnAttributes(xmemArea, 0, 0,   0,   0, 1);
	btnAttributes(xmemArea, 1, 0,   0,   0, 1);

	// Display menu buttons across the bottom of the screen
	btnDisplay(xmemArea,0);
	btnDisplay(xmemArea,1);
				
	dispContrastBar(level, 1);
	for(;;)
	{
		costate
		{
			waitfor((selection = contrast_user_selection()));
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
			}							
		}
	}
}
