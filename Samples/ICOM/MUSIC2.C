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

	music2.c

	Demonstration of playing "background music" while other processing
	is going on by using costatements.  Keys pressed on the keypad will
	be printed to the display while the music is playing.

	See comments in MUSIC.C for more information on playing music with
	the Intellicom.

*******************************************************************************/
#class auto					/* Change local var storage default to "auto" */

#define NUM_NOTES		12
#define NUM_OCTAVES	 4

// definitions for the notes in each octave
#define	_A		0
#define	_As		1
#define	_B		2
#define	_C		3
#define	_Cs		4
#define	_D		5
#define	_Ds		6
#define	_E		7
#define	_F		8
#define	_Fs		9
#define	_G		10
#define	_Gs		11

const int	freq[NUM_OCTAVES][NUM_NOTES] = {
	{ 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831 },
	{ 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661 },
	{ 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322 },
	{ 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645 } };

#define	REST	0x00

// note lengths (in milliseconds)
#define	WHOLE		  2500
#define	HALF		 (WHOLE/2)
#define	QUARTER	 (WHOLE/4)
#define	EIGHTH	 (WHOLE/8)
#define	SIXTEENTH (WHOLE/16)

#define	THREEQTR	 (HALF + QUARTER)

// note volume (0-3)
#define VOL	1

////////////////////////////////////////////////////////////////////////////////

void main()
{
	int	wKey;

	brdInit();
	keypadDef();		// set default keypad layout

	while (1) {			// loop forever

		// large costatement that will play Fur Elise (Beethoven) in the background
		costate {
			spkrOut(freq[2][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_Ds], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_Ds], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_B], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_D], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_C], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_A], VOL);	waitfor(DelayMs(EIGHTH));
			spkrOut(REST, VOL);			waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_C], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_A], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_B], VOL);	waitfor(DelayMs(EIGHTH));
			spkrOut(REST, VOL);			waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_Gs], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_B], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_C], VOL);	waitfor(DelayMs(EIGHTH));
			spkrOut(REST, VOL);			waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_Ds], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_Ds], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_B], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_D], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_C], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_A], VOL);	waitfor(DelayMs(EIGHTH));
			spkrOut(REST, VOL);			waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_C], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_A], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_B], VOL);	waitfor(DelayMs(EIGHTH));
			spkrOut(REST, VOL);			waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[1][_E], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_C], VOL);	waitfor(DelayMs(SIXTEENTH));
			spkrOut(freq[2][_B], VOL);	waitfor(DelayMs(SIXTEENTH));

			spkrOut(freq[2][_A], VOL);	waitfor(DelayMs(QUARTER));

			spkrOut(0, 0);					waitfor(DelayMs(WHOLE));
		}


		//	costatement to process keypad press/hold/release
		costate {
			keyProcess ();
			waitfor ( DelayMs(10) );
		}


		// costatement to handle any key presses
		costate {
			waitfor (wKey = keyGet());		//	Wait for Keypress
			dispPrintf ("%c", wKey);
		}

	}	// end of while(1)
}

////////////////////////////////////////////////////////////////////////////////

