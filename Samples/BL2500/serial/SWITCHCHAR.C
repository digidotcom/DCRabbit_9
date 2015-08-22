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

	This program is used with BL2500 series controllers and demo boards.

	Description
	===========
  	This program transmits and then receives an ASCII string on serial
	ports E and F. It also displays the serial data received from both
	ports in STDIO window.

	Board Connections
  	=======================
   Place wire jumpers on RS232 connector:

     	   TXE <---> RXF
         RXE <---> TXF


	Demo Board Connections
  	=======================
	Make the following connections:

	Controller		Demo Board
	----------		----------
						Jumper H2 pins 3-5
						Jumper H2 pins 4-6
			IN00 <->	SW1
			IN01 <->	SW2
	 J11 pin 9 <-> GND
	  J7 pin 3 <-> +K (not to exceed 18V)


  	Instructions
   ============
   1. Compile and Run this program.
   2. Press and release SW1 and SW2 on the proto-board.
	3. View data sent between serial ports in the STDIO window.

**************************************************************************/

#define IN00 0
#define IN01 1
#define DS3 2			//controller led
#define DS4 3			//controller led
#define ON	0
#define OFF 1

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define EINBUFSIZE 	255
#define EOUTBUFSIZE 	255

#define FINBUFSIZE 	255
#define FOUTBUFSIZE  255

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

	static const char string1[] = {"This message has been Rcv'd from serial port F !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port E !!!\n\n\r"};

	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();							//initialize board for this demo

	led1=led2=1;						//initialize led to off value
	sw1=sw2=0;							//initialize switch to false value

	// Initialize serial port F, set baud rate to 19200
 	serFopen(19200);
	serFwrFlush();
 	serFrdFlush();

  	// Initialize serial port E, set baud rate to 19200
   serEopen(19200);
   serEwrFlush();
   serErdFlush();

	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));

   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports E and F
   //---------------------------------------------------------------------
   for(;;)
   {
		costate
		{
			if (digIn(IN00))					//wait for switch S1 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (digIn(IN00))					//wait for switch release
			{
				sw1=!sw1;						//set valid switch
				abort;
			}
		}

		costate
		{
			if (digIn(IN01))					//wait for switch S2 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (digIn(IN01))					//wait for switch release
			{
				sw2=!sw2;						//set valid switch
				abort;
			}
		}

		costate
		{	// toggle DS3 upon valid S2 press/release
			if (sw1)
			{
				ledOut(DS3, ON);   		//turn on DS3 led
				sw1=!sw1;

   			// Transmit an ascii string from serial port E to serial port F
				memcpy(buffer, string2, strlen(string2));
   			serEputs(buffer);
				memset(buffer, 0x00, sizeof(buffer));

   			// Get the data string that was transmitted by port E
		    	i = 0;
		     	while((ch = serFgetc()) != '\r')
     			{
		     		// Copy only valid RCV'd characters to the buffer
					if(ch != -1)
					{
						buffer[i++] = ch;
					}
				}
				buffer[i++] = ch;			 //copy '\r' to the data buffer
     			buffer[i]   = '\0';      //terminate the ascii string

		     	// Display ascii string received from serial port E
     			printf("%s", buffer);

		  		// Clear buffer
				memset(buffer, 0x00, sizeof(buffer));
				ledOut(DS3, OFF);		//turn off DS3
			}
		}

		costate
		{	// toggle DS4 upon valid S3 press/release
			if (sw2)
			{
				ledOut(DS4, ON);		//turn on DS4 led
				sw2=!sw2;

		   	// Transmit an ascii string from serial port F to serial port E
				memcpy(buffer, string1, strlen(string1));
     			serFputs(buffer);
		     	memset(buffer, 0x00, sizeof(buffer));

				// Get the data string that was transmitted by serial port F
     			i = 0;
		     	while((ch = serEgetc()) != '\r')
     			{
					// Copy only valid RCV'd characters to the buffer
					if(ch != -1)
					{
						buffer[i++] = ch;
					}
				}
		     	buffer[i++] = ch; 		//copy '\r' to the data buffer
		     	buffer[i]   = '\0';     //terminate the ascii string

	     		// Display ascii string received from serial port F
		   	printf("%s", buffer);
				ledOut(DS4, OFF);		//turn off DS4
			} //endif
		} //endcostate
	} //end for
}
///////////////////////////////////////////////////////////////////////////