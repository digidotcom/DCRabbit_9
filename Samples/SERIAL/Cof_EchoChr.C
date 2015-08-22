/*****************************************************

     Cof EchoChr.c
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
     will echo characters sent from the serial utility back to the serial 
     utility where they will appear in its send/receive window.

     This program does not have a destructive backspace (try it), nor
     does it "cook" a RETURN into a return-linefeed combination.  Those
     changes are left as an exercise to the reader.  :)

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
#define Esc 27

void main()
{
	int c;

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	c = 0;
   serBopen(19200);
   while (c != Esc) {             
   	loophead();
   	costate {
	      wfd c = cof_serBgetc(); // yields until successfully getting a character
   	   wfd cof_serBputc(c);    // then yields here until c successfully put 
      }
   }
   serBclose();                  // disables B serial port reading and writing
}
