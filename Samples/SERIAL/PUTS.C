/*****************************************************

     Puts.c
     Z-World, 1999

     This program writes a null terminated string over serial port B.
     It must be run with a serial utility such as Hyperterminal.
     This program will ensure all chars are sent, then exit

     Connect RS232 cable from PC to Rabbit:
     	 Connect PC's RS232 GND to Rabbit GND
       Connect PC's RS232 TD  to Rabbit RXB
       Connect PC's RS232 RD  to Rabbit TXB

     Configure the serial utility for port connected to RS232, 19200 8N1.

     Run Hyperterminal.
     Run this program.
     See the message appear.

******************************************************/
#class auto

/*****************************************************
     The input and output buffers sizes are defined here. If these
     are not defined to be (2^n)-1, where n = 1...15, or they are
     not defined at all, they will default to 31 and a compiler
     warning will be displayed.
******************************************************/
#define BINBUFSIZE 15
#define BOUTBUFSIZE 15

void main()
{
	static const char s[] = "Hello Z-World\r\n";

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	serBopen(19200);
	serBputs(s);

	// first, wait until the serial buffer is empty
	while (serBwrFree() != BOUTBUFSIZE);

	// then, wait until the Tx data register and the Tx shift register
	//  are both empty
	while (BitRdPortI(SBSR, 3) || BitRdPortI(SBSR, 2));

	// now we can close the serial port without cutting off Tx data
	serBclose();
}