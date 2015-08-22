/**************************************************************************

     Samples\BL2000\RS232\Puts.c

     Z-World, 2001

	  This sample program is for the BL20XX series controllers.

	  This program transmits and then receives an ASCII string on serial 
	  ports B and C. It also displays the serial data received from both
	  ports in Dynamic C STDIO window.
	
     Add a wire jumper between the following points on the controller:
     	 On connector J1 connect RS232 TXD1 to RXD2 
       On connector J1 connect RS232 TXD2 to RXD1	
       
     Run this program.

**************************************************************************/
#class auto


// The input and output buffers sizes are defined here. If these
// are not defined to be (2^n)-1, where n = 1...15, or they are
// not defined at all, they will default to 31 and a compiler
// warning will be displayed.

#define BINBUFSIZE 	255
#define BOUTBUFSIZE 	255

#define CINBUFSIZE 	255
#define COUTBUFSIZE  255


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
	
	static const char string1[] = "Rcv'd data on serial port C from serial port B\n\r";
	static const char string2[] = "Rcv'd data on serial port B from serial Port C\n\r";

	brdInit();		//required for BL2000 series boards
	
	// initialize serial portB, set baud rate to 19200
   serBopen(19200);
   serBwrFlush();
   serBrdFlush();

   // initialize serial portC, set baud rate to 19200
   serCopen(19200);
   serCwrFlush();
   serCrdFlush();

   serMode(0);			//required for BL2000 series bds...must be done after serXopen function(s)
   
	//clear data buffer
   memset(buffer, 0x00, sizeof(buffer));


   printf("\nStart of Sample Program!!!\n\n\n\r");
   for(;;)
   {
   	//transmit an ascii string from serial port B to serial port C
		memcpy(buffer, string1, strlen(string1));
     	serBputs(buffer);
		memset(buffer, 0x00, sizeof(buffer));
     	
		//get the data string that was transmitted by serial port B
     	i = 0;
     	while((ch = serCgetc()) != '\r')
     	{
			if(ch != -1)
			{
				buffer[i++] = ch;
			}
		}
     	buffer[i++] = ch; 		//copy '\r' to the data buffer
     	buffer[i]   = '\0';     //terminate the ascii string
     	
     	// display ascii string received from serial port B
   	printf("%s", buffer);
    	
   	//transmit an ascii string from serial port C to serial port B
		memcpy(buffer, string2, strlen(string2));
   	serCputs(buffer);
		memset(buffer, 0x00, sizeof(buffer));

   	//get the data string that was transmitted by port C
    	i = 0;
     	while((ch = serBgetc()) != '\r')
     	{	
			if(ch != -1)
			{
				buffer[i++] = ch;
			}
		}
		buffer[i++] = ch;			 //copy '\r' to the data buffer
     	buffer[i]   = '\0';      //terminate the ascii string

     	// display ascii string received from serial port C
     	printf("%s", buffer);
   }
}
