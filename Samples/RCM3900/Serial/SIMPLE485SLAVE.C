/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/*******************************************************************************
	simple485slave.c

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	Two controllers are required for this demonstration of simple communication
	between RS-485 "slave" and "master" boards.  This sample program is for the
	RS-485 slave controller, which expects to receive lower case letters from an
	RS-485 master controller.  The RS-485 master controller should be running the
	simple485master.c sample program.

	This RS-485 slave controller receives each lower case letter, converts the
	letter to upper case and then transmits it back to the RS-485 master
	controller.  This (RS-485 slave controller) sample program also displays
	RS-485 received vs. sent characters messages in the STDIO window.

	By default, this sample program uses serial port C (TXC and RXC) for RS-485
	communication and PD7 for the RS-485 transmit enable control line.

	Prototyping board connections
	=============================
	Make sure pins 1-2 and 5-6 are connected on both the RS-485 slave and master
	controllers' prototyping board JP5 (the RS-485 termination jumper).  RS-485
	should be terminated at the far ends of the communication wire.  In this case
	there are only two boards connected via RS-485, so each should be terminated.

	Make the following connections between the two RS-485 controllers:
		Slave        Master
		-----        -----
		 485+ <----> 485+
		 485- <----> 485-
		  GND <----> GND

	Instructions
	============
	1. Ensure that the simple485master.c sample program is running on the RS-485
	   master (other) controller.

	2. If necessary, modify the macros below to select the appropriate serial
	   port.

	3. Compile and run this program in debug mode on the RS-485 slave (this)
	   controller.  Note that single-stepping is not practical due to the RS-485
	   driver / receiver fast switching time requirement.

	4. Restart the simple485master.c sample program that is running on the RS-485
	   master (other) controller.

	5. Observe the sequence of received vs. sent characters as reported in the
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

nodebug
void WaitForTxRxSwitch(void)
{
	auto unsigned long msStart;

	// Note:  Even a very brief delay on the RS-485 slave side is generally
	//        enough to prevent RS-485 master / slave transmitter contention.
	msStart = MS_TIMER;
	while (MS_TIMER == msStart);
}

void main(void)
{
	static int nIn;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	ser485Rx();					// disable transmitter, initially in receive mode
	ser485open(BAUD_RATE);	// open the RS-485 port, set baud rate
	ser485wrFlush();			// ensure no old serial data is hanging about
	ser485rdFlush();

	while (1) {
		while ((nIn = ser485getc()) == -1);	// wait for received character
		WaitForTxRxSwitch();						// time for master's Tx to Rx switch
		ser485Tx();									// enable transmitter
		ser485putc(toupper(nIn));				// send converted upper case character
		while (ser485getc() == -1);			// wait for echo
		ser485Rx();									// disable transmitter
		if (isprint(nIn)) {
			printf("Received '%c', sent '%c' via RS-485.\n", nIn, toupper(nIn));
		} else {
			printf("Received 0x%X, sent 0x%X via RS-485.\n", nIn, toupper(nIn));
		}
	}
}

