/**********************************************************
	slave.c

   Z-World, 2000
	This sample program is used with Smart Star products.
	
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

**********************************************************/
#class auto

// rs485 communications
#define ser485open serDopen	
#define ser485close serDclose
#define ser485wrFlush serDwrFlush	
#define ser485rdFlush serDrdFlush	
#define ser485putc serDputc	
#define ser485getc serDgetc

// serial baud rate
#ifndef _485BAUD
#define _485BAUD 115200
#endif

// serial D buffer size
#define DINBUFSIZE  15
#define DOUTBUFSIZE 15


void msdelay (long sd)
{
	auto unsigned long t1;

	t1 = MS_TIMER + sd;
	while ((long)(MS_TIMER-t1) < 0);
}

/////////////////////////////////////////////////////////

void main()
{
	auto int nIn1;
	auto char cOut;

	brdInit();
	
	ser485open(_485BAUD);		//set baud rate first
	serMode(0);						//setup lines for rs485
	ser485wrFlush();
	ser485rdFlush();
	
	while (1)
	{
		while ((nIn1 = ser485getc()) == -1);		//	Wait for lowercase byte
		msdelay ( 50 );									//	Turn around delay
		ser485rdFlush();									//	Clear buffers
		ser485wrFlush();
		ser485Tx();											//	Enable transmitter
		ser485putc ( toupper(nIn1) );					//	Echo uppercase byte
		while ((nIn1=ser485getc()) == -1);			//	Wait for echo
		ser485Rx();											//	Disable transmitter
	}
}
/////////////////////////////////////////////////////////
