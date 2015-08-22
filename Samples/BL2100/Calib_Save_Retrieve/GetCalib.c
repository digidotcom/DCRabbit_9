/**************************************************************************

	Samples\BL2100\Calib_Save_Retrieve\GetCalib.c

   Z-World, 2001
	This sample program is for the BL2100 series controllers.
	
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
	   a) Baud Rate of 19200, 8 bits, no parity and 1 stop bit.
	   b) Enable the "Local Echo" option.
	   c) Set line feed options to:  Receive = CR     Transmit = CR+LF

	Hardware setup:
	---------------
	1. Connect PC (tx) to BL2100 RXB on J4.   	
  	2. Connect PC (rcv) to BL2100 TXB on J4.
	3. Connect PC GND to BL2100 GND on J4.


	Program Instructions:
   ---------------------
   1. Compile and run this program. Verify that the message "Waiting,
      Please Send Data file!!!" message is being display in Tera Term
      display window before proceeding.

   2. From within Tera Term do the following:
   	- Select...File-->Send File--> Path and filename
   	- Select...Open 
   
	3. Once the Data file has been downloaded it will prompt you to
	   write the ADC and DAC calibration data.

	4. When the program has completed it will indicate if the calibration
	   data was successfully written or not.
   		
**************************************************************************/
#class auto

#ifndef FLASH_COMPILE
   #error "This program must be compiled to flash."
#endif


#define BINBUFSIZE  2047
#define BOUTBUFSIZE 2047

#define FILEBUFSIZE	4096	//4K max file size 
#define TERMINATOR	'\n'

//------------------------------------------------------------------------
// Here's the File Format to assist in interpreting the code
//------------------------------------------------------------------------
//
// >!!!!! Start of Calibration Table
// >Enter serial number xxxxx
// >
// >:SN#
// >xxxxx
// >
// >:ADC
// >ADC channel 0, .000123, 2341
// >  "   "
// >
// >:DAC
// >DAC channel 0, .023423, 1234
// > "   "
// >
// >!!!!! End of Calibration Table
//
// - The :ADC and :DAC are identifier TAGS being used to identify the start
//   of the each calibration table.
// - The comma is being used as a delimiter to seperate the gain and offset
//   fields.
//-------------------------------------------------------------------------

#define DAC_DATA 0
#define ADC_DATA 1

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
	serBwrite(ptr, strlen(ptr));
	sprintf(ptr, msg);
	serBwrite(ptr, strlen(ptr));
	
	// Make sure all data gets transmitted before exiting the program
	while (serBwrFree() != BOUTBUFSIZE);
   while((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04));
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

/////////////////////////////////////////////////////////////////////
// Parse the calibration data into gain and offset values
/////////////////////////////////////////////////////////////////////
int get_cal_data(unsigned long fileptr, int cal, long len)
{
	auto int channel, max_channel, i;
	auto float gain, offset;
	auto long index;
	auto char data;
	
		
	if(cal == DAC_DATA) max_channel = DACHANNELS;							
	else					  max_channel = ADCHANNELS;

	// Send linefeed to terminal for better display viewing
	sprintf(string, "\r\n");
	serBwrite(string, strlen(string));
	
	index = 0;
	for(channel = 0; channel < max_channel; channel++)
	{	
		i = 0;
		memset(string, 0x00, sizeof(string));
		do
		{
			// locate the end of the ascii line....bump thru file line-by-line 
	   	xmem2root(&data, (fileptr+index++), 1);
	   	string[i++] = data;
	 
			// exit if length of the file has been exceeded
			if((fileptr+index) - fileptr >= len) return(0);
		} while((data != TERMINATOR) && (channel < max_channel));

		// get calibration offset value
		*strchr(string, TERMINATOR) = '\0';
		offset = atof((strrchr (string, ',')+1));
		if(_xtoxErr) return(-1);	// check for atof conversion error

		// write offset value to calibration table in ram
		if(cal == ADC_DATA)	{_adcCalib[channel][1] = offset;}
		else 						{_dacCalib[channel][1] = offset;}
	
		// get calibration gain value
		*strrchr (string, ',') = '\0';
		gain = atof((strchr (string, ',')+1));
		if(_xtoxErr) return(-1); 	// check for atof conversion error

		// write gain value to calibration table in ram
		if(cal == ADC_DATA)	{_adcCalib[channel][0] = gain;}
		else 						{_dacCalib[channel][0] = gain;}

		sprintf(buffer, "Channel = %d ", channel);
		strcpy(string, buffer); 
		sprintf(buffer, "Gain    = %f  Offset = %f\n\r", gain, offset);
		strcat(string, buffer);
		serBwrite(string, strlen(string));
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

	if(cal == DAC_DATA) ptr = "DAC";
	else 	ptr = "ADC";

	if(get_cal_data(lptr, cal, len))
	{
		return(-1);
	}

	sprintf(string, "\r\nWrite %s calibration data displayed on your screen ? (Y/N) ", ptr);
	serBwrite(string, strlen(string));

	do
	{	
		if((ch = serBgetc()) != -1)
		{
			ch = toupper(ch);
		}
	
	}while(ch != 'N' && ch != 'Y');


	serBwrite("\r\n", strlen("\r\n"));
	// Get the option that the user has selected
	if (toupper(ch) == 'N' ) 	        
	{
		sprintf(string, "\r\nExiting program without rewriting calibration data!");
		serBwrite(string, strlen(string));
		while(kbhit()) getchar();        
  		exit(0);               
	}       

	if(cal == DAC_DATA)
	{
		// Write DAC Calibration data to Flash
		for(dac_channel = 0; dac_channel < DACHANNELS; dac_channel++)
		{
			anaOutEEWr(dac_channel);
		}
	}
	else
	{
		// Write ADC Calibration data to Flash
		for(adc_channel = 0; adc_channel < ADCHANNELS; adc_channel++)
		{
			anaInEEWr(adc_channel);
		}
	}
}

/////////////////////////////////////////////////////////////////////
//	Read the file from the serial port 
/////////////////////////////////////////////////////////////////////
unsigned long getfile( unsigned long xmem_ptr )
{
	auto char buffer[256];
	auto unsigned int len;
	auto unsigned int total_len, file_done;

	serBrdFlush();
	while (serBrdFree() == BINBUFSIZE);
	total_len = 0;
	file_done = FALSE;
	while(!file_done)
	{
		// Use the serial timeout to determine that the dowload is completed  
		if((len = serBread(buffer, 256, 100)) < 256)
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
	
	// read in the TAG 
	index = 0;
	do 
	{  xmem2root(&data, (fileptr + (index)), 1);
		buffer[index++] = data;
	} while(data != TERMINATOR);
		
	// replace linefeed with a NULL terminator
	*strchr(buffer, TERMINATOR) = '\0';
	
	// Check if its the TAG that we're looking for?
	if(strstr(buffer, serial_num))
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
		// make sure you have local echo enabled in Tera Term, so that you
	  	// can see what was typed.
		ch = 0;
		while((ch = serBgetc()) == -1);
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
	auto unsigned long fileptr, tptr, xmem_ptr, index;
	auto unsigned long len;
	auto int i;
	auto char serialNumber[64];
	
	//------------------------------------------------------------------------
	//		Initialize the Controller
	//------------------------------------------------------------------------
	brdInit();
	serBopen(19200);	//set baud rates for the serial ports to be used  
	serBwrFlush();		//clear Rx and Tx data buffers 
	serBrdFlush();
	serMode(0);

	//------------------------------------------------------------------------
	//		Allocate and Clear XMEM  
	//------------------------------------------------------------------------

	// Allocate XMEM memory for the file that will be read in from the PC
	xmem_ptr = xalloc(FILEBUFSIZE);

	// Clear the buffer in XMEM
	for(index =0; index < FILEBUFSIZE; index++)
	{
		root2xmem(xmem_ptr + index, "\x00", 1);
	}

	//------------------------------------------------------------------------
	//		Display the Sign-On message and get the Serial Number 
	//------------------------------------------------------------------------
	sprintf(string, "\r\n\r\nSample program to reload your analog calibration data");
	serBwrite(string, strlen(string));
	sprintf(string, "\r\n-----------------------------------------------------");
	serBwrite(string, strlen(string));
	serBputs("\r\nEnter the serial number of your controller = ");

	get_serial_number(serialNumber);
	
	//------------------------------------------------------------------------
	//		Display Send Data File Message 
	//------------------------------------------------------------------------
	sprintf(string, "\r\nWaiting...Please Send Data file!!!\n\r");
	serBwrite(string, strlen(string));

	//------------------------------------------------------------------------
	//		Download the Data File from the PC 
	//------------------------------------------------------------------------
	// Get the calibration data file from the PC and put it into XMEM
	if(!(len = getfile(xmem_ptr)))
	{
		caldata_error(string, "\r\n\nEncounter an error while reading calibration file");
		exit(1);
	}
	fileptr = xmem_ptr; 
	sprintf(string, "\r\n\n!!!!   END OF DATA FILE  !!!!\n\n\n\n\r");
	serBwrite(string, strlen(string));
	
	//------------------------------------------------------------------------
	//		Search for Serial Number entered
	//------------------------------------------------------------------------
	if((tptr = locate_serial_num(serialNumber, fileptr, len)) == 0 )
	{
		caldata_error(string, "\r\n\nExiting...Unable to locate serial# entered!!!");	
		exit(1);
	}

	sprintf(string, "Calibration Data for Serial#%s\n\n\r", serialNumber);
	serBwrite(string, strlen(string));
	for(i=0; i < strlen(serialNumber); i++)
	{
		buffer[i] = '-';
	}
	buffer[i] = '\0';
	sprintf(string, "----------------------------%s\n\n\r", buffer);
	serBwrite(string, strlen(string));

	// Set file pointer to beginning of the calibration data for the controller  
	len -= tptr - fileptr;
	fileptr = tptr;
	//------------------------------------------------------------------------
	//		Rewrite the ADC calibration data 
	//------------------------------------------------------------------------
	// Find the ADC calibration data TAG within the file
	if((tptr = find_tag(fileptr, "ADC", len)) == 0 )
	{
		caldata_error(string, "\r\n\nExiting...ADC identifier TAG missing or corrupted");	
		exit(1);
	}

	if(rewrite_calibration_data(tptr, ADC_DATA, len))
	{	
		caldata_error(string, "\r\n\nExiting...ADC floating point data missing or corrupted");
		exit(1);
	}
	
	//------------------------------------------------------------------------
	//		Rewrite the DAC calibration data 
	//------------------------------------------------------------------------
	// Find the DAC calibration data TAG within the file
	if((tptr = find_tag(fileptr, "DAC", len)) == 0)
	{
		caldata_error(string, "\r\n\nExiting...DAC identifier TAG missing or corrupted");	
		exit(1);
	}
	
	if(rewrite_calibration_data(tptr, DAC_DATA, len))
	{	
		caldata_error(string, "\r\n\nExiting...DAC floating point data missing or corrupted");
		exit(1);
	}

	//------------------------------------------------------------------------
	//		Display the Program Exit Message
	//------------------------------------------------------------------------
	sprintf(string, "\r\n\nExiting....Calibration data was successfully written!!!\n\n\r");
	serBwrite(string, strlen(string));
}
