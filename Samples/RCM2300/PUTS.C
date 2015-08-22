/*****************************************************

     puts.c
     Z-World, 2001

     This program writes a null terminated string over serial port A,B,C or D.
     It must be run with a serial utility such as Hyperterminal.

     Connect RS232 cable from PC to Rabbit:
     	 Connect PC's RS232 GND to Rabbit GND
       Connect PC's RS232 TD  to Rabbit RXA, RXB, RXC or RXD
       Connect PC's RS232 RD  to Rabbit TXA, TXB, TXC or TXD

     Configure the serial utility for port connected to RS232, 19200 8N1.

     Run Hyperterminal.
     Run this program.
     See the message appear.

******************************************************/
#class auto

/*
 *		Serial port Settings
 *		Serial PORT 1=A, 2=B, 3=C, 4=D
 *
 */
#define SERIAL_PORT	2
#define BAUD_RATE		19200L

/*****************************************************
     The input and output buffers sizes are defined here. If these
     are not defined to be (2^n)-1, where n = 1...15, or they are
     not defined at all, they will default to 31 and a compiler
     warning will be displayed.
******************************************************/
#define INBUFSIZE		31
#define OUTBUFSIZE 	255


///////////////////////////////////////////////////////////////////////

/*
 * 	Select serial port to use.
 */

#if (SERIAL_PORT==1)

	#define serXopen  	serAopen
	#define serXread  	serAread
	#define serXgetc  	serAgetc
	#define serXrdUsed  	serArdUsed
	#define serXputs  	serAputs
	#define serXwrite 	serAwrite
	#define serXclose 	serAclose
	#define serXwrFlush	serAwrFlush
	#define serXrdFlush	serArdFlush
	#define serXwrFree	serAwrFree
   #define SxSR			SASR

	#define AINBUFSIZE	INBUFSIZE
	#define AOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==2)

	#define serXopen  	serBopen
	#define serXread  	serBread
	#define serXgetc  	serBgetc
	#define serXrdUsed  	serBrdUsed
	#define serXputs  	serBputs
	#define serXwrite 	serBwrite
	#define serXclose 	serBclose
	#define serXwrFlush	serBwrFlush
	#define serXrdFlush	serBrdFlush
	#define serXwrFree	serBwrFree
   #define SxSR			SBSR

	#define BINBUFSIZE	INBUFSIZE
	#define BOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==3)

	#define serXopen  	serCopen
	#define serXread  	serCread
	#define serXgetc  	serCgetc
	#define serXrdUsed  	serCrdUsed
	#define serXputs  	serCputs
	#define serXwrite 	serCwrite
	#define serXclose 	serCclose
	#define serXwrFlush	serCwrFlush
	#define serXrdFlush	serCrdFlush
	#define serXwrFree	serCwrFree
   #define SxSR			SCSR

	#define CINBUFSIZE	INBUFSIZE
	#define COUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==4)

	#define serXopen  	serDopen
	#define serXread  	serDread
	#define serXgetc  	serDgetc
	#define serXrdUsed  	serDrdUsed
	#define serXputs  	serDputs
	#define serXwrite 	serDwrite
	#define serXclose 	serDclose
	#define serXwrFlush	serDwrFlush
	#define serXrdFlush	serDrdFlush
	#define serXwrFree	serDwrFree
   #define SxSR			SDSR

	#define DINBUFSIZE	INBUFSIZE
	#define DOUTBUFSIZE	OUTBUFSIZE

#else
	#error "Unknown SERIAL_PORT value!"
#endif


///////////////////////////////////////////////////////////////////////

void main()
{
	const static char s[] = "Hello Z-World\r\n";
   serXopen(BAUD_RATE);
   serXputs(s);

	// first, wait until the serial buffer is empty
   while (serXwrFree() != OUTBUFSIZE) ;

	// then, wait until the Tx data register and the Tx shift register
	//  are both empty
	while (BitRdPortI(SxSR, 3) || BitRdPortI(SxSR, 2));

	// now we can close the serial port without cutting off Tx data
   serXclose();

	/* Note: if you run this program with the programming cable removed,
	 * 	it will restart immediately upon reaching the end here.  The
	 * 	user will observe the message displaying continuously.  To _not_
	 * 	have this occur, put a "while(1) ;" block at the end.  This
	 * 	will prevent the program from ending, and thus restarting.
	 */

    //while (1);
}