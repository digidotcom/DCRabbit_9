/**************************************************************************

	master.c

   Z-World, 2001
	This sample program is for the OP7200 series controllers.
	
	Connect circuitry as	described in the User Manual in the main
	hardware chapter under Serial Communication.
	
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

**************************************************************************/
#class auto

// serial B buffer size
#define BINBUFSIZE  255
#define BOUTBUFSIZE 255

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int nIn1;
	auto char cOut;
	auto int sync;

	
	brdInit();			// Required for controllers
	
	serBopen(19200);	// Set baud rate first
	serBwrFlush();		// Clear Rx and Tx data buffers 
	serBrdFlush();

	serMode(0);

	printf ("Waiting to sync up to the other controller\r");
	// Sync up to the slave controller
	sync = FALSE;
	while(!sync)
	{
		cOut = 0x55;
		ser485Tx();							//	enable transmitter
		serBputc ( cOut );				//	send lowercase byte
		while (serBgetc() == -1);		//	wait for echo
		ser485Rx();							// disable transmitter
		msDelay(5);
		if((nIn1 = serBgetc ()) == -1)	//	check for reply
		{
			printf ("Waiting to sync up to the other controller\r");		
		}
		else
		{
			if(nIn1 == cOut)
			{
				sync = TRUE;
			}
		}
	}
			
	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{
			ser485Tx();										//	enable transmitter
			serBputc ( cOut );							//	send lowercase byte
			while (serBgetc() == -1);					//	wait for echo
			ser485Rx();										// disable transmitter
			
			while ((nIn1 = serBgetc ()) == -1);		//	wait for reply
			if (nIn1 == (toupper(cOut)))
				printf ("\n\rUpper case %c is %c", cOut, nIn1 );		
		}
	}
}
///////////////////////////////////////////////////////////////////////////