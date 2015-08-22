/***************************************************************************
	speaker.c
	Z-World Inc. Copyright 2000

	This sample program is used with Intellicom Series products.

	This program demonstrates how to adjust the speaker for frequency and
	volume.  The keypad and	display are used to increase or decrease
	frequency.  Volume can be adjusted at four levels.
	Contrast and backlight are unsupported on VFD's.

	Keypad Layout:

	[  1  ] [  2  ] [  3  ] [  4  ] [  5  ] [  .  ]
	[  6  ] [  7  ] [  8  ] [  9  ] [  0  ] [Enter]

	Press '1' - '4' for no to louder volume.
	Press '5' to increase frequency every 100 Hz up to 3000 Hz.
	Press '0' to decrease frequency every 100 Hz down to 600 Hz.

***************************************************************************/
#class auto					/* Change local var storage default to "auto" */


#define HIFREQ	3000
#define LOFREQ	600
#define STEP 100

#define BCONTRAST 20  //Adjust for best contrast a value from 4 to 252

void main (void)
{
	unsigned	int aFreq, aAmp, wKey, akey, validkey;

	brdInit();				// initialize board, keypad, display
	keypadDef();			// set keypad to default layout
	dispContrast(BCONTRAST);		// adjust contrast
	dispBacklight(1);		// turn on backlight

	dispGoto(0,0);
	dispPrintf ("1-4 for volume" );
	dispGoto(0,1);
	dispPrintf ("'5' inc, '0' dec" );

	aFreq = LOFREQ;
	aAmp = 0;
	wKey=akey='0';
	validkey = 1;

	dispGoto(0,2);
	dispPrintf("Vol level %c", wKey);
	dispGoto(0,3);
	dispPrintf("Frequency %d", aFreq);

	for (;;) {
		costate {								//	Process Keypad Press/Hold/Release
			keyProcess ();
			waitfor ( DelayMs(10) );
			}

		costate {									//	Process Keypad Press/Hold/Release
			waitfor ( wKey = keyGet() );		//	Wait for Keypress
			switch ( wKey) {
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
					if (aFreq < HIFREQ)
						aFreq+=STEP;    // increase frequency
					break;
				case	'0':
					if (aFreq > LOFREQ)
						aFreq-=STEP;		// decrease frequency
					break;
				default:
					validkey = 0;
					break;
				}

			if (validkey) {
				dispGoto(10,2);
				dispPrintf("%c  ", akey);
				dispGoto(10,3);
				dispPrintf("%d  ", aFreq);
				spkrOut (aFreq, aAmp );		// output to speaker
				}
			else
				validkey=1;
			}
		}
}

