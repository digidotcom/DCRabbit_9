/**********************************************************
	simple485master.c
   Z-World, 2003

	This program is used with RCM3700 series controllers
	with prototyping boards.

	Description
	===========
	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	This program uses TxE (PG6), RxE	(PG7) and PF5 for
	transmit enable.

	Use simple485slave.c to program the slave controller.

	Prototyping board connections
	=============================
	Make sure pins 3-5 and 4-6 are connected on JP2.

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

// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

///////
// rs485 communications
// change serial port function here
// for example:  serEopen to serCopen
///////
#define ser485open 		serEopen
#define ser485close 		serEclose
#define ser485wrFlush	serEwrFlush
#define ser485rdFlush 	serErdFlush
#define ser485putc 		serEputc
#define ser485getc 		serEgetc

///////
// change serial buffer name and size here
// for example:  EINBUFSIZE to CINBUFSIZE
///////
#define EINBUFSIZE  15
#define EOUTBUFSIZE 15

///////
// change serial baud rate here
///////
#ifndef _485BAUD
#define _485BAUD 115200
#endif

/*------------------------------------------------------------------
Enables the RS485 transmitter.
Transmitted data is echoed back into the receive data	buffer.
The echoed data may be used as an indictator for disabling the
transmitter by using one of the following	methods:

a) Byte mode...disable the transmitter after the same byte
   that is transmitted, is detected in the receive data
   buffer.
b) Block data mode...disable the transmitter after the same
   number of bytes transmitted are detected in the receive
   data buffer.
-------------------------------------------------------------------*/
nodebug
void ser485Tx( void )
{
#asm
	push	ip							;save off IP state
	ipset	1							;set interrupt priority to level 1
	ld		a,(PFDRShadow)			;get copy of shadow reg
	or		0x20						;set bit 5 high
	ld		(PFDRShadow),a			;update shadow reg
	ioi	ld	(PFDR),a				;set PF5 high
	pop	ip							;restore IP to the previous state
	ret
#endasm
}

/*------------------------------------------------------------------
Disables the RS485 transmitter.
This puts the device in listen mode which allows to receive	data
from the RS485 interface.
--------------------------------------------------------------------*/
nodebug
void ser485Rx( void )
{
#asm
	push	ip							;save off IP state
	ipset	1							;set interrupt priority to level 1
	ld		a,(PFDRShadow)			;get copy of shadow reg
	and	0xDF						;clear bit 5
	ld		(PFDRShadow),a			;update shadow reg
	ioi	ld	(PFDR),a				;set PF5 low
	pop	ip							;restore IP to the previous state
	ret
#endasm
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
main()
{
	auto int nIn1;
	auto char cOut;

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