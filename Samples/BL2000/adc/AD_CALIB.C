/***************************************************************************
	Samples\BL2000\ADC\ad_calib.c

   Z-World, 2001
	This sample program is used with the BL20XX series controllers.

	This program demonstrates how to recalibrate an ADC channel using two
	known	voltages to generate two coefficients, gain and offset, which 
	will be rewritten	into user block data area. It will also continuously
	display the voltage thats being monitored.

	!!! Caution !!!
	1. Make sure that you don't exceed the voltage range of a given A/D channel.
	2. This program will overwrite the calibration constants set at the factory.
	3. This program must be compiled to Flash.
	                      
	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that your using.

	2. Preset the voltage on the power supply to be within the voltage range
	of the A/D converter channel that your going to calibrate.
 		* A/D Channels 0 - 3. Input voltage range is	-10 to +10 volts.
		* A/D Channels 4 - 8. Input voltage range is	0 to +48 volts.
		* A/D Channels 9 - 10. Depends on the controller type that you have:
			- BL2000/BL2020  Sample program N/A for these two controllers.
			- BL2010/BL2030  Channels have input voltage range of 0 to + 48v.

	3. Power-on the controller.
	
	4. Connect output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Follow the calibration instructions	prompted from this program
	during execution.
	
	7. Vary the voltage (see above for voltage range) on a A/D channel,
	the voltage displayed in the STDIO window and the voltmeter should
	track each other. 
	   
***************************************************************************/
#class auto

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////
void main ()
{
	auto int rawdata, datapoint1, datapoint2;
	auto float voltequ, lovoltage, hivoltage;
	auto int channel;
	auto char buffer[128];

	brdInit();

	//check for BL2010 and BL2030 controller types
	if(_BOARD_TYPE_ == 0x0801 || _BOARD_TYPE_ == 0x0803)
	{
		DispStr(2, 2, "Please enter an input channel, 0 thru A (10)....");
		do
		{
			channel = getchar();
		} while (!(isdigit(channel)) && (channel != 'a') && (channel != 'A'));
		if ((channel == 'a') || (channel == 'A'))
		{
			printf("channel 10 chosen.");
			channel=10;
		}
		else
		{
			printf("channel %d chosen.", channel-=0x30);
		}
	}
	else
	{ // must be BL2000 and BL2020 controllers
		DispStr(2, 2, "Please enter an input channel, 0 thru 8....");
		do
		{
			channel = getchar();
		} while (!(isdigit(channel)));
		printf("channel %d chosen.", channel-=0x30);
	}
	
	/////get raw data from two known voltage points
	if(channel < 4)
	{
		DispStr(2, 4, "Adjust the voltage to appox -9.0 volts and then ENTER the actual");
	}
	else
	{
		DispStr(2, 4, "Adjust the voltage to appox +1.0 volts and then ENTER the actual");
	}
	DispStr(2, 5, "voltage being measured, (floating point value) = ");
	gets(buffer);
	datapoint1 = anaIn(channel);
	lovoltage = atof(buffer);

	if(channel < 4)
	{
		DispStr(2, 7, "Adjust the voltage to appox +9.0 volts and then ENTER the actual");
	}
	else
	{
		DispStr(2, 7, "Adjust the voltage to appox +46.0 volts and then ENTER the actual");
	}
	DispStr(2, 8, "voltage being measured, (floating point value) = ");
	gets(buffer);
	datapoint2 = anaIn(channel);
	hivoltage = atof(buffer);
		
	/////calculate gains and offsets
	if	(anaInCalib(channel, datapoint1, lovoltage, datapoint2, hivoltage))
	{
		DispStr(2,10, "Cannot make coefficients...exiting the sample program");
	}
	else
	{
		/////store coefficients into eeprom (simulated in flash memory)
		while (anaInEEWr(channel));
		
		/////read back coefficients from eeprom
		while (anaInEERd(channel));
	
		if(channel < 4)
		{
			sprintf(buffer, "Vary voltage on channel %d, -10 to +10 volts", channel);
			DispStr(2, 12, buffer);
		}
		else
		{
			sprintf(buffer, "Vary voltage on channel %d, 0 to +48 volts", channel);
			DispStr(2, 12, buffer);
		}
		while (1)
		{
			voltequ = anaInVolts(channel);
			sprintf(buffer, "Voltage at CH%d is %.4f ", channel, voltequ);
			DispStr(2, 13, buffer); 
		}
	}
}	
///////////////////////////////////////////////////////////////////////////

