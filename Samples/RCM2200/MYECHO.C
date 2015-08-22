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
        Samples/RCM2200/echo.c

        A basic server, that when a client connects, echos back to them
        any data that they send.  Choose port to server by #define PORT
        value.  Internet std sez port #7 is echo, port #9 is discard.
        Most telnet programs accept a second arg of which port to use.
        Ie, from a host, try:    telnet 10.10.6.100 9

        TELNET port runs in line mode, any other port number is binary xfer.

*******************************************************************************/

// Set a standard configuration
#define TCPCONFIG 1

#memmap xmem
#use "dcrtcp.lib"

/*  Port #7 is ECHO, port #23 is TELNET */
#define IPPORT_TELNET	23
#define IPPORT_ECHO		7
#define IPPORT_DISCARD  9

#define PORT	(IPPORT_TELNET)


tcp_Socket echosock;

void main()
{
	char buffer[2048];
	int status;
	int  len;

	sock_init();

	while(1) {
		echosock.err_msg = '\0';
		tcp_listen(&echosock,PORT,0,0,NULL,0);

		/* Wait for connection. */
		printf( " waiting .. " );
		while( ! tcp_established( & echosock ) ) {
			if( ! tcp_tick( (sock_type *) & echosock ) ) {
				printf( "ERROR: listen: %s\n", echosock.err_msg );
				goto again;
			}
		}

		printf("Receiving incoming connection, port %d\n", PORT);
		if( PORT == IPPORT_TELNET ) {
			/**  Always handle TELNET_PORT in ASCII mode.  **/
			sock_mode(&echosock,TCP_MODE_ASCII);

			/*  While connection exists, try to get a message to echo.
			 *  If connection breaks, most likely tcp_tick() will return 0
			 *  rather than sock_fastread() returning -1.
			 */
			while(tcp_tick(&echosock)) {
				len = sock_gets( &echosock, buffer, sizeof(buffer)-1 );
				if( len > 0 ) {
					/* A line of text has been given to us!! */
					for( len=(int)(SEC_TIMER+5); (int)SEC_TIMER != len ; ) tcp_tick(NULL);
					sock_puts(&echosock,buffer);
				} else
				if( len < 0 ) {
					printf( "ERROR: socket gets\n" );
					goto again;
				}
			}

		} else {
			/**  Other ports are BINARY xfers.  **/
			sock_mode(&echosock,TCP_MODE_BINARY);

			/*  While connection exists, try to get a message to echo.
			 *  If connection breaks, most likely tcp_tick() will return 0
			 *  rather than sock_fastread() returning -1.
			 */
			while(tcp_tick(&echosock)) {
				if((len=sock_fastread(&echosock,buffer,sizeof(buffer))) > 0) {
					if( PORT != IPPORT_DISCARD ) {
						sock_flushnext(&echosock);  		/* Hint to set the PSH flag */
						sock_write(&echosock,buffer,len);
					}
				} else
				if( len < 0 ) {
					printf( "ERROR: socket read\n" );
					goto again;
				}
			}   /* while socket still open.. */
		}

		/* These services stay open as long as the client holds it.
		 *  Therefore, closing isn't really necessary.
		 *  XXX - is this needed before calling tcp_listen() again?
		 */
	again :
//		sock_close(&echosock);
	}   /* end forever loop.. */

}