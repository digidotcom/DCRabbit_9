/*******************************************************************
	parity.c
	Z-World 2002

	This program is used with RCM3200 series controllers
	with prototyping boards.

	The sample library, \Samples\RCM3200\rcm3200.lib, contains a
	brdInit() function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program demonstrates use of parity modes by repeatedly
	sending byte values 0-127 from serial port B to serial port C.
	The program will switch between generating parity or not on
	port B. Port C will always be checking parity, so parity
	errors should occur during every other sequence.

	Prototyping Board Connections
	=============================

		On J5, RS232 connector
		-----------------------------

		(PC4) TXB <-------> RXC (PC3)


	Instructions
	============
	1.  Run this program and observe error sequence in STDIO.

*******************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

#use rcm3200.lib    		//sample library used for this demo

#define BINBUFSIZE 31
#define BOUTBUFSIZE 15
#define CINBUFSIZE 31
#define COUTBUFSIZE 31

const long BAUD_RATE = 9600L;


void main()
{
	auto char received;
	auto int i;
	auto int txconfig;

	brdInit();				//initialize board for this demo

	serBopen(BAUD_RATE);
	serCopen(BAUD_RATE);

	serBparity(PARAM_OPARITY);
	serCparity(PARAM_OPARITY);
	txconfig = PARAM_OPARITY;

	printf("Starting...\n");

	while (1)
	{
		costate
		{
			//send as fast as we can
			for (i = 0; i < 128; i++)
			{
				waitfor(DelayMs(10)); 	//necessary if we are not using
											 	//flow control
				waitfordone{ cof_serBputc(i); }
			}
			// wait for data buffer, internal data and shift registers to become empty
  			waitfor(serBwrFree() == BOUTBUFSIZE);
  			waitfor(!((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04)));
			yield;

			//toggle between sending parity bits, and not
			if (txconfig)
			{
				txconfig = PARAM_NOPARITY;
				printf("\nParity option set to no parity\n");
			}
			else
			{
				txconfig = PARAM_OPARITY;
				printf("\nParity option set to odd parity\n");
			}
			serBparity(txconfig);
		}

		costate
		{
			//receive characters in a leisurely fashion
			waitfordone
			{
				received = cof_serCgetc();
			}
			printf("received 0x%x\n", received);
			if (serCgetError() & SER_PARITY_ERROR)
			{
				printf("PARITY ERROR\n");
			}
	   }
	}
}
