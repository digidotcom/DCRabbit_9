/**************************************************************************

	Samples\Jackrab\JR_Parity.c
	Demonstration of parity modes

	This demonstration will repeatedly send byte values 0-127 from serial
	port B to serial port C. Each time the program will switch between
	generating parity or not on port B. Port C will always be checking
	parity, so parity errors should occur during every other sequence.

	To perform this test you will need to connect TXB to RXC


	Note: For the sequence that does get parity errors, the errors won't
	occur for each byte received. This is due to the fact certain byte
	patterns along with the stop bit will appear to generate the proper
	parity for the UART.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


#define BINBUFSIZE 31
#define BOUTBUFSIZE 15
#define CINBUFSIZE 31
#define COUTBUFSIZE 31

const long baud_rate = 9600L;

void main()
{
	auto char received;
	auto int i;
	auto int txconfig;
					
	serBopen(baud_rate);
	serCopen(baud_rate);

	serBparity(PARAM_OPARITY);
	serCparity(PARAM_OPARITY);
	txconfig = PARAM_OPARITY;
	
	printf("Starting...Parity option set properly\n");
				
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
				// wait for data buffer, internal data and shift registers to become empty
   			waitfor(serBwrFree() == BOUTBUFSIZE);
   			waitfor(!((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04)));
				txconfig = PARAM_NOPARITY;
				printf("\n\nParity option set inproperly\n");
			}
			else
			{
				// wait for data buffer, internal data and shift registers to become empty
   			waitfor(serBwrFree() == BOUTBUFSIZE);
   			waitfor(!((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04)));
				txconfig = PARAM_OPARITY;
				printf("\n\nParity option set properly\n");
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
