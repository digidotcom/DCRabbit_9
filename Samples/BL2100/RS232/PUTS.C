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

	  This sample program is for the BL2100 series controllers.

	  This program transmits and then receives an ASCII string on serial 
	  ports B and C. It also displays the serial data received from both
	  ports in Dynamic C STDIO window.
	
     Add a wire jumper between the following points on the controller:
     	 On connector J4 connect TXB to RXC 
       On connector J4 connect TXC to RXB	
	 (This cross connects the two serial ports.)
            
     Run this program.
     It runs endlessly...

**************************************************************************/
#class auto


// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define CINBUFSIZE 	255
#define COUTBUFSIZE 	255

#define BINBUFSIZE 	255
#define BOUTBUFSIZE  255

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data
	
	const static char string1[] = {"This message has been Rcv'd from serial port B !!!\n\n\r"};
	const static char string2[] = {"This message has been Rcv'd from serial port C !!!\n\n\r"};


	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();		//required for BL2100 series boards

	// Initialize serial portB, set baud rate to 19200
 	serBopen(19200);
	serBwrFlush();
 	serBrdFlush();

  	// Initialize serial portC, set baud rate to 19200
   serCopen(19200);
   serCwrFlush();
   serCrdFlush();   
  
 	// Required for BL2100 series bds...must be done after serXopen function(s)  
	serMode(0);
 	
	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));
   
   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports B and C	
   //---------------------------------------------------------------------
   for(;;)
   {
   	//------------------------------------------------------------------
   	// Transmit an ascii string from serial port B to serial port C
		//------------------------------------------------------------------
		memcpy(buffer, string1, strlen(string1));
     	serBputs(buffer);
     	memset(buffer, 0x00, sizeof(buffer));

		// Get the data string that was transmitted by serial port B
     	i = 0;
     	while((ch = serCgetc()) != '\r')
     	{
			// Copy only valid RCV'd characters to the buffer
			if(ch != -1)
			{
				buffer[i++] = ch;	
			}
		}
     	buffer[i++] = ch; 		//copy '\r' to the data buffer
     	buffer[i]   = '\0';     //terminate the ascii string
     	

     	// Display ascii string received from serial port B
   	printf("%s", buffer);

		//------------------------------------------------------------------   	
   	// Transmit an ascii string from serial port C to serial port B
		//------------------------------------------------------------------	

		memcpy(buffer, string2, strlen(string2));
   	serCputs(buffer);
		memset(buffer, 0x00, sizeof(buffer));
		
   	// Get the data string that was transmitted by port C
    	i = 0;
     	while((ch = serBgetc()) != '\r')
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
