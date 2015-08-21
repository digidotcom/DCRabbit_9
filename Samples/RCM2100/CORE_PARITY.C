/*****************************************
Demonstration of parity modes

This demonstration will repeatedly send byte values 0-127 from serial port B
to serial port C. Each time the program will switch between generating
parity or not on port B. Port C will always be checking parity,
so parity errors should occur during every other sequence.

To perform this test you will need to connect PC4 to PC3

*******************************************/
#class auto


#define BINBUFSIZE 31
#define BOUTBUFSIZE 15
#define CINBUFSIZE 31
#define COUTBUFSIZE 31

#define  BAUD_RATE 	9600L

void main()
{
	auto char received;
	auto int i;
	auto int txconfig;
					
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
				waitfor(DelayMs(10)); //necessary if we are not using
											 //flow control
				waitfordone{ cof_serBputc(i); }
			}
			//toggle between sending parity bits, and not
			if (txconfig)
			{
				txconfig = 0;
			}
			else
			{
				txconfig = PARAM_OPARITY;
			}
			serBparity(txconfig);
		}   /* serial B costate.. */

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
	   }   /* serial C costate.. */

	}
}
	