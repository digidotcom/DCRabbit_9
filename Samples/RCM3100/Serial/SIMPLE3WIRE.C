/**********************************************************
	simple3wire.c
   Z-World, 2002

	This program is used with RCM3100 series controllers
	with prototyping boards.
	
	The sample library, rcm3100.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program demonstrates basic initialization for a
	simple RS232 3-wire loopback displayed in STDIO window.
	
	Normally we would connect to another controller,

		Tx  <---> Rx
		Rx  <---> Tx
		Gnd <---> Gnd

	However, for this simple demonstration, make the
	following connections:
	
	Proto-board Connections
	=======================

		TxB <---> RxC
		RxB <---> TxC

	Instructions
	============
	1.  Compile and run this program.
	2.  Lower case characters are sent by TxC. RxB receives
	    the character.  TxB sends the converted uppercase
	    character to RxC and displays in STDIO.

**********************************************************/

#use rcm3100.lib    		//sample library used for this demo

///////
// change serial buffer name and size here
// for example:  CINBUFSIZE to DINBUFSIZE
///////
#define CINBUFSIZE  15
#define COUTBUFSIZE 15

#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

///////
// change serial baud rate here
///////
#ifndef _232BAUD
#define _232BAUD 115200
#endif


main()
{
	auto int nIn1, nIn2;
	auto char cOut;

	brdInit();				//initialize board for this demo
	
	serCopen(_232BAUD);
	serBopen(_232BAUD);
	serCwrFlush();
	serCrdFlush();
	serBwrFlush();
	serBrdFlush();
	
	while (1)
	{
		for (cOut='a';cOut<='z';++cOut)
		{			
			serCputc (cOut);								//	Send lowercase byte
			while ((nIn1=serBgetc()) == -1);			// Wait for echo
			serBputc (toupper(nIn1));					//	Send the converted upper case byte
			while ((nIn2=serCgetc()) == -1);			// Wait for echo
			printf ("Serial C sent %c, serial B sent %c, serial C received %c\n",
						cOut, toupper(nIn1), nIn2);
		}
	}
}

