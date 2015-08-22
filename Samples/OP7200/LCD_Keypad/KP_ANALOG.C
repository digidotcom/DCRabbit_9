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
/**************************************************************************
	kp_analog.c

	DESCRIPTION
	===========
	Sample program for use with the OP7200 controller (analog Input version).
	Demonstrates the multiple range capable analog inputs, as well as
	some bar graphing techniques.


	EQUIPMENT NEEDED
	================
	A variable power supply ranging from 0 to 20VDC.

	WIRING CONNECTIONS
	==================
	1.	Connect the ground of the variable power supply to the AGND terminal
		of the OP7200.
	2.	Connect the positive lead of the power supply to the AINx input that
		you wish to view.  All 8 inputs are visible in the sample, and can
		be disabled individually if needed.
	
	
	INSTRUCTIONS
	============
	1.	Follow the WIRING CONNECTIONS instructions above.
	2. Run the Sample program.
	3.	Press the key just below the label 'CONFIG DEMO'.  A Menu will appear
		with the choices.

		1. System Setup.  (Backlight, and contrast)
		2. Analog Setup.	(For Analog channel setup)
		3. Exit Menu.	   (To return to the bargraphing demo).

	4.	Under Analog Setup:
		1.	Choose the Analog Channel you wish to disable or change range.
			1. ANALOG CHANNEL 0
			2. ANALOG CHANNEL 1
			3. ANALOG CHANNEL 2
			4. ANALOG CHANNEL 3
			5. ANALOG CHANNEL 4

			You may press the '-' key( see keypadDef()) to get to the last
			options;
			6. ANALOG CHANNEL 5
			7. ANALOG CHANNEL 6
			8. ANALOG CHANNEL 7
			9. RETURN TO START.
			option.
		2. After choosing a channel, you will see some text in the lower
			left hand corner describing the current setting of the channel.
			(for example if you choose channel 0, you will see),
			
			INPUT		->		0
			STATUS	->		ENABLED
			MODE		->		0 to 10VDC
			
		3.	Select what option you wish to change from the menu.
			1. ENABLE DISPLAY
			2. DISABLE DISPLAY
			3. SET RANGE.
			4. RETURN TO ANALOG SETUP.
			
		4.	If you choose SET RANGE.
			a menu of range choices will appear.
			1.	0 to 20VDC
			2. 0 to 10VDC
			3. 0 to 5VDC
			4. 0 to 4VDC
			5  0 to 2.5VDC
			You may press the '-' key( see keypadDef()) to get to the
			last choices,
			6. 0 to 2VDC
			7. 0 to 1.25VDC
			8. 0 to 1VDC
		5.
			Choose the Range you want.  Once choosen, the Information on the
			channel will display the new MODE selected.

		6. Repeat steps 2 - 5 for all channels.

		7. Once done go back to the START MENU and choose 3. EXIT MENU.
		8. The bargraph demo will start back up using the settings you supplied.
		
**************************************************************************/

#memmap xmem
#use "kpcust16key.lib"


#define	BARYCOORD	100
#define	BARHEIGHT	100

#define	HOTKEY0		0 + 0x30
#define	HOTKEY1		1 + 0x30
#define	HOTKEY2		2 + 0x30
#define	HOTKEY3		3 + 0x30

typedef struct
{
	byte  In;									// input number
	byte	GainMode;							// current gain mode
	byte	Enable;								// Disabled or Enabled value
	float	Range;								
	float anaVal;								// Current Analog VDC value
	float Res;								
	int	xCoord,yCoord;						// Starting xy coord for the graph
	int	Height;								// Height of the graph
} _AnaSetting;


_AnaSetting AnaSetting[8];					// analog Input structure

//	These const chars setup up the Display options in each menu screen.

const char *statusMsg[] =
{
	"DISABLED",
	"ENABLED ",
};

const char *modeMsg[] =
{
	
	"0 to 20vdc.  ",
	"0 to 10vdc.  ",
	"0 to 5vdc.   ",
	"0 to 4vdc.   ",
	"0 to 2.5vdc. ",
	"0 to 2vdc.   ",
	"0 to 1.25vdc ",
	"0 to 1vdc.   ",
};

fontInfo fi10x16, fi6x8, fi8x10, fi17x35;

char LastSec;
char LastContrast;
int	anaWidth,anaHeight,anaOffset,anatotal;

const char *startMenu[] =
{
	" 1. System Setup. ",
	" 2. Analog Setup. ",
	" 3. Exit Menu.    ",
	""
};

const char *systemSetup[] =
{
	" 1. Turn On Backlight. ",
	" 2. Turn Off Backlight. ",
	" 3. Adjust Contrast.  ",
	" 4. Return To Start. ",
	""

}; 

const char *adjContrast[] =
{
	" 1. Increase Contrast. ",
	" 2. Decrease Contrast. ",
	" 3. Return to System Setup. ",
	""
};

const char *analogSetup[] =
{
	" 1. Analog Channel 0. ",
	" 2. Analog Channel 1. ",
	" 3. Analog Channel 2. ",
	" 4. Analog Channel 3. ",
	" 5. Analog Channel 4. ",
	" 6. Analog Channel 5. ",
	" 7. Analog Channel 6. ",
	" 8. Analog Channel 7. ",
	" 9. Return To Start. ",
	""
};

const char *analogSet2[] =
{
	" 1. Enable Display. ",
	" 2. Disable Display. ",
	" 3. Set Range. ",
	" 4. Return To Analog Setup ",
	""
};

const char *rangeSelect[] =
{
	" 1. 0 to 20vdc. ",
	" 2. 0 to 10vdc. ",
	" 3. 0 to 5vdc. ",
	" 4. 0 to 4vdc. ",
	" 5. 0 to 2.5vdc. ",
	" 6. 0 to 2vdc. ",
	" 7. 0 to 1.25vdc ",
	" 8. 0 to 1vdc. ",
	""
};

// These are needed for the Menu system
windowFrame textWindow;
windowMenu MenuStart, MenuSysSetup, MenuAnaSetup, MenuAdjContrast,
					MenuRangeSel, MenuAnaSet2;

int		bgMenuStart (void);					// Start Menu Control
int		bgMenuSys   (void);					// System Setup control
int		bgMenuAna1  (void);					// Analog Setup Control
int		bgMenuAdjC  (void);					// Contrast Adjustment control
int		bgMenuRSel  ( _AnaSetting *Input ); // Range Select Control
int		bgMenuAna2  ( _AnaSetting *Input ); // Analog Setup2 control


	// The Main Screen 
int		bgMainScreen(void);

	// Sets up the Bargraphs for the Inputs
int		bgBarSet (fontInfo *pInfo );

	// Displays the Bargraphs
int		bgBarGraph  ( _AnaSetting *Input, fontInfo *pInfo );


int 		bgBarSet (fontInfo *pInfo )
{
	auto byte loop,index;
	auto int TotWidth;
	anatotal = 0;
	for (loop = 0 ; loop < 8 ; loop++)
	{
		if ( AnaSetting[loop].Enable )
		{
			anatotal++;
		}
	}
	TotWidth  = 320 / anatotal;
	anaOffset = TotWidth /  anatotal;
	anaWidth  = TotWidth - ( anaOffset ) ;
	index = 0;
	glBuffLock();
	for (loop = 0 ; loop < 8 ; loop++)
	{
		if (AnaSetting[loop].Enable)
		{
			AnaSetting[loop].xCoord = ( anaOffset / 2 ) + ( TotWidth * index );
			AnaSetting[loop].Height = BARHEIGHT;
			AnaSetting[loop].yCoord = BARYCOORD;
			switch ( AnaSetting[loop].GainMode )
			{
				case 0:
					AnaSetting[loop].Range = 20.0;	// 0 to 20vdc
					break;
				case 1:
					AnaSetting[loop].Range = 10.0;	// 0 to 10vdc
					break;
				case 2:
					AnaSetting[loop].Range = 5.0;		// 0 to 5vdc
					break;
				case 3:
					AnaSetting[loop].Range = 4.0;		// 0 to 4vdc
					break;
				case 4:
					AnaSetting[loop].Range = 2.5;		// 0 to 2.5vdc
					break;
				case 5:
					AnaSetting[loop].Range = 2.0;		// 0 to 2.0vdc
					break;
				case 6:
					AnaSetting[loop].Range = 1.25;	// 0 to 1.25 vdc
					break;
				case 7:
					AnaSetting[loop].Range = 1.00;	// 0 to 1.0 vdc
					break;
			}
	glPrintf(( AnaSetting[loop].xCoord + ( anaWidth/2 ) )- ( (pInfo->pixWidth * 2) /2 ),
			  AnaSetting[loop].yCoord - ( pInfo->pixHeight * 3 ),
			  pInfo,"A%d",loop);
	glPrintf(( AnaSetting[loop].xCoord+( anaWidth/2 ) )- ( (pInfo->pixWidth * 4) /2 ),
			  AnaSetting[loop].yCoord - ( pInfo->pixHeight * 2 ),
			  pInfo,"----",loop);	
	AnaSetting[loop].Res =(float)AnaSetting[loop].Height/(float)AnaSetting[loop].Range;
	index++;
		}
	}
	glBuffUnlock();
	return 1;
}


// Displays the Individual bargraph for the input.

int		bgBarGraph  ( _AnaSetting *Input, fontInfo *pInfo )
{
	
	if ( Input ->Enable )
	{
		glSetBrushType(PIXWHITE);// change brush type to White pixels
		glBlock(Input->xCoord,Input->yCoord,anaWidth,BARHEIGHT);
		glSetBrushType(PIXBLACK);
		
		/****** These four glPlotLines form the bar graph *******/
		glPlotLine(Input->xCoord,Input->yCoord,Input->xCoord + anaWidth,
						Input->yCoord);  
		glPlotLine(Input->xCoord,Input->yCoord + BARHEIGHT,
						Input->xCoord + anaWidth,Input->yCoord + BARHEIGHT);
		glPlotLine(Input->xCoord,Input->yCoord,Input->xCoord,
						Input->yCoord + BARHEIGHT);
		glPlotLine(Input->xCoord + anaWidth,Input->yCoord,
						Input->xCoord + anaWidth,Input->yCoord + BARHEIGHT);
		Input->anaVal = anaInVolts(Input->In,Input->GainMode);
		if (Input->anaVal > Input->Range) Input->anaVal = Input->Range;
		if (Input->anaVal < 0 ) Input->anaVal = 0;
		
		glBlock(Input->xCoord,
				  ((Input->yCoord+ BARHEIGHT)-(int)(Input->anaVal* Input->Res)),
				  anaWidth,
				  (int)( Input->anaVal * Input->Res ) );
				  
		glPrintf(( Input->xCoord+(anaWidth/2))-((pInfo->pixWidth * 5)/2),
					  Input->yCoord - pInfo->pixHeight,
					  pInfo,"%5.2f",
					  Input->anaVal);
		/********************************************************/
		
	}
	return 1;
}

// Displays the Initial Startup Screen.

int		bgMainScreen(void)
{
	glBuffLock();
	glBlankScreen();
	glkMsgBox(0,0,320,60,&fi17x35,"ANALOG DEMO",1,0);
	glkMsgBox(250,210,70,30,&fi8x10,"CONFIG\nDEMO",1,0);
	glBuffUnlock();
	return 1;
}

// Displays the Start Menu 
int		bgMenuStart (void)
{
	static int state, done;
	auto int	options;
	state = MENU_INIT;
	done = 0;
	glBuffLock();
	glBlankScreen();
	glBuffUnlock();
	while (!done)
	{
		keyProcess();
		costate
		{
			waitfor (( options = glMenu(&MenuStart, &state, 0, 0 )) != 0 );
			switch (options)
			{
				case 1:
					waitfor ( bgMenuSys() );
					state = MENU_REFRESH;
					break;
				case 2:
					waitfor ( bgMenuAna1() );
					state = MENU_REFRESH;
					break;
				case 3:
					done = 1;
					break;
			}
		}		
		
	}
	glBuffLock();
	glMenuClear(&MenuStart);	// Clear this menu
	glBuffUnlock();
	return 1;
}


// Displays and Controls the System System Menu
int		bgMenuSys   (void)
{
	static int state, done;
	auto int	options;
	state = MENU_INIT;
	done = 0;
	while (!done)
	{
		keyProcess();
		costate
		{
			waitfor (( options = glMenu(&MenuSysSetup, &state, 15, 15))!= 0);
			switch (options)
			{
				case 1:
					glBackLight(1);
					state = MENU_REFRESH;
					break;
				case 2:
					glBackLight(0);
					state = MENU_REFRESH;
					break;
				case 3:
					waitfor( bgMenuAdjC() );
					state = MENU_REFRESH;
					break;
				case 4:
					done = 1;
					break;
			}
		}
	}		
	glBuffLock();
	glMenuClear(&MenuSysSetup);	// Clear this menu
	glRefreshMenu(&MenuStart);		// Refresh the Start Menu
	glBuffUnlock();	
	return 1;
}


// Displays and Controls the Analog Channel Selection Menu
int		bgMenuAna1  (void)
{
	static int state, done;
	auto int	options;
	state = MENU_INIT;
	done = 0;
	while ( !done )
	{
		keyProcess();
		costate
		{
			waitfor ( (options = glMenu(&MenuAnaSetup, &state, 15, 15))!= 0);
			switch (options)
			{
				
				case 9:
					done = 1;
					break;
					
				default:
					waitfor ( bgMenuAna2 ( &AnaSetting[ options - 1 ] ) );
					state = MENU_REFRESH;
					break;
			}
		}
	}		
	glBuffLock();
	glMenuClear( &MenuAnaSetup );	// Clear this menu
	glRefreshMenu( &MenuStart );		// Refresh the Start Menu
	glBuffUnlock();
	return 1;
}
// The Contrast Adjustment Menu
int		bgMenuAdjC  (void)
{
	static int state, done;
	auto int	options;
	state = MENU_INIT;
	done = 0;
	while (!done)
	{
		keyProcess();
		costate
		{
			waitfor ((options= glMenu(&MenuAdjContrast,&state, 30, 30 ))!= 0);
			switch (options)
			{
				case 1:
					if (LastContrast < 255 ) LastContrast++;
					glSetContrast(LastContrast);
					state = MENU_REFRESH;
					break;
				case 2:
					if (LastContrast > 0 )   LastContrast--;
					glSetContrast(LastContrast);
					state = MENU_REFRESH;
					break;
				case 3:
					done = 1;
					break;
			}
		}
	}		
	glBuffLock();
	glMenuClear( &MenuAdjContrast );	// Clear this menu
	glRefreshMenu( &MenuSysSetup );		// Refresh the Start Menu
	glBuffUnlock();
	return 1;
}
// The Range Selection Menu
int		bgMenuRSel  ( _AnaSetting *Input )
{
	static int state, done;
	auto int	options;
	state = MENU_INIT;
	done = 0;
	while (!done)
	{
		keyProcess();
		costate
		{
			waitfor ((options = glMenu( &MenuRangeSel,&state,45,45 ))!= 0);
			Input ->GainMode = options - 1;
			done = 1;
		}
	}		
	glBuffLock();
	glMenuClear( &MenuRangeSel );	// Clear this menu
	glRefreshMenu( &MenuAnaSet2 );		// Refresh the Start Menu
	glBuffUnlock();
	return 1;
}
// The Analog Channel Setup Menu
int		bgMenuAna2  ( _AnaSetting *Input )
{
	static int state, done;
	auto int	options;
	state = MENU_INIT;
	done = 0;
	
	while ( !done )
	{
		keyProcess();
		costate
		{
			waitfor(DelayMs(1));
			glBuffLock();
			glSetBrushType(PIXWHITE);
			glBlock(0,180,320,60);
			glSetBrushType(PIXBLACK);
			glPrintf(0,180,&fi10x16,"Input  -> %d",Input->In );
			glPrintf(0,200,&fi10x16,"Status -> %s",statusMsg[Input->Enable]);
			glPrintf(0,220,&fi10x16,"Mode   -> %s",modeMsg[Input->GainMode]);
			statusMsg[Input->Enable],
			glBuffUnlock();
			waitfor ((options =glMenu( &MenuAnaSet2, &state, 30, 30 ) )!= 0);
			switch (options)
			{
				
				case 1:
					Input ->Enable = 1;
					done = 1;
					break;

				case 2:
					Input ->Enable = 0;
					done = 1;
					break;
				case 3:
					waitfor ( bgMenuRSel ( Input ) );
					break;
				case 4:
					done = 1;
					break;
			}
		}
	}		
	glBuffLock();
	glSetBrushType(PIXWHITE);
	glBlock(0,180,320,60);
	glSetBrushType(PIXBLACK);
	glMenuClear( &MenuAnaSet2 );	
	glRefreshMenu( &MenuAnaSetup );		
	glBuffUnlock();
	return 1;
}

main ()
{
	static int enable,stopUpdate,updatecomplete;
	auto int loop,keyPress;
	static byte init,input;
	static CoData BarGraph;
	
		//Set all the Channels Structures to a default state
		// Enabled, 0 to 10Vdc range.
	for (loop = 0 ; loop < 8 ; loop++)
	{
		AnaSetting[loop].In = loop;
		AnaSetting[loop].GainMode = 1;
		AnaSetting[loop].Enable = 1;
	}
	
	updatecomplete = 0;
	stopUpdate = 1;
	brdInit();			// Initialize the Controller
	glInit();			// Initialize the LCD
	keyInit();			// Initialize the Keypad
	keypadDef();		// Setup the Keypad Default Values
	LastContrast = 24;  // Setup the Inital Contrast Setting
	// Turn on the Backlight
	glBackLight(1);
	// Set the Contrast
	glSetContrast(LastContrast);
	// Setup the Fonts used in this sample
	glXFontInit(&fi6x8,   6,  8,  32, 255, Font6x8);
	glXFontInit(&fi8x10,  8,  10, 32, 127, Font8x10);
	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);
	glXFontInit(&fi17x35, 17,35,0x20,0xFF, Font17x35);
	// Setup the Menus 
	glMenuInit( &MenuStart, &fi10x16, DOUBLE_LINE, SHADOW,startMenu,
	 				" START MENU ", -1 );
	glMenuInit( &MenuSysSetup, &fi10x16, DOUBLE_LINE, SHADOW,systemSetup,
					" SYSTEM SETUP ", -1 );
	glMenuInit( &MenuAnaSetup, &fi10x16, DOUBLE_LINE, SHADOW,analogSetup,
					" ANALOG SETUP ", 5 );
	glMenuInit( &MenuAdjContrast, &fi10x16, DOUBLE_LINE, SHADOW,adjContrast,
					" CONSTRAST ADJUST ", -1 );
	glMenuInit( &MenuRangeSel, &fi10x16, DOUBLE_LINE, SHADOW,rangeSelect,
					" RANGE SELECT ", 5 );
	glMenuInit( &MenuAnaSet2, &fi10x16, DOUBLE_LINE, SHADOW,analogSet2,
					" ANALOG SETUP2 ", -1 );
	input = 0;
	init = 1;
	for (;;)
	{
			// Check for any keypresses
		keyProcess();

		// This costate displays the Initial screen, and Starts the
		// Bargraphing demo
		costate
		{
			waitfor (init);
			waitfor ( bgMainScreen () );
			init = 0;
			input = 0;
			waitfor ( bgBarSet(&fi8x10) );
			stopUpdate = 0;
			CoBegin( &BarGraph );
		}

		// This costate checks for the 'S' key to be pressed
		//	(key just below the label 'CONFIG DEMO')
																				
		costate
		{
			waitfor ( ( keyPress = keyGet() )== 'S' );
			stopUpdate = 1;
			waitfor (updatecomplete);
			CoReset( &BarGraph );
			waitfor ( bgMenuStart() );
			init = 1;
		}
		
		// This costate displays the Bargraphs
		costate BarGraph  
		{
			waitfor(!stopUpdate);
			waitfor(IntervalMs(350));
			updatecomplete = 0;
			glBuffLock();
			input = 0;
			while (input <= 7)
			{
				if ( AnaSetting[input].Enable )
				{
					waitfor ( bgBarGraph ( &AnaSetting[ input ],
								 &fi8x10 ) );
				}
				input++;
				waitfor (DelayMs(1));
			}
			glBuffUnlock();
			updatecomplete = 1;	
			CoBegin( &BarGraph );
		}
	}
}
