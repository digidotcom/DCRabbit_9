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

	switchchar.c
 	
	This program is used with RCM3400 series controllers
	with prototyping boards.
	
	Description
	===========
  	This program transmits and then receives an ASCII string on serial 
	ports D and C. It also displays the serial data received from both
	ports in STDIO window.

	Proto-Board Connections
  	=======================
   Place wire jumpers on RS232 connector:
   
     	   TXD <---> RXC 
         RXD <---> TXC

  	Instructions
   ============
   1. Compile and Run this program.
   2. Press and release S2 and S3 on the proto-board.
	3. View data sent between serial ports in the STDIO window.
	  
**************************************************************************/
#define DS1 6			//led, port D bit 6
#define DS2 7			//led, port D bit 7
#define S2  5			//switch, port D bit 5
#define S3  4			//switch, port D bit 4
#define ON	0
#define OFF 1

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define CINBUFSIZE 	255
#define COUTBUFSIZE 	255

#define DINBUFSIZE 	255
#define DOUTBUFSIZE  255

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

/////
// read state of switches S2 or S3
/////
int pbRdSwitch(int swstate)
{
	return (BitRdPortI(PDDR, swstate));
}

/////
// write state to led DS1 or DS2
/////
void pbWrLed(int led, int onoff)
{
	BitWrPortI(PDDR, &PDDRShadow, onoff, led);
}


///////////////////////////////////////////////////////////////////////////
main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data
	auto int sw1, sw2, led1, led2;
	
	static const char string1[] = {"This message has been Rcv'd from serial port D !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port C !!!\n\n\r"};

	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();							//initialize board for this demo

	led1=led2=1;						//initialize led to off value 
	sw1=sw2=0;							//initialize switch to false value 
	
	// Initialize serial port D, set baud rate to 19200
 	serDopen(19200);
	serDwrFlush();
 	serDrdFlush();

  	// Initialize serial portC, set baud rate to 19200
   serCopen(19200);
   serCwrFlush();
   serCrdFlush();   
  
	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));
   
   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports D and C	
   //---------------------------------------------------------------------
   for(;;)
   {
		costate
		{	
			if (pbRdSwitch(S2))				//wait for switch S2 press
				abort;
			waitfor(DelayMs(50));
			if (pbRdSwitch(S2))				//wait for switch release
			{
				sw1=!sw1;
				abort;
			}
		}
		
		costate
		{	
			if (pbRdSwitch(S3))				//wait for switch S3 press
				abort;
			waitfor(DelayMs(50));
			if (pbRdSwitch(S3))				//wait for switch release
			{
				sw2=!sw2;
				abort;
			}
		}

		costate
		{	// toggle DS1 upon valid S2 press/release
			if (sw1)
			{
				pbWrLed(DS1, ON);   		//turn on DS1 led
				sw1=!sw1;
				
   			// Transmit an ascii string from serial port C to serial port D
				memcpy(buffer, string2, strlen(string2));
   			serCputs(buffer);
				memset(buffer, 0x00, sizeof(buffer));
		
   			// Get the data string that was transmitted by port C
		    	i = 0;
		     	while((ch = serDgetc()) != '\r')
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
				pbWrLed(DS1, OFF);		//turn off DS1
			}
		}
		
		costate
		{	// toggle DS2 upon valid S3 press/release
			if (sw2)
			{
				pbWrLed(DS2, ON);		//turn on DS2 led
				sw2=!sw2;

		   	// Transmit an ascii string from serial port D to serial port C
				memcpy(buffer, string1, strlen(string1));
     			serDputs(buffer);
		     	memset(buffer, 0x00, sizeof(buffer));

				// Get the data string that was transmitted by serial port D
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

	     		// Display ascii string received from serial port D
		   	printf("%s", buffer);
				pbWrLed(DS2, OFF);		//turn off DS2
			} //endif
		} //endcostate
	} //endfor
}
///////////////////////////////////////////////////////////////////////////
