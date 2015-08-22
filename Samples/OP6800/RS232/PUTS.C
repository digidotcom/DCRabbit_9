/**************************************************************************

     Puts.c

     Z-World, 2001

	  This sample program is for the OP6800 series controllers.

	  This program transmits and then receives an ASCII string on serial 
	  ports B and C. It also displays the serial data received from both
	  ports in Dynamic C STDIO window.

	  OP6800 Demo Board Setup:
	  ------------------------
	  1. Connect a +12v power source to the OP6800 Demo Board connector
	     J3/J5 as follows:

	   	PS     	OP6800 Demo Board
	   	--			-----------------
	   	+12v		+RAW Terminal  
	   	GND      GND Terminal

     2. Add a wire jumper between the following points on the OP6800 Demo Board:
     	   a) On connector J3/J5 connect TXB to RXC 
         b) On connector J3/J5 connect TXC to RXB	

         					+-----------------+
         					|						|
         [ GND | Vcc | TXB | RXB | TXC | RXC | +485 | -485 ]
										|		|
										+-----+

     Test Instructions:
     ------------------    
     1. Compile and Run this program.
	  2. View data being sent between serial ports in the STDIO window.
	  
**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.

// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define CINBUFSIZE 	255
#define COUTBUFSIZE 	255

#define BINBUFSIZE 	255
#define BOUTBUFSIZE  255

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 


///////////////////////////////////////////////////////////////////////////


void main()
{
	auto int i, ch;
	auto char buffer[64];	//buffer used for serial data
	
	static const char string1[] = {"This message has been Rcv'd from serial port B !!!\n\n\r"};
	static const char string2[] = {"This message has been Rcv'd from serial port C !!!\n\n\r"};


	//---------------------------------------------------------------------
	//	Initialize the controller
	//---------------------------------------------------------------------
	brdInit();		//required for OP6800 series boards

	// Initialize serial portB, set baud rate to 19200
 	serBopen(19200);
	serBwrFlush();
 	serBrdFlush();

  	// Initialize serial portC, set baud rate to 19200
   serCopen(19200);
   serCwrFlush();
   serCrdFlush();   
  
 	// Required for OP6800 series bds...must be done after serXopen function(s)  
	serMode(0);
 	
	// Clear data buffer
   memset(buffer, 0x00, sizeof(buffer));
   
   printf("\nStart of Sample Program!!!\n\n\n\r");
   //---------------------------------------------------------------------
   // Do continuous loop transmitting data between serial ports B and C	
   //---------------------------------------------------------------------
   for(;;)
   {
   	//------------------------------------------------------------------
   	// Transmit an ascii string from serial port B to serial port C
		//------------------------------------------------------------------
		memcpy(buffer, string1, strlen(string1));
     	serBputs(buffer);
     	memset(buffer, 0x00, sizeof(buffer));

		// Get the data string that was transmitted by serial port B
     	i = 0;
     	while((ch = serCgetc()) != '\r')
     	{
			// Copy only valid RCV'd characters to the buffer
			if(ch != -1)
			{
				buffer[i++] = ch;	
			}
		}
     	buffer[i++] = ch; 		//copy '\r' to the data buffer
     	buffer[i]   = '\0';     //terminate the ascii string
     	

     	// Display ascii string received from serial port B
   	printf("B: %s", buffer);

		//------------------------------------------------------------------   	
   	// Transmit an ascii string from serial port C to serial port B
		//------------------------------------------------------------------	

		memcpy(buffer, string2, strlen(string2));
   	serCputs(buffer);
		memset(buffer, 0x00, sizeof(buffer));
		
   	// Get the data string that was transmitted by port C
    	i = 0;
     	while((ch = serBgetc()) != '\r')
     	{
     		// Copy only valid RCV'd characters to the buffer
			if(ch != -1)
			{
				buffer[i++] = ch;
			}
		}
		buffer[i++] = ch;			 //copy '\r' to the data buffer
     	buffer[i]   = '\0';      //terminate the ascii string

     	// Display ascii string received from serial port C
     	printf("C: %s", buffer);

  		// Clear buffer
		memset(buffer, 0x00, sizeof(buffer));
   }
}

