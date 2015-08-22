/********************************************************************
	Simple5wire.c
   Z-World 2004

	This program is used with BL2600 series controllers.

	Description
	===========
	This program demonstrates basic initialization for a
	simple RS232 5-wire loopback displayed in STDIO window.

	Here's the typical connections for a 5 wire interface that
   would be made between controllers.

		 TX <---> RX
       RX <---> TX
		RTS <---> CTS
      CTS <---> RTS
		Gnd <---> Gnd


	Connections
	===========
	1. Connect TXC to RXC located on J17.
   2. Connect TXF to RXF located on J17.


	============
	1.  Compile and run this program.
	2.  TxF and RxF become the flow control RTS and CTS.
	3.  To test flow control, disconnect RTS from CTS while
		 running	this program.  Characters should stop printing
		 in STDIO window and continue when RTS and CTS are
		 connected again.

********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


// serial buffer size
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

// serial baud rate
#define BAUD232 115200

main()
{
	auto int nIn;
	auto char cOut;

   // Initialize controller
	brdInit();

   // Open serial port
	serCopen(BAUD232);

   // Must use serial mode 2 or 3 for 5 wire RS232 operation and
   // serMode must be executed after the serXopen function(s).
   serMode(2);

   // Enable flow control
	serCflowcontrolOn();

   // Clear serial buffers
	serCwrFlush();
	serCrdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serCputc (cOut);								//	Send Byte
			while ((nIn=serCgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
		}
	}
}


