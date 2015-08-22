/**********************************************************
	echo_server.c
 	Z-World, 2002

	This program is used with RCM3000 series controllers.
	
	The sample library, rcm3000.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
   This program demonstrates a basic server which will
   echo back any data sent from a client connect.

   Use echo_client.c to program a client controller.

   This program was adapted from \Samples\TCPIP\echo.c.
   
	Instructions
	============
   1. Modify the MY_IP_ADDRESS, MY_NETMASK, MY_GATEWAY,
      and MY_PORT to the appropriate settings for the
      network.
	2. Compile and run this program.
	
**********************************************************/
#class auto

#use rcm3000.lib    		//sample library used for this demo

#define ARP_DEBUG
#define ARP_VERBOSE

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

#define MY_PORT 			2200


//************************************************************************************************
#define	ETH_MTU		 1500
#define  BUFF_SIZE	 (ETH_MTU-40)	//must be smaller than (ETH_MTU - (IP Datagram + TCP Segment))
#define  TCP_BUF_SIZE ((ETH_MTU-40)*4)	// sets up (ETH_MTU-40)*2 bytes for Tx & Rx buffers

//************************************************************************************************

#memmap xmem
#use "dcrtcp.lib"

#define INCOMING_IP		0 		//accept all connections
#define INCOMING_PORT	0		//accept all ports
#define TIME_OUT			1000	// if in any state for more than a second re-initialize


tcp_Socket sock;
int bytes;
static char buff[BUFF_SIZE];

int receive_packet(void)
{
	/* receive the packet */
	bytes = sock_fastread(&sock,buff,BUFF_SIZE);
	
	switch(bytes)
	{
		case -1:
			return 4; // there was an error go to state 4 (NO_WAIT_CLOSE)
		case  0:
			return 2; // connection is okay, but no data received
		default:
			return 3;	//now go to state 3 (SEND)
	}
}

int send_packet(void)
{
	/* send the packet */
	bytes = sock_write(&sock,buff,bytes);
	switch(bytes)
	{
		case -1:
			return 4; // there was an error go to state 4 (NO_WAIT_CLOSE)
		default:
			return 2;	//now go to state 2 (RECEIVE)
	}
}

void main()
{
	int state;
	
	brdInit();				//initialize board for this demo
	
	state = 0;
	sock_init();
	
	while(1)
	{
		switch(state)
		{
			case 0:/*INITIALIZATION*/					// listen for incoming connection
				tcp_listen(&sock,MY_PORT,INCOMING_IP,INCOMING_PORT,NULL,0);
				MS_TIMER = 0L;
				state++;										// init complete move onto next state
				break;
			case 1:/*LISTEN*/
				if(sock_established(&sock))			// check for a connection
					state++;									//   we have connection so move on
				else if (MS_TIMER > TIME_OUT)			// if 1 sec and no sock
					state = 4;								//	  abort and re-init
				break;
			case 2:/*RECEIVE*/
				state = receive_packet();				// see function for details
				if (MS_TIMER > TIME_OUT)				// if 1 sec and still waiting
					state = 4;								//	  abort and re-init
				break;
			case 3:/*SEND*/
				MS_TIMER = 0L;								// reset the timer
				state = send_packet();					// see function for details
				break;
			case 4:/*NO WAIT_CLOSE*/
				sock_abort(&sock);						// close the socket
				state = 0;									// go back to the INIT state
		}
		tcp_tick(&sock);
	}	
}

