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
        Samples\tcpip\UDP\udp_srv.c

        A UDP example, that receives the 'heartbeat' packets sent from
        udp_cli.c, and will receive either the direct-send packets or
        broadcast packets, depending on the configuration.

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
 */
#define TCPCONFIG 1

/*
 * Define the number of socket buffers that will be allocated for
 * UDP sockets.  We only need one UDP socket, so one socket buffer
 * will be enough.
 */
#define MAX_UDP_SOCKET_BUFFERS 1

/*
 * UDP demo configuration
 */
 
/* what local UDP port to use - we receive packets only sent to this port */
#define LOCAL_PORT	1234

/*
 * If this is set to "0", we will accept a connection from anybody.
 * The first person to connect to us will complete the socket with
 * their IP address and port number, and the local socket will be
 * limited to that host only.
 *
 * If it is set to all "255"s, we will receive all broadcast
 * packets instead.
 */
#define REMOTE_IP			"0"
//#define REMOTE_IP			"255.255.255.255" /*broadcast*/

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"

udp_Socket sock;

/* receive one packet (heartbeat) */
int receive_packet(void)
{
	static char buf[128];

	#GLOBAL_INIT
	{
		memset(buf, 0, sizeof(buf));
	}

	/* receive the packet */
	if (-1 == udp_recv(&sock, buf, sizeof(buf))) {
		/* no packet read. return */
		return 0;
	}

	printf("Received-> %s\n",buf);
	return 1;
}

void main()
{
	sock_init();
	/*printf("Opening UDP socket\n");*/
	
	if(!udp_open(&sock, LOCAL_PORT, resolve(REMOTE_IP), 0, NULL)) {
		printf("udp_open failed!\n");
		exit(0);
	}

	/* receive heartbeats */
	for(;;) {
		tcp_tick(NULL);
		receive_packet();
	}
}
