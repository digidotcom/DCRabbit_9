/*******************************************************************************
        Samples\tcpip\udp\udp_cli.c
        Z-World, 2000

        A UDP example, that sends out 'heartbeat' packets to either a specific
        receiver, or broadcasted over the local subnet. These packets can be
        received by ucp_srv.c

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
 * configuration.  If value >= 100, then use "custom_config.h"
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

/* How often (in seconds) to send out UDP packet. */
#define  HEARTBEAT_RATE 	4
 
/* what local UDP port to use - packets we send come from here */
#define LOCAL_PORT	1234

/*
 * Where to send the heartbeats. If it is set to all '255's,
 * it will be a broadcast packet.
 */
#define REMOTE_IP			"10.10.6.177"
//#define REMOTE_IP			"255.255.255.255" /*broadcast*/

/* the destination port to send to */
#define REMOTE_PORT	1234

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"

udp_Socket sock;

/* send one packet (heartbeat) */
int send_packet(void)
{
	static long sequence;
	auto char 	buf[128];
	auto int 	length, retval;

	#GLOBAL_INIT
	{
		sequence = 0;
	}

	/* fill the packet with interesting data (a sequence number) */
	sequence++;
	sprintf(buf, "SEQ=%ld",sequence);
	length = strlen(buf) + 1;

	/* send the packet */
	retval = udp_send(&sock, buf, length);
	if (retval < 0) {
		printf("Error sending datagram!  Closing and reopening socket...\n");
		if (sequence == 1) {
			printf("   (initial ARP request may not have finished)\n");
		}
		sock_close(&sock);
		if(!udp_open(&sock, LOCAL_PORT, resolve(REMOTE_IP), REMOTE_PORT, NULL)) {
			printf("udp_open failed!\n");
			exit(0);
		}
	}

	tcp_tick(NULL);	
	return 1;
}

void main()
{
	sock_init();
	/*printf("Opening UDP socket\n");*/
	
	if(!udp_open(&sock, LOCAL_PORT, resolve(REMOTE_IP), REMOTE_PORT, NULL)) {
		printf("udp_open failed!\n");
		exit(0);
	}

	/* send heartbeats */
	for(;;) {
		//putchar('.');
		tcp_tick(NULL);
		costate {
			waitfor(IntervalSec(HEARTBEAT_RATE));
			waitfor(send_packet());
		}
	}
}

