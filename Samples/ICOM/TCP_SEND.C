/********************************************************************
	tcp_send.c
	Z-World, 2000

 	This sample program is used with Intellicom Series.

	This program and tcp_respond.c are to be executed on two different
	Intellicom boards so that the two boards communicate with each
	other.  In the absence of a second board, pcrespond.exe can be
	used on the PC console side at the command prompt. This executable
	and source code is located in "...\samples\icom\windows" directory.

	When a key on the keypad is pressed, a message associated with
	that key	is sent to a specified destination address and port.
	The destination then responds to that message.  The response is
	displayed to the LCD.

********************************************************************/
#class auto					/* Change local var storage default to "auto" */

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

/*
 * 	Have TCP install text error messages when trouble occurs.  If not
 * 	defined, then TcpSocket.err_msg is not used at all.
 * 	TcpSocket.err_msg == NULL if none.
 */
#define _SOCKET_MESSAGES

/*
 * 	How many seconds to wait for active connection.
 */
#define  CONNECT_TIMEOUT	10

/*
 * Also note that you will need to set up the messages in the main()
 * function.
 */

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"

// The structure that holds a message and the destination
typedef struct {
	char *text;
	char *addr;
	word port;
} Message;

#define IGNORE 99

/*
 * Send the message specified by the key value.  Note that the message
 * number is actually one less than the key number (since key number 0
 * cannot be used).
 */
void SendMessage(unsigned key, Message messageArray[])
{
	auto tcp_Socket	tcpSock;
	auto longword		remIP;
	auto int 			numBytes;
	auto char 			buffer[500];
	auto longword 		timeout;
	auto char *			p;
	auto int 			result;

	dispClear();
	remIP = inet_addr(messageArray[key-1].addr);
	tcp_open(&tcpSock, 0, remIP, messageArray[key-1].port, NULL);

	/* Wait for connection -- 10 seconds. */
	timeout = SEC_TIMER + CONNECT_TIMEOUT;
	while( !tcp_established(&tcpSock) ) {
		if( (long)(SEC_TIMER - timeout) >= 0 ) {
			dispPrintf( "ERROR: connect: timeout");
			// exit(99);
			return;
		}
		if( ! tcp_tick( (sock_type *) & tcpSock ) ) {
			dispPrintf( "ERROR: connect: %s\n", tcpSock.err_msg );
			// exit(99);
			return;
		}
	}

	/*  Write the message text, including the trailing NUL byte. */
	p = messageArray[key-1].text;
	for( numBytes = strlen(p)+1 ; numBytes > 0 ; 0 ) {
		result = sock_fastwrite( &tcpSock, p, numBytes );
		if( result < 0 || ! tcp_tick( (sock_type *) &tcpSock ) ) {
			dispPrintf( "ERROR: write: %s\n", tcpSock.err_msg );
			// exit( 99 );
			return ;
		}
		numBytes -= result;
		p += result;
	}

	dispPrintf("Message sent");

	/*  Wait for some data to appear, or an error... */
	while( 0 == (numBytes = sock_fastread(&tcpSock, buffer, sizeof(buffer))) ) {
		if( ! tcp_tick( &tcpSock ) ) {
	bad_read:
			dispPrintf( "ERROR: read: %s\n", tcpSock.err_msg );
			exit( 99 );
		}
	}
	dispClear();
	if (numBytes != -1) {
		buffer[numBytes] = '\0';
		dispPrintf("%s", buffer);
	} else {
		dispPrintf("Error");
	}
	sock_close(&tcpSock);
	while (tcp_tick(&tcpSock) != 0);
}

void main(void)
{
	Message 		messageArray[12];
	int 			i;
	unsigned 	key;	  // User keypress

	// Initialize the message array
	for (i = 0; i < 12; i++) {
		messageArray[i].text = "";
	}

	// Define messages here--note that you only need define the messages you
	// will actually use.  The IP addresses should NOT be MY_IP_ADDRESS !
	messageArray[0].text = "This is test 1.";
	messageArray[0].addr = "10.10.6.111";
	messageArray[0].port = 4040;

	messageArray[1].text = "This is test 2.";
	messageArray[1].addr = "10.10.6.111";
	messageArray[1].port = 4040;

	brdInit();
	sock_init();

	// Configure the upper row of keys on the keypad, in order from left
	// to right
	for (i = 0; i < 6; i++) {
		// Only enable a key if there is a corresponding message
		if (strcmp(messageArray[i].text, "") != 0) {
			keyConfig ( 5-i, i+1, 0, 0, 0, 0, 0 );
		} else {
			keyConfig ( 5-i, IGNORE, 0, 0, 0, 0, 0 );
		}
	}
	// Configure the lower row of keys on the keypad, in order from left
	// to right
	for (i = 6; i < 12; i++) {
		// Only enable a key if there is a corresponding message
		if (strcmp(messageArray[i].text, "") != 0) {
			keyConfig ( 19-i, i+1, 0, 0, 0, 0, 0 );
		} else {
			keyConfig ( 19-i, IGNORE, 0, 0, 0, 0, 0 );
		}
	}

	while (1) {
		costate {								//	Process Keypad Press/Hold/Release
			keyProcess ();
			waitfor ( DelayMs(10) );
		}

		costate {								//	Process Keypad Press/Hold/Release
			waitfor ( key = keyGet() );	//	Wait for Keypress
			if (key != IGNORE) {
				SendMessage(key, messageArray);
			}
		}
	}
}