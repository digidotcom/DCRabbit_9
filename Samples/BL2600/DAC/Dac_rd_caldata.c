/***************************************************************************
 	dac_rd_caldata.c
   Z-World Inc 2004

   This sample program is intended for the BL2600 controller.

	Description
	===========
	This program dumps the calibration data for all the DAC channels
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
   auto int i;
   auto int channel;
   auto calib cal_data;

   // Initialize controller
	brdInit();

   printf("Calibration data, DAC unipolar voltage operation\n");
   printf("------------------------------------------------\n");

   // Clear memory
   memset((char*)&cal_data, 0x00, sizeof(cal_data));

   // Read calibration data for DAC channels 0 - 3,
   // unipolar output configuration.
    for(channel = 0; channel < 4; channel++)
   {
   	_anaOutEERd(channel, DAC_VOLT0_INDEX, &cal_data);
   	printf("CH = %d  Gain = %f  Offset = %d\n",
             channel,  cal_data.gain, cal_data.offset);
   }

   printf("\n\n");
   printf("Calibration data, DAC bipolar voltage operation\n");
   printf("-----------------------------------------------\n");

   // Clear memory
	memset((char*)&cal_data, 0x00, sizeof(cal_data));

   // Read calibration data for DAC channels 0 - 3,
   // bipolar output configuration.
   for(channel = 0; channel < 4; channel++)
   {
   	_anaOutEERd(channel, DAC_VOLT1_INDEX, &cal_data);
   	printf("CH = %d  Gain = %f  Offset = %d\n",
             channel, cal_data.gain, cal_data.offset);
   }

   printf("\n\n");
   printf("Calibration data, DAC unipolar 4-20ma operation\n");
   printf("-----------------------------------------------\n");

   // Clear memory
	memset((char*)&cal_data, 0x00, sizeof(cal_data));

   // Read calibration data for DAC channels 0 - 3,
   // bipolar output configuration.
   for(channel = 0; channel < 4; channel++)
   {
   	_anaOutEERd(channel, DAC_MA0_INDEX, &cal_data);
   	printf("CH = %d  Gain = %f  Offset = %d\n",
             channel, cal_data.gain, cal_data.offset);
   }

   printf("\n\n");
   printf("Calibration data, DAC bipolar 4-20ma operation\n");
   printf("----------------------------------------------\n");

   // Clear memory
	memset((char*)&cal_data, 0x00, sizeof(cal_data));

   // Read calibration data for DAC channels 0 - 3,
   // bipolar output configuration.
   for(channel = 0; channel < 4; channel++)
   {
   	_anaOutEERd(channel, DAC_MA1_INDEX, &cal_data);
   	printf("CH = %d  Gain = %f  Offset = %d\n",
             channel, cal_data.gain, cal_data.offset);
   }


   // Wait for any key from keyboard to exit program
   while(!kbhit());
   while(kbhit()) getchar();
}
///////////////////////////////////////////////////////////////////////////

