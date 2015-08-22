/**************************************************************************

	switchchar.c
 	Z-World, 2002

	This program is used with RCM3200 series controllers
	with prototyping boards.

	The sample library, \Samples\RCM3200\rcm3200.lib, contains a
	brdInit() function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
  	This program transmits and then receives an ASCII string on serial
	ports B and C. It also displays the serial data received from both
	ports in STDIO window.

	Proto-Board Connections
  	=======================
   Place wire jumpers on J5 connector:

     	   TXB <---> RXC
         RXB <---> TXC

  	Instructions
   ============
   1. Compile and Run this program.
   2. Press and release S2 and S3 on the proto-board.
	3. View data sent between serial ports in the STDIO window.

**************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

#use rcm3200.lib		//required for this demo

#define DS1 6		//led, port G bit 6
#define DS2 7		//led, port G bit 7
#define S2  1		//switch, port G bit 1
#define S3  0		//switch, port G bit 0

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define CINBUFSIZE 	255
#define COUTBUFSIZE 	255

#define BINBUFSIZE 	255
#define BOUTBUFSIZE  255

#define BAUD_RATE 	19200L

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data
	auto int sw1, sw2, led1, led2;

	static const char string1[] = {"This message has been Rcv'd from serial port B !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port C !!!\n\n\r"};

	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();							//initialize board for this demo

	led1=led2=1;						//initialize led to off value
	sw1=sw2=0;							//initialize switch to false value

	// Initialize serial portB, set baud rate to 19200
 	serBopen(BAUD_RATE);
	serBwrFlush();
 	serBrdFlush();

  	// Initialize serial portC, set baud rate to 19200
   serCopen(BAUD_RATE);
   serCwrFlush();
   serCrdFlush();

	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));

   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports B and C
   //---------------------------------------------------------------------
   for(;;)
   {
		costate
		{
			if (BitRdPortI(PGDR, S2))		//wait for switch S1 press
				abort;
			waitfor(DelayMs(50));
			if (BitRdPortI(PGDR, S2))		//wait for switch release
			{
				sw1=!sw1;
				abort;
			}
		}

		costate
		{
			if (BitRdPortI(PGDR, S3))		//wait for switch S3 press
				abort;
			waitfor(DelayMs(50));
			if (BitRdPortI(PGDR, S3))		//wait for switch release
			{
				sw2=!sw2;
				abort;
			}
		}

		costate
		{	// toggle DS1 upon valid S2 press/release
			if (sw1)
			{
				BitWrPortI(PGDR, &PGDRShadow, 0, DS1);   //turn on DS1 led
				sw1=!sw1;

   			// Transmit an ascii string from serial port C to serial port B
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
				BitWrPortI(PGDR, &PGDRShadow, 1, DS1);		//turn off DS1
			}
		}

		costate
		{	// toggle DS2 upon valid S3 press/release
			if (sw2)
			{
				BitWrPortI(PGDR, &PGDRShadow, 0, DS2);		//turn on DS2 led
				sw2=!sw2;

		   	// Transmit an ascii string from serial port B to serial port C
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
				BitWrPortI(PGDR, &PGDRShadow, 1, DS2);		//turn off DS2
			} //endif
		} //endcostate
	} //endfor

}
