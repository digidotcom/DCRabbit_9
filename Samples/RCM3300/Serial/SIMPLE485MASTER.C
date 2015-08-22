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
/**********************************************************
	simple485master.c

	This program is used with RCM3300 series controllers
	and prototyping boards.

	Description
	===========
	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	This program uses serial port C (TxC and RxC) and PD7 for
	transmit enable.

	Use simple485slave.c to program the slave controller.

	Prototyping board connections
	=============================
	Make sure pins 1-2 and 5-6 are connected on JP5, termination
   jumper.

	Make the following connections to each controller:

		Master   to   Slave
		  485+ <----> 485+
		  485- <----> 485-
		  GND  <----> GND


	Instructions
	============
	1.  Modify the macros below if you use different ports
	    from the ones in this program.
	2.  Compile and run this program.

**********************************************************/
#class auto
#use rcm33xx.lib		//sample library to use with this application

///////
// rs485 communications
// change serial port function here
// for example:  serEopen to serCopen
///////
#define ser485open 		serCopen
#define ser485close 		serCclose
#define ser485wrFlush	serCwrFlush
#define ser485rdFlush 	serCrdFlush
#define ser485putc 		serCputc
#define ser485getc 		serCgetc

///////
// change serial buffer name and size here
// for example:  EINBUFSIZE to CINBUFSIZE
///////
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

///////
// change serial baud rate here
///////
#ifndef _485BAUD
#define _485BAUD 115200
#endif

//////////////////////////////////////////////////////////
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

//////////////////////////////////////////////////////////
main()
{
	auto int nIn1;
	auto char cOut;

   #if _USER
   #warns "This sample permanantly disables the RabbitSys serial console."
   #warns "Remove power and the battery to restore serial console operation."   
	if (_sys_con_disable_serial() != 0) {
   	printf("Error: Cannot disable RabbitSys serial port.");
   }
   #endif
	brdInit();						//initialize board for this demo

	ser485Rx();						//Disable transmitter, initially put in receive mode
	ser485open(_485BAUD);		//set baud rate first
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