/**********************************************************
	Samples\RCM2100\slave2.c

   Z-World, 2001
	This sample program is used with RCM2100 series controllers.
	
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
#define ser485open serBopen	
#define ser485close serBclose
#define ser485wrFlush serBwrFlush	
#define ser485rdFlush serBrdFlush	
#define ser485putc serBputc	
#define ser485getc serBgetc

// serial baud rate
#ifndef _485BAUD
#define _485BAUD 115200
#endif

// serial B buffer size
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15


void msdelay (long sd)
{
	auto unsigned long t1;

	t1=MS_TIMER;
	for (t1=MS_TIMER; MS_TIMER<(sd+t1); );
}

//////////////////////////////////////////////////////////
//  Functions to Enable/Disable the RS485 Transmitter
//////////////////////////////////////////////////////////
// Enable 485 transmissions
nodebug
void ser485Tx() 
{
	// set port D bit 0 high
	BitWrPortI(PDDR, &PDDRShadow, 1, 0);
}

// Disable 485 transmissions, put in receive mode
nodebug
void ser485Rx()
{
	// set port D bit 0 low
	BitWrPortI(PDDR, &PDDRShadow, 0, 0);	
}


/////////////////////////////////////////////////////////
// Mainline...RS485 Echo sample program 
/////////////////////////////////////////////////////////

void main()
{
	auto int nIn1;
	auto char cOut;

	// Setup I/O Port D bit PD0 to control the RS485 transceiver 
	WrPortI(PDFR, &PDFRShadow, (PDFRShadow & ~0x01));		// Set PD0 to be a normal I/O pin
	WrPortI(PDDCR, &PDDCRShadow, (PDDCRShadow & ~0x01));	// Set PD0 to drive high or low
	WrPortI(PDDDR, &PDDDRShadow, (PDDDRShadow |  0x01));  // Set PD0 to be an Output

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
