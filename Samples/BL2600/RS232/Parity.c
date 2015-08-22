/**************************************************************************

	parity.c
	Z-World, 2004

	This sample program is for the BL2600 series controller.

   Description
	===========
	This demonstration will repeatedly send byte values 0-127 from serial
	port F to serial port C. The program will switch between generating
   parity or not on port F. Port C will always be checking parity, so
   parity errors should occur during every other sequence.

   Note: For the sequence that does get parity errors, the errors won't
	occur for each byte received. This is because certain byte patterns
   along with the stop bit will appear to generate the proper parity for
   the UART.

   Connections
	===========
	1. Connect serial port pin TXF to serial port pin RXC.

   Instructions
	============
   1. Power-on the controller.
	2. Compile and run this program.
   3. Stop program.
	4. View STDIO window to see the test results of the sample program.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

#define FINBUFSIZE 255
#define FOUTBUFSIZE 255


#define CINBUFSIZE 255
#define COUTBUFSIZE 255

const long baud_rate = 9600L;

void main()
{
	auto int received, receive_count;
	auto int i;
	auto int txconfig;

   // Initialize the controller
	brdInit();

	serCopen(baud_rate);
   serFopen(baud_rate);

   // Serial mode must be done after opening the serial ports
   serMode(0);

   // Clear serial data buffers
   serCrdFlush();
   serFrdFlush();
   serCwrFlush();
   serFwrFlush();

	printf("Start with the parity option set properly\n");
	serCparity(PARAM_OPARITY);
   serFparity(PARAM_OPARITY);

	txconfig = PARAM_OPARITY;

	while (1)
	{
		costate
		{
      	receive_count = 0;
			// Send data value 0 - 127
			for (i = 0; i < 128; i++)
			{
         	yield; // Yield so data can be read from serial port C
				serFputc(i);
			}
         // Wait for data buffer, internal data and shift registers to become empty
   		waitfor(serFwrFree() == FOUTBUFSIZE);
   		waitfor(!((RdPortI(SFSR)&0x08) || (RdPortI(SFSR)&0x04)));

         // Wait for entire 128 bytes to be processed
         waitfor(receive_count == 128);

			// Toggle between parity options
			if (txconfig)
			{
				txconfig = PARAM_NOPARITY;
				printf("\n\nInproperly set parity option\n");
			}
			else
			{
				txconfig = PARAM_OPARITY;
				printf("\n\nProperly set parity option\n");
			}
			serFparity(txconfig);
		}
		costate
		{
      	// Receive characters in a leisurely fashion
 			if((received = serCgetc()) != -1)
         {
         	receive_count++;
				printf("received 0x%x\n", received);
				if (serCgetError() & SER_PARITY_ERROR)
				{
					printf("PARITY ERROR\n");
				}
	   	}
      }
	}
}