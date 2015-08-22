/*****************************************************

     Cof EchoStr.c
     Z-World, 1999

     This program echos a block of characters over serial port B.
     It must be run with a serial utility such as Hyperterminal.
     
     Connect RS232 cable from PC to Rabbit:
     	 Connect PC's RS232 GND to Rabbit GND
       Connect PC's RS232 TD  to Rabbit RXB
       Connect PC's RS232 RD  to Rabbit TXB
       
     Configure the serial utility for port connected to RS232, 19200 8N1.
     
     Run this program.
     Run Hyperterminal.
     Type characters into the serial utility window.
      
     This program uses the single user cofunction form of the serial read
     and write routines, which allows it to cooperatively multi-task with
     other costates when it is not actively reading or writing data. It 
     will echo a Carriage Return terminated character strings sent from 
     the serial utility back to the serial utility where they will appear 
     in its send/receive window.

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
#define maxs 5
#define timeout 3000UL // will time out 3 seconds after receiving 
                       // a character unless cof_serBread completes

void main()
{
	int getOk, done;
	char s[maxs + 1]; // plus 1 for null terminator

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	done = 0;
   serBopen(19200);
   while (!done) {
		loophead();
   	costate {
   		wfd getOk = cof_serBgets(s, maxs, timeout); // yields until getting a null terminated string
   		if (getOk) {
      		wfd cof_serBputs(s);                     // then yields until the string is written
      	}
      	else {
      		if (!strcmp(s, "q")) {
      			wfd cof_serBputs("Done"); 
      			done = 1;
      		}
      		else
      			wfd cof_serBputs("Timed out!"); 
      	}
      }
   }
   while (serBwrFree() != BOUTBUFSIZE) ;      // allow transmission to complete before closing
   serBclose();
}
