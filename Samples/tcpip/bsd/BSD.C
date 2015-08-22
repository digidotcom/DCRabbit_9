/*******************************************************************************
        bsd.c
        Z-World, 2000

        A demonstration of several advanced TCP/IP functions, that are used
        for namespace (DNS) issues, and finding out information about the
        remote host.
*******************************************************************************/

/*********************************************************
 * Example of the use of various naming functions
 *
 * To use:
 *     Set the IP address / netmask above.
 *     Set the domainname / hostname above.
 *     Run, and when it prints your domain name,
 *       telnet to it from another host
 *     You should see information about the connection
 *       be printed out.
 **********************************************************/
#class auto


/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1


/*
 * 	Place to listen for incoming request.  23 is TELNET port.
 */
#define  MY_TEST_PORT	23

/*
 * Domain name configuration
 *
 * This sets your hostname and domainname. If your computer has an
 * internet address like: "test.zweng.com", your hostname
 * is "test", and your domainname is "zweng.com". 
 */
#define MY_HOST 	"test"
#define MY_DOMAIN	"zweng.com"

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"

tcp_Socket tcpsock;
tcp_Socket *const s = &tcpsock;

char textbuf[64];
char *ip_to_text(longword ip)
{
	sprintf(textbuf,"%d.%d.%d.%d",
		(byte)(ip >> 24),
		(byte)(ip >> 16),
		(byte)(ip >> 8),
		(byte)(ip)
		);
	return textbuf;
}

void main()
{
	int status;
	struct sockaddr sock_addr;
	
	sock_init();

	sethostname(MY_HOST);
	setdomainname(MY_DOMAIN);

	printf("We are %s.%s\n", gethostname(NULL, 0), getdomainname(NULL, 0));

	tcp_listen((tcp_Socket*)s,MY_TEST_PORT,0,0,NULL,0);
	while( ! tcp_established( (tcp_Socket *)s ) ) {
		if( ! tcp_tick( (sock_type *) s ) ) {
			printf( "ERROR: listening\n" );
			exit( 99 );
		}
	}

	printf("Receiving incoming connection\npsocket(s) -> ");
	psocket((sock_type *)s);

	printf("\n\n");
	if(0 == getpeername((sock_type *)s, &sock_addr, NULL)) {
		printf("getpeername: %s:%d",
			ip_to_text(sock_addr.s_ip),
			sock_addr.s_port
			);
	} else {
		printf("getpeername() retuned no data!\n");
	}

	printf("\n");
	
	if(0 == getsockname((sock_type *)s, &sock_addr, NULL)) {
		printf("getmyname: %s:%d",
			ip_to_text(sock_addr.s_ip),
			sock_addr.s_port
			);	
	} else {
		printf("getsockname() retuned no data!\n");
	}

	
	printf("\n\nClosing socket...\n");
	
	sock_close(s);
	// Discard any data until fully closed.
	while(tcp_tick(s)) sock_fastread(s, NULL, 1024);
		
	printf("Socket closed. All done!\n");
}
