/*******************************************************************************
	simple485master.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	Two controllers are required for this demonstration of simple communication
	between RS-485 "master" and "slave" boards.  This sample program is for the
	RS-485 master controller, which transmits lower case letters to an RS-485
	slave controller.

	The RS-485 slave controller should be running the simple485slave.c sample
	program.  The RS-485 slave controller is expected to receive each lower case
	letter, convert the letter to upper case and then transmit it back to the
	RS-485 master controller.

	This (RS-485 master controller) sample program then displays RS-485 sent vs.
	received characters messages in the STDIO window.

	By default, this sample program uses serial port C (TXC and RXC) for RS-485
	communication and PD7 for the RS-485 transmit enable control line.

	Prototyping board connections
	=============================
	Make sure pins 1-2 and 5-6 are connected on both the RS-485 master and slave
	controllers' prototyping board JP5 (the RS-485 termination jumper).  RS-485
	should be terminated at the far ends of the communication wire.  In this case
	there are only two boards connected via RS-485, so each should be terminated.

	Make the following connections between the two RS-485 controllers:
		Master        Slave
		------        -----
		  485+ <----> 485+
		  485- <----> 485-
		   GND <----> GND

	Instructions
	============
	1. Ensure that the simple485slave.c sample program is running on the RS-485
	   slave (other) controller.

	2. If necessary, modify the macros below to select the appropriate serial
	   port.

	3. Compile and run this program in debug mode on the RS-485 master (this)
	   controller.  Note that single-stepping is not practical due to the RS-485
	   driver / receiver fast switching time requirement.

	4. Observe the sequence of sent vs. received characters as reported in the
	   STDIO window.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

///////
// RS-485 Communications Set Up
//  The RCM3300 prototyping board's RS-485 connections use serial port C.
//  If using a different board, change serial port function here as necessary.
//  For example:  serCopen to serEopen.
///////
#define ser485open		serCopen
#define ser485close		serCclose
#define ser485wrFlush	serCwrFlush
#define ser485rdFlush	serCrdFlush
#define ser485putc		serCputc
#define ser485getc		serCgetc

///////
// Serial Buffers Set Up
//  The RCM3300 prototyping board's RS-485 connections use serial port C.
//  If using a different board, change buffers' name and size as necessary.
//  For example:  CINBUFSIZE to EINBUFSIZE.
//  Note:  The serial port referenced here should match the one set up above.
///////
#define CINBUFSIZE	15
#define COUTBUFSIZE	15

///////
// change serial baud rate here
///////
#define BAUD_RATE	115200L

void main(void)
{
	static char cOut;
	static int nIn;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	ser485Rx();					// disable transmitter, initially in receive mode
	ser485open(BAUD_RATE);	// open the RS-485 port, set baud rate
	ser485wrFlush();			// ensure no old serial data is hanging about
	ser485rdFlush();

	cOut = 'a';	// our initial lower case letter
	while (1) {
		ser485Tx();									// enable transmitter
		ser485putc(cOut);							// send lower case letter
		while (ser485getc() == -1);			// wait for our echo to complete
		ser485Rx();									// disable transmitter
		while ((nIn = ser485getc()) == -1);	// wait for reply
		if (isprint(nIn)) {
			printf("Sent '%c', received '%c' via RS-485.\n", cOut, nIn);
		} else {
			printf("Sent '%c', received 0x%X via RS-485.\n", cOut, nIn);
		}
		if ('z' == cOut) {
			cOut = 'a';	// reset back to our initial lower case letter
		} else {
			++cOut;		// increment to the next lower case letter
		}
	}
}

