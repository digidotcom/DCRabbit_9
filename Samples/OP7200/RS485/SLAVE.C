/**************************************************************************

	slave.c

   Z-World, 2001
	This sample program is for the OP7200 series controllers.

	Connect circuitry as	described in the User Manual in the main
	hardware chapter under Serial Communication.
	
	This program demonstrates a simple RS485 transmission
	of alpha characters to a master controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.
	
	Use master.c to program the master controller.
		
	Make the following connections:

		Master to  Slave
		485+ <---> 485+							
		485- <---> 485-							
		GND  <---> GND

**************************************************************************/
#class auto

// serial D buffer size
#define BINBUFSIZE  255
#define BOUTBUFSIZE 255


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
	auto int nIn1;
	auto char cOut;

	brdInit();				// Required for controllers

	serBopen(19200);		// Set baud rate first
	serBwrFlush();			// Clear Rx and Tx data buffers
	serBrdFlush();

	serMode(0);
	
	while (1)
	{
		while ((nIn1 = serBgetc()) == -1);	//	Wait for lowercase ascii byte
		ser485Tx();									//	Enable transmitter
		serBputc ( toupper(nIn1) );			//	Echo uppercase byte
		while (serBgetc() == -1);				//	Wait for echo
		ser485Rx();									//	Disable transmitter
	}
}
///////////////////////////////////////////////////////////////////////////
