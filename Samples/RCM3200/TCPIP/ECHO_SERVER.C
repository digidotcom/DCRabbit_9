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
/**********************************************************
	echo_server.c

	This program is used with RCM3200 series controllers.
	
	The sample library, rcm3200.lib, contains a brdInit()
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
   1. Modify the TCPCONFIG, INCOMING_IP, INCOMING_PORT,
      and MY_PORT to the appropriate settings for the
      network.
	2. Compile and run this program.
	
**********************************************************/
#class auto

#use rcm3200.lib    		//sample library used for this demo

//#define DCRTCP_DEBUG		// display tcp/ip debugging information
//#define DCRTCP_VERBOSE	// allow stepping through tcp/ip functions

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

#define MY_PORT 			1111

//************************************************************************************************
#define	ETH_MTU		 1000
#define  BUFF_SIZE	 (ETH_MTU-40)	//must be smaller than (ETH_MTU - (IP Datagram + TCP Segment))
#define  TCP_BUF_SIZE ((ETH_MTU-40)*4)	// sets up (ETH_MTU-40)*2 bytes for Tx & Rx buffers

//************************************************************************************************

#memmap xmem
#use "dcrtcp.lib"

#define INCOMING_IP		0		// accept all connections
#define INCOMING_PORT	0		// accept all ports
#define TIME_OUT			5000	// if in any state for more than X milliseconds re-initialize

#define	RED		"\x1b[31m"	// foreground colors for printf
#define	GREEN		"\x1b[32m"	// foreground colors for printf
#define	BLUE		"\x1b[34m"	// foreground colors for printf

tcp_Socket sock;
int bytes;
long statetime;
static char buff[BUFF_SIZE];

///////////////////////////////////////////////////////////////////////

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
			statetime = MS_TIMER+TIME_OUT;	// reset time to be in the SEND state
			return 3;	//now go to state 3 (SEND)
	}
}

int send_packet(void)
{
	/* send the packet */
	memset(buff,'S',BUFF_SIZE);	// fill buffer with S's
	buff[BUFF_SIZE-1] = '\n';		// null terminate the buffer
	
	bytes = sock_fastwrite(&sock,buff,BUFF_SIZE);
	switch(bytes)
	{
		case -1:
			return 3; // there was an error stay is SEND state and retry
		default:
			statetime = MS_TIMER+TIME_OUT;	// reset time to be in the RECEIVE state
			return 2;	//now go to state 2 (RECEIVE)
	}
}

void main()
{
	int state,status;
	state = 0;
	
	sock_init();
	
	while(1)
	{
		switch(state)
		{
			case 0:/*INITIALIZATION*/					// listen for incoming connection
				if(!tcp_listen(&sock,MY_PORT,INCOMING_IP,INCOMING_PORT,NULL,0))
					printf("%sERROR OPENNING SOCKET!\n",RED);
				else
				{
					printf("%swaiting for incomming session\n",GREEN);
					statetime = MS_TIMER+TIME_OUT;		// setup the time to be in LISTEN state
					state++;										// init complete move onto next state
				}
				break;
			case 1:/*LISTEN*/
				if(sock_established(&sock))			// check for a connection
				{
					printf("%ssocket established\n",BLUE);
					statetime = MS_TIMER+TIME_OUT;	// reset time to be in the RECEIVE state
					state++;									//   we have connection so move on
				}
				else if ((long)(MS_TIMER-statetime) > 0)	// if X millisecs and still waiting
					state = 4;										//	  abort and re-init
				break;
			case 2:/*RECEIVE*/
				state = receive_packet();				// see function for details
				if ((long)(MS_TIMER-statetime) > 0)	// if X millisecs and still waiting
					state = 4;								//	  abort and re-init
				break;
			case 3:/*SEND*/
				state = send_packet();					// see function for details
				if ((long)(MS_TIMER-statetime) > 0) // if X millisecs and still waiting
					state = 4;								//	  abort and re-init
				break;
			case 4:/*NO WAIT_CLOSE*/
				printf("%sABORT SOCKET\n",RED);
				sock_abort(&sock);						// close the socket
				state = 0;									// go back to the INIT state
		}
		status = tcp_tick(&sock);
	}	
}

