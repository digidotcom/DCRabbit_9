/**********************************************************
	echo_client.c
 	Z-World, 2002

	This program is used with RCM3000 series controllers.
	
	The sample library, rcm3000.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
   This program demonstrates a basic client which will
	send a packet and wait for the server connect to echo
	it back.  After every 100 sends and receives, a print
	out of the transfer times is shown in the stdio window.
        
   Use echo_server.c to program a server controller.

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

#define MY_PORT			2200

//************************************************************************************************
#define	ETH_MTU		 1500
#define  BUFF_SIZE	 (ETH_MTU-40)	//must be smaller than (ETH_MTU - (IP Datagram + TCP Segment))
#define  TCP_BUF_SIZE ((ETH_MTU-40)*4)	// sets up (ETH_MTU-40)*2 bytes for Tx & Rx buffers

//************************************************************************************************
#memmap xmem
#use "dcrtcp.lib"

#define REMOTE_IP			"10.10.6.124"
#define REMOTE_PORT 		2200
#define NUM_OF_SENDS		200
#define TIME_OUT			1000	// if in any state for more than a second re-initialize


tcp_Socket sock;
int bytes,i;
static char buff[BUFF_SIZE];

int receive_packet(void)
{
	/* receive the packet */
	bytes = sock_fastread(&sock,buff,BUFF_SIZE);
	switch(bytes)
	{
		case -1:
			return 4; // there was an error go to state 4 (WAIT_CLOSE)
		case  0:
			return 3; // connection is okay, but no data received
		default:
			i--;
			return 2;	//now go to state 2 (SEND)
	}
}

int send_packet(void)
{
	/* send a packet */
	memset(buff,'C',BUFF_SIZE);
	buff[BUFF_SIZE-1] = '\n';
	
	if(i<=0)
		return 3;
	
	bytes = sock_write(&sock,buff,BUFF_SIZE);
	switch(bytes)
	{
		case -1:
			return 4; // there was an error go to state 4 (WAIT_CLOSE)
		default:
			return 3;	//now go to state 3 (RECEIVE)
	}
}

void main()
{
	int state,status;
	long statetime;
	float endtime,TimePerTrans,TimePerByte,TimePerBit,BitsPerTime;

	brdInit();				//initialize board for this demo

	state = 0;

	#asm					// this delay is just for debouncing the plugging in of the power supply.
	ld		hl,1000
	call	tickwait
	#endasm
	
	sock_init();

//	#asm					// with these four lines commented out the client cannot connect to the server
//	ld		hl,350		//   when using the RCM2200 as the Client.  Other Rabbit boards work with or 
//	call	tickwait		//   without these four lines.
//	#endasm
	
	while(1)
	{
		switch(state)
		{
			case 0:/*CREATE SESSION*/
				if(tcp_open(&sock,MY_PORT,resolve(REMOTE_IP),REMOTE_PORT,NULL))
					state++;									// complete move onto next state
				MS_TIMER = 0L;
				i = NUM_OF_SENDS;
				break;
			case 1:/*LISTEN*/
				if(sock_established(&sock))			// check for a connection
				{
					statetime = MS_TIMER = 0L;
					state++;									//   we have connection so move on
				}
				else if (MS_TIMER > TIME_OUT)			// if 1 sec and no sock
					state = 4;								//	  abort and re-init
				break;
			case 2:/*SEND*/
				statetime = MS_TIMER;					// reset time to be in the RECEIVE state
				state = send_packet();					// see function for details
				break;
			case 3:/*RECEIVE*/
				state = receive_packet();				// see function for details
				if (MS_TIMER > (statetime+TIME_OUT))// if 1 sec and still waiting
					state = 4;								//	  abort and re-init
				break;
			case 4:/*NO_WAIT_CLOSE*/
				sock_abort(&sock);						// close the socket
				state = 0;									// go back to the INIT state
		}
		status = tcp_tick(&sock);
		if(i==0)
		{
			endtime = (float)MS_TIMER/(float)1000;
			TimePerTrans = endtime/(float)(NUM_OF_SENDS*2);
			TimePerByte	 = TimePerTrans/(float)BUFF_SIZE;
			TimePerBit	 = TimePerByte/(float)8;
			BitsPerTime	 = (float)1/TimePerBit;
			
			printf("\n\nTotal Transfer Time:	%+fsec\n",endtime);
			printf("Time per Trans:	%+fsec/trans\n",TimePerTrans);
			printf("Time per Byte:	%+fsec/byte\n", TimePerByte);
			printf("Time per Bit:	%+fsec/bit\n",  TimePerBit);
			printf("Bits per Time:	%+fbits/sec\n", BitsPerTime);
			i--;
			state = 4;
		}
	}	
}
