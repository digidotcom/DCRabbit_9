/***************************************************************************
   ad_sample.c
   Z-World, 2003

	This sample program is for RCM3600 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates how to use the A/D low level driver on single-
	ended inputs.  The program will continuously display the voltage (average
	of 10 samples) that is present on the A/D channels.

	NOTE:	AIN7 is used as a thermistor input and therefore not used in
			this demonstration.

	!!!This program must be compiled to Flash.

	Prototyping board connections
	=============================
	Make sure pins 3-5 are connected on JP5, JP6, and JP7.  JP8 should
	not have any pins connected.

	Connect the power supply positive output to channels AIN0-AIN6 on the
	prototyping board and the negative output to GND on the controller.
	Connect a volt meter to monitor the voltage inputs.

	NOTE:	AIN7 is used as a thermistor input and therefore not used in
			this demonstration.

	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that your
	going to be using.

	2. Preset the voltage on the power supply to be within the voltage
	range of the A/D converter channel that your going to test. For
	A/D Channels AIN0 - AIN6, input voltage range is 0 to +20V .

	3. Power-on the controller.

	4. Connect the output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Vary the voltage on a A/D channel, the voltage displayed in the STDIO
	window and voltmeter should track each other.

***************************************************************************/
#class auto

#define NUMSAMPLES 1		//change number of samples here
#define STARTCHAN 0
#define ENDCHAN 6
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

	//convert channel and gain to ADS7870 format
	// in a direct mode
	cmd = 0x80|(GAINSET*16+(channel|0x08));

	for (rawdata=0, sample=num_samples; sample>0; sample--)
	{
		rawdata += (long) anaInDriver(cmd, 12); 	// execute low level A/D driver
	}
	return ((unsigned int) (rawdata/num_samples));
}

float convert_volt(int channel, int value)
{
	auto float voltage;

	// convert the averaged samples to a voltage
	voltage = (value - _adcCalibS[channel][GAINSET].offset)*(_adcCalibS[channel][GAINSET].kconst);

	return voltage;
}

//////////////////////////////////////////////////////////
// millisecond delay
//////////////////////////////////////////////////////////
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

///////////////////////////////////////////////////////////////////////////
main ()
{
	auto int channel;
	auto unsigned int avg_sample;
	auto char s[80];
	auto char display[80];
	auto float ad_inputs[ENDCHAN+1];

	brdInit();

	//initially start up A/D oscillator and charge up cap
	anaIn(0,SINGLE,GAINSET);

	DispStr(1, 1, "\t\t<<< Analog input channels 0 - 6: >>>");
	DispStr(1, 3, "\t AIN0\t AIN1\t AIN2\t AIN3\t AIN4\t AIN5\t AIN6");
	DispStr(1, 4, "\t------\t------\t------\t------\t------\t------\t------");

	for(;;)
	{
		for(channel = STARTCHAN; channel <= ENDCHAN; channel++)
		{
			// sample each channel
			avg_sample = sample_ad(channel, NUMSAMPLES);
			ad_inputs[channel] = convert_volt(channel, avg_sample);
		}

		display[0] = '\0';
		for(channel =  STARTCHAN; channel <= ENDCHAN; channel++)
		{
			sprintf(s, "\t%6.3f", ad_inputs[channel]);
			strcat(display, s);
		}

		DispStr(1, 5, display);
		msDelay(1000);				//delay one second for viewing
	}
}
///////////////////////////////////////////////////////////////////////////

