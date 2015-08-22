/**************************************************************************
	Simple5wire.c
   Z-World 2004

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

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
   1. Jumper TXE to RXE together located on J1 pins 3 and 5 with
   	using the 0.1 spacing shunt.
   2. Jumper TXF to RXF together located on J1 pins 4 and 6 with
      using the 0.1 spacing shunt.

	============
	1.  Compile and run this program.
	2.  TXF and RXF become the flow control RTS and CTS.
	3.  To test flow control, remove the jumper on J1 pins 4 and 6
       which will cause the characters to stop printing in STDIO
       window and will continue when you reinstall the jumper.

***************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

// serial buffer size
#define EINBUFSIZE  15
#define EOUTBUFSIZE 15

// serial baud rate
#define BAUD232 115200

main()
{
	auto int nIn;
	auto char cOut;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

   // Open serial port
	serEopen(BAUD232);

   // Must use serial mode 1 for 5 wire RS232 operation and
   // serMode must be executed after the serXopen function(s).
   serMode(1);

   // Enable flow control
	serEflowcontrolOn();

   // Clear serial buffers
	serEwrFlush();
	serErdFlush();

	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{
			serEputc (cOut);								//	Send Byte
			while ((nIn=serEgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));
		}
	}
}


