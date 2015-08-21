/**********************************************************
	master.c

   Z-World, 2000
	This sample program is used with Smart Star products.
	
	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.
	
	Use slave.c to program the slave controller.
		
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


/////////////////////////////////////////////////////////

void main()
{
	auto int nIn1;
	auto char cOut;

	brdInit();
	
	ser485open(_485BAUD);		//set baud rate first
	serMode(0);						//setup lines
	ser485wrFlush();
	ser485rdFlush();
	
	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{
			ser485wrFlush();									//	Clear echo buffer
			ser485Tx();											//	Enable transmitter
			ser485putc ( cOut );								//	Send lowercase byte
			while (ser485getc() == -1);					//	Wait for echo
			ser485Rx();											// Disable transmitter
			while ((nIn1 = ser485getc ()) == -1);		//	Wait for reply
			ser485rdFlush();									// Clear buffer
			if (nIn1 == (toupper(cOut)))
				printf ("Upper case %c is %c\n", cOut, nIn1 );		
		}
	}
}
/////////////////////////////////////////////////////////