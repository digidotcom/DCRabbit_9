/********************************************************************
	icom5wire.c

   Z-World Inc 2000

	This sample program is used with products such as
	Intellicom Series.

	You must have an RS232 device properly installed and configured
	as a 5-wire circuit.  Requires hardware modifications.

	This program demonstrates a simple serial communication
	with flow control.  Loopback is displayed in STDIO window.

	Serial port C and B on parallel port C's alternate function
	is	used. TxC and RxC become RTS and CTS.  TxB and RxB become
	Tx and Rx.

   NOTE: You must first make hardware modications, including
   	desoldering some components.
	Make the following loopback connections labeled on J7.

		 Tx (RTS) <---> Rx (CTS)		// DATA
		485+ (Tx) <---> 485- (Rx)		// FLOW CONTROL

	To test flow control, disconnect RTS from CTS while running
	this program.  Characters should stop printing in STDIO window
	and continue when RTS and CTS are connected again.

********************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

// serial B buffer size
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif


main()
{
	int nIn;
	char cOut;

	serBopen(_232BAUD);		//initialize serial port B Tx, Rx
	serMode(2);					//initialize serial port C RTS, CTS
	serBflowcontrolOn();		//enable flow control
	serBwrFlush();				//clear buffers
	serBrdFlush();

	while (1) {
		for (cOut='a'; cOut<='z'; ++cOut) {			//	Send lowercase sequence
			serBputc (cOut);								//	Send Byte
			while ((nIn=serBgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));	 //Display Byte vs. Echo
			}
		}
}

