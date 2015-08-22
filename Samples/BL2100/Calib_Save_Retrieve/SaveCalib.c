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
/**************************************************************************

	Samples\BL2100\Calib_Save_Retrieve\SaveCalib.c

	This sample program is for the BL2100 series controllers.
	
	This program demonstrates how to save-off your analog calibration
	coefficients with using a serial port and a PC serial utility such
	as Tera Term.
		
	Note: To retrieve the data and rewrite it to the controller, see
	      sample program GetCalib.c

	!!!This program must be compiled to Flash.
	
	The Tera Term serial utility can be downloaded from their WEB page
	located at:
	http://hp.vector.co.jp/authors/VA002416/teraterm.html
	
	Program setup:
	--------------
	1. Startup Tera Term on your PC.
	2. Configure the serial parameters for the following:
	   a) Baud Rate of 19200, 8 bits, no parity and 1 stop bit.
	   b) Enable the "Local Echo" option.
	   c) Set line feed options to:  Receive = CR     Transmit = CR+LF


  	Hardware setup:
	---------------
	1. Connect PC (tx) to BL2100 RXB on J4.   	
  	2. Connect PC (rcv) to BL2100 TXB on J4.
	3. Connect PC GND to BL2100 GND on J4.

	Smaple Program Instructions:
   ----------------------------
   1. From within Tera Term do the following:
   	- Select...File-->Log-->Path and Filename
		!!!! The next step is important, if not done if will overwrite previously 	!!!!
		!!!! saved calibration data when using same the filename. 						!!!!
   	- Enable...File APPEND option.
   	- Select...OPEN option.
   	     	      
   2. Compile and run this program. (At this point you should see a
      message displayed in the Tera Term display window)
       
   3. In the Tera Term display window enter the serial number of the controller,
      then press the ENTER key. (At this point you should see the calibration
      data being displayed in the Tera Term display window)
   
	!!! Attention this next step is important as Tera Term won't  		!!!
	!!! complete writing and close the file until this step is done	!!!
	
   4. Once the program is done executing, do the CLOSE option from the
      within the Tera Term: LOG window (A separate pop-up window that would
      probably be minimized at the bottom of your PC screen)

   5. Open your data file and verify that the calibration data has been 
      written properly. For example:

      !!!!! Start of Calibration Table !!!!!
		Enter the serial number of your controller = 12345

		:SN#
		12345
		
		:ADC
		ADC CHANNEL = 0, x.xxxxxxxx, x.xxxxxxxx  
		ADC CHANNEL = 1, x.xxxxxxxx, x.xxxxxxxx  	
		ADC CHANNEL = 2, x.xxxxxxxx, x.xxxxxxxx 
		ADC CHANNEL = 3, x.xxxxxxxx, x.xxxxxxxx  
		ADC CHANNEL = 4, x.xxxxxxxx, x.xxxxxxxx  
		ADC CHANNEL = 5, x.xxxxxxxx, x.xxxxxxxx 
		ADC CHANNEL = 6, x.xxxxxxxx, x.xxxxxxxx  
		ADC CHANNEL = 7, x.xxxxxxxx, x.xxxxxxxx
		ADC CHANNEL = 8, x.xxxxxxxx, x.xxxxxxxx  
		ADC CHANNEL = 9, x.xxxxxxxx, x.xxxxxxxx  
		ADC CHANNEL = 10,x.xxxxxxxx, x.xxxxxxxx
		
		:DAC
		DAC CHANNEL = 0, x.xxxxxxxx, x.xxxxxxxx   
		DAC CHANNEL = 1, x.xxxxxxxx, x.xxxxxxxx
		DAC CHANNEL = 2, x.xxxxxxxx, x.xxxxxxxx  
		DAC CHANNEL = 3, x.xxxxxxxx, x.xxxxxxxx  
		!!!!! End of Calibration Table !!!!!
     
**************************************************************************/
#class auto

#ifndef FLASH_COMPILE
   #error "This program must be compiled to flash."
#endif

#define BINBUFSIZE  1023
#define BOUTBUFSIZE 1023

char buffer[128];

void main()
{
	auto char serialNumber[64];
	auto char buffer[128];
	auto int adc_channel, dac_channel;
	auto int i;
	auto int ch;

	brdInit();
	serBopen(19200);	//set baud rates for the serial ports to be used
	serMode(0);
	serBwrFlush();		//clear Rx and Tx data buffers 
	serBrdFlush();


	while(serBrdFree() != BINBUFSIZE) serBgetc();
	
	// Send data out the serial port to the PC 	
	serBputs("\r\n    ");  // send out blank line to make the file more readable 
	serBputs("\r\n!!!!! Start of Calibration Table !!!!!");
	serBputs("\r\nEnter the serial number of your controller = ");

	i  = 0;
	ch = 0;
	while(ch != '\r')
	{
		// make sure you have local echo enabled in Tera Term, so that the
	  	// serial number will written to the calibration file.
		ch = 0;
		while((ch = serBgetc()) == -1);
		if(isalnum(ch))
		{
			serialNumber[i++] = ch;	
		}
		// Check for a BACKSPACE...allow editing of the serial number
		if(ch == '\b' && i > 0)
		{
			--i;
		}  
	}
	serialNumber[i] = '\0';
	serBputs("\r\n:SN#\r\n");
	sprintf(buffer, "%s", serialNumber);
	serBputs(buffer);
	serBputs("\r\n");

	// load ADC and DAC coefficients from flash into memory
	for(adc_channel = 0; adc_channel < ADCHANNELS; adc_channel++)
	{
		anaInEERd(adc_channel);
	}
	for(dac_channel = 0; dac_channel < DACHANNELS; dac_channel++)
	{
		anaOutEERd(dac_channel);
	}

	serBputs("\r\n:ADC");
	for(adc_channel=0; adc_channel < ADCHANNELS; adc_channel++)
	{
		sprintf(buffer, "\r\nADC CHANNEL = %d, ", adc_channel);
		serBputs(buffer);
		sprintf(buffer, "%.8f, " , _adcCalib[adc_channel][0]);
		serBputs(buffer);
		sprintf(buffer, "%.8f",    _adcCalib[adc_channel][1]);
		serBputs(buffer);
	}

	serBputs("\r\n:DAC");
	for(dac_channel=0; dac_channel < DACHANNELS; dac_channel++)
	{
		sprintf(buffer, "\r\nDAC CHANNEL = %d, ", dac_channel);
		serBputs(buffer);
		sprintf(buffer, "%.8f, ",  _dacCalib[dac_channel][0]);
		serBputs(buffer);
		sprintf(buffer, "%.8f",    _dacCalib[dac_channel][1]);
		serBputs(buffer);
	}
	serBputs("\r\n!!!!! End of Calibration Table !!!!!\r\n");

	// Make sure all data gets transmitted before exiting the program
	while (serBwrFree() != BOUTBUFSIZE);
   while((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04));
   exit(0);
}


