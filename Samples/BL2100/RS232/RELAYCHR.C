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

		relaychr.c

	  	This sample program is for the BL2100 series controllers.

     	This program echoes characters to/from a serial utility such as
    	hyperterminal. The connection is as follows:

  	 	Hyperterminal Setup
  	 	-------------------
  	 	1. Set serial parameters to 19200, n, 8, 1 and no handshaking.
  	 	2. Disable "Echo characters typed locally" option. (The
  	 	   controller will echo back characters that are typed)
  	 	3. Enable send line end with line feeds.
  	 	4. Enable append line feeds to incoming lines.         

  	  	PC to controller connections
	  	----------------------------
	 	1. Connect PC Tx to serial PortB labeled RXB located on J4.
	  	2. Connect PC Rx to serial PortC labeled TXC located on J4.
	  	3. Connect PC GND to the controller's GND located on J4.

	 	Controller serial port B and port C connection
		-----------------------------------------------
 		Connect serial portB labeled TXB to port C labeled RXC located
 		on J4. 

  	   Test Instructions:
  	   ------------------
     	1. Run this program.
     	2. Run Hyperterminal.
   	3. Type characters into the serial utility window.
   	4. When done, hit the ESCAPE key on the host computer.

     	Note: This program will echo characters sent from the serial utility back to 
     	the serial utility where they will appear in its send/receive window.
     
**************************************************************************/
#class auto


//The input and output buffers sizes are defined here. If these
//are not defined to be (2^n)-1, where n = 1...15, or they are
//not defined at all, they will default to 31 and a compiler
//warning will be displayed.

#define CINBUFSIZE  63
#define COUTBUFSIZE 63

#define BINBUFSIZE  63
#define BOUTBUFSIZE 63

#define  BAUD_RATE	19200L

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
   

	brdInit();		//required for BL2100 series boards
		
	c = 0;			//initialize variables
	parallel_counter = 0;
	loop_counter	  = 0;
	done				  = FALSE;

	sprintf(s, "Character counter = %d", 0);	//initialize for proper STDIO effects
   DispStr(2, 2, s);

   // display exit message
   DispStr(2, 5, "Press the ESC key in Hyperterminal to exit the program");
   
	serCopen(BAUD_RATE);	//set baud rates for the serial ports to be used
   serBopen(BAUD_RATE);
   
	serCwrFlush();		//clear Rx and Tx data buffers 
	serCrdFlush();

	serBwrFlush();		
	serBrdFlush();
   
   serMode(0);			//required for BL2100 series bds...must be done after serXopen function(s)
 
   while (!done) {
               
   	loophead();			//required for single-user cofunctions
   	
   	costate				//single-user serial cofunctions 
   	{
   		// Wait for char from hyperterminal
	      wfd c = cof_serBgetc(); // yields until successfully getting a character

			//do clean exit from costatement
	      if(c == ESC)
   	   {
   	   	//flag used to exit out of this WHILE loop and other costatements
				done = TRUE;
				
				//abort this costatement
				abort;		
   	   }

	      // send character to serial port C
   	   wfd cof_serBputc(c);    // yields until c successfully put

   	   // wait for char from serial port C
   	   wfd c = cof_serCgetc(); // yields until successfully getting a character

   	   //send character back to hyperterminal
   	   wfd cof_serCputc(c);    // yields until c successfully put

   	   waitfor(serCwrUsed() == 0);
   	   
   	   //demonstrates that the above cofunctions only yields to other costates
   	   //and not to the code within the same costatement section.
   	   sprintf(s, "Character counter = %d", ++loop_counter);
   	   DispStr(2, 2, s);
      }
      costate
      {
      	// Abort this costatement if the done flag has been set
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
   
   // wait for memory data buffer, serial holding register, and shift
   // register all to become empty
   while (serBwrFree() != BOUTBUFSIZE);
   while((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04));

	// read data and send to hyperterminal
   num_bytes = serCread(buffer, sizeof(buffer), 5);
   buffer[num_bytes] = '\0';
	serCwrite(buffer, strlen(buffer));
	
   // wait for memory data buffer, serial holding register, and shift
   // register all to become empty
   while (serCwrFree() != COUTBUFSIZE);
   while((RdPortI(SCSR)&0x08) || (RdPortI(SCSR)&0x04));
 
   //close the serial ports
   serCclose();
   serBclose();
}
///////////////////////////////////////////////////////////////////////////
