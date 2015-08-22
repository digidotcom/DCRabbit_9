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
   anainconfig.c

	This sample program is for the RCM3700 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates using the Register Mode method to read
	single-ended analog input values for display as voltages. It uses the
	function call anaInConfig() and the ADS7870 Convert line to accomplish
	this task.

	For proper register addressing and commands, refer to the ADS7870
	specification.

	NOTE:	AIN7 is used as a thermistor input and therefore not used in
			this demonstration.


	Prototyping board connections
	=============================
	Connect PE4 on J3 connector to CNVRT terminal on J8 connector.

	Make sure pins 3-5 are connected on JP5, JP6, and JP7.  JP8 should
	not have any pins connected.

   Note:  PE4 is also used as infrared FIR_SEL pin.

	Instructions
	============
	1. Connect a voltmeter to the output of the power supply that your
		going to be using.
	2. Preset the voltage on the power supply to be within the voltage
		range of the A/D converter channel that your going to test. For
		A/D Channels AIN0 - AIN6, input voltage range is 0 to +20V .
	3. Power-on the controller.
	4. Connect the output of power supply to the A/D channels.
	5. Compile and run this program.
	6. Vary the voltage on a A/D channel, the voltage displayed in the STDIO
		window and voltmeter should track each other.


	Some useful macros
	==================
	// ADC instruction byte macros
	ADLSBYTE	0x00			//LS byte register
	ADMSBYTE	0x01			//MS byte register
	ADGAINMUXREG 0x04		//Gain Mux register
	ADREFOSCREG	 0x07		//Ref Osc register

	// ADC write command byte macros
	ADREADINS  0x40		//read instruction or bit
	ADWRITEINS 0x00		//write instruction or bit
	ADLEN8	0x00		   //8-bit length instruction or bit
	ADMODE0	0x00			//mode 0 read instruction req'd or bit
	ADSINGLE 0x08			//Single ended line or bit
	ADDIFFNL 0x00			//Differential line or bit

	// gain code macros
	GAIN_1 0			//gaincode for gain=1
	GAIN_2 1			//gaincode for gain=2
	GAIN_4 2			//gaincode for gain=4
	GAIN_5 3			//gaincode for gain=5
	GAIN_8 4			//gaincode for gain=8
	GAIN_10 5		//gaincode for gain=10
	GAIN_16 6		//gaincode for gain=16
	GAIN_20 7		//gaincode for gain=20

***************************************************************************/
#class auto

// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

#define STARTCHAN 0
#define ENDCHAN 6
#define GAINSET GAIN_1

//---------------------------------------------------------
// set the STDIO cursor location and display a string
//---------------------------------------------------------
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

//---------------------------------------------------------
// asserts PE4 high as the convert pin for 2 ads7870 cclk's
//---------------------------------------------------------
void adconvert(void)
{
#asm
	ld		a,(PEDRShadow)
	and	0efh				;ensure 2 adc cclks low before asserting
	ioi	ld (PEDR),a

	or		010h				;set high
	ioi	ld (PEDR),a

	ld		a,(PEDRShadow)	;this does nothing except ensure 2 adc cclks
	and	0efh				;set low
	ioi	ld (PEDR),a
#endasm
}

//---------------------------------------------------------
// second delay
//---------------------------------------------------------
nodebug
void sDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = SEC_TIMER + delay;
   while( (long) (SEC_TIMER - done_time) < 0 );
}

///////////////////////////////////////////////////////////////////////////
main ()
{
	auto int channel;
	auto unsigned int value, cmd, msbyte, lsbyte;
	auto char s[80];
	auto char display[80];
	auto float ad_inputs[ENDCHAN+1];

	brdInit();

	/// set port E bit 4 as the convert pin
	BitWrPortI(PEDR, &PEDRShadow, 0, 4);			//set output low
	BitWrPortI(PEDDR, &PEDDRShadow, 1, 4);			//configure bit 4 to output

	/// the following two lines must be set
	anaInConfig(ADWRITEINS|ADLEN8|ADCNTLREG, ADMODE0, 0); 			//make Register Mode 0
	anaInConfig(ADWRITEINS|ADLEN8|ADREFOSCREG, AD_OSC_ENABLE, 0);	//enable ref/osc reg  2.048Vref
	sDelay(1);		//delay one second here once to charge up cap


	DispStr(1, 1, "\t\t<<< Analog input channels 0 - 6: >>>");
	DispStr(1, 3, "\t AIN0\t AIN1\t AIN2\t AIN3\t AIN4\t AIN5\t AIN6");
	DispStr(1, 4, "\t------\t------\t------\t------\t------\t------\t------");

	for(;;)
	{
		for(channel = STARTCHAN; channel <= ENDCHAN; channel++)
		{
			cmd = (GAINSET*16)+(ADSINGLE|channel);						//calculate command for single-ended lines
			anaInConfig(ADWRITEINS|ADLEN8|ADGAINMUXREG, cmd, 0);	//write to gain mux register
			adconvert();		//send convert signal

			lsbyte = anaInConfig(ADREADINS|ADLEN8|ADLSBYTE, 0, 0); 	//get least significant byte
			msbyte = anaInConfig(ADREADINS|ADLEN8|ADMSBYTE, 0, 0);	//get most significant byte
			if (lsbyte & 0x01)
				value = ADOVERFLOW;						//if bit 0 set then over-range
			else
				value = (msbyte<<4)|(lsbyte>>4);		//convert to 12 bit number

			//convert to equivalent voltage
			ad_inputs[channel] = (value - _adcCalibS[channel][GAINSET].offset)*(_adcCalibS[channel][GAINSET].kconst);
		}

		/// update display
		display[0] = '\0';
		for(channel =  STARTCHAN; channel <= ENDCHAN; channel++)
		{
			sprintf(s, "\t%6.3f", ad_inputs[channel]);
			strcat(display, s);
		}
		DispStr(1, 5, display);
		sDelay(1);
	}

}
///////////////////////////////////////////////////////////////////////////

