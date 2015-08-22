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

     Puts.c

	  This sample program is for the OP7200 series controllers.

	  This program transmits and then receives an ASCII string on serial
	  ports C and D. It also displays the serial data received from both
	  ports in Dynamic C STDIO window.

     Add a wire jumper between the following points on the controller:
     	 On connector J10 connect TXC to RXD
       On connector J10 connect TXD to RXC

     Run this program.

**************************************************************************/
#class auto

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define CINBUFSIZE 	255
#define COUTBUFSIZE 	255

#define DINBUFSIZE 	255
#define DOUTBUFSIZE  255


///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data

	static const char string1[] = {"This message has been Rcv'd from serial port C !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port D !!!\n\n\r"};


	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();		// Required for controllers

  	// Initialize serial portC, set baud rate to 19200
   serCopen(19200);
   serCwrFlush();
   serCrdFlush();

   // Initialize serial portD, set baud rate to 19200
 	serDopen(19200);
	serDwrFlush();
 	serDrdFlush();

 	// Set serial mode...must be done after serXopen function(s)
	serMode(0);

	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));

   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports C and D
   //---------------------------------------------------------------------
   for(;;)
   {
   	//------------------------------------------------------------------
   	// Transmit an ascii string from serial port C to serial port D
		//------------------------------------------------------------------
		memcpy(buffer, string1, strlen(string1));
     	serCputs(buffer);
     	memset(buffer, 0x00, sizeof(buffer));

		// Get the data string that was transmitted by serial port C
     	i = 0;
     	while((ch = serDgetc()) != '\r')
     	{
			// Copy only valid RCV'd characters to the buffer
			if(ch != -1)
			{
				buffer[i++] = ch;
			}
		}
     	buffer[i++] = ch; 		//copy '\r' to the data buffer
     	buffer[i]   = '\0';     //terminate the ascii string


     	// Display ascii string received from serial port D
   	printf("%s", buffer);

		//------------------------------------------------------------------
   	// Transmit an ascii string from serial port D to serial port C
		//------------------------------------------------------------------

		memcpy(buffer, string2, strlen(string2));
   	serDputs(buffer);
		memset(buffer, 0x00, sizeof(buffer));

   	// Get the data string that was transmitted by port D
    	i = 0;
     	while((ch = serCgetc()) != '\r')
     	{
     		// Copy only valid RCV'd characters to the buffer
			if(ch != -1)
			{
				buffer[i++] = ch;
			}
		}
		buffer[i++] = ch;			 //copy '\r' to the data buffer
     	buffer[i]   = '\0';      //terminate the ascii string

     	// Display ascii string received from serial port C
     	printf("%s", buffer);

  		// Clear buffer
		memset(buffer, 0x00, sizeof(buffer));
   }
}

