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
	Samples\BL2000\ADC\ad2.c
	
	This sample program is used the BL20XX series controllers.

	This program demonstrates how to access the A/D channels with the API
	anaInVolt function. With using the anaInVolt function the program
	will continuously display the voltage that is being monitored.

	!!!This program must be compiled to Flash.
	
	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that
	your going to use.

	2. Preset the voltage on the power supply to be within the voltage
	range of the A/D converter channel that your going to test.
	
 	* A/D Channels 0 - 3. Input voltage range is	-10 to +10 volts.
	* A/D Channels 4 - 8. Input voltage range is	0 to +48 volts.
	* A/D Channels 9 - 10. Depends on the controller type that you have:

	For channels 9 and 10:
	 - BL2000/BL2020  Sample program N/A for these two controllers.
	 - BL2010/BL2030  Channels 9 and 10 have input voltage range of 0 to + 48v.

	3. Power-on the controller.
	
	4. Connect the output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Vary the voltage to the A/D channel, the voltage displayed in the
	STDIO	window and voltmeter should track each other.

***************************************************************************/
#class auto

#if(_BOARD_TYPE_ == 0x0800 || _BOARD_TYPE_ == 0x0802)
#define NUM_CHANNELS 9 		//number of A/D channels for BL2000/BL2020
#endif

#if(_BOARD_TYPE_ == 0x0801 || _BOARD_TYPE_ == 0x0803)
#define NUM_CHANNELS 11 	//number of A/D channels for BL2010/BL2030
#endif


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
	auto int channel;
	auto float voltage;
	auto char s[80];

	brdInit();
	
	sprintf(s, "A/D input voltages for channels 0 - %d", NUM_CHANNELS - 1 );
	DispStr(2, 2, s);
	DispStr(2, 3, "--------------------------------------");

	for(;;)
	{
		for(channel = 0; channel < 4; channel++)
		{
			if((voltage = anaInVolts(channel)) < -10.99 || voltage > 10.99)
			{
				sprintf(s, "Channel = %2d Voltage = out of range (voltage range -10 to +10)", channel);
				DispStr(2,channel + 4, s);
			}
			else {
				sprintf(s, "Channel = %2d Voltage = %9.4f  (voltage range -10 to +10)  ", channel, voltage);
				DispStr(2,channel + 4, s);
			}
		}
		for(channel = 4; channel <  NUM_CHANNELS; channel++)
		{
			if((voltage = anaInVolts(channel)) < -0.99 || voltage > 48.99)
			{
				sprintf(s, "Channel = %2d Voltage = out of range (voltage range 0 to +48)", channel);
				DispStr(2,channel + 4, s);
			}
			else {
				sprintf(s, "Channel = %2d Voltage = %9.4f  (voltage range 0 to +48)  ", channel, voltage);
				DispStr(2,channel + 4, s);
			}
		}
	}

}
