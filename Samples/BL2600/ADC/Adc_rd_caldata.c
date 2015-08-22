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
 	adc_rd_caldata.c

	This sample program is for the BL2600 series controller.

	Description
	===========
	This program dumps the calibration data for all the ADC channels
   and the modes of operation. The program will display the calibration
   gain factor and offset value via the STDIO window for each channel
   and mode of operation.

	Instructions
	============
	1. Compile and run this program.
	2. View STDIO window for calibration data values.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}


main ()
{
   auto int gaincode, channel, opmode;
   calib cal_data;

  	// Initialize the controller
	brdInit();

   printf("\n\n");
   printf("Calibration data for ADC set for unipolar operation\n");
   printf("---------------------------------------------------\n");
   memset((char*)&cal_data, 0x00, sizeof(cal_data));
  	for(gaincode = 0; gaincode < 8; gaincode++)
   {
     	for(channel = 0; channel < 8; channel++)
   	{
   		_anaInEERd(channel, SE0_MODE, gaincode, &cal_data);
   		printf("CH = %d  GAIN = %d  Gain = %f  Offset = %d\n",
                 channel, gaincode, cal_data.gain, cal_data.offset);
     	}
   }

   printf("\n\n");
   printf("Calibration data for ADC set for bipolar operation\n");
   printf("--------------------------------------------------\n");
   memset((char*)&cal_data, 0x00, sizeof(cal_data));
  	for(gaincode = 0; gaincode < 6; gaincode++)
   {
     	for(channel = 0; channel < 8; channel++)
   	{
   		_anaInEERd(channel, SE1_MODE, gaincode, &cal_data);
   		printf("CH = %d  GAIN = %d  Gain = %f  Offset = %d\n",
                 channel, gaincode, cal_data.gain, cal_data.offset);
     	}
   }

   printf("\n\n");
   printf("Calibration data for ADC set for differential operation\n");
   printf("-------------------------------------------------------\n");
   memset((char*)&cal_data, 0x00, sizeof(cal_data));
   for(gaincode = 0; gaincode < 8; gaincode++)
   {
     	for(channel = 0; channel < 8; channel+=2)
   	{
   		_anaInEERd(channel, DIFF_MODE, gaincode, &cal_data);
   		printf("CH = %d  GAIN = %d  Gain = %f  Offset = %d\n",
                channel, gaincode, cal_data.gain, cal_data.offset);
   	}
   }

   printf("\n\n");
   printf("Calibration data for ADC set for 4-20ma operation\n");
   printf("-------------------------------------------------\n");
   memset((char*)&cal_data, 0x00, sizeof(cal_data));
   for(channel = 0; channel < 4; channel++)
   {
   	_anaInEERd(channel, mAMP_MODE, mAMP_GAINCODE, &cal_data);
   	printf("CH = %d  GAIN = %d  Gain = %f  Offset = %d\n",
             channel, gaincode, cal_data.gain, cal_data.offset);
   }

   while(!kbhit());
   while(kbhit()) getchar();
}
///////////////////////////////////////////////////////////////////////////

