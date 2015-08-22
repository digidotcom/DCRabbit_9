/*******************************************************************************
	parity.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This sample program demonstrates use of parity modes by repeatedly sending
	character byte values 0-127 out TXE (serial port E transmit) to RXF (serial
	port F transmits).

	The program cycles between generating odd parity, even parity and no parity
	on serial port E.  Serial port F is always checking for odd parity, so parity
	errors should occur on all characters received during even parity sequences
	and on many characters received during no parity sequences.

	Prototyping Board Connections
	=============================
	On the RS232 connector:
		TXE <-------> RXF

	Instructions
	============
	1. Compile and run this sample program in debug mode.

	2. Observe the sequence of parity status, received characters and error
	   messages reported in the STDIO window.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define BAUD_RATE 9600L

#define EINBUFSIZE 31
#define EOUTBUFSIZE 15
#define FINBUFSIZE 31
#define FOUTBUFSIZE 31

void main(void)
{
	static char received;
	static int errors, i;
	static int txconfig;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	// initialize our Rx parity errors count to zero
	errors = 0;

	serEopen(BAUD_RATE);
	serFopen(BAUD_RATE);

	printf("Starting...\n");

	serEparity(PARAM_OPARITY);
	serFparity(PARAM_OPARITY);
	txconfig = PARAM_OPARITY;

	printf("Parity option set to odd parity.\n");

	while (1) {
		costate {
			for (i = 0; i < 128; ++i) {
				waitfordone {
					cof_serEputc(i);
				}
				waitfor(DelayMs(1)); // necessary, as we are not using flow control
			}

			// first, wait for Tx data buffer empty
			waitfor(serEwrFree() == EOUTBUFSIZE);
			// then, wait for internal data and shift registers empty
			waitfor(!((RdPortI(SESR) & 0x08) || (RdPortI(SESR) & 0x04)));
			// finally, wait for the Rx side to process the last Tx'd character(s)
			waitfor(DelayMs(100));

			// cycle between sending odd / even / no parity bits
			if (PARAM_OPARITY == txconfig) {
				txconfig = PARAM_EPARITY;
				printf("\nParity option set to even parity.\n");
			}
			else if (PARAM_EPARITY == txconfig) {
				txconfig = PARAM_NOPARITY;
				printf("\nParity option set to no parity.\n");
			} else {
				txconfig = PARAM_OPARITY;
				printf("\nParity option set to odd parity.\n");
			}
			serEparity(txconfig);
		}

		costate {
			// receive characters in a leisurely fashion
			waitfordone {
				received = cof_serFgetc();
			}
			if (serFgetError() & SER_PARITY_ERROR) {
				printf("Received 0x%x with PARITY ERROR!\n", received);
				++errors;
			} else {
				printf("Received 0x%x.\n", received);
			}
			if (0x7f == received) {
				printf ("There were %d parity errors in this sequence.\n", errors);
				errors = 0;	// reset our error count to zero for the next sequence
			}
		}
	}
}

