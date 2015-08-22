/**********************************************************
	simple485slave.c
   Z-World, 2003

	This program is used with RCM3300 series controllers
	and prototyping boards.

	Description
	===========
	This program demonstrates a simple RS485 transmission
	of alpha characters to a master controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	This program uses serial port C (TxC and RxC) and PD7 for
	transmit enable.

	Use simple485master.c to program the master controller.

	Prototyping board connections
	=============================
	Make sure pins 1-2 and 5-6 are connected on JP5, termination
   jumper.

	Make the following connections:

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
// millisecond delay
//////////////////////////////////////////////////////////
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

//////////////////////////////////////////////////////////
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

	ser485Rx();						//	Disable transmitter, put in receive mode
	ser485open(_485BAUD);		//set baud rate first
	ser485wrFlush();
	ser485rdFlush();

	while (1)
	{
		while ((nIn1 = ser485getc()) == -1);	//	Wait for lowercase ascii byte
		ser485Tx();										//	Enable transmitter
		ser485putc ( toupper(nIn1) );				//	Echo uppercase byte
		while (ser485getc() == -1);				//	Wait for echo
		ser485Rx();										//	Disable transmitter
	}
}
/////////////////////////////////////////////////////////