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

	This program is used with RCM3300 series controllers
	and prototyping boards.

	Description
	===========
  	This program transmits and then receives an ASCII string on serial
	ports E and F. It also displays the serial data received from both
	ports in STDIO window.

	Proto-Board Connections
  	=======================
   Place wire jumpers on RS232 connector:

     	   TXF <---> RXE
         RXF <---> TXE

  	Instructions
   ============
   1. Compile and Run this program.
   2. Press and release S2 and S3 on the proto-board.
	3. View data sent between serial ports in the STDIO window.

**************************************************************************/
#class auto
#use rcm33xx.lib

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6

#define S2  2
#define S3  3

#define ON	1
#define OFF 0

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define FINBUFSIZE 	255
#define FOUTBUFSIZE 	255

#define EINBUFSIZE 	255
#define EOUTBUFSIZE  255

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


///////////////////////////////////////////////////////////////////////////
main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data
	auto int sw1, sw2, led1, led2;

	static const char string1[] = {"This message has been Rcv'd from serial port E !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port F !!!\n\n\r"};

   #if _USER
   #warns "This sample permanantly disables the RabbitSys serial console."
   #warns "Remove power and the battery to restore serial console operation."   
	if (_sys_con_disable_serial() != 0) {
   	printf("Error: Cannot disable RabbitSys serial port.");
   }
   #endif

   //---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();							//initialize board for this demo

	led1=led2=1;						//initialize led to off value
	sw1=sw2=0;							//initialize switch to false value

   // Initialize serial port E, set baud rate to 19200
 	serEopen(19200);
	serEwrFlush();
 	serErdFlush();

  	// Initialize serial port C, set baud rate to 19200
   serFopen(19200);
   serFwrFlush();
   serFrdFlush();

	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));

   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports E and C
   //---------------------------------------------------------------------
   for(;;)
   {
		costate
		{
			if (switchIn(S2))				//wait for switch press
				abort;
			waitfor(DelayMs(50));
			if (switchIn(S2))				//wait for switch release
			{
				sw1=!sw1;
				abort;
			}
		}

		costate
		{
			if (switchIn(S3))				//wait for switch press
				abort;
			waitfor(DelayMs(50));
			if (switchIn(S3))				//wait for switch release
			{
				sw2=!sw2;
				abort;
			}
		}

		costate
		{	// toggle led upon valid switch press/release
			if (sw1)
			{
				ledOut(DS3,ON);   		//turn on led
				sw1=!sw1;

   			// Transmit an ascii string from serial port C to serial port E
				memcpy(buffer, string2, strlen(string2));
   			serFputs(buffer);
				memset(buffer, 0x00, sizeof(buffer));

   			// Get the data string that was transmitted by port F
		    	i = 0;
		     	while((ch = serEgetc()) != '\r')
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
				ledOut(DS3,OFF);		//turn off led
			}
		}

		costate
		{	// toggle led upon valid switch press/release
			if (sw2)
			{
				ledOut(DS4,ON);		//turn on led
				sw2=!sw2;

		   	// Transmit an ascii string from serial port E to serial port F
				memcpy(buffer, string1, strlen(string1));
     			serEputs(buffer);
		     	memset(buffer, 0x00, sizeof(buffer));

				// Get the data string that was transmitted by serial port E
     			i = 0;
		     	while((ch = serFgetc()) != '\r')
     			{
					// Copy only valid RCV'd characters to the buffer
					if(ch != -1)
					{
						buffer[i++] = ch;
					}
				}
		     	buffer[i++] = ch; 		//copy '\r' to the data buffer
		     	buffer[i]   = '\0';     //terminate the ascii string

	     		// Display ascii string received from serial port E
		   	printf("%s", buffer);
				ledOut(DS4,OFF);		//turn off led
			} //endif
		} //endcostate
	} //endfor
}
///////////////////////////////////////////////////////////////////////////

