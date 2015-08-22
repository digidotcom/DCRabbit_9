/**************************************************************************

	Samples\RabbitFLEX_SBC40\rs485\slave.c
   Rabbit Semiconductor, 2006

	This sample program is for the RabbitFLEX SBC40 series controllers.
   You must have the RS485 option installed to use this sample
   program.

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
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// serial C buffer size
#define CINBUFSIZE  255
#define COUTBUFSIZE 255


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

	serCopen(19200);		// Set baud rate first
	serCwrFlush();			// Clear Rx and Tx data buffers
	serCrdFlush();

	while (1)
	{
		while ((nIn1 = serCgetc()) == -1);	//	Wait for lowercase ascii byte
		ser485Tx();									//	Enable transmitter
		serCputc ( toupper(nIn1) );			//	Echo uppercase byte
		while (serCgetc() == -1);				//	Wait for echo
		ser485Rx();									//	Disable transmitter
	}
}
///////////////////////////////////////////////////////////////////////////