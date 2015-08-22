/**********************************************************
	sstar232.c
	
	Z-World, 2001
	This sample program is used with Smart Star products.
	
	This program demonstates a simple RS232 loopback
	using both serial ports C and D.
	
	Make the following connections.

		TxC <---> RxD
		TxD <---> RxC

**********************************************************/
#class auto

// serial C buffer size
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

// serial D buffer size
#define DINBUFSIZE  15
#define DOUTBUFSIZE 15

// serial baud rate
#ifndef _232BAUD
#define _232BAUD 115200
#endif

//////////////////////////////////////////////////////////
void main()
{
	auto int nIn1, nIn2;
	auto char cOut;

	brdInit();
	
	serCopen(_232BAUD);
	serDopen(_232BAUD);
	serMode(1);
	serCwrFlush();
	serCrdFlush();
	serDwrFlush();
	serDrdFlush();
	
	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{			
			serCputc (cOut);								//	Send lowercase byte
			while ((nIn1=serDgetc()) == -1);			// Wait for echo
			serDputc (toupper(nIn1));					//	Send the converted upper case byte
			while ((nIn2=serCgetc()) == -1);			// Wait for echo
			printf ("Serial C sent %c, serial D sent %c, serial C received %c\n",
						cOut, toupper(nIn1), nIn2);
		}
	}
}
//////////////////////////////////////////////////////////

