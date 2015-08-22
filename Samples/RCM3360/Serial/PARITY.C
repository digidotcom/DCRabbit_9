/*******************************************************************
	parity.c
	Z-World 2003

	This program is used with RCM3300 series controllers
	and prototyping boards.

	Description
	===========
	This program demonstrates use of parity modes by repeatedly
	sending byte values 0-127 from TXE (serial port E) to RXF
   (serial port F).
	The program will switch between generating parity or not on
	port E. Port F will always be checking parity, so parity
	errors should occur during every other sequence.

	Prototyping Board Connections
	=============================

		On the RS232 connector
		-----------------------------

		 TXE <-------> RXF


	Instructions
	============
	1.  Run this program and observe error sequence in STDIO.

*******************************************************************/
#class auto
#use rcm33xx.lib		//sample library to use with this application

#define EINBUFSIZE 31
#define EOUTBUFSIZE 15
#define FINBUFSIZE 31
#define FOUTBUFSIZE 31

const long baud_rate = 9600L;

main()
{
	auto char received;
	auto int i;
	auto int txconfig;

   #if _USER
   #warns "This sample permanantly disables the RabbitSys serial console."
   #warns "Remove power and the battery to restore serial console operation."   
	if (_sys_con_disable_serial() != 0) {
   	printf("Error: Cannot disable RabbitSys serial port.");
   }
   #endif
	brdInit();				//initialize board for this demo

	serEopen(baud_rate);
	serFopen(baud_rate);

	serEparity(PARAM_OPARITY);
	serFparity(PARAM_OPARITY);
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
				waitfordone{ cof_serEputc(i); }
			}
			// wait for data buffer, internal data and shift registers to become empty
  			waitfor(serEwrFree() == EOUTBUFSIZE);
  			waitfor(!((RdPortI(SESR)&0x08) || (RdPortI(SESR)&0x04)));
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
			serEparity(txconfig);
		}

		costate
		{
			//receive characters in a leisurely fashion
			waitfordone
			{
				received = cof_serFgetc();
			}
			printf("received 0x%x\n", received);
			if (serFgetError() & SER_PARITY_ERROR)
			{
				printf("PARITY ERROR\n");
			}
	   }
	}
}

