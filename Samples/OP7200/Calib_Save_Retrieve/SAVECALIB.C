/**************************************************************************

	savecalib.c

   Z-World, 2002
	This sample program is for the OP7200 series controllers.
	
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
	1. Connect PC (tx) to OP7200 RXB on J4.   	
  	2. Connect PC (rcv) to OP7200 TXB on J4.
	3. Connect PC GND to OP7200 GND on J4.

	Smaple Program Instructions:
   ----------------------------
   1. From within Tera Term do the following:
   	- Select...File-->Log-->Path and Filename
			!!!! The next step is important, if not done if will overwrite previously 	!!!!
			!!!! saved calibration data when using same the filename. 						!!!!
   		1. Enable the File APPEND option located at the bottom of dialog box.
   		2. Select the OPEN option at the right-hand side of dialog box.
   		3. Tera Term is now ready to write all data receieved on the serial
   		   port to your specified file.
   	     	      
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
		
		:ADC_SE
		AD0 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int
		AD0 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int
		 :
		 :
		 v
		AD7 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int
		AD7 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int


		:ADC_DIFF
		AD0 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int
		AD0 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int
		 :
		 :
		 v
		AD3 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int
		AD3 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int

		:ADC_mAMP
		AD0 Gaincode 4, Float, int 
		AD0 Gaincode 4, Float, int 
		 :
		 :
		 v
		AD7 Gaincode 4, Float, int 
		AD7 Gaincode 4, Float, int

		:ADC_TouchScreen
		X-Cal, Float, int
		Y-Cal, Float, int
		
		!!!!! End of Calibration Table !!!!!
     
**************************************************************************/
#class auto

#ifndef FLASH_COMPILE
   #error "This program must be compiled to flash."
#endif

#define DINBUFSIZE  1023
#define DOUTBUFSIZE 1023

char buffer[128];

void main()
{
	auto char serialNumber[64];
	auto char buffer[128];
	auto int channel, gaincode;
	auto int i;
	auto int ch;

	brdInit();
	serDopen(19200);	//set baud rates for the serial ports to be used
	serMode(0);
	serDwrFlush();		//clear Rx and Tx data buffers 
	serDrdFlush();
	
	while(serDrdFree() != DINBUFSIZE) serDgetc();
	
	// Send data out the serial port to the PC 	
	serDputs("\r\n    ");  // send out blank line to make the file more readable 
	serDputs("\r\n!!!!! Start of Calibration Table !!!!!");
	serDputs("\r\nEnter the serial number of your controller = ");

	i  = 0;
	ch = 0;
	while(ch != '\r')
	{
		// make sure you have local echo enabled in Tera Term, so that the
	  	// serial number will written to the calibration file.
		ch = 0;
		while((ch = serDgetc()) == -1);
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
	serDputs("\r\n:SN#\r\n");
	sprintf(buffer, "%s", serialNumber);
	serDputs(buffer);
	serDputs("\r\n");

	anaInEERd(ALL_CHANNELS, SE_MODE,   1);
	anaInEERd(ALL_CHANNELS, DIFF_MODE, 1);
	anaInEERd(ALL_CHANNELS, mAMP_MODE, 1);

	serDputs("\r\n\n:ADC_SE");
	gaincode = 0;
	for(channel=0; channel < 8; channel++)
	{
		serDputs("\r\n");
		sprintf(buffer, "AD%d Gaincode 0-3", channel);
		serDputs(buffer);
		for(gaincode=0; gaincode < 8; gaincode++)
		{	
			sprintf(buffer, ", %.8f" , _adcCalibSEND[channel][gaincode].gain);
			serDputs(buffer);
			sprintf(buffer, ", %x", _adcCalibSEND[channel][gaincode].offset);
			serDputs(buffer);
			if(gaincode == 3)
			{
				serDputs("\r\n");
				sprintf(buffer, "AD%d Gaincode 4-7", channel);
				serDputs(buffer);
			}	
		}
	}

	serDputs("\r\n\n:ADC_DIFF");
	gaincode = 0;
	for(channel=0; channel < 4; channel++)
	{
		serDputs("\r\n");
		sprintf(buffer, "AD%d Gaincode 0-3", channel);
		serDputs(buffer);
		for(gaincode=0; gaincode < 8; gaincode++)
		{	
			sprintf(buffer, ", %.8f" , _adcCalibDIFF[channel][gaincode].gain);
			serDputs(buffer);
			sprintf(buffer, ", %x", _adcCalibDIFF[channel][gaincode].offset);
			serDputs(buffer);
			if(gaincode == 3)
			{
				serDputs("\r\n");
				sprintf(buffer, "AD%d Gaincode 4-7", channel);
				serDputs(buffer);
			}	
		}
	}

	serDputs("\r\n\n:ADC_mAMP");
	gaincode = 0;
	for(channel=0; channel < 8; channel++)
	{
		serDputs("\r\n");
		sprintf(buffer, "AD%d Gaincode 4", channel);
		serDputs(buffer);
		sprintf(buffer, ", %.8f" , _adcCalibmAMP[channel].gain);
		serDputs(buffer);
		sprintf(buffer, ", %x", _adcCalibmAMP[channel].offset);
		serDputs(buffer);
		if(gaincode == 3)
		{
			serDputs("\r\n");
			sprintf(buffer, "AD%d Gaincode 4", channel);
			serDputs(buffer);
		}	
	}

	serDputs("\r\n\n:ADC_TouchScreen");
	gaincode = 0;
	serDputs("\r\n");
	sprintf(buffer, "X-Cal, %.8f, %d\r\n",  _adcCalibTS.x_gainfactor, _adcCalibTS.x_offset);
	serDputs(buffer);
	sprintf(buffer, "Y-Cal, %.8f, %d\r\n",  _adcCalibTS.y_gainfactor, _adcCalibTS.y_offset);
	serDputs(buffer);
	serDputs("\r\n!!!!! End of Calibration Table !!!!!\r\n");

	
	// Make sure all data gets transmitted before exiting the program
	while (serDwrFree() != DOUTBUFSIZE);
   while((RdPortI(SDSR)&0x08) || (RdPortI(SDSR)&0x04));
   exit(0);
}


