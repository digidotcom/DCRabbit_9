/**********************************************************************
    tcp.c
    Z-World, 2001

    This program is intended to be run on a DeviceMate.  It can be used
    in conjunction with the TCP/IP-related target samples.  Be sure to
    set the network configuration correctly.
**********************************************************************/

#define TC_I_AM_DEVMATE		// necessary for all DeviceMates

//#define TARGETPROC_SERC
//#define XTC_DEBUG
//#define TARGETPROC_TCP_VERBOSE			// Print TCP subsystem state changes
//#define XTC_VERBOSE						// Print XTC state transitions etc.
//#define XTC_PRINTPKT						// Dump packets transmitted or received
//#define XTC_PRINTPKT_COLOR_DARKBG		// Print using ANSI color escapes on a tube
//#define XTC_PRINTPKT_COLOR_LIGHTBG	//   ...with dark or light background.

/*
 * Define this if all printfs are to be directed to a telnet
 * session.  To use this, run this program on the DeviceMate
 * and immediately telnet to port 23 (the default) on this
 * IP address.  After this, the program will run as normal,
 * but all printf output will appear on the telnet console.
 * You can press enter in the telnet session to pause/resume
 * execution (at "printf" points only).
 */
//#define printf tprintf
nodebug void tprintf(char *fmt, ...);

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1


int tinit;

/*
 * Choose TCP/IP services.
 */ 
#define USE_TC_TCPIP
#use "tc_conf.lib"


static tcp_Socket telsock;

nodebug void __qe2t(int c, char *__printfbuf, int *__printfcnt)
{
	if (*__printfcnt < 255) 
	{
		__printfbuf[(*__printfcnt)++] = c;
	}	
	else
		__printfbuf[255] = '\0';

}

nodebug void tprintf(char *fmt, ...)
{
	auto int __printfcnt;
	auto char __printfbuf[256];
	auto char * args;

	args = (char*)&fmt + sizeof(fmt);
	__printfcnt = 0;
	doprnt(__qe2t, fmt, args, NULL, __printfbuf, &__printfcnt);
	__qe2t(0, __printfbuf, &__printfcnt);
	if (tinit)
		sock_write(&telsock, __printfbuf, __printfcnt);
	if (sock_bytesready(&telsock) > 0) {
		sock_fastread(&telsock, __printfbuf, 256);
		sock_write(&telsock, "...paused...\n", 13);
		while (sock_bytesready(&telsock) <= 0)
			tcp_tick(NULL);
		sock_fastread(&telsock, __printfbuf, 256);
	}
		
}

void main()  
{

	tinit = 0;

	sock_init();
	targetproc_init();

#ifdef printf
	tinit = 1;
	// Init debugging socket for tprintf()
	tcp_listen(&telsock, 23, 0, 0, NULL, 0);

	/* Wait for establishment.  You need to telnet to this Rabbit. */
	while( ! tcp_established( & telsock ) ) {
		if( ! tcp_tick( (sock_type *) & telsock ) ) {
			printf( "ERROR: telsock: %s\n", telsock.err_msg );
			exit( 99 );
		}
	}
	printf("Hello telnet console!\n");
#endif
	
	
	for (;;) {
		targetproc_tick();
	}
		
}   /* end main() */
