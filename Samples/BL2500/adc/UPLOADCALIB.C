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
	uploadcalib.c

	This sample program is for the BL2500 series controllers to get
	the on-board calibrations for channels AD0, DA0, and DA1 from
	the controller.

	Description
	===========
	This program demonstrates the reading of calibrations constants from
	a controller's user block in Flash and transmitting the file using a
	serial port a PC serial utility such as Tera Term.

	Note: To download calibrations created by this program, use
			dnloadcalib.c

	!!!This program must be compiled to Flash.

	The Tera Term serial utility can be downloaded from their WEB page
	located at:
	http://hp.vector.co.jp/authors/VA002416/teraterm.html

  	Hardware setup
  	==============
	1. Connect PC (tx) to the controller RXE, J6-5.
  	2. Connect PC (rx) to the controller TXE, J6-3.
	3. Connect PC GND to GND on J6-9.

	Tera Term setup
	===============
	1. Startup Tera Term on your PC.
	2. Configure the serial parameters for the following:
	   a) Baud Rate of 19200, 8 bits, no parity and 1 stop bit.
	   b) Enable the "Local Echo" option.
	   c) Set line feed options to:  Receive = CR     Transmit = CR+LF
 	3. Select File-->Log-->Path and Filename
		The next steps are important, if not done if will overwrite previously
		saved calibration data when using same the filename.
   	a) Enable the File APPEND option located at the bottom of dialog box.
   	b) Select the OPEN option at the right-hand side of dialog box.
   	c) Tera Term is now ready to log all data received on the serial
   		port to your specified file.

	Program Instructions
	====================
   1. Compile and run this program. (At this point you should see a
      message displayed in the Tera Term display window)

   2. In the Tera Term display window enter the serial number of the controller,
      then press the ENTER key. (At this point you should see the calibration
      data being displayed in the Tera Term display window)

   3. Once the program is done executing, choose CLOSE from the
      within the Tera Term LOG window (A separate pop-up window that would
      probably be minimized at the bottom of your PC screen)
      This will finish logging and close the file.

   4. Open your data file and verify that the calibration data has been
      written properly. For example:


	Serial port transmission
	========================
	Uploading calibration table . . .
	Enter the serial number of your controller = 9MN234

	::
	SN9MN234
	ADSE
	0
	float_num,float_num,
	DASE
	0
	float_num,float_num,
	1
	float_num,float_num,
	END
	::
	End of table upload

***************************************************************************/
#class auto

#define PRINTDEBUG 0		//define as 1 to show debug

/////
// define your start and channels here
//	presently configured to prototyping boards defaults
///
#define STARTAD	0		//start single-ended input lines
#define ENDAD		0		//end single-ended lines
#define STARTDA	0		//start single-ended output lines
#define ENDDA		1		//end single-ended lines

/////
// configure your serial port connection here
//	presently configured serial port E
///
#define seropen	serEopen
#define serclose	serEclose
#define serputs	serEputs
#define serwrite	serEwrite
#define sergetc	serEgetc
#define serrdFlush serErdFlush
#define serwrFlush serEwrFlush
#define serrdFree	serErdFree
#define serwrFree	serEwrFree
#define SXSR SESR
#define EINBUFSIZE 255
#define EOUTBUFSIZE 255

#define INBUFSIZE EINBUFSIZE
#define OUTBUFSIZE EOUTBUFSIZE
#define BAUDRATE 19200l

main()
{
	auto int channel, gaincode, ch;
	char tempbuf[64];
	char sendbuf[128];
	char i;

	brdInit();
	seropen(BAUDRATE);
	serrdFlush();
	serwrFlush();

	if (PRINTDEBUG)
	{
		printf("Block size = 0x%x\n", 4096*GetIDBlockSize());

		printf("ADC single-ended address 0x%x\n", ADC_CALIB_ADDRS);		//single-ended address start
		printf("DAC single-ended address 0x%x\n", DAC_CALIB_ADDRS);		//single-ended address start

		printf("\nRead constants from user block\n");
	}
	anaInEERd(ALLCHAN);			//read all single-ended input
	anaOutEERd(ALLCHAN);			//read all single-ended output

	memset(sendbuf, '\x0', sizeof(sendbuf));
	while(serrdFree() != INBUFSIZE) sergetc();

	// Send data out the serial port to the PC
	serputs("Uploading calibration table . . .\n\r\x0");
	serputs("Enter the serial number of your controller = \x0");

	ch = 0;
	i=0;
	while (ch != '\r')
	{
		// make sure you have local echo enabled in Tera Term, so that the
	  	// serial number will written to the calibration file.
		while ((ch = sergetc()) == -1);

		// Check for a BACKSPACE...allow editing of the serial number
		if (ch == '\b' && i > 0)
		{
			--i;
		}
		else
			tempbuf[i++] = ch;
	}

	tempbuf[i] = '\x0';
	sprintf(sendbuf, "\n\r::\n\rSN");
	strcat(sendbuf, tempbuf);
	strcat(sendbuf, "\n\r\x0");
	serputs(sendbuf);

	////
	////
	if (PRINTDEBUG) printf("\n\nFormatting single-ended channels to transmit\n");
	sprintf(sendbuf, "ADSE\n\r\x0");
	serputs(sendbuf);

	for(channel = STARTAD; channel <= ENDAD; channel++)
	{
		memset(sendbuf, '\x0', sizeof(sendbuf));
		sprintf(sendbuf, "%d\n\r\x0", channel);
		sprintf(tempbuf, "%9.6f,%9.6f,", _adcCalibS[channel].kconst, _adcCalibS[channel].offset);
		strcat(sendbuf, tempbuf);
		strcat(sendbuf, "\n\r\x0");
		serwrite(sendbuf, sizeof(sendbuf));
	}

	////
	////
	if (PRINTDEBUG) printf("\n\nFormatting single-ended channels to transmit\n");
	sprintf(sendbuf, "DASE\n\r\x0");
	serputs(sendbuf);

	for(channel = STARTDA; channel <= ENDDA; channel++)
	{
		memset(sendbuf, '\x0', sizeof(sendbuf));
		sprintf(sendbuf, "%d\n\r\x0", channel);
		sprintf(tempbuf, "%9.6f,%9.6f,", _dacCalibS[channel].kconst, _dacCalibS[channel].offset);
		strcat(sendbuf, tempbuf);
		strcat(sendbuf, "\n\r\x0");
		serwrite(sendbuf, sizeof(sendbuf));
	}

	sprintf(sendbuf, "END\n\r::\n\r\x0");
	serputs(sendbuf);

	serEputs("End of table upload\n\n\r\x0");

	// Make sure all data gets transmitted before exiting the program
	while (serwrFree() != OUTBUFSIZE);
   while((RdPortI(SXSR)&0x08) || (RdPortI(SXSR)&0x04));
	serclose();
}