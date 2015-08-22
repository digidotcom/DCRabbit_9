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
/**********************************************************************
 *		Samples\TcpIp\active.c
 *		This program demonstrates the tcp_open call.
 *
 * 	A simple demonstration of a TCP/IP session, by retrieving a
 * 	web page from a remote site.  Change DEST and PORT to the
 * 	remote machine.
 *
 **********************************************************************/

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
 */
#define TCPCONFIG 1


#use "dcrtcp.lib"

/*
 *  Remote computer to contact:
 */
#define  DEST 		"www.zworld.com"
#define  PORT 		80

////////////////////////////////////////////////////////////////////////

void main()
{
	char	buffer[100]; 	/* Currently (DC 7.30), printf() has max 127 bytes it can output. */
	int 	bytes_read;
	longword  destIP;
	tcp_Socket socket;

	printf( "initializing..\n" );
	sock_init();
	// Wait for the interface to come up
	while (ifpending(IF_DEFAULT) == IF_COMING_UP) {
		tcp_tick(NULL);
	}
	
	if( 0L == (destIP = resolve(DEST)) ) {
		printf( "ERROR: Cannot resolve \"%s\" into an IP address\n", DEST );
		exit(2);
	}
	tcp_open(&socket,0,destIP,PORT,NULL);

	printf("Waiting for connection...\n");
	while(!sock_established(&socket) && sock_bytesready(&socket)==-1) {
		tcp_tick(NULL);
	}

	printf("Connection established, sending get request...\n");

	/*
	 *  If don't send the HTTP version number, then server believes we are
	 *  a pre-1.0 version client.
	 */
	sock_write(&socket,"GET /\r\n\r\n",9);

	/*
	 *  When tcp_tick()-ing on a specific socket, we get non-zero return while
	 *  it is active, and zero when it is closed (as used here).
	 */
	do {
		bytes_read=sock_fastread(&socket,buffer,sizeof(buffer)-1);

		if(bytes_read>0) {
			buffer[bytes_read] = '\0';
			/*
			 * By using the "%s" format, if there are "%" in the buffer, printf()
			 *  won't try to interpret them!
			 */
			printf("%s",buffer);
		}
	} while(tcp_tick(&socket));

	sock_abort(&socket);
	printf("\nConnection closed...\n");
}