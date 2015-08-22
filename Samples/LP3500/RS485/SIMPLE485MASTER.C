/**********************************************************
	simple485master.c
   Z-World, 2002

	This program is used with LP3500 series controllers
	with prototyping boards.


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
#class auto					/* Change local var storage default to "auto" */


///////
// rs485 communications
// change serial port function here
// for example:  serFopen to serCopen
///////
#define ser485open serFopen
#define ser485close serFclose
#define ser485wrFlush serFwrFlush
#define ser485rdFlush serFrdFlush
#define ser485putc serFputc
#define ser485getc serFgetc

///////
// change serial buffer name and size here
// for example:  DINBUFSIZE to CINBUFSIZE
///////
#define FINBUFSIZE  15
#define FOUTBUFSIZE 15

///////
// change serial baud rate here
///////
#ifndef _485BAUD
#define _485BAUD 19200
#endif


//////////////////////////////////////////////////////////

void main()
{
	auto int nIn1;
	auto char cOut;

	brdInit();						//initialize board for this demo

	ser485Rx();						//Disable transmitter, initially put in receive mode
	ser485open(_485BAUD);		//set baud rate first
	ser485wrFlush();
	ser485rdFlush();
	serMode(0);

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

