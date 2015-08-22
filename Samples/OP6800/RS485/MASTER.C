/**************************************************************************

	master.c

   Z-World, 2001
	This sample program is for the OP6800 series controllers.
	
	Connect circuitry as	described in the User Manual in the main
	hardware chapter under Serial Communication.
	
	This program demonstrates a simple RS485 transmission
	of lower case letters to a slave controller.  The slave
	will send back converted upper case letters back to the
	master controller and displayed in STDIO window.
	
	Use slave.c to program the slave controller.

 	OP6800 Demo Board Setup:
	------------------------
	1. Connect a +12v power source to the OP6800 Demo Board connector
	   J3/J5 as follows:

   	PS     	OP6800 Demo Board
   	--			-----------------
   	+12v		+RAW Terminal  
   	GND      GND Terminal

	2. Make the following connections between the OP6800 Demo board and the
	   second controller:

		Master(Demo bd)     	Slave (Other Controller)
		------     				-----
		485+ <-------------> 485+							
		485- <-------------> 485-							
		GND  <-------------> GND

**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.

// serial B buffer size
#define DINBUFSIZE  255
#define DOUTBUFSIZE 255

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

	
	brdInit();			//required for OP6800 series boards
	
	serDopen(19200);	//set baud rate first
	serDwrFlush();		//clear Rx and Tx data buffers 
	serDrdFlush();

	serMode(0);
	
	printf ("Waiting to sync up to the other controller\r");
	// Sync up to the slave controller
	sync = FALSE;
	while(!sync)
	{
		cOut = 0x55;
		nIn1 = 0xaa;
		ser485Tx();							//	enable transmitter
		serDputc ( cOut );				//	send lowercase byte
		while ((nIn1 = serDgetc()) == -1);		//	wait for echo
		ser485Rx();							// disable transmitter
		msDelay(5);
		if((nIn1 = serDgetc ()) == -1)	//	check for reply
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
			serDputc ( cOut );							//	send lowercase byte
			while (serDgetc() == -1);					//	wait for echo
			ser485Rx();										// disable transmitter
			
			while ((nIn1 = serDgetc ()) == -1);		//	wait for reply
			if (nIn1 == (toupper(cOut)))
				printf ("\n\rUpper case %c is %c", cOut, nIn1 );		
		}
	}
}
///////////////////////////////////////////////////////////////////////////