/**********************************************************
	Samples\RCM2300\master.c

   Z-World, 2001
	This sample program is used with RCM2300 series
	controllers.
	
	Connect circuitry as	described in your User Manual
	Sample Circuit Section.
	
	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.
	
	Use slave.c to program the slave controller.
		
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
