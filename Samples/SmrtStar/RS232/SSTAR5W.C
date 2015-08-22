/********************************************************************
	sstar5w.c

   Z-World, 2001
	This sample program is used with Smart Star products.
	
	This program demonstrates a simple 5-wire serial communication
	with flow control.  Loopback is displayed in STDIO window.
	
	Make the following loopback connections as labeled on the
	CPU board:
			
				TxC  <---> RxC
		(RTS) TxD  <---> RxD (CTS)
				
	TxC and RxC provides data transmissions while connections to TxD
	and RxD provide connections for flow control, RTS and CTS.
	serMode(2) configures RTS and CTS to ports PD0 and PD2.  See
	serMode description for more details.
	
	To test flow control, disconnect RTS from CTS while running
	this program.  Characters should stop printing in STDIO window
	and continue when RTS and CTS are connected again.

********************************************************************/
#class auto


// serial buffer size
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif

#define serOpen serCopen
#define serFlowControlOn serCflowcontrolOn
#define serWrFlush serCwrFlush
#define serRdFlush serCrdFlush
#define serPutc serCputc
#define serGetc serCgetc

void main()
{
	auto int nIn;
	auto char cOut;

	brdInit();
	
	serOpen(_232BAUD);		//initialize Tx, Rx
	serMode(2);					//initialize RTS, CTS
	serFlowControlOn();		//enable flow control
	serWrFlush();				//clear buffers
	serRdFlush();
	
	while (1) {
		for (cOut='a'; cOut<='z'; ++cOut) {			//	Send lowercase byte
			serPutc (cOut);								//	Send Byte
			while ((nIn=serGetc()) == -1);			// Wait for echo
			printf ("Upper case %c is %c\n", cOut, toupper(nIn));	 
			}
		}
}

