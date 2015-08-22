/***************************************************************************
   ad_sample.c

   Z-World, 2002
	This sample program is used the LP3500 series controllers.

	This program demonstrates how to use the A/D low level driver on single-
	ended inputs.  The program will continuously display the voltage (average
	 of 10 samples) that is present on the A/D channels.

	!!!This program must be compiled to Flash.

	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that your
	going to be using.

	2. Preset the voltage on the power supply to be within the voltage
	range of the A/D converter channel that your going to test. For
	A/D Channels AIN0 - AIN7, input voltage range is 0 to +20V .

	3. Power-on the controller.

	4. Connect the output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Vary the voltage on a A/D channel, the voltage displayed in the STDIO
	window and voltmeter should track each other.


***************************************************************************/
#class auto					/* Change local var storage default to "auto" */

#if FLASH_COMPILE == 0
#error  "MUST COMPILE TO FLASH!"
#endif

#define NUMSAMPLES 10
#define STARTCHAN 0
#define ENDCHAN 7
#define GAINSET GAIN_1		//other gain macros
									//GAIN_1
									//GAIN_2
									//GAIN_4
									//GAIN_5
									//GAIN_8
									//GAIN_10
									//GAIN_16
									//GAIN_20

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

// read the A/D with using the low level driver
//nodebug
unsigned int sample_ad(int channel, int num_samples)
{
	auto unsigned long rawdata;
	auto unsigned int sample;
	auto unsigned int cmd;

	rawdata = 0;
	sample = num_samples;

	//convert channel and gain to ADS7870 format
	// in a direct mode
	cmd = 0x80|(GAINSET*16+(channel|0x08));

	while (sample-- > 0)
	{
		rawdata += (long) anaInDriver(cmd, 12); 	// execute low level A/D driver
	}

	return ((unsigned int) (rawdata/num_samples));
}

float convert_volt(int channel, int value)
{
	auto float voltage;

	// convert the averaged samples to a voltage
	voltage = (float)((value - _adcCalibS[channel][GAINSET].offset)*(_adcCalibS[channel][GAINSET].kconst));

	return voltage;
}

///////////////////////////////////////////////////////////////////////////

void main ()
{
	auto float voltage;
	auto int channel, chanpos;
	auto unsigned int avg_sample;
	auto char s[80];
	auto float ad_inputs[ENDCHAN+1];
	static float start_time, end_time, total_time;

	brdInit();

	//initially start up A/D oscillator and charge up
	anaIn(0,SINGLE,GAINSET);

	sprintf(s, "A/D input voltage for channels %d - %d", STARTCHAN, ENDCHAN );
	DispStr(2, 2, s);
	DispStr(2, 3, "--------------------------------------");

	DispStr(2, 5, "Voltage range 0 to +20v");

	for(;;)
	{
		for(channel = STARTCHAN; channel <= ENDCHAN; channel++)
		{
			// sample each channel
			avg_sample = sample_ad(channel, NUMSAMPLES);
			ad_inputs[channel] = convert_volt(channel, avg_sample);
		}

		for(channel = STARTCHAN; channel <= ENDCHAN; channel++)
		{
			voltage = ad_inputs[channel];
			sprintf(s, "Channel = %2d Voltage = %.3f  ", channel, voltage);
			DispStr(2, channel+ 6, s);
		}
	}
}
///////////////////////////////////////////////////////////////////////////

