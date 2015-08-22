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

	  	This sample program is for the OP7200 series controllers.

     	This program echos characters to/from a serial utility such as
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
	 	1. Connect PC Tx to serial PortC labeled RXC located on J11.
	  	2. Connect PC Rx to serial PortD labeled TXD located on J11.
	  	3. Connect PC GND to the controller's GND located on J11.

	 	Controller serial port C and port D connection
		-----------------------------------------------
 		Connect serial portC labeled TXC to port D labeled RXD located
 		on J11. 

  	   Test Instructions:
  	   ------------------
     	1. Run this program.
     	2. Run Hyperterminal.
     	3. Type characters into the serial utility window.

     	Note: This program will echo characters sent from the serial utility
     	      back to the serial utility where they will appear in its send-
     	      receive window.
     
**************************************************************************/
#class auto


//The input and output buffers sizes are defined here. If these
//are not defined to be (2^n)-1, where n = 1...15, or they are
//not defined at all, they will default to 31 and a compiler
//warning will be displayed.

#define CINBUFSIZE  63
#define COUTBUFSIZE 63

#define DINBUFSIZE  63
#define DOUTBUFSIZE 63


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
   
	// Required for controllers
	brdInit();		

	// Initialize variables
	c = 0;			
	parallel_counter = 0;
	loop_counter	  = 0;
	done				  = FALSE;

	sprintf(s, "Character counter = %d", 0);	// Initialize for proper STDIO effects
   DispStr(2, 2, s);

   // display exit message
   DispStr(2, 5, "Press the ESC key in Hyperterminal to exit the program");
   
	serCopen(19200);	// Set baud rates for the serial ports to be used
   serDopen(19200);
   
	serCwrFlush();		// Clear Rx and Tx data buffers 
	serCrdFlush();

	serDwrFlush();		
	serDrdFlush();
   
   serMode(0);			// Configure serial mode...must be done after serXopen function(s)
 
   while (!done) {
               
   	loophead();			//Required for single-user cofunctions
   	
   	costate				//Single-user serial cofunctions 
   	{
   		// Wait for char from hyperterminal
	      wfd c = cof_serCgetc(); // yields until successfully getting a character

			//do clean exit from costatement
	      if(c == ESC)
   	   {
   	   	//flag used to exit out of this WHILE loop and other costatements
				done = TRUE;
				
				//abort this costatement
				abort;		
   	   }

	      // send character to serial port D
   	   wfd cof_serCputc(c);    // yields until c successfully put

   	   // wait for char from serial port C
   	   wfd c = cof_serDgetc(); // yields until successfully getting a character

   	   //send character back to hyperterminal
   	   wfd cof_serDputc(c);    // yields until c successfully put

   	   waitfor(serDwrUsed() == 0);
   	   
   	   // Demonstrates that the above cofunctions only yields to other costates
   	   // and not to the code within the same costatement section.
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
			// Execute code while waiting for characters from hyperterminal
			sprintf(s, "Parallel code execution counter = %d\r", parallel_counter++);
			DispStr(2, 3, s);
      }
   }
   
   // Send program exit message
   serCputs("\n\n\rProgram Done...exiting");
   
   // Wait for memory data buffer, serial holding register, and shift
   // register to become empty
   while (serCwrFree() != COUTBUFSIZE);
   while((RdPortI(SCSR)&0x08) || (RdPortI(SCSR)&0x04));

	// Send data to hyperterminal
   num_bytes = serCread(buffer, sizeof(buffer), 5);
   buffer[num_bytes] = '\0';
	serDwrite(buffer, strlen(buffer));
	
   // Wait for memory data buffer, serial holding register, and shift
   // register to become empty
   while (serDwrFree() != DOUTBUFSIZE);
   while((RdPortI(SDSR)&0x08) || (RdPortI(SDSR)&0x04));
 
   //close the serial ports
   serCclose();
   serDclose();
}
///////////////////////////////////////////////////////////////////////////
