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
/*******************************************************************************
   telnet.c

   This program takes anything that comes in on a PORT and sends it
   out serial port E. It uses a I/O input to indicate that the TCP/IP
   connection should be closed and a I/O output to toggle a LED to
   indicate that there's an active connection.

   PC terminal utility setup:
	--------------------------
	1. Startup your PC serial utility such	as Tera Term.
	2. Configure the serial parameters to be the following:
	   a) Baud Rate of 19200, 8 bits, no parity and 1 stop bit.
	   b) Enable the "Local Echo" option.
      c) Enable option to append linefeeds to incomming  and out going
         messages.

  	PowerCoreFLEX protoboard connections:
	-------------------------------
	1. Connect PC (tx) to protoboard RXF on J1, pin 4.
  	2. Connect PC (rcv) to protoboard TXF on J1, pin 6.
	3. Connect PC GND to protoboard GND on J1, pin 9.

   Test Instructions:
	------------------
	1. Compile and run this program.
   2. Start a Telnet session to this board.
   3. View the LED, when toggling it indicates that there's an active telnet
      connection.
   4. Press S2 to close the telent connection, LED0 (DS5) should stop toggling
   	to indicate that the telnet connection has been closed.
   5. Type message in telnet window, then view same message in PC serial
      PC serial utility program.


*******************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"


/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

// User macro for TCP port to use for processing
//  23 = telnet.
#define PORT 23

#use "dcrtcp.lib"

/*
 *		MACRO used to indicate when a key has been pressed.
 */

#define KEY_PRESSED	1

/*
 *		TIMEOUT is the number of seconds of no activity before closing
 *		the connection.
 *
 */

#define TIMEOUT		60


/*
 *		Led control MACRO's
 *
 */

#define TOGGLE_LED		0
#define RESET_LED   		1
#define LED_DELAY			100


/*
 *		Serial port Settings
 *		Serial PORT 1=A, 2=B, 3=C, 4=D, 5=E, 6=F
 */

#define SERIAL_PORT	6
#define BAUD_RATE		19200L
#define INBUFSIZE		127
#define OUTBUFSIZE 	127

#if (SERIAL_PORT==1)

	#define serXopen  	serAopen
	#define serXread  	serAread
	#define serXwrite 	serAwrite
	#define serXclose 	serAclose
	#define serXwrFlush	serAwrFlush
	#define serXrdFlush	serArdFlush
	#define serXwrFree	serAwrFree

	#define AINBUFSIZE	INBUFSIZE
	#define AOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==2)

	#define serXopen  	serBopen
	#define serXread  	serBread
	#define serXwrite 	serBwrite
	#define serXclose 	serBclose
	#define serXwrFlush	serBwrFlush
	#define serXrdFlush	serBrdFlush
	#define serXwrFree	serBwrFree

	#define BINBUFSIZE	INBUFSIZE
	#define BOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==3)

	#define serXopen  	serCopen
	#define serXread  	serCread
	#define serXwrite 	serCwrite
	#define serXclose 	serCclose
	#define serXwrFlush	serCwrFlush
	#define serXrdFlush	serCrdFlush
	#define serXwrFree	serCwrFree

	#define CINBUFSIZE	INBUFSIZE
	#define COUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==4)

	#define serXopen  	serDopen
	#define serXread  	serDread
	#define serXwrite 	serDwrite
	#define serXclose 	serDclose
	#define serXwrFlush	serDwrFlush
	#define serXrdFlush	serDrdFlush
	#define serXwrFree	serDwrFree

	#define DINBUFSIZE	INBUFSIZE
	#define DOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==5)

	#define serXopen  	serEopen
	#define serXread  	serEread
	#define serXwrite 	serEwrite
	#define serXclose 	serEclose
	#define serXwrFlush	serEwrFlush
	#define serXrdFlush	serErdFlush
	#define serXwrFree	serEwrFree

	#define EINBUFSIZE	INBUFSIZE
	#define EOUTBUFSIZE	OUTBUFSIZE


#elif (SERIAL_PORT==6)

	#define serXopen  	serFopen
	#define serXread  	serFread
	#define serXwrite 	serFwrite
	#define serXclose 	serFclose
	#define serXwrFlush	serFwrFlush
	#define serXrdFlush	serFrdFlush
	#define serXwrFree	serFwrFree

	#define FINBUFSIZE	INBUFSIZE
	#define FOUTBUFSIZE	OUTBUFSIZE

#endif

///////////////////////////////////////////////////////////////////////

/*
 *
 * Function to strip out telnet commands
 * Losses if there are NUL bytes inside of telnet string.
 */

int strip_telnet_cmds(char *ptr, int bytes_read)
{
	int index;

	// write the NUL terminator at the end of the data string
	ptr[bytes_read] = '\0';

	index = 0;
	// Check for telnet command header, if none exit without altering the buffer
	if(ptr[index] == 0xff) {

		// Scan data looking for telnet commands
		while(ptr[index] == 0xff && index < bytes_read) {

			switch(ptr[index+1])
			{
				case 251:
				case 252:
				case 253:
				case 254:
					index += 3;
					break;

				case 255:
					break;

				default:
					index += 2;
					break;
			}
		}
		if(index >= bytes_read)
		{	//return 0 for no valid ascii data recieved in this packet
			return(0);
		}
		//overwrite the leading telnet commands with the valid ascii message
		strcpy(ptr, ptr+index);
	}

	//return with length of buffer
	return(strlen(ptr));
}

/*
 *		led(void)
 *		This function displays a binary count on the LED's.
 *
 */

void led(int mode)
{
	static int toggle;
	static unsigned long led_delay;

	#GLOBAL_INIT{toggle = 0;}

	if(mode == RESET_LED)
	{
		led_delay = MS_TIMER+LED_DELAY;
	  	ledOut(0, 1);
	}
	else if((long) (MS_TIMER - led_delay) >= 0 )
	{
		//update led
		if(toggle)
		{	// turn led OFF
			ledOut(0, 1);
			toggle = 0;
		}
		else
		{	// turn led ON
			ledOut(0, 0);
			toggle = 1;
		}
		// set the interval for the next led update
		led_delay = MS_TIMER+LED_DELAY;
	}
}

/*
 *		debounce_keys(void)
 *		This function checks for a valid keypress
 *
 */

int debounce_key(void)
{
	auto int status;
	static int state;
	static unsigned long debounce_period;

	// initialize state machine during system initialization
	#GLOBAL_INIT {state=0;}

	status = !KEY_PRESSED;
	switch(state)
	{
		case 0:
		   // wait for the switch to be pressed
			if(!digIn(0))
			{  //set debounce period
				state++;
				debounce_period = MS_TIMER + 50; // debounce period = 50ms
			}
			break;

		case 1:
			// check if switch is still being pressed, if not reset state machine
			if((long) (MS_TIMER-debounce_period) >= 0 )
			{
				if(!digIn(0))
				{
					state++;
					status = KEY_PRESSED;
				}
				else
				{
					state = 0;
				}
			}
			break;

		case 2:
		   // wait for the switch to be released
			if(digIn(0))
			{
				state++;
				debounce_period = MS_TIMER + 100;	// debounce period = 100ms
			}
			break;

		case 3:
			// verify that the switch is still released after debounce period
			if((long)(MS_TIMER-debounce_period) >= 0 )
			{
				if(digIn(0))
				{
					state = 0;
				}
				else
				{
					debounce_period = MS_TIMER + 20;	// debounce period = 20ms
				}
			}
			break;

		default: // should never get here!
			state = 0;
			break;
	}
	return(status);
}

/*
 *		main()
 *
 *		This function drives the state machine that manages
 *		the TCP->serial connection.
 *
 *		The INIT state sets a socket up for listening.
 *		The LISTEN state checks for a connection and sets
 *			up the data structures for an open socket.
 *		The OPEN state transfers as much bytes as we have
 *			buffer space from serial to TCP or vice-versa.
 *		The WAITCLOSED state waits for the socket to be
 *			available again.
 *
 */

#define CONNECTION_INIT			0
#define CONNECTION_LISTEN		1
#define CONNECTION_OPEN			2
#define CONNECTION_CLOSED		3

void main()
{
	int state;
	long timeout;
	int bytes_read;

	static char buffer[64];
	static tcp_Socket socket;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

	serXopen(BAUD_RATE);				// set up serial port
   serMode(0);

	sock_init();						// initialize DCRTCP
	tcp_reserveport(PORT);			// set up PORT for SYN Queueing
											// which will hold a pending connection
											// until we are ready to process it.
	state=CONNECTION_INIT;

	while(1) {
		if(state==CONNECTION_OPEN) {
			if(debounce_key()) {
				sock_close(&socket);
				state=CONNECTION_CLOSED;
			}
		}
		/*
		 *		Make sure that the connection hasn't closed on us.
		 *
		 */

		if(tcp_tick(&socket)==0 && state!=CONNECTION_INIT) {
				sock_close(&socket);
				state=CONNECTION_CLOSED;
		}

		switch(state) {
			case CONNECTION_INIT:
				tcp_listen(&socket,PORT,0,0,NULL,0);
				state=CONNECTION_LISTEN;
				break;

			case CONNECTION_LISTEN:
				if(sock_established(&socket)) {
					state=CONNECTION_OPEN;
					timeout=SEC_TIMER+TIMEOUT;
					serXrdFlush();
					serXwrFlush();
					led(RESET_LED);
 			}
				break;

			case CONNECTION_OPEN:

				/*
				 *		close the socket on a timeout
				 *
				 */
				if((long) (SEC_TIMER-timeout) >= 0) {
					sock_close(&socket);
					state=CONNECTION_CLOSED;
					break;
				}

				/*
				 *		read as many bytes from the socket as we have
				 *		room in the serial buffer. Also strip out the
				 *    telnet commands.
				 */
				bytes_read = 0;
				if(sock_bytesready(&socket) != -1)
				{
					bytes_read=sock_fastread(&socket, buffer, min(sizeof(buffer), serXwrFree()));
					bytes_read=strip_telnet_cmds(buffer, bytes_read);
				}

				/*
				 *		close the socket on an error
				 *
				 */

				if(bytes_read<0) {
					sock_close(&socket);
					state=CONNECTION_CLOSED;
					break;
				}

				/*
				 *		copy any bytes that we read
				 *
				 */

				if(bytes_read > 0) {
					timeout=SEC_TIMER+TIMEOUT;
					serXwrite(buffer, bytes_read);
				}

				/*
				 *		read as many bytes from the serial port as we
				 *		have room in the socket buffer.
				 *
				 */

				bytes_read=serXread(buffer,min(sizeof(buffer),sock_tbleft(&socket)),100);
				if(bytes_read>0) {
					timeout=SEC_TIMER+TIMEOUT;
					if(sock_fastwrite(&socket,buffer,bytes_read)<0) {
						sock_close(&socket);
						state=CONNECTION_CLOSED;
					}
				}
				led(TOGGLE_LED);
				break;

			case CONNECTION_CLOSED:
				serXrdFlush();
				serXwrFlush();
				sprintf(buffer, "\n\rConnection closed\n\r");
				serXwrite(buffer, strlen(buffer));
				led(RESET_LED);
				state=CONNECTION_INIT;
				break;
		}
	}
}