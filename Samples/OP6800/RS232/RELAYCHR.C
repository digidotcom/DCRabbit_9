/**************************************************************************

		relaychr.c

     	Z-World, 2001

	  	This sample program is for the OP6800 series controllers.

     	This program echos characters to/from a serial utility such as
    	hyperterminal. The connection is as follows:

  	 	Hyperterminal Setup
  	 	-------------------
  	 	1. Set serial parameters to 19200, n, 8, 1 and no handshaking.
  	 	2. Disable "Echo characters typed locally" option. (The
  	 	   controller will echo back characters that are typed.)
  	 	3. Enable send line end with line feeds.
  	 	4. Enable append line feeds to incoming lines.       
  	 	 	 	
		OP6800 Demo Board Setup:
	   ------------------------
	   1. Connect a +12v power source to the OP6800 Demo Board connector
	      J3/J5 as follows:

	   	PS     	OP6800 Demo Board
	   	--			-----------------
	   	+12v		+RAW Terminal  
	   	GND      GND Terminal

	 	2. Connect PC computer Tx to RXB located on J3/J5 of the OP6800 Demo Board.
	  	3. Connect PC computer Rx to TXC located on J3/J5 of the OP6800 Demo Board.
	  	4. Connect PC computer GND to GND located on J3/J5 of the OP6800 Demo Board.
 		5. Connect serial portB labeled TXB to portC labeled RXC located on J3/J5.

         					+-----------------+
         					|						|
         [ GND | Vcc | TXB | RXB | TXC | RXC | +485 | -485 ]
										|		|
										|		|
							PC TX <--+		+--> PC RX


  	   Test Instructions:
  	   ------------------
     	Run this program.
     	Run Hyperterminal.
     	Type characters into the serial utility window.

     	This program will echo characters sent from the serial utility back to 
     	the serial utility where they will appear in its send/receive window.
     
**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.

//The input and output buffers sizes are defined here. If these
//are not defined to be (2^n)-1, where n = 1...15, or they are
//not defined at all, they will default to 31 and a compiler
//warning will be displayed.

#define CINBUFSIZE  63
#define COUTBUFSIZE 63

#define BINBUFSIZE  63
#define BOUTBUFSIZE 63


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
   

	brdInit();		//required for OP6800 series boards
		
	c = 0;			//initialize variables
	parallel_counter = 0;
	loop_counter	  = 0;
	done				  = FALSE;

	sprintf(s, "Character counter = %d", 0);	//initialize for proper STDIO effects
   DispStr(2, 2, s);

   // display exit message
   DispStr(2, 5, "Press the ESC key in Hyperterminal to exit the program");
   
	serCopen(19200);	//set baud rates for the serial ports to be used
   serBopen(19200);
   
	serCwrFlush();		//clear Rx and Tx data buffers 
	serCrdFlush();

	serBwrFlush();		
	serBrdFlush();
   
   serMode(0);			//required for OP6800 series bds...must be done after serXopen function(s)
 
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
   // register to become empty
   while (serBwrFree() != BOUTBUFSIZE);
   while((RdPortI(SBSR)&0x08) || (RdPortI(SBSR)&0x04));

	// read data and send to hyperterminal
   num_bytes = serCread(buffer, sizeof(buffer), 5);
   buffer[num_bytes] = '\0';
	serCwrite(buffer, strlen(buffer));
	
   // wait for memory data buffer, serial holding register, and shift
   // register to become empty
   while (serCwrFree() != COUTBUFSIZE);
   while((RdPortI(SCSR)&0x08) || (RdPortI(SCSR)&0x04));
 
   //close the serial ports
   serCclose();
   serBclose();
}
