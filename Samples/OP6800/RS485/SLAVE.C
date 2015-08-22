/**************************************************************************

	slave.c

   Z-World, 2001
	This sample program is for the OP6800 series controllers.

	Connect circuitry as	described in the User Manual in the main
	hardware chapter under Serial Communication.
	
	This program demonstrates a simple RS485 transmission
	of alpha characters to a master controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.
	
	Use master.c to program the master controller.

	Controller Setup:
	-----------------
	1. Connect power to your controller. (This controller can be
	   any Rabbit based controller with RS485) 
	
	2. Make the following connections to the Master controller:

		Master to  Slave
		485+ <---> 485+							
		485- <---> 485-							
		GND  <---> GND

**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.

// serial D buffer size
#define DINBUFSIZE  255
#define DOUTBUFSIZE 255


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

	brdInit();				//required for OP6800 series boards

	serDopen(19200);		//set baud rate first
	serDwrFlush();			//clear Rx and Tx data buffers
	serDrdFlush();

	serMode(0);
	
	while (1)
	{
		while ((nIn1 = serDgetc()) == -1);	//	wait for lowercase ascii byte
		ser485Tx();									//	enable transmitter
		serDputc ( toupper(nIn1) );			//	echo uppercase byte
		while (serDgetc() == -1);				//	wait for echo
		ser485Rx();									//	disable transmitter
	}
}
///////////////////////////////////////////////////////////////////////////
