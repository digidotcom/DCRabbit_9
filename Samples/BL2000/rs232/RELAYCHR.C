/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/**************************************************************************

     Samples\BL2000\RS232\relaychr.c

	  This sample program is for the BL20XX series controllers.

     This program echos characters over serial port B to serial port C.
     It must be run with a serial utility such as Hyperterminal.
       
     Make the following connection:
     
	   	PC to controller connections
			----------------------------
			1. Connect PC Tx to serial PortB RXD1 located on J1.
			2. Connect PC Rx to serial PortC TXD2 located on J1.
			3. Connect PC GND to the controller's GND located on J1.

			Controller serial port B and port C connection
			-----------------------------------------------
  	 		1. Connect serial portB TXD1 to portC RXD2 located on J1. 

  	  Configure the serial utility for the port connected to RS232, 19200  
  	  8N1, and disable any kind of "echo typed characters locally" option.
     
     Run this program.
     Run Hyperterminal.
     Type characters into the serial utility window.

     This program will echo characters sent from the serial utility back to 
     the serial utility where they will appear in its send/receive window.
     
**************************************************************************/
#class auto


//The input and output buffers sizes are defined here. If these
//are not defined to be (2^n)-1, where n = 1...15, or they are
//not defined at all, they will default to 31 and a compiler
//warning will be displayed.

#define BINBUFSIZE  63
#define BOUTBUFSIZE 63

#define CINBUFSIZE  63
#define COUTBUFSIZE 63


// General 
#define ESC	27

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

///////////////////////////////////////////////////////////////////////////

void main()
{
   auto int c, num_bytes, done;
   auto char *ptr;
	auto int parallel_counter, loop_counter;
	auto char buffer[256];
	auto char s[256];
   

	brdInit();		//required for BL2000 series boards
		
	c = 0;			//initialize variables
	parallel_counter = 0;
	loop_counter	  = 0;
	done				  = FALSE;

	sprintf(s, "Character counter = %d", 0);	//initialize for proper STDIO effects
   DispStr(2, 2, s);

   // display exit message
   DispStr(2, 5, "Press the ESC key in Hyperterminal to exit the program");
   
	serBopen(19200);	//set baud rates for the serial ports to be used
   serCopen(19200);
   
	serBwrFlush();		//clear Rx and Tx data buffers 
	serBrdFlush();
	serCwrFlush();		
	serCrdFlush();
   
   serMode(0);			//required for BL2000 series bds...must be done after serXopen function(s)
 
   while (!done) {
               
   	loophead();			//required for single-user cofunctions
   	
   	costate				//single-user serial cofunctions 
   	{
   		// Wait for char from hyperterminal
	      wfd c = cof_serBgetc(); // yields until successfully getting a character
			//do clean exit from costatement
	      if(c == ESC)
   	   {	
				done = TRUE;	//set flag to exit other costatements and the while loop
				abort;			//only aborts out of this costatement
   	   }

	      // send character to serial port C
   	   wfd cof_serBputc(c);    // yields until c successfully put

   	   // wait for char from serial port B 
   	   wfd c = cof_serCgetc(); // yields until successfully getting a character

   	   //send character back to hyperterminal
   	   wfd cof_serCputc(c);    // yields until c successfully put

   	   //demonstrates that the above cofunctions only yields to other costates
   	   //and not to the code within the same costatement section.
   	   sprintf(s, "Character counter = %d", ++loop_counter);
   	   DispStr(2, 2, s);
      }
      costate
      {
      	// when ESC is detected in above costatement, also abort this costatement
			if(done)
   	   {
				abort;	//do clean exit of costatement
   	   }
			//execute code while waiting for characters from hyperterminal
			sprintf(s, "Parallel code execution counter = %d\r", parallel_counter++);
			DispStr(2, 3, s);
      }
   }
   
   // send program exit message
   serBputs("\n\n\rProgram Done...exiting");
   
    // wait for data buffer, internal data and shift registers to become empty
   while (serBwrFree() != BOUTBUFSIZE);
   while((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04));

	// read data and send to hyperterminal
   num_bytes = serCread(buffer, sizeof(buffer), 5);
   buffer[num_bytes] = '\0';
	serCwrite(buffer, strlen(buffer));
	
   // wait for data buffer, internal data and shift registers to become empty
   while (serCwrFree() != COUTBUFSIZE);
   while((RdPortI(SCSR)&0x08) || (RdPortI(SCSR)&0x04));
 
   //close the serial ports
   serBclose();
   serCclose();
}
