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
   Samples\BL2000\ADC\ad4.c
	
	This sample program is used the BL20XX series controllers.

	This program demonstrates how to use the A/D low level driver. The
	program will continuously display the voltage (average of 10 samples)
	that is present on the A/D channels.

	!!!This program must be compiled to Flash.
	
	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that your
	going to be using.

	2. Preset the voltage on the power supply to be within the voltage
	range of the A/D converter channel that your going to test.
 	 * A/D Channels 0 - 3. Input voltage range is	-10 to +10 volts.
	 * A/D Channels 4 - 8. Input voltage range is	  0 to +48 volts.
	 * A/D Channels 9 - 10. Depends on the controller type that you have:
		- BL2000/BL2020  Sample program N/A for these two controllers.
		- BL2010/BL2030  Channels have input voltage range of 0 to + 48v.

	3. Power-on the controller.
	
	4. Connect the output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Vary the voltage on a A/D channel, the voltage displayed in the STDIO
	window and voltmeter should track each other.

==================================================================================
	Here's additional information on the AD driver:

	10/12 bit modes: 
	---------------------- 
	1. For faster conversion times (approx. 100us) use the low level function
		_anaIn(). See sample program AD3.C for channel to channel conversions
		or AD4.C which is setup to do multiple samples on a given channel. 
		
		The trick to obtaining the faster conversion speeds is to either do the
		channel to channel conversions continually or by sampling a given channel
		multiple times (min. of 10 times), by doing this you're basically NULL'ing
		out the one throw away cycle that the ADC chip requires to sync-up to your
		conversion request. 

		This is why anaIn takes approx. 200us, as it has to do the throw away cycle
		and do the cycle that actually gets your data! 

	8 bit mode: 
	-------------- 
	1. Only the BL2000 and BL2020 controllers have the 8 bit mode capability
   	because the AD chip being used on the BL2010/BL2030 doesn't support it. 

	2. The rawdata value is valid when using 8 bit mode, however you will have
   	to recalibrate the analog channels if you want to use our conversion from
   	rawdata to volts, as the BL2000 and BL2020 controllers are calibrated with
   	12 bit data values. (This will require changing the AD_Calib.C sample program
   	to use the low level A/D function _anaIn to be able to generate the 8 bit
   	calibration data)
   	
***************************************************************************/
#class auto

//// Initialization for the BL2000 and BL2020 controllers
#if(_BOARD_TYPE_== 0x0800 || _BOARD_TYPE_ == 0x0802)

#define NUM_CHANNELS 9 			//number of A/D channels
#define OUTPUTBITS AD12BITS	//initialize for 12 bit operation

//*****************
// Attention!!! For 8 bit mode, you'll need to recalibrate the analog 
//				    channels if you want to use our conversion from rawdata
//              to volts.
//
//#define OUTPUTBITS AD8BITS	  //initialize for 8 bit operation
//*****************                              


//// Initialization for the BL2010 and BL2030 controllers
#else
#define OUTPUTBITS AD10BITS	//initialize for 10 bit operation 
#define NUM_CHANNELS 11 		//number of A/D channels 
#endif


// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

// read the A/D with using the low level driver
nodebug
unsigned int sample_ad(int channel, int num_samples)
{
	auto unsigned long rawdata;
	auto unsigned int samples;
	auto unsigned char adc_cmd;

	samples  = num_samples;
	rawdata  = 0;
	adc_cmd  = OUTPUTBITS;	// ADC resolution
	adc_cmd 	= (channel<<4) + (adc_cmd<<2);
	
	// Do conversion of channel X which will be valid on the next A/D cycle
	_anaIn(adc_cmd, OUTPUTBITS);

	// Sample AD channel per the number of samples requested
	while(samples-- > 0)
	{	//data is for the A/D channel that was in sent in the previous adc_cmd 
		rawdata += (long) _anaIn(adc_cmd,  OUTPUTBITS); 	// execute low level A/D driver
	}
	return((unsigned int) (rawdata/num_samples));
}

///////////////////////////////////////////////////////////////////////////

void main ()
{
	auto float voltage;
	auto int channel;
	auto unsigned int avg_sample;
	auto float ad_inputs[NUM_CHANNELS];
	auto char s[80];
	
	brdInit();

	sprintf(s, "A/D input voltage for channels 0 - %d", NUM_CHANNELS - 1 );
	DispStr(2, 2, s);
	DispStr(2, 3, "--------------------------------------");

	for(;;)
	{
		for(channel = 0; channel < NUM_CHANNELS; channel++)
		{
			// sample each channel 10 times... 
			avg_sample = sample_ad(channel, 10); 
		
			// convert the averaged 10 samples to a voltage
			ad_inputs[channel] = _adcCalib[channel][0] * (_adcCalib[channel][1] - avg_sample);
		}
	
		for(channel = 0; channel < 4; channel++)
		{
			voltage = ad_inputs[channel]; 
			sprintf(s, "Channel = %2d Voltage = %9.4f (voltage range -10 to +10v)  ", channel, voltage);
			DispStr(2,channel + 4, s);
		}
		for(channel = 4; channel <  NUM_CHANNELS; channel++)
		{
			voltage = ad_inputs[channel]; 
			sprintf(s, "Channel = %2d Voltage = %9.4f (voltage range 0 to +48)     ", channel, voltage);
				DispStr(2,channel + 4, s);
		}
	}
}
