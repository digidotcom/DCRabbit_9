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

	getcalib.c

	This sample program is for the OP7200 series controllers.
	
	This program demonstrates how to retrieve your analog calibration data
 	to rewrite it back to simulated eeprom in flash with using a serial
 	utility such as Tera Term.
 	
	Note: Calibration data would have been previously saved-off to a file
	by the sample program SaveCalib.c 

	!!!This program must be compiled to Flash.
	
	The Tera Term serial utility can be downloaded from their WEB page
	located at:
	http://hp.vector.co.jp/authors/VA002416/teraterm.html	
	
	Program setup:
	--------------
	1. Startup Tera Term on your PC.
	2. Configure the serial parameters for the following:
		- Select....Setup->Serial_port
	   			   a) Baud Rate of 19200, 8 bits, no parity and 1 stop bit.

	   - Select....Setup->Terminal				 	
	   				a) Enable the "Local Echo" option.
	   				b) Set Newline options to the following:
	   				   Receive  = CR
	   				   Transmit = CR+LF

	Hardware setup:
	---------------
	1. Connect PC (tx) to OP7200 RXD on J10.   	
  	2. Connect PC (rcv) to OP7200 TXD on J10.
	3. Connect PC GND to OP7200 GND on J10.


	Program Instructions:
   ---------------------
   1. Compile and run this program. Verify that the message "Waiting,
      Please Send Data file!!!" message is being display in Tera Term
      display window before proceeding.

   2. From within Tera Term do the following:
   	- Select...File-->Send File-->Path and filename
                 a) Select the OPEN option within dialog box.   		  
   
	3. Once the Data file has been downloaded it will prompt you to
	   write the ADC and DAC calibration data.

	4. When the program has completed it will indicate if the calibration
	   data was successfully written or not.
   		
**************************************************************************/
#class auto

#ifndef FLASH_COMPILE
   #error "This program must be compiled to flash."
#endif

#define DINBUFSIZE  2047
#define DOUTBUFSIZE 2047

#define FILEBUFSIZE	8192	// 8K maximum file size 
#define TERMINATOR	'\n'

//------------------------------------------------------------------------
// 	Here's the File Format to assist in interpreting the code
//------------------------------------------------------------------------
//   !!!!! Start of Calibration Table !!!!!
//		Enter the serial number of your controller = 12345
//
//		:SN#
//		12345
//		
//		:ADC_SE
//		AD0 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int,
//		AD0 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int,
//		 :
//		 :
//		 v
//		AD7 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int,
//		AD7 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int,
//
//
//		:ADC_DIFF
//		AD0 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int,
//		AD0 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int,
//		 :
//		 :
//		 v
//		AD3 Gaincode 0-3, Float, int, Float, int, Float, int, Float, int,
//		AD3 Gaincode 4-7, Float, int, Float, int, Float, int, Float, int,
//
//		:ADC_mAMP
//		AD0 Gaincode 4, Float, int, 
//		AD0 Gaincode 4, Float, int, 
//		 :
//		 :
//		 v
//		AD7 Gaincode 4, Float, int, 
//		AD7 Gaincode 4, Float, int,
//
//   	:ADC_TouchScreen
//		X-Cal, Float, int
//		Y-Cal, Float, int
//
//		!!!!! End of Calibration Table !!!!!
//
// - The Tags :ADC_DIFF, :ADC_mAMP, and  :ADC_SE are identifier used to
//   identify the start of the each calibration table.
// - The comma is being used as a delimiter to seperate the various fields.
//
//-------------------------------------------------------------------------

#class auto

#ifndef FLASH_COMPILE
   #error "This program must be compiled to flash."
#endif

#define ADC_SE   			0
#define ADC_DIFF 			1
#define ADC_mAMP 			2
#define ADC_TouchScreen 3

char string[512];
char buffer[512];

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
	serDwrite(ptr, strlen(ptr));
	sprintf(ptr, msg);
	serDwrite(ptr, strlen(ptr));
	
	// Make sure all data gets transmitted before exiting the program
	while (serDwrFree() != DOUTBUFSIZE);
   while((RdPortI(SDSR)&0x08) || (RdPortI(SDSR)&0x04));
}

/////////////////////////////////////////////////////////////////////
// Locate the calibration data within the file using known
// identifier TAGS.
/////////////////////////////////////////////////////////////////////
unsigned long find_tag(unsigned long fileptr, char *keyword, long len)
{
	auto char data;
	auto long index,i;
	auto char *ptr;
	
	index = 0;
	for(;;)
	{
		// Look for the ':' which will be the first character of a newline
		for(;;)
		{
			xmem2root(&data, (fileptr+index++), 1);
			if(data == ':') break;

			// locate the end of the ascii line....bumping thru file line by line 
			while(data != TERMINATOR && (fileptr+ (index))- fileptr < len)
			{
		      xmem2root(&data, (fileptr+index++) , 1);
			}
			// exit if exceeded length of the file
			if((fileptr+index) - fileptr >= len) return(0);
		}

		// read-in the identifier TAG 
		i = 0;
		do 
		{
			xmem2root(&data, (fileptr + (index++)), 1);
			string[(int)i++] = data;
		} while(data != TERMINATOR);
		
		// replace linefeed with a NULL terminator
		*strchr(string, TERMINATOR) = '\0';
	
		// Check if its the TAG that we're looking for?
		if(strstr(string, keyword))
		{
			return(fileptr + index);
		}
		// Check for END OF FILE, if not EOF check again for the TAG
		else if ((fileptr+index) - fileptr >= len)  return(0);
	}
}

int get_tokens(char *ptr[], char *string, int maxTokenSets)
{
	auto int i;

	// Parse the string
	i = 0;
	
	strtok(string, ",");
	while(maxTokenSets--)
	{	
		ptr[i++] = strtok(NULL, ",");
		ptr[i++] = strtok(NULL, ",");
	}
	return 0;
}

int read_line(unsigned long fileptr, char *string,
              long *index, long len)
{
	auto int i;
	auto char data;
	
	i = 0;
	memset(string, 0x00, sizeof(string));
	do
	{
		// locate the end of the ascii line....bump thru file line-by-line 
	  	xmem2root(&data, (fileptr + (*index)++), 1);
	  	string[i++] = data;
		
		// exit if length of the file has been exceeded
		if((fileptr + (*index)) - fileptr >= len)
			return(-1);
	} while((data != TERMINATOR));
	return 0;
}

int process_ADC_data(int channel, char *ptr[],
                     int calSetPerLine, int cal, int gaincode)
{
	auto int calSetCnt;

	serDputs("\r\n");
	if(cal == ADC_mAMP)
	{
		
		sprintf(buffer, "AD%d 4-20ma", channel);
	}
	else
	{
		sprintf(buffer, "AD%d Gaincode %d-%d", channel, gaincode, gaincode+3 );
	}
	serDputs(buffer);
	for(calSetCnt = 0; calSetCnt < (calSetPerLine*2); calSetCnt+=2)
	{
		switch(cal)
		{
			case ADC_SE:
				_adcCalibSEND[channel][gaincode].gain   = atof(ptr[calSetCnt]);
				_adcCalibSEND[channel][gaincode].offset = atoi(ptr[calSetCnt+1]);
				sprintf(buffer, ", %.8f" , _adcCalibSEND[channel][gaincode].gain);
				serDputs(buffer);
				sprintf(buffer, ", %x", _adcCalibSEND[channel][gaincode++].offset);
				serDputs(buffer);
				break;

			case ADC_DIFF:
				_adcCalibDIFF[channel][gaincode].gain   = atof(ptr[calSetCnt]);
				_adcCalibDIFF[channel][gaincode].offset = atoi(ptr[calSetCnt+1]);
				sprintf(buffer, ", %.8f" , _adcCalibDIFF[channel][gaincode].gain);
				serDputs(buffer);
				sprintf(buffer, ", %x", _adcCalibDIFF[channel][gaincode++].offset);
				serDputs(buffer);
				break;

			case ADC_mAMP:
				_adcCalibmAMP[channel].gain   = atof(ptr[calSetCnt]);
				_adcCalibmAMP[channel].offset = atoi(ptr[calSetCnt+1]);				
				sprintf(buffer, ", %.8f" , _adcCalibmAMP[channel].gain);
				serDputs(buffer);
				sprintf(buffer, ", %x", _adcCalibmAMP[channel].offset);
				serDputs(buffer);
				break;
				
			default:
				return -1;
		}		
	}
	return 0;
}

int process_TS_data(int xy, char *ptr[])
{
	switch(xy)
	{
		case 0:
			serDputs("\r\n");
			sprintf(buffer, "TouchScreen X-Cal");
			serDputs(buffer);

			_adcCalibTS.x_gainfactor = atof(ptr[0]);
			_adcCalibTS.x_offset     = atoi(ptr[1]);
			

			sprintf(buffer, ", %.8f" , _adcCalibTS.x_gainfactor);
			serDputs(buffer);
			sprintf(buffer, ", %x", _adcCalibTS.x_offset);
			serDputs(buffer);
			break;

		case 1:
			serDputs("\r\n");
			sprintf(buffer, "TouchScreen Y-Cal");
			serDputs(buffer);

			_adcCalibTS.y_gainfactor = atof(ptr[0]);
			_adcCalibTS.y_offset     = atoi(ptr[1]);
			

			sprintf(buffer, ", %.8f" , _adcCalibTS.y_gainfactor);
			serDputs(buffer);
			sprintf(buffer, ", %x", _adcCalibTS.y_offset);
			serDputs(buffer);
			break;
		default:
			return -1;
	}		
	return 0;
}		
/////////////////////////////////////////////////////////////////////
// Parse the calibration data into gain and offset values
/////////////////////////////////////////////////////////////////////
int get_cal_data(unsigned long fileptr, int cal, long len)
{
	auto int channel, max_channel, max_gaincode;
	auto int i;
	auto int gaincode;
	auto int offset;
	auto long index;
	auto char data;
	auto float gain;
	auto char *ptr[10];
	auto int calSetPerLine;
	auto int calTokenSets;
	
	switch(cal)
	{
		case ADC_SE:
			max_channel   = 8;
			calSetPerLine = 4;
			break;
		case ADC_DIFF:
			max_channel   = 4;
			calSetPerLine = 4;
			break;
		case ADC_mAMP:
			max_channel   = 8;
			calSetPerLine = 1;
			break;			
		case ADC_TouchScreen:
			calSetPerLine = 2;
			break;
	}

	// Send linefeed to terminal for better display viewing
	sprintf(string, "\r\n");
	serDwrite(string, strlen(string));

	index = 0;
	if(cal == ADC_SE || cal == ADC_DIFF || cal == ADC_mAMP)
	{
		for(channel = 0; channel < max_channel; channel++)
		{
			if(read_line(fileptr, string, &index, len))
				return -1;
			if(get_tokens(ptr, string, calSetPerLine))
				return -1;
			if(process_ADC_data(channel, ptr, calSetPerLine, cal, 0))
				return -1;
		
			if(cal != ADC_mAMP)
			{
				if(read_line(fileptr, string, &index, len))
					return -1;
				if(get_tokens(ptr, string, calSetPerLine))
					return -1;
				if(process_ADC_data(channel, ptr, calSetPerLine, cal, 4))
					return -1;
			}
		}
	}
	else if(cal == ADC_TouchScreen)
	{
		for(i=0; i < 2; i++)
		{
			if(read_line(fileptr, string, &index, len))
				return -1;
			if(get_tokens(ptr, string, calSetPerLine))
				return -1;
			if(process_TS_data(i, ptr))
				return -1;
		}
	}
	return(0);
}

/////////////////////////////////////////////////////////////////////
//	Rewrite the calibration data to flash 
/////////////////////////////////////////////////////////////////////
int rewrite_calibration_data(unsigned long lptr, int cal, long len)
{
	auto int adc_channel, dac_channel;
	auto int ch;
	auto char *ptr;

	switch(cal)
	{
		case ADC_SE:
			ptr = "ADC_SE";
			break;

		case ADC_DIFF:
			ptr = "ADC_DIFF";
			break;

		case ADC_mAMP:
			ptr = "ADC_mAMP";
			break;

		case ADC_TouchScreen:
			ptr = "ADC_TouchScreen";
			break;
			
		default:
			return -1;
	}
	if(get_cal_data(lptr, cal, len))
	{
		return(-1);
	}

	sprintf(string, "\r\nWrite the %s calibration data displayed on your screen ? (Y/N) ", ptr);
	serDwrite(string, strlen(string));

	do
	{	
		if((ch = serDgetc()) != -1)
		{
			ch = toupper(ch);
		}
	
	}while(ch != 'N' && ch != 'Y');
	serDwrite("\r\n", strlen("\r\n"));

	// Get the option that the user has selected
	if (toupper(ch) == 'N' ) 	        
	{
		sprintf(string, "\r\nExiting program without rewriting calibration data!");
		serDwrite(string, strlen(string));
		while(kbhit()) getchar();        
  		exit(0);               
	}
	if(cal != ADC_TouchScreen)
		anaInEEWr(ALL_CHANNELS, cal, 0);
	else
	   TsCalibEEWr();
}

/////////////////////////////////////////////////////////////////////
//	Read the file from the serial port 
/////////////////////////////////////////////////////////////////////
unsigned long getfile( unsigned long xmem_ptr )
{
	auto char buffer[256];
	auto unsigned int len;
	auto unsigned int total_len, file_done;

	serDrdFlush();
	while (serDrdFree() == DINBUFSIZE);
	total_len = 0;
	file_done = FALSE;
	while(!file_done)
	{
		// Use the serial timeout to determine that the dowload is completed  
		if((len = serDread(buffer, 256, 100)) < 256)
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
//	Check if the serial number matches what was typed-in.
/////////////////////////////////////////////////////////////////////
int verify_serial_num(char *serial_num, unsigned long fileptr, long len)
{
	auto char buffer[128];
	auto char data;
	auto int index;
	auto int file_SN;
	auto int user_SN;
	
	// read in the serial number 
	index = 0;
	do 
	{  xmem2root(&data, (fileptr + (index)), 1);
		buffer[index++] = data;
	} while(data != TERMINATOR);
		
	// replace linefeed with a NULL terminator
	*strchr(buffer, TERMINATOR) = '\0';
	
	// Check if its the TAG that we're looking for?

	file_SN = atoi(buffer);
	user_SN = atoi(serial_num);	
	if(file_SN == user_SN)
	{
		return(1);	
	}
	else
	{
		return(0);
	}										
}

/////////////////////////////////////////////////////////////////////
//	Locate the calibration data within the data file 
/////////////////////////////////////////////////////////////////////
unsigned long locate_serial_num(char *serial_num, unsigned long fileptr, long len)
{
	auto long tptr;

	while(1)
	{
		if((tptr = find_tag(fileptr, "SN#", len)) == 0)
		{
			return(0);
		}
		len -= (tptr - fileptr);
		fileptr = tptr;
		if(verify_serial_num(serial_num, fileptr, len))
		{
			return(fileptr);
		}		
	}
}

/////////////////////////////////////////////////////////////////////
//	Get the serial number from the user 
/////////////////////////////////////////////////////////////////////
void get_serial_number(char *ptr)
{
	auto int i, ch;
	
	i  = 0;
	ch = 0;
	while(ch != '\r')
	{
		// Make sure you have local echo enabled in Tera Term, so that you
	  	// can see what was typed.
		ch = 0;
		while((ch = serDgetc()) == -1);
		if(isalnum(ch))
		{
			ptr[i++] = ch;	
		}
		// Check for a BACKSPACE...allow editing of the serial number
		if(ch == '\b' && i > 0)
		{
			--i;
		}  
	}
	ptr[i] = '\0';
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
	serDopen(19200);	//set baud rates for the serial ports to be used  
	serDwrFlush();		//clear Rx and Tx data buffers 
	serDrdFlush();
	serMode(0);

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
	//		Display the Sign-On message and get the Serial Number 
	//------------------------------------------------------------------------
	sprintf(string, "\r\n\r\nSample program to reload your analog calibration data");
	serDwrite(string, strlen(string));
	sprintf(string, "\r\n-----------------------------------------------------");
	serDwrite(string, strlen(string));
	serDputs("\r\nEnter the serial number of your controller = ");

	get_serial_number(serialNumber);
	
	//------------------------------------------------------------------------
	//		Display Send Data File Message 
	//------------------------------------------------------------------------
	sprintf(string, "\r\nWaiting...Please Send Data file!!!\n\r");
	serDwrite(string, strlen(string));

	//------------------------------------------------------------------------
	//		Download the Data File from the PC 
	//------------------------------------------------------------------------
	// Get the calibration data file from the PC and put it into XMEM
	if(!(len = getfile(xmemPtr)))
	{
		caldata_error(string, "\r\n\nEncounter an error while reading calibration file");
		exit(1);
	}
	fileptr = xmemPtr; 
	sprintf(string, "\r\n\n!!!!   END OF DATA FILE  !!!!\n\n\n\n\r");
	serDwrite(string, strlen(string));
	
	//------------------------------------------------------------------------
	//		Search for Serial Number entered
	//------------------------------------------------------------------------
	if((tempPtr = locate_serial_num(serialNumber, fileptr, len)) == 0 )
	{
		caldata_error(string, "\r\n\nExiting...Unable to locate serial# entered!!!");	
		exit(1);
	}

	sprintf(string, "Calibration Data for Serial#%s\n\n\r", serialNumber);
	serDwrite(string, strlen(string));
	for(i=0; i < strlen(serialNumber); i++)
	{
		buffer[i] = '-';
	}
	buffer[i] = '\0';
	sprintf(string, "----------------------------%s\n\n\r", buffer);
	serDwrite(string, strlen(string));

	// Set file pointer to beginning of the calibration data for the controller  
	len -= tempPtr - fileptr;
	fileptr = tempPtr;
	//------------------------------------------------------------------------
	//		Rewrite the ADC_SE calibration data 
	//------------------------------------------------------------------------
	// Find the ADC calibration data TAG within the file
	if((tempPtr = find_tag(fileptr, "ADC_SE", len)) == 0 )
	{
		caldata_error(string, "\r\n\nExiting...ADC_SE identifier TAG missing or corrupted");	
		exit(1);
	}

	if(rewrite_calibration_data(tempPtr, ADC_SE, len))
	{	
		caldata_error(string, "\r\n\nExiting...ADC_SE calibration data missing or corrupted");
		exit(1);
	}
	
	//------------------------------------------------------------------------
	//		Rewrite the ADC_DIFF calibration data 
	//------------------------------------------------------------------------
	// Find the DAC calibration data TAG within the file
	if((tempPtr = find_tag(fileptr, "ADC_DIFF", len)) == 0)
	{
		caldata_error(string, "\r\n\nExiting...ADC_DIFF identifier TAG missing or corrupted");	
		exit(1);
	}
	
	if(rewrite_calibration_data(tempPtr, ADC_DIFF, len))
	{	
		caldata_error(string, "\r\n\nExiting...ADC_DIFF calibration data missing or corrupted");
		exit(1);
	}

	//------------------------------------------------------------------------
	//		Rewrite the ADC_mAMP calibration data 
	//------------------------------------------------------------------------
	// Find the DAC calibration data TAG within the file
	if((tempPtr = find_tag(fileptr, "ADC_mAMP", len)) == 0)
	{
		caldata_error(string, "\r\n\nExiting...ADC_mAMP identifier TAG missing or corrupted");	
		exit(1);
	}
	
	if(rewrite_calibration_data(tempPtr, ADC_mAMP, len))
	{	
		caldata_error(string, "\r\n\nExiting...ADC_mAMP calibration data missing or corrupted");
		exit(1);
	}

	//------------------------------------------------------------------------
	//		Rewrite the TouchScreen calibration data 
	//------------------------------------------------------------------------
	// Find the DAC calibration data TAG within the file
	if((tempPtr = find_tag(fileptr, "ADC_TouchScreen", len)) == 0)
	{
		caldata_error(string, "\r\n\nExiting...ADC_TouchScreen identifier TAG missing or corrupted");	
		exit(1);
	}
	
	if(rewrite_calibration_data(tempPtr, ADC_TouchScreen, len))
	{	
		caldata_error(string, "\r\n\nExiting...ADC_TouchScreen calibration data missing or corrupted");
		exit(1);
	}


	//------------------------------------------------------------------------
	//		Display the Program Exit Message
	//------------------------------------------------------------------------
	sprintf(string, "\r\n\nExiting....Calibration data was successfully written!!!\n\n\r");
	serDwrite(string, strlen(string));
}
