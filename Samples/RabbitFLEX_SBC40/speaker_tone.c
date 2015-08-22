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
        Samples\RabbitFLEX_SBC40\speaker_tone.c

        This program demonstrates the tone driver for the speaker on the
        RabbitFLEX SBC40 board.  A menu of choices is displayed, from which 5
        different tones can be played.  Additionally, the tones can be
        repeated indefinitely at user control.

        Note that this differs from the audio driver, which can play a
        longer sound a single time.

        Note that you must change the macro at the beginning of the program
        to point to the speaker.  Refer to your list of pins and software
        names for your specific RabbitFlex board for the appropriate information
        to put here.

*******************************************************************************/

/*
 * Replace the macro definition below with the correct RabbitFlex IO pin
 * information structure.  These are the software names for each of your flex
 * features.  The feature that you specify here is the one that will be used
 * for this sample.
 */
#define MY_SPEAKER flex_speaker

/*
 * This is the number of times the tone will be repeated in non-repeating
 * mode.  This essentially changes the length of the tone played.
 */
#define REPEAT_TONE  50

/*
 * This defines the size of the tones.
 */
#define TONE_SIZE    150

char tones[5][TONE_SIZE];		// Holds the 5 different tone waveforms
int tone_len[5];					// Holds the lengths of the 5 tones

void refwave_organ();
void refwave_saw();
int buildwave(char tone, int freq);

void main(void)
{
	char ch;				// Holds the user command
	char repeat;		// Indicates whether or not the tone should repeat
	int request_exit;	// Indicates if the user has requested to exit the program

	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// This function sets the correct PWM channel to use for the speaker.
	// Simply pass a pointer to the speaker structure.
	flexSpeakerPWM(&MY_SPEAKER);

	// Activate the tone system
   flexToneActivate();

	// Build a wave of an organ-like sound
   refwave_organ();

   // Build waveformss of different frequencies of the organ-like sound
   tone_len[1] = buildwave(1,988);
   tone_len[2] = buildwave(2,698);
   tone_len[3] = buildwave(3,523);
   tone_len[4] = buildwave(4,392);

   // Build a wave of a raspy sound
   refwave_saw();
   // Build a waveform of the raspy sound at a specific frequency
   tone_len[0] = buildwave(0,220);   // Build fail tone waveform

	// Display a menu of choices to the user
   printf("Speaker Menu\n--------\n");
   printf("0)   Play a raspy sound\n");
   printf("1-4) Play organ sounds\n");
   printf("r)   Toggle whether or not tones repeat forever\n");
   printf("x)   Exit the program\n");

	// Set the tones to not repeat (if it were >= 128, it would repeat
	// indefinitely)
	repeat = REPEAT_TONE;

	// Indicate that program exit has not been requested
	request_exit = 0;

	// Continue until the user requests to exit the program
   while (!request_exit) {
   	ch = getchar();
   	// Perform the user command
   	switch (ch) {
   	case '0':
   		// Play the raspy tone
		   flexToneLoad(tones[0], tone_len[0], repeat);
     		break;
   	case '1':
   		// Play an organ tone
		   flexToneLoad(tones[1], tone_len[1], repeat);
   		break;
   	case '2':
   		// Play an organ tone
		   flexToneLoad(tones[2], tone_len[2], repeat);
   		break;
   	case '3':
   		// Play an organ tone
		   flexToneLoad(tones[3], tone_len[3], repeat);
   		break;
   	case '4':
   		// Play an organ tone
		   flexToneLoad(tones[4], tone_len[4], repeat);
   		break;
   	case 'r':
   		// Toggle repeating of the tones
   		if (repeat < 128) {
   			repeat = 255;
   		}
   		else {
   			repeat = REPEAT_TONE;
   			// Stop any tone that is currently playing
   			flexToneStop();
   		}
   		break;
   	case 'x':
   		// Request an exit of the program
			request_exit = 1;
			break;
		}
   }

   // Turns off the tone system.  This frees up the processor for other tasks,
   // but in this case, we're just exiting the program.
   flexToneShutdown();
}

#define SAMPLERATE 22000
#define REFWAVE_SIZE 400
#define REFWAVE_FREQ SAMPLERATE/REFWAVE_SIZE
#define PAUSECOUNTS 50

unsigned char refwave[REFWAVE_SIZE];  //reference waveform

/*
 * Set up sine wave with even harmonics to create an electronic organ sound
 */
void refwave_organ()
{
	int i;
   float temp, fraction;

   // Build an electronic organ reference waveform
   for(i = 0;i < REFWAVE_SIZE;i++)
   {
   	fraction = ((float)i)/REFWAVE_SIZE;
   	temp = 128.0 +
      			60.0*sin(2.0*PI*fraction) +
      			40.0*sin(4.0*PI*fraction) +
               20.0*sin(8.0*PI*fraction);
   	refwave[i] = (unsigned char)temp;
   }
}

/*
 * Build a sawtooth wave which will give a somewhat raspy sound
 */
void refwave_saw()
{
	int i;

   // Build a sawtooth reference waveform
   for(i = 0;i < REFWAVE_SIZE;i++)
   {
   	refwave[i] = i/2 + 10;
   }
}

/*
 * Build tone sample based on frequency given and save in tones array as
 * tone given.  One of the above refwave functions must be called before
 * calling this function.  The refwave function sets the basic sound of
 * the waveform to be created here.
 */
int buildwave(char tone, int freq)
{
	int count, ref_count;
   unsigned long sum;

   count = 0;      // Initialize variables
   ref_count = 0;
   sum = 0;

   // Transform reference wave into frequency shifted output wave
   // and save one cycle of the created waveform in tones array
   while(ref_count < REFWAVE_SIZE)
   {
   	tones[tone][count] = refwave[ref_count];
     	count++;
     	sum += freq;
     	ref_count = (int)(sum / (REFWAVE_FREQ));
   }
	return count;    // return with the size of the created sample
}

