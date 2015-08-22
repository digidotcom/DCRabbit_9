/**************************************************************************

	Samples\BL2000\RS485\slave.c

   Z-World, 2001
	This sample program is for the BL20XX series controllers.

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

	serDopen(19200);		//set baud rate first
	serDwrFlush();				//clear Rx and Tx data buffers
	serDrdFlush();
	
	while (1)
	{
		while ((nIn1 = serDgetc()) == -1);	//	wait for lowercase ascii byte
		ser485Tx();									//	enable transmitter
		serDputc ( toupper(nIn1) );			//	echo uppercase byte
		while (serDgetc() == -1);				//	wait for echo
		ser485Rx();									//	disable transmitter
	}
}
