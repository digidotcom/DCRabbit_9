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
	ad2.c
	
	This sample program is used the BL2100 series controllers.

	This program demonstrates how to access the A/D channels with the API
	anaInVolt function. With using the anaInVolt function the program
	will continuously display the voltage that is being monitored.

	!!!This program must be compiled to Flash.
	
	Instructions:
	-------------
	1. Connect a voltmeter to the output of the power supply that your
	going to use.

	2. Preset the voltage on the power supply to be within the voltage
	range of the A/D converter channel that your going to test. For A/D
	channels 0 - 10 the input voltage range is -10 to +10V or 0 to 10V
	depending on your controller.

	3. Power-on the controller.
	
	4. Connect the output of power supply to one of the A/D channels.

	5. Compile and run this program.

	6. Vary the voltage to the A/D channel, the voltage displayed in the
	STDIO	window and voltmeter should track each other.

***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif


#define NUM_CHANNELS 11 	

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
	auto int skip;
	auto float voltage;
	auto char s[80];

	brdInit();		// Required for BL2100 series boards
	
	sprintf(s, "A/D input voltages for channels 0 - %d", NUM_CHANNELS - 1 );
	DispStr(2, 2, s);
	DispStr(2, 3, "--------------------------------------");

	for(;;)
	{
		skip = 4;
		for(channel = 0; channel <= 10; channel++)
		{
			/*  Put some space where the DAC channels are on J1: */
			if( 5 == channel ) { skip = 5; }

			if((voltage = anaInVolts(channel)) < -10.99 || voltage > 10.99)
			{
				sprintf(s, "Channel = %2d Voltage = out of range (voltage range -10 to +10)  ", channel);
				DispStr(2,channel + skip, s);
			}
			else
			{
				sprintf(s, "Channel = %2d Voltage = %.4f (voltage range -10 to +10)        ", channel, voltage);
				DispStr(2,channel + skip, s);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////

