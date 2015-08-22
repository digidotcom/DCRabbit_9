/*******************************************************************************

        Samples\RCM2100\ethcore2.c
        Z-World, 2000

        This program takes anything that comes in on PORT and sends it
        out serial port c.  It uses SW2 as a signal that the connection
        should be closed and PA0 as an indication that there is an open
        connection.  You could change SW2 and PA0 to something that your
        application controls.

        It is important to note that a TCP/IP connection that it is
        difficult to detect a broken connection when there is no data
        moving across the link.  It is recommended that you occasionally
        send data across the link or implement timeouts to handle this
        situation.
        
*******************************************************************************/
#class auto

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
 *
 * It is best to explicitly turn off DHCP in your configuration.  This
 * will allow the console to completely manage DHCP, so that it does
 * not acquire the lease at startup in sock_init() when it may not need
 * to (if the user has not turned on DHCP).
 */
#define TCPCONFIG 1


#define PORT 23		// User macro for TCP port to use for processing

#use "dcrtcp.lib"

/*
 *		TIMEOUT is the number of seconds of no activity before closing
 *		the connection.
 *
 */

#define TIMEOUT		60

/*
 *		Serial port Settings
 *		Serial PORT 1=A, 2=B, 3=C, 4=D
 *
 */

#define SERIAL_PORT	1
#define BAUD_RATE		57600L
#define INBUFSIZE		127
#define OUTBUFSIZE 	127

#if (SERIAL_PORT==0)

	#define serXopen  	serAopen
	#define serXread  	serAread
	#define serXwrite 	serAwrite
	#define serXclose 	serAclose
	#define serXwrFlush	serAwrFlush
	#define serXrdFlush	serArdFlush
	#define serXwrFree	serAwrFree
	
	#define AINBUFSIZE	INBUFSIZE
	#define AOUTBUFSIZE	OUTBUFSIZE
	
#elif (SERIAL_PORT==1)

	#define serXopen  	serBopen
	#define serXread  	serBread
	#define serXwrite 	serBwrite
	#define serXclose 	serBclose
	#define serXwrFlush	serBwrFlush
	#define serXrdFlush	serBrdFlush
	#define serXwrFree	serBwrFree

	#define BINBUFSIZE	INBUFSIZE
	#define BOUTBUFSIZE	OUTBUFSIZE
	
#elif (SERIAL_PORT==2)

	#define serXopen  	serCopen
	#define serXread  	serCread
	#define serXwrite 	serCwrite
	#define serXclose 	serCclose
	#define serXwrFlush	serCwrFlush
	#define serXrdFlush	serCrdFlush
	#define serXwrFree	serCwrFree

	#define CINBUFSIZE	INBUFSIZE
	#define COUTBUFSIZE	OUTBUFSIZE
	
#elif (SERIAL_PORT==3)

	#define serXopen  	serDopen
	#define serXread  	serDread
	#define serXwrite 	serDwrite
	#define serXclose 	serDclose
	#define serXwrFlush	serDwrFlush
	#define serXrdFlush	serDrdFlush
	#define serXwrFree	serDwrFree

	#define DINBUFSIZE	INBUFSIZE
	#define DOUTBUFSIZE	OUTBUFSIZE
	
#endif

/*
 *		debounce_keys(int reset)
 *		This function is called in two modes.
 *
 *		reset==1
 *		you are asking the function to clear the debouncing
 *		registers.
 *
 *		reset==0
 *		you are asking it to search for the key moving from
 *		the off to the on position. Which will form a 1,1,0
 *		in killbutton[2,1,0].
 *
 */

int debounce_keys(int reset)
{
	static int killbutton[3];

	if(reset) {
		killbutton[2]=killbutton[1]=killbutton[0]=1;
		return 0;
	}

	killbutton[2]=killbutton[1];
	killbutton[1]=killbutton[0];
	killbutton[0]=RdPortI(PBDR)&0x04;

	if(killbutton[0]&& !killbutton[1] && !killbutton[2])
		return 1;

	return 0;
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

#define CON_INIT			0
#define CON_LISTEN		1
#define CON_OPEN			2
#define CON_WAITCLOSED	3

void main()
{
	int state;
	long timeout;
	int bytes_read;
	
	static char buffer[64];
	static tcp_Socket socket;

	WrPortI(SPCR,NULL,0x84); 		// make PORT A outputs
	WrPortI(PADR,NULL,0x0f); 		// turn lights off
	
	sock_init();						// initialize DCRTCP

	serXopen(BAUD_RATE);				// set up serial port
	
	tcp_reserveport(PORT);			// set up PORT for SYN Queueing
											//    which will hold a pending connection
											//    until we are ready to process it.
	state=CON_INIT;

	while(1) {
		if(state==CON_OPEN) {
			if(debounce_keys(0)) {
				sock_close(&socket);
				state=CON_WAITCLOSED;
			}
		}

		/*
		 *		Make sure that the connection hasn't
		 *		closed on us.
		 *
		 */
		
		if(tcp_tick(&socket)==0 && state!=CON_INIT) {
			state=CON_INIT;
			serXrdFlush();
			serXwrFlush();
			serXwrite("connection closed\r\n",18);
			WrPortI(PADR,NULL,0x0f);
			debounce_keys(1); // reset debouncing
		}
			
		switch(state) {
			case CON_INIT:
				tcp_listen(&socket,PORT,0,0,NULL,0);
				state=CON_LISTEN;
				break;

			case CON_LISTEN:
				if(sock_established(&socket)) {
					state=CON_OPEN;
					timeout=SEC_TIMER+TIMEOUT;
					serXrdFlush();
					serXwrFlush();
					WrPortI(PADR,NULL,0x0e);
					debounce_keys(1); // reset debouncing
				}
				break;

			case CON_OPEN:
				if(timeout<SEC_TIMER) {
					sock_close(&socket);
					state=CON_WAITCLOSED;
					break;
				}

				/*
				 *		read as many bytes from the socket as we have
				 *		room in the serial buffer.
				 *
				 */
			
				bytes_read=sock_fastread(&socket,buffer,min(sizeof(buffer),serXwrFree()));

				/*
				 *		close the socket on an error
				 *
				 */

				if(bytes_read<0) {
					sock_close(&socket);
					state=CON_WAITCLOSED;
					break;
				}

				/*
				 *		copy any bytes that we read
				 *
				 */

				if(bytes_read>0) {
					timeout=SEC_TIMER+TIMEOUT;
					serXwrite(buffer,bytes_read);
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
						state=CON_WAITCLOSED;
					}
				}
				break;

			case CON_WAITCLOSED:
				break;
		}
	}
}
