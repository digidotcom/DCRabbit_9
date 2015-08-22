/********************************************************************
	Samples\ICOM\icomdemo.c

	OP6600 and OP6700 Intellicom Series demonstration.

	This sample program will demostrate some of the display and
	keypad features.  Backlight and contrast adjustment are
	unsupported on VFD's.

	Compile and run this program to view demo.

********************************************************************/
#class auto					/* Change local var storage default to "auto" */


#define ENTERKEY 'E'		//Enter key
#define EXITKEY 'E'		//Exit key

#define BCONTRAST 20  //Adjust for best contrast a value from 4 to 252
#define HICONTRAST 4
#define LOCONTRAST 120

#define HISPEAKER	3000
#define LOSPEAKER	600

/*===================================================================
	delay - provides micro second delays
===================================================================*/
debug void delay (long mdelay)
{
	for (; mdelay>0; mdelay--);
}

/*===================================================================
	mdelay - provides milli-second delays
===================================================================*/
nodebug void mdelay (long millis)
{
	auto unsigned long t1;

	t1=MS_TIMER + millis;
   while( (int)(MS_TIMER - t1) < 0 );
}

/*===================================================================
	sdelay - provides second delays
===================================================================*/
nodebug void sdelay (long secs)
{
	auto unsigned long t1;

	t1=SEC_TIMER + secs;
   while( (int)(SEC_TIMER - t1) < 0 );
}

#memmap xmem

/*===================================================================
	pong - demo for cursor movement
===================================================================*/
void pong ()
{
	auto int	px,py;                        // Current Position
	auto int   dx,dy;                        // Current Direction
	auto int   nx,ny;                        // New Position
	auto int	xl, xh,	yl, yh, i;

	xl =  0;									// box coordinates
	xh = 20;
	yl =  0;
	yh =  4;   						// lines 0 .. yh-1
   px = xl; py = yl;        	// Position Ball
   dx = 3; dy = 2;                 	// Give Direction

	dispCursor (DISP_CURBLINK);

   for (i=0; i<30; i++)  {
      dispGoto ( px,py );

      mdelay(200);

      nx = px + dx;               	// Try New Position
      ny = py + dy;

      if (nx <= xl || nx >= xh)     // Avoid Collision
         dx = -dx;
      if (ny <= yl || ny >= yh)
         dy = -dy;

      nx = px + dx;               	// Next Position
      ny = py + dy;

      dispGoto ( px,py );
      mdelay(200);

      px = nx; py = ny;           	// Move Ball
   }
	dispCursor (DISP_CUROFF);
}

/*===================================================================
===================================================================*/
void adjCursor()
{
	static const char *const stra = "        [4]         ";
	static const char *const strb = "[8]  Press keys  [0]";
	static const char *const strc = "        [9]         ";
	static const char *const stre = " Press Enter to end ";

	auto char inkey;
	auto int xdir, ydir;

	dispClear();
	dispGoto(0,0); dispPrintf("%s", stra);
	dispGoto(0,1); dispPrintf("%s", strb);
	dispGoto(0,2); dispPrintf("%s", strc);
	dispGoto(0,3); dispPrintf("%s", stre);

	xdir=9;
	ydir=1;
	dispGoto(xdir,ydir);
	dispCursor(DISP_CURBLINK);

	do {
		costate {									//	Process Keypad Press/Hold/Release
			keyProcess ();
			waitfor ( DelayMs(10) );
			}

		costate {
			waitfor ( inkey = keyGet() );		//	Wait for Keypress
			switch (inkey) {
				case '4':
					ydir--;
					if (ydir < 0)
						ydir=3;
					break;
				case '8':
					xdir--;
					if (xdir < 0)
						xdir=19;
					break;
				case '9':
					ydir++;
					if (ydir > 3)
						ydir=0;
					break;
				case '0':
					xdir++;
					if (xdir > 19)
						xdir=0;
					break;
				default:
					break;
				}
			dispGoto(xdir,ydir);
			}
		} while (inkey != EXITKEY);

	dispClear();
	dispCursor(DISP_CUROFF);
}
/*===================================================================
===================================================================*/
void adjContrast()
{
	static const char *const stra = "      Press [1]     ";
	static const char *const strb = " for more contrast  ";
	static const char *const strc = "      Press [6]     ";
	static const char *const strd = " for less contrast  ";
	static const char *const stre = " Press Enter to end ";

	auto char inkey;
	auto unsigned int scontrast;

	scontrast=BCONTRAST;

	dispClear();
	dispGoto(0,3); dispPrintf("%s", stre);

	do {
		costate {								//	Process Keypad Press/Hold/Release
			keyProcess ();
			waitfor ( DelayMs(10) );
			}

		costate {
			dispGoto(0,0); dispPrintf("%s", stra);
			dispGoto(0,1); dispPrintf("%s", strb);
			waitfor (DelaySec(2));
			yield;
			dispGoto(0,0); dispPrintf("%s", strc);
			dispGoto(0,1); dispPrintf("%s", strd);
			waitfor (DelaySec(2));
			yield;
			}

		costate {
			waitfor ( inkey = keyGet() );		//	Wait for Keypress
			switch (inkey) {
				case '1':
					if (scontrast > HICONTRAST)
						scontrast-=8;
					break;
				case '6':
					if (scontrast <= LOCONTRAST)
						scontrast+=8;
					break;
				default:
					break;
				}
			dispContrast (scontrast);
			}

		} while (inkey != EXITKEY);

	dispClear();
	dispContrast (BCONTRAST);
}

/*===================================================================
===================================================================*/

void adjBacklight()
{
	static const char *const stra = "      Light on      ";
	static const char *const strb = "      Light off     ";
	static const char *const strd = "  Press [2] again   ";
	static const char *const stre = " Press Enter to end ";

	auto char inkey;
	auto unsigned int slite;

	dispClear();
	dispGoto(0,2); dispPrintf("%s", strd);
	dispGoto(0,3); dispPrintf("%s", stre);

	do {
		do {
			keyProcess ();
			} while (!(inkey = keyGet()));

			if (inkey== '2') {
				dispBacklight(slite = slite ? 0:1); //turn on/off LCD backlight
				}

			if (slite) {
				dispGoto(0,0); dispPrintf("%s", stra);
				}
			else {
				dispGoto(0,0); dispPrintf("%s", strb);
				}
		} while (inkey != EXITKEY);

	dispClear();
}

/*===================================================================
===================================================================*/
void adjSpeaker()
{
	auto unsigned int aFreq, aAmp, wKey, akey, validkey;

	aFreq = LOSPEAKER;
	aAmp = 0;
	wKey=akey='0';
	validkey = 1;

	dispClear();

	dispGoto(0,0);
	dispPrintf("Volume:  Press 1-4  ");
	dispGoto(0,1);
	dispPrintf("Freq: Press 5 or 0  ");

	dispGoto(0,2);
	dispPrintf("Volume level %c     ", wKey);
	dispGoto(0,3);
	dispPrintf("Freq level    %d  ", aFreq);

	do {
		do	{
			keyProcess ();
			} while (!(wKey = keyGet()));

		switch (wKey) {
			case	'1':
				aAmp = 0;		// no volume
				akey=wKey;
				break;
			case	'2':
				aAmp = 2;		// level 1
				akey=wKey;
				break;
			case	'3':
				aAmp = 1;		// level 2
				akey=wKey;
				break;
			case	'4':
				aAmp = 3;		// level 3
				akey=wKey;
				break;
			case	'5':
				if (aFreq < HISPEAKER)
					aFreq+=100;    // increase frequency
				break;
			case	'0':
				if (aFreq > LOSPEAKER)
					aFreq-=100;		// decrease frequency
				break;
			default:
				validkey = 0;
				break;
			}

		if (validkey) {
			dispGoto(13,2);
			dispPrintf("%c", akey);
			dispGoto(13,3);
			dispPrintf("%4d", aFreq);
			spkrOut (aFreq, aAmp );		// output to speaker
			}
		else
			validkey=1;
		} while (wKey != EXITKEY);

	dispClear();
	spkrOut (0, 0);		// output off to speaker
}

/*===================================================================
===================================================================*/
void scrnfadeout()
{
	auto int wcv;

	for (wcv=BCONTRAST; wcv<=LOCONTRAST; wcv+=8) {
		dispContrast (wcv);
		mdelay(100);
		}
}

/*===================================================================
===================================================================*/
void scrnfadein()
{
	auto int wcv;

	for (wcv=LOCONTRAST; wcv>=BCONTRAST; wcv-=8) {
		dispContrast (wcv);
		mdelay(100);
		}
}

/*===================================================================
===================================================================*/
void docontrast()
{
	auto int wcv;

	for (wcv=BCONTRAST; wcv<=LOCONTRAST; wcv+=16) {
		dispContrast (wcv);
		mdelay (500);
		}

	for (wcv=LOCONTRAST; wcv>=HICONTRAST; wcv-=16) {
		dispContrast (wcv);
		mdelay (500);
		}
	dispContrast(BCONTRAST);
}

/*===================================================================
===================================================================*/
void dointro()
{
	auto unsigned int i, slite;

	static const char str0[][] = {	"     Z-World's      ",
												" INTELLICOM SERIES  ",
												"  A C-Programmable  ",
												"Intelligent Terminal"
											};

	static const char str1[][] = {	"   In addition to   ",
												"  Z-World standard  ",
												" features, here are ",
												"   a few more...    "
											};

	static const char str2[][] = {	"    >>>>>>>>>>>>    ",
												"     Adjustable     ",
												"      Contrast      ",
												"    <<<<<<<<<<<<    "
											};

	static const char str3[][] = {	"    ************    ",
												"     Backlight      ",
												"      Support       ",
												"    ************    "
											};

	static const char str4[][] = {	"    ::::::::::::    ",
												"      Speaker       ",
												"      Control       ",
												"    ::::::::::::    "
											};

	static const char str5[][] = {	"    ++++++++++++    ",
												"       Cursor       ",
												"      Movement      ",
												"    ++++++++++++    "
											};

	static const char str6[][] = {	"                    ",
												"    Now You Try!    ",
												"                    ",
												"                    "
											};

	/// main intro
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str0[i]);
		}
	sdelay(5);

	/// intro
	scrnfadeout();
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str1[i]);
		}
	scrnfadein();
	sdelay(3);

	/// show off contrast
	scrnfadeout();
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str2[i]);
		}
	scrnfadein();
	docontrast();
	sdelay(3);

	/// show off backlight
	scrnfadeout();
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str3[i]);
		}
	scrnfadein();
	slite=0;
	for (i=0; i<=8; i++) {
		dispBacklight(slite=slite?0:1);
		sdelay(1);
		}
	dispBacklight(1);
	sdelay(3);

	/// show off speaker
	scrnfadeout();
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str4[i]);
		}
	scrnfadein();
	for (i=0; i<3; i++) {
		spkrOut (200, 3 );		// output to speaker
		mdelay(500);
		spkrOut (500, 0);
		mdelay(50);

		spkrOut (500, 3 );		// output to speaker
		mdelay(800);
		spkrOut (500, 0);
		mdelay(50);

		spkrOut (1800, 3 );		// output to speaker
		mdelay(1000);
		spkrOut (1000, 3);
		mdelay(500);

		spkrOut (600, 3);
		mdelay(800);
		}
	spkrOut (0, 0);
	sdelay(3);

	/// show off cursor movement
	scrnfadeout();
	dispClear();
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str5[i]);
		}
	scrnfadein();
	pong();
	sdelay(3);

	/// show off interactive
	scrnfadeout();
	for (i=0; i<=3; i++) {
		dispGoto ( 0,i );
		dispPrintf ("%s", str6[i]);
		}
	scrnfadein();
	sdelay(5);
}

/*===================================================================
	rundemo - for demo, main function
===================================================================*/

void rundemo()
{
	char inkey;
	int i,j,k,m;

	brdInit();					//initialize controller
	keypadDef();
	dispContrast (BCONTRAST);
	dispBacklight(1); 		//turn on LCD backlight

 	dointro();

	while (1) {
		dispClear ();
		//dispContrast(wC);
		dispGoto(0,0);  dispPrintf("%s", "Press [1] Contrast ");
		dispGoto(0,1);  dispPrintf("%s", "Press [2] Backlight");
		dispGoto(0,2);  dispPrintf("%s", "Press [3] Speaker  ");
		dispGoto(0,3);  dispPrintf("%s", "Press [4] Cursor   ");

		do {
			keyProcess();
			} while (!(inkey = keyGet()));

		switch (inkey) {
			case '1':
				adjContrast();
				break;
			case '2':
				adjBacklight();
				break;
			case '3':
				adjSpeaker();
				break;
			case '4':
				adjCursor();
				break;
			default:
				break;
			} //end switch
		} //end forever while
}

/********************************************************************
********************************************************************/

void main (void)
{
	rundemo();			//no program cable
}