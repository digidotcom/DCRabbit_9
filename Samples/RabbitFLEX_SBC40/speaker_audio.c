/****************************************************************************
        Samples\RabbitFLEX_SBC40\speaker_audio.c
        Rabbit Semiconductor, 2006

WAV files are a standard for uncompressed, digital audio samples. The
RabbitFLEX SBC40 is capable of playing sampled audio, and in this
application we'll explore how to parse and play a WAV file.

The sample is written in a modular fashion with a small, straightforward
main() section that is easy to follow.  Complex code has been moved into
separate functions.  If you want to play WAV files in your own
applications, you don't need to understand how this code works.  Simply
cut and paste the start_wav() and play_wav() functions into your program
and call them.

Important Concepts:

1. Sampled digital audio is simply a sequence of values, each one
representing the amplitude of an audio signal at a given point in time.

2. Within the Rabbit microcontroller system, there are two types of memory
storage. They are called root memory and extended memory. The Rabbit has
only 64 kilobytes of root memory, and much of this is already needed for
a variety of things. Therefore if the Rabbit needs to store a large piece
of data such as an audio file, we must use extended memory. Dynamic C
has a special command for including a data file when downloading to
the Rabbit (#ximport) which is demonstrated below.

3. Parsing is the process of extracting information from formatted data.
A WAV file consists of two parts: the audio data itself, and a block of
information about the data at the beginning of the file called a "header."
Before the audio data can be played, the header must be parsed to determine
things such as the length and sampling rate of the data. In this program,
all of the parsing is done in the function start_wav(). It is not necessary
to fully understand the workings of this function in order to use it.


***************************************************************************/

/*
 * Replace the macro definition below with the correct RabbitFlex IO pin
 * information structure.  These are the software names for each of your
 * RabbitFlex features.  The feature that you specify here is the one that
 * will be used for this sample.
 */
#define MY_SPEAKER flex_speaker

/*
 * This is where we load the audio file into the Rabbit along with the
 * program.  The variable 'wav_file' is set to the location of this file
 * in the Rabbit's memory.
 */
#ximport "audio/cantdo.wav" wav_file

/*
 * These are function declarations. They are put here at the beginning of
 * the program so that they can be called from in the main() section below.
 * The functions are actually defined at the end of the program.
 */
int start_wav(unsigned long xptr);
int play_wav(void);


void main(void)
{
	// This function must be called to initialize the RabbitFlex board
	brdInit();

	// This function sets the correct PWM channel to use for the speaker.
	// Simply pass a pointer to the speaker structure.
 	flexSpeakerPWM(&MY_SPEAKER);

	// Activate the audio system
	flexAudioActivate();

	// State the wav file playing
	start_wav(wav_file);
	// Continue to play the file
	while (1) {
		play_wav();
	}
}

/*
 * This is the end of the main program. What follows are the functions
 * used to parse WAV files and load them into the audio driver buffer.
 */

#define WAV_BUF_SIZE 100

unsigned char wav_buf[WAV_BUF_SIZE];

unsigned long wav_xptr;		// pointer to current position in ximport file
unsigned long wav_count;   // count of number of data samples played

/*
 * This function parses a WAV file and sets up the audio driver to play it.
 * It will return 1 if the parsing and setup are successful, otherwise it
 * will return 0.
 * After calling this, play_wav() must be called repeatedly to actually
 * play the sample.
 */
int start_wav(unsigned long xptr)
{
	unsigned long file_len;
   char temp[24];
   int channels, sample_bytes;
   long sample_rate;
   char format_flag;

   wav_xptr = xptr;
   // The first 4 bytes at the ximport location hold the total length of
   // the following data.
   xmem2root(&file_len, wav_xptr, 4);
   wav_xptr += 4;
   if(file_len < 40) {
   	return 0; //too short for valid WAV file
   }

   // Is it a wav file?
   xmem2root(temp, wav_xptr, 12);
   wav_xptr += 12;
   if (strncmp(temp, "RIFF", 4) || strncmp(temp+8, "WAVE", 4)) {
   	return 0; // not a wav file
   }
   format_flag = 0;
   while ((wav_xptr - xptr) < file_len) {
	   xmem2root(temp, wav_xptr, 8);
	   wav_xptr += 8;
   	if (strncmp(temp, "fmt ", 4) == 0) {
   		// process format chunk
         xmem2root(temp, wav_xptr, 16);
         wav_xptr += 16;
         channels = *((int*)(temp+2)); // # of channels
         if (channels != 1) {
         	return 0; // only mono is supported
         }
         sample_rate = *((long*)(temp+4));
         if (sample_rate > 0xffff) {
         	return 0; // sample rate too high
         }
         sample_bytes = *((int *)(temp + 12));
         if (sample_bytes != 1) {
         	return 0; // only 8-bit mono supported
         }
         format_flag = 1; // format has been parsed
      }
      else if (strncmp(temp, "data", 4) == 0) {
      	if (!format_flag) {
         	return 0; // data before format chunk
         }
         wav_count = *((long*)(temp+4));
         flexAudioSetRate((unsigned int)sample_rate);
         return 1; // ready to go
      }
      else {
      	// unknown chunk type, skip
         wav_xptr += *((long*)(temp+4));
      }
   } //end of while
   return 0; //bad parse, went past end of file
}

/*
 * Plays a started WAV file. Must be called repeatedly until it returns 0.
 */
int play_wav(void)
{
	int write_size, written;

   while (wav_count) {
   	// still bytes left to be loaded into the audio driver's buffer
		if (wav_count > WAV_BUF_SIZE) {
     		write_size = WAV_BUF_SIZE;
   	}
   	else {
    		write_size = (int)wav_count;
   	}
      // grab a chunk of the ximport data and pass it to the audio driver
   	xmem2root(wav_buf, wav_xptr, write_size);
   	written = flexAudioLoad(wav_buf, write_size);
      wav_xptr += written;
      wav_count -= written; // count down the number of bytes left to play
      if (written < write_size) {
         // If we couldn't write the whole chunk, then the driver's buffer
         // is full, so we are done for now.
         if (!flexAudioPlaying()) {
         	flexAudioPlay();
         }
      	return 1; // filled buffer
      }
	}
   // all samples written to audio driver buffer
   if (!flexAudioPlaying()) {
   	return 0; // nothing playing, everything is done
   }
   else {
   	return 1; // still playing last samples out of audio driver buffer
   }
}

