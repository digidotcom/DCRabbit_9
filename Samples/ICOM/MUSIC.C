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

	music.c

	This program provides an example of playing simple music through the
	Intellicom speaker.  By providing the frequency of each note and delaying
	for a certain amount of time, simple tunes can be played, such as the
	first line of the chorus to "Bicycle Built For Two (Daisy Bell)" below.

	Four octaves are available in this demo (all above middle C).  To play
	a note, simply call spkrOut() with the appropriate frequency, which can
	be found in the array

								freq[octave][note]

	where 'octave' ranges from 0-3 (starting with the note A) and 'note'
	ranges from 0-11.  Note that in this demo, one octave ranges from
	'A' note to 'A' note.

	To adjust the pace of your music, change the length definition of
	a whole note ("#define WHOLE 1000" below) to something appropriate.

	NOTE that, although the frequencies in freq[] go down as low as 440 Hz
	(A above middle C), the Intellicom cannot reliably play frequences below
	about 575 Hz (D above middle C).  Scale your octaves accordingly!

 *******************************************************************************/
#class auto					/* Change local var storage default to "auto" */

#define	NUM_NOTES	12
#define	NUM_OCTAVES	 4

// definitions for the notes in each octave
#define	_A	0
#define	_As	1		// A-sharp
#define	_B	2
#define	_C	3
#define	_Cs	4		// C-sharp
#define	_D	5
#define	_Ds	6		// D-sharp
#define	_E	7
#define	_F	8
#define	_Fs	9		// F-sharp
#define	_G	10
#define	_Gs	11		// G-sharp

// actual frequency data for four octaves
const int	freq[NUM_OCTAVES][NUM_NOTES] = {
	{ 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831 },
	{ 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661 },
	{ 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322 },
	{ 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645 } };

// note lengths (in milliseconds)
#define	WHOLE		 1000
#define	HALF		 (WHOLE/2)
#define	QUARTER	 (WHOLE/4)
#define	EIGHTH	 (WHOLE/8)
#define	SIXTEENTH (WHOLE/16)
#define	THREEQTR	 (HALF + QUARTER)

// note volume (0-3)
#define VOL	1
int delayMS(int);
////////////////////////////////////////////////////////////////////////////////

void main()
{
	brdInit();

	spkrOut(freq[1][_D], VOL);		dispPrintf("   Dai");
	delayMS(THREEQTR);
	spkrOut(freq[1][_B], VOL);		dispPrintf("sy, ");
	delayMS(THREEQTR);
	spkrOut(freq[0][_G], VOL);		dispPrintf("Dai");
	delayMS(THREEQTR);
	spkrOut(freq[0][_D], VOL);		dispPrintf("sy,\n");
	delayMS(HALF);

	// 1/4 rest
	spkrOut(0, 0);						delayMS(QUARTER);

	spkrOut(freq[0][_E], VOL);		dispPrintf("   give ");
	delayMS(QUARTER);
	spkrOut(freq[0][_Fs], VOL);		dispPrintf("me ");
	delayMS(QUARTER);
	spkrOut(freq[0][_G], VOL);		dispPrintf("your\n");
	delayMS(QUARTER);
	spkrOut(freq[0][_E], VOL);		dispPrintf("     an");
	delayMS(HALF);
	spkrOut(freq[0][_G], VOL);		dispPrintf("swer,");
	delayMS(QUARTER);
	spkrOut(freq[0][_D], VOL);		dispPrintf("do!");
	delayMS(THREEQTR+HALF);

	spkrOut(0, 0);
}

////////////////////////////////////////////////////////////////////////////////
// a blocking delay for the number of milliseconds provided

int delayMS(int msec)
{
	static unsigned long	t0;

	t0 = MS_TIMER + msec;
	while( (int)(MS_TIMER - t0) < 0 )
		; // do nothing
}

////////////////////////////////////////////////////////////////////////////////