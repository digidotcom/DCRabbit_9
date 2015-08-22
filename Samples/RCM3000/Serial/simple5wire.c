/********************************************************************
	simple5wire.c
   Z-World 2002
	
	This program is used with RCM3000 series controllers with
	prototyping boards.
	
	The sample library, rcm3000.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program demonstrates basic initialization for a
	simple RS232 5-wire loopback displayed in STDIO window.
	
	Normally we would connect to another controller,

		 Tx <---> Rx
		RTS <---> CTS
		Gnd <---> Gnd

	RTS and CTS can be most any parallel port pin if properly
	configured. However, for this simple demonstration, make
	the following connections:
	
	Proto-board Connections
	=======================
	
			TxB <---> RxB
	(RTS)	TxC <---> RxC (CTS)

	Instructions
	============
	1.  Compile and run this program.
	2.  TxC and RxC become the flow control RTS and CTS.
	3.  To test flow control, disconnect RTS from CTS while
		 running	this program.  Characters should stop printing
		 in STDIO window and continue when RTS and CTS are
		 connected again.

********************************************************************/

#use rcm3000.lib    		//sample library used for this demo

///////
// PC2 and PC3 were for flow control RTS and CTS
///////
#define SERB_RTS_PORT PCDR
#define SERB_RTS_SHADOW PCDRShadow
#define SERB_RTS_BIT 2			//PC2; TxC on protoboard
#define SERB_CTS_PORT PCDR
#define SERB_CTS_BIT 3			//PC3; RxC on protoboard

// serial B buffer size
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif

main()
{
	auto int nIn;
	auto char cOut;

	brdInit();				//initialize board for this demo

	serBopen(_232BAUD);		//initialize TxB, RxB
	serBflowcontrolOn();		//enable flow control
	serBwrFlush();				//clear buffers
	serBrdFlush();
	
	while (1)
	{
		for (cOut='a'; cOut<='z'; ++cOut)
		{			
			serBputc (cOut);								//	Send Byte
			while ((nIn=serBgetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));	
			}
	}
}

