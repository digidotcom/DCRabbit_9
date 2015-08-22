/**************************************************************************

	dnloadcalib.c
   Z-World, 2002

	This sample program is for the BL2500 series controllers to send
	the on-board calibrations for channels AD0, DA0, and DA1 back to
	the controller.

	Description
	===========
	This program demonstrates the sending of a data file to download
	calibrations constants to a controller's user block in Flash and
	by transmitting the file using a	serial port a PC serial utility
	such as Tera Term.

	Note: To upload calibrations to be used by this program, use
			uploadcalib.c

	!!!This program must be compiled to Flash.

	The Tera Term serial utility can be downloaded from their WEB page
	located at:
	http://hp.vector.co.jp/authors/VA002416/teraterm.html

  	Hardware setup
  	==============
	1. Connect PC (tx) to the controller RXE, J6-5.
  	2. Connect PC (rx) to the controller TXE, on J6-3.
	3. Connect PC GND to GND on J6-9.

	Tera Term setup
	===============
	1. Startup Tera Term on your PC.
	2. Configure the serial parameters for the following:
	   a) Baud Rate of 19200, 8 bits, no parity and 1 stop bit.
	   b) Enable the "Local Echo" option.
	   c) Set line feed options to:  Receive = CR     Transmit = CR+LF

	Program Instructions
   ====================
   1. Compile and run this program. Verify that the message "Waiting,
      Please Send Data file" message is being display in Tera Term
      display window before proceeding.

   2. From within Tera Term do the following:
   	- Select...File-->Send File-->Path and filename
      a) Select the OPEN option within dialog box.

	3. Once the data file has been downloaded it will indicate if the
		calibration data was successfully written.

	Example data file
	================

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

**************************************************************************/
#class auto

/////
// configure your serial port connection here
//	presently configured serial port E
///
#define seropen	serEopen
#define serclose	serEclose
#define serputs	serEputs
#define serwrite	serEwrite
#define serread	serEread
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

#define FILEBUFSIZE	4096	//4K max file size
#define TERMINATOR	'\n'

char string[128];
char buffer[128];

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

/////////////////////////////////////////////////////////////////////
// Calibration data error handler
/////////////////////////////////////////////////////////////////////
void caldata_error(char *ptr, char *msg)
{
	memset(ptr, 0x20, 80);
	ptr[0]  = '\r';
	ptr[80] = '\0';
	serwrite(ptr, strlen(ptr));
	sprintf(ptr, msg);
	serwrite(ptr, strlen(ptr));

	// Make sure all data gets transmitted before exiting the program
	while (serwrFree() != OUTBUFSIZE);
   while((RdPortI(SXSR)&0x08) || (RdPortI(SXSR)&0x04));
}


/////////////////////////////////////////////////////////////////////
// Locate the calibration data within the file using known
// identifier TAGS.
/////////////////////////////////////////////////////////////////////
unsigned long find_tag(unsigned long fileptr, long len)
{
	auto char data[2047];
	auto long index,i;
	auto char *begptr, *firstline, *secondline, *saveptr;
	auto int eofile, eoline, nextcase, dnstate, channel, gaincode;

	index = 0;
	xmem2root(data, fileptr, (int)len);
	begptr = strtok(data, "\n\r");		//begin data file
	while (strncmp(begptr, "::", 2))		//look for start
	{
		begptr = strtok(NULL, "\n\r");
	}
	begptr = strtok(NULL, "\n\r");
	serputs("\n\rData file serial number is \x0");
	serwrite(begptr, strlen(begptr));

	eofile = FALSE;
	saveptr = begptr+strlen(begptr)+1;
	while (!eofile)
	{
		eoline = FALSE;
		nextcase = 0;
		begptr = strtok(saveptr, "\n\r");
		saveptr = begptr+strlen(begptr)+1;
		if (!strncmp(begptr, "ADSE", 4))
		{
			dnstate = 2;
		}
		else
			if (!strncmp(begptr, "DASE", 4))
			{
				dnstate = 3;
			}
			else
				if (!strncmp(begptr, "END", 3))
				{
					eofile = TRUE;
					eoline = TRUE;
				}
				else
					nextcase = 1;

		while (!eoline)
		{
			switch (nextcase)
			{
				case 2:		//single ended input
					firstline = strtok(NULL, "\n\r");			//get the whole line
					begptr = strtok(firstline, ",");
					_adcCalibS[channel].kconst = atof(begptr);
					begptr = strtok(NULL, ",");
					_adcCalibS[channel].offset = atof(begptr);
					saveptr = begptr+strlen(begptr)+2;
					eoline = TRUE;
					break;
				case 3:		//single ended output
					firstline = strtok(NULL, "\n\r");			//get the whole line
					begptr = strtok(firstline, ",");
					_dacCalibS[channel].kconst = atof(begptr);
					begptr = strtok(NULL, ",");
					_dacCalibS[channel].offset = atof(begptr);
					saveptr = begptr+strlen(begptr)+2;
					eoline = TRUE;
					break;
				case 1:
					channel = atoi(begptr);
					nextcase = dnstate;
					eoline = FALSE;
					break;
				case 0:
					eoline = TRUE;
					break;
			} //switch

		} //while not eoline
	} //while not eofile

	anaInEEWr(ALLCHAN);			//read all single-ended
	anaOutEEWr(ALLCHAN);			//read all single-ended

}


/////////////////////////////////////////////////////////////////////
//	Read the file from the serial port
/////////////////////////////////////////////////////////////////////
unsigned long getfile( unsigned long xmem_ptr )
{
	auto char buffer[256];
	auto unsigned int len;
	auto unsigned int total_len, file_done;

	serrdFlush();
	while (serrdFree() == INBUFSIZE);
	total_len = 0;
	file_done = FALSE;
	while(!file_done)
	{
		// Use the serial timeout to determine that the dowload is completed
		if((len = serread(buffer, 256, 100)) < 256)
		{
			file_done = TRUE;
		}
		if(!file_done)	msDelay(100);

		// Move data to large XMEM buffer
		root2xmem((xmem_ptr+total_len), buffer, len);
		total_len += len;
	}
	return(total_len);
}


/////////////////////////////////////////////////////////////////////
//	Retrieve analog calibration data and rewrite to the flash
/////////////////////////////////////////////////////////////////////
void main()
{
	auto unsigned long fileptr, tempPtr, xmemPtr, index;
	auto unsigned long len;
	auto int i;
	auto char serialNumber[64];

	//------------------------------------------------------------------------
	//		Initialize the Controller
	//------------------------------------------------------------------------
	brdInit();
	seropen(BAUDRATE);	//set baud rates for the serial ports to be used
	serwrFlush();		//clear Rx and Tx data buffers
	serrdFlush();

	//------------------------------------------------------------------------
	//		Allocate and Clear XMEM
	//------------------------------------------------------------------------

	// Allocate XMEM memory for the file that will be read in from the PC
	xmemPtr = xalloc(FILEBUFSIZE);

	// Clear the buffer in XMEM
	for(index =0; index < FILEBUFSIZE; index++)
	{
		root2xmem(xmemPtr + index, "\x00", 1);
	}

	//------------------------------------------------------------------------
	//		Download the Data File from the PC
	//------------------------------------------------------------------------
	sprintf(string, "\r\nWaiting...Please Send Data file\n\r");
	serwrite(string, strlen(string));

	// Get the calibration data file from the PC and put it into XMEM
	if(!(len = getfile(xmemPtr)))
	{
		caldata_error(string, "\r\n\nEncounter an error while reading calibration file");
		exit(1);
	}
	fileptr = xmemPtr;
	sprintf(string, "\r\n\nDownload Complete\n\n\r");
	serwrite(string, strlen(string));

	//------------------------------------------------------------------------
	//	 Parse data file and write to calibrations to flash
	//------------------------------------------------------------------------
	sprintf(string, "\r\nParsing data file\n\r");
	serwrite(string, strlen(string));

	tempPtr = find_tag(fileptr, len);

	sprintf(string, "\r\n\nExiting....Calibration data successfully written\n\n\r");
	serwrite(string, strlen(string));
	while (serwrFree() != OUTBUFSIZE);
   while((RdPortI(SXSR)&0x08) || (RdPortI(SXSR)&0x04));
	serclose();
}

