/**********************************************************
	simple485master.c
   Z-World, 2002

	This program is used with BL2500 series controllers.
	
	Description
	===========
	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	Use simple485slave.c to program the slave controller.
		
	Connections
	===========
									
	Make the following connections to each controller:

		Master   to   Slave
		  485+ <----> 485+							
		  485- <----> 485-							
		  GND  <----> GND

		
	Instructions
	============
	1.	 Make the circuitry connections as in the sample
	    circuit.
	2.  Modify the macros below if you use different ports 
	    from the ones in this program.
	3.  Compile and run this program.
	
**********************************************************/
#class auto

///////
// rs485 communications
// change serial port function here
// for example:  serDopen to serCopen
///////
#define ser485open serDopen	
#define ser485close serDclose
#define ser485wrFlush serDwrFlush	
#define ser485rdFlush serDrdFlush	
#define ser485putc serDputc	
#define ser485getc serDgetc

///////
// change serial buffer name and size here
// for example:  DINBUFSIZE to CINBUFSIZE
///////
#define DINBUFSIZE  15
#define DOUTBUFSIZE 15

///////
// change serial baud rate here
///////
#ifndef _485BAUD
#define _485BAUD 19200
#endif


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
main()
{
	auto int nIn1;
	auto char cOut;

	brdInit();						//initialize board for this demo

	ser485open(_485BAUD);		//set baud rate first
	ser485Rx();						//Disable transmitter, initially put in receive mode
	ser485wrFlush();
	ser485rdFlush();
		
	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{
			ser485Tx();											//	Enable transmitter
			ser485putc ( cOut );								//	Send lowercase byte
			while (ser485getc() == -1);					//	Wait for echo
			ser485Rx();											// Disable transmitter
			
			while ((nIn1 = ser485getc ()) == -1);		//	Wait for reply
			if (nIn1 == (toupper(cOut)))
				printf ("Upper case %c is %c\n", cOut, nIn1 );		
		}
	}
}
