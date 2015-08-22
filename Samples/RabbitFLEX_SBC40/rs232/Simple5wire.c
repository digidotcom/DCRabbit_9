/**************************************************************************
	Simple5wire.c
   Rabbit Semiconductor, 2006

 	This program is used with RabbitFLEX SBC40 boards.

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
   1. Connect TXF and RXF together (J6 pins 2 and 4).
   2. Connect TXD/E and RXD/E together (J6 pins 1 and 3).

   Instructions
	============
	1.  Compile and run this program.
	2.  TXD and RXD (or TXE and RXE, depending on which serial port
	    is present) become the flow control RTS and CTS.
	3.  To test flow control, disconnect J6 pins 1 and 3,
       which will cause the characters to stop printing in STDIO
       window and will continue when you reconnect the pins.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// serial buffer size
#define FINBUFSIZE  15
#define FOUTBUFSIZE 15

// serial baud rate
#define BAUD232 115200

main()
{
	auto int nIn;
	auto char cOut;

	// Initialize I/O for the RabbitFLEX SBC40
	brdInit();

   // Open serial port
	serFopen(BAUD232);

   // Must use serial mode 1 for 5 wire RS232 operation and
   // serMode must be executed after the serXopen function(s).
   serMode(1);

   // Enable flow control
	serFflowcontrolOn();

   // Clear serial buffers
	serFwrFlush();
	serFrdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serFputc (cOut);								//	Send Byte
			while ((nIn=serFgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
		}
	}
}


