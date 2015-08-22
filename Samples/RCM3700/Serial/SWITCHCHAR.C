/**************************************************************************

	switchchar.c
 	Z-World, 2003

	This program is used with RCM3700 series controllers
	with prototyping boards.

	Description
	===========
  	This program transmits and then receives an ASCII string on serial
	ports C and E. It also displays the serial data received from both
	ports in STDIO window.

	Proto-Board Connections
  	=======================
	Make sure pins 1-3 and 2-4 are connected on JP2.

   Place wire jumpers on RS232 connector:

     	   TXC <---> RXE
         RXC <---> TXE

  	Instructions
   ============
   1. Compile and Run this program.
   2. Press and release S1 and S2 on the proto-board.
	3. View data sent between serial ports in the STDIO window.

**************************************************************************/
#class auto

#define DS1 6		//led, port F bit 6
#define DS2 7		//led, port F bit 7
#define S1  4		//switch, port F bit 4
#define S2  7		//switch, port B bit 7
#define ON	0
#define OFF 1

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define CINBUFSIZE 	255
#define COUTBUFSIZE 	255

#define EINBUFSIZE 	255
#define EOUTBUFSIZE  255

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

/////
// read state of switches S1 or S2
/////
int pbRdSwitch(int swstate)
{
	if (swstate == S1)
		return (BitRdPortI(PFDR, swstate));
   else
      return (BitRdPortI(PBDR, swstate));
}

/////
// write state to led DS1 or DS2
/////
void pbWrLed(int led, int onoff)
{
	BitWrPortI(PFDR, &PFDRShadow, onoff, led);
}


///////////////////////////////////////////////////////////////////////////
main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data
	auto int sw1, sw2, led1, led2;

	static const char string1[] = {"This message has been Rcv'd from serial port E !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port C !!!\n\n\r"};

	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();							//initialize board for this demo

   BitWrPortI(PEDR, &PEDRShadow, 0, 5);	//set low to enable rs232 device

	led1=led2=1;						//initialize led to off value
	sw1=sw2=0;							//initialize switch to false value

	// Initialize serial port E, set baud rate to 19200
 	serEopen(19200);
	serEwrFlush();
 	serErdFlush();

  	// Initialize serial port C, set baud rate to 19200
   serCopen(19200);
   serCwrFlush();
   serCrdFlush();

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
			if (pbRdSwitch(S1))				//wait for switch S1 press
				abort;
			waitfor(DelayMs(50));
			if (pbRdSwitch(S1))				//wait for switch release
			{
				sw1=!sw1;
				abort;
			}
		}

		costate
		{
			if (pbRdSwitch(S2))				//wait for switch S2 press
				abort;
			waitfor(DelayMs(50));
			if (pbRdSwitch(S2))				//wait for switch release
			{
				sw2=!sw2;
				abort;
			}
		}

		costate
		{	// toggle DS1 upon valid S1 press/release
			if (sw1)
			{
				pbWrLed(DS1, ON);   		//turn on DS1 led
				sw1=!sw1;

   			// Transmit an ascii string from serial port C to serial port E
				memcpy(buffer, string2, strlen(string2));
   			serCputs(buffer);
				memset(buffer, 0x00, sizeof(buffer));

   			// Get the data string that was transmitted by port C
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
				pbWrLed(DS1, OFF);		//turn off DS1
			}
		}

		costate
		{	// toggle DS2 upon valid S2 press/release
			if (sw2)
			{
				pbWrLed(DS2, ON);		//turn on DS2 led
				sw2=!sw2;

		   	// Transmit an ascii string from serial port E to serial port C
				memcpy(buffer, string1, strlen(string1));
     			serEputs(buffer);
		     	memset(buffer, 0x00, sizeof(buffer));

				// Get the data string that was transmitted by serial port E
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

	     		// Display ascii string received from serial port E
		   	printf("%s", buffer);
				pbWrLed(DS2, OFF);		//turn off DS2
			} //endif
		} //endcostate
	} //endfor
}
///////////////////////////////////////////////////////////////////////////