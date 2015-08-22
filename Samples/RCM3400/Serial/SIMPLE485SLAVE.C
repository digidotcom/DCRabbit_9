/**********************************************************
	simple485slave.c
   Z-World, 2002

	This program is used with RCM3400 series controllers
	with prototyping boards.
	
	Description
	===========
	This program demonstrates a simple RS485 transmission
	of alpha characters to a master controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.

	This program uses TxE (PG6), RxE	(PG7) and PD0 for
	transmit enable.
	
	Use simple485master.c to program the master controller.
		
	Connections
	===========
	Make the following connections:

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

///////
// rs485 communications
// change serial port function here
// for example:  serDopen to serCopen
///////
#define ser485open 		serEopen	
#define ser485close 		serEclose
#define ser485wrFlush	serEwrFlush	
#define ser485rdFlush 	serErdFlush	
#define ser485putc 		serEputc	
#define ser485getc 		serEgetc

///////
// change serial buffer name and size here
// for example:  DINBUFSIZE to CINBUFSIZE
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
	ld		a,(PDDRShadow)			;get copy of shadow reg
	or		0x01						;set bit 0 high
	ld		(PDDRShadow),a			;update shadow reg
	ioi	ld	(PDDR),a				;set PE0 high
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
	ld		a,(PDDRShadow)			;get copy of shadow reg
	and	0xfe						;clear bit 0
	ld		(PDDRShadow),a			;update shadow reg
	ioi	ld	(PDDR),a				;set PE0 low
	pop	ip							;restore IP to the previous state
	ret
#endasm
}

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
