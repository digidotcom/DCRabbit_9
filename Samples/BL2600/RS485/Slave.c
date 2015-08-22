/**************************************************************************

	slave.c
   Z-World, 2004

	This sample program is for the BL2600 series controllers.

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

   Note: By default this program is setup to run on a BL2600 series
   		controller, if this program is ran on a different controller
         series, then the serMode function parameter may need to change
         for the given controller that you're using.
**************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// serial E buffer size
#define EINBUFSIZE  255
#define EOUTBUFSIZE 255


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

   // Initialize the controller
	brdInit();

	serEopen(19200);		// Set baud rate first
	serEwrFlush();			// Clear Rx and Tx data buffers
	serErdFlush();

   // Must use serial mode 1 0r 3 depending on the RS232 option you select
	serMode(1);

	while (1)
	{
		while ((nIn1 = serEgetc()) == -1);	//	Wait for lowercase ascii byte
		ser485Tx();									//	Enable transmitter
		serEputc ( toupper(nIn1) );			//	Echo uppercase byte
		while (serEgetc() == -1);				//	Wait for echo
		ser485Rx();									//	Disable transmitter
	}
}
///////////////////////////////////////////////////////////////////////////