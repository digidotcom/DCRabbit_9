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
	slave.c

	This sample program is used with RCM2200 series controllers.
	
	Connect circuitry as	described in your User Manual
	Sample Circuit Section.
	
	This program demonstrates a simple RS485 transmission
	of alpha characters to a master controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.
	
	Use master.c to program the master controller.
		
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


nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 


//////////////////////////////////////////////////////////
//  Functions to Enable/Disable the RS485 Transmitter
//////////////////////////////////////////////////////////
// Enable 485 transmissions
nodebug
void ser485Tx() 
{
	// set port D bit 3 high
	BitWrPortI(PDDR, &PDDRShadow, 1, 3);
}

// Disable 485 transmissions, put in receive mode
nodebug
void ser485Rx()
{
	// set port D bit 3 low
	BitWrPortI(PDDR, &PDDRShadow, 0, 3);	
}


/////////////////////////////////////////////////////////
// Mainline...RS485 Echo sample program 
/////////////////////////////////////////////////////////

main()
{
	auto int nIn1;
	auto char cOut;

	// Setup I/O Port D bit PD3 to control the RS485 transceiver 
	BitWrPortI(PDFR, &PDFRShadow, 0, 3);	// Set PD3 to be a normal I/O pin
	BitWrPortI(PDDCR, &PDDCRShadow, 0, 3);	// Set PD3 to drive high or low
	BitWrPortI(PDDDR, &PDDDRShadow, 1, 3); // Set PD3 to be an Output

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
