/********************************************************************
	tcp_respond.c
	Z-World, 2000

 	This sample program is used with Intellicom Series.

	This program and tcp_send.c are to be executed on two different
	Intellicom boards so that the two boards communicate with each
	other.  A TCP socket is used to send a very reliable datagram.

	In the absence of a second board, pcsend.exe can be used on
	the PC console side at the command prompt.  This executable and
	source code is located in \samples\icom\windows directory.

	If you use telnet to connect (ie "telnet 10.10.6.100 4040"),
	this code will disconnect after sending each response.  Simply
	re-connect (run telnet again) to get the next response.

	This program waits for a message from another machine.  The message
	received is displayed on the LCD, and the user is allowed to respond
	by pressing a key on the keypad.  The response is then sent to the
	remote machine.

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

// Port to listen on
#define PORT				4040

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"

#define IGNORE 99

/*
 * Receive a message.  The socket is re-opened and message read.  The need
 * for a response is indicated.  tcp_tick() fails if peer closed socket.
 */
void RecvMessage(tcp_Socket* tcpSock, int* respond)
{
	auto char buffer[500];
	auto int numBytes;

	tcp_listen(tcpSock, PORT, 0, 0, NULL, 0);
	dispClear();

	/* Wait for connection. */
	while( ! tcp_established( tcpSock ) ) {
		if( ! tcp_tick( (sock_type *) tcpSock ) ) {
			dispPrintf( "ERROR: listening: %s\n", tcpSock->err_msg );
			exit( 99 );
		}
	}

	/*  Wait for some data to appear, or an error... */
	while( 0 == (numBytes = sock_fastread(tcpSock, buffer, sizeof(buffer))) ) {
		if( ! tcp_tick( tcpSock ) ) {
	bad_read:
			dispPrintf( "ERROR: read: %s\n", tcpSock->err_msg );
			exit( 99 );
		}
	}
	if (numBytes == -1) {
		goto bad_read;
	}

	buffer[numBytes] = '\0';
	dispPrintf("%s", buffer);
	*respond = 1;	// Indicate the need for a response

}   /* end RecvMessage() */

/*
 * Send the response to the remote machine.  Close the socket.
 */
void SendMessage(tcp_Socket *tcpSock, unsigned key, char* messages[])
{
	auto char * p;
	auto int 	len;

	/* Use sock_fastwrite() here to encourage good programming practices. */

	sock_write(tcpSock, messages[key-1], strlen(messages[key-1])+1);
	sock_close(tcpSock);
	while (tcp_tick(tcpSock) != 0);
	dispClear();
	dispPrintf("Response sent");
}

void main(void)
{
	char* messages[12];
	int i;
	unsigned key;    // User keypress
	tcp_Socket tcpSock;
	int respond;

	respond = 0;

	// Initialize the message array
	for (i = 0; i < 12; i++) {
		messages[i] = "";
	}

	// Define messages here--note that you only need define the messages
	// you will actually use.
	messages[0] = "I hear ya";
	messages[1] = "Hello, there";
	messages[2] = "I'm here";
	messages[3] = "This Rabbit is fast!";
   messages[6] = "That's right, Charlie!";
   messages[7] = "Daisy, Daisy, Give me your answer, do.";

	brdInit();
	sock_init();

	// Configure the upper row of keys on the keypad, in order from left
	// to right
	for (i = 0; i < 6; i++) {
		// Only enable a key if there is a corresponding message
		if (strcmp(messages[i], "") != 0) {
			keyConfig ( 5-i, i+1, 0, 0, 0, 0, 0 );
		} else {
			keyConfig ( 5-i, IGNORE, 0, 0, 0, 0, 0 );
		}
	}
	// Configure the lower row of keys on the keypad, in order from left
	// to right
	for (i = 6; i < 12; i++) {
		// Only enable a key if there is a corresponding message
		if (strcmp(messages[i], "") != 0) {
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
			// Only handle the keypress if it corresponds to a message and if
			// a response is currently needed
			if ((key != IGNORE) && (respond == 1)) {
				SendMessage(&tcpSock, key, messages);
				respond = 0;
			}
		}

		// Receive a message.  RecvMessage() will block until something comes in.
		if (respond == 0) {
			RecvMessage(&tcpSock, &respond);
		}
	}
}