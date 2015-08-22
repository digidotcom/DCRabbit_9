/*****************************************************

     EchoChr.c
     Z-World, 1999

     This program echos characters over serial port B.
     It must be run with a serial utility such as Hyperterminal.
     
     Connect RS232 cable from PC to Rabbit:
     	 Connect PC's RS232 GND to Rabbit GND
       Connect PC's RS232 TD  to Rabbit RXB
       Connect PC's RS232 RD  to Rabbit TXB
       
     Configure the serial utility for port connected to RS232, 19200 8N1.
     
     Run this program.
     Run Hyperterminal.
     Type characters into the serial utility window.
      
     This program will echo characters sent from the serial utility back to 
     the serial utility where they will appear in its send/receive window.
     It will terminate after reading a single <Esc>.
     
******************************************************/
#class auto

/*****************************************************
     The input and output buffers sizes are defined here. If these
     are not defined to be (2^n)-1, where n = 1...15, or they are
     not defined at all, they will default to 31 and a compiler
     warning will be displayed.
******************************************************/
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15

#define  CH_ESCAPE	27

#define  B_BAUDRATE	19200L

xmem void main()
{
   int c;

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	c = 0;
   serBopen(B_BAUDRATE);
   while (c != 27) {  // Exit on Esc
      if ((c = serBgetc()) != -1 && c != CH_ESCAPE) {
         serBputc(c);
      	if( c == '\r' ) { 		/* Cook ENTER into CR_LF */
      		serBputc('\n');
      	}
      }
   }
   serBputs("Done\r\n");
   while (serBwrFree() != BOUTBUFSIZE) ;      // allow transmission to complete before closing
   serBclose();
}
