/**********************************************************
	echo_client.c
 	Z-World, 2002

	This program is used with RCM3200 series controllers.
	
	The sample library, rcm3200.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
   This program demonstrates a basic client which will
	send a packet and wait for the server connect to echo
	it back.  After every number of sends and receives,
	transfer times are shown in the stdio window.
        
   Use echo_server.c to program a server controller.

   This program was adapted from \Samples\TCPIP\echo.c.
   
	Instructions
	============
   1. Modify the TCPCONFIG, REMOTE_IP, REMOTE_PORT,
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

#define MY_PORT			2222

//************************************************************************************************
#define	ETH_MTU		 1000
#define  BUFF_SIZE	 (ETH_MTU-40)		//must be smaller than (ETH_MTU - (IP Datagram + TCP Segment))
#define  TCP_BUF_SIZE ((ETH_MTU-40)*4)	// sets up (ETH_MTU-40)*2 bytes for Tx & Rx buffers

//************************************************************************************************
#memmap xmem
#use "dcrtcp.lib"

#define REMOTE_IP			"10.10.6.1"
#define REMOTE_PORT 		1111
#define NUM_OF_SENDS		1000
#define TIME_OUT			5000	// if in any state for more than a X milliseconds re-initialize

#define	RED		"\x1b[31m"	// foreground colors for printf
#define	GREEN		"\x1b[32m"	// foreground colors for printf
#define	BLUE		"\x1b[34m"	// foreground colors for printf

tcp_Socket sock;
int bytes,i,j;
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
			return 4; // there was an error go to state 4 (WAIT_CLOSE)
		case  0:
			return 3; // connection is okay, but no data received, stay in RECEIVE state
		default:
			i--;
			statetime = MS_TIMER+TIME_OUT;	// reset time to be in the SEND state
			return 2;	//now go to state 2 (SEND)
	}
}

int send_packet(void)
{
	/* send a packet */
	memset(buff,'C',BUFF_SIZE);	// fill buffer with C's
	buff[BUFF_SIZE-1] = '\n';		// null terminate the buffer
	
	if(i<=0)
		return 3;
	
	bytes = sock_fastwrite(&sock,buff,BUFF_SIZE);
	switch(bytes)
	{
		case -1:
			return 2; // there was an error stay in SEND state and try again
		default:
			printf("%s%06d\r",BLUE,j++);		// number of packets send
			statetime = MS_TIMER+TIME_OUT;	// reset time to be in the RECEIVE state
			return 3;	//now go to state 3 (RECEIVE)
	}
}

void main()
{
	int state,status;
	long start;
	float endtime,TimePerTrans,TimePerByte,TimePerBit,BitsPerTime;
	state = 0;

	sock_init();
	
	while(1)
	{
		switch(state)
		{
			case 0:/*CREATE SESSION*/
				if(!tcp_open(&sock,MY_PORT,resolve(REMOTE_IP),REMOTE_PORT,NULL))
					printf("%sERROR OPENNING SOCKET!\n",RED);
				else
				{
					printf("%ssession created\n",GREEN);
					printf("%swaiting for socket to be established\n",GREEN);
					statetime = MS_TIMER+TIME_OUT;	// setup the time to be LISTEN state
					state++;									// complete move onto next state
					i = NUM_OF_SENDS;
					j=0;										// contains the number of packets sent
				}
				break;
			case 1:/*LISTEN*/
				if(sock_established(&sock))			// check for a connection
				{
					printf("%ssocket established\n",GREEN);
					start = MS_TIMER;						// time socket was established
					statetime = MS_TIMER+TIME_OUT;	// reset time to be in the SEND state
					state++;									//  we have connection so move on
				}
				else if ((long)(MS_TIMER-statetime) > 0)	// if time out and no sock
					state = 4;										//	  abort and re-init
				break;
			case 2:/*SEND*/
				state = send_packet();					// see function for details
				if ((long)(MS_TIMER-statetime) > 0) // if X millisecs and still waiting
					state = 4;								//	  abort and re-init
				break;
			case 3:/*RECEIVE*/
				state = receive_packet();				// see function for details
				if ((long)(MS_TIMER-statetime) > 0) // if X millisecs and still waiting
					state = 4;								//	  abort and re-init
				break;
			case 4:/*NO_WAIT_CLOSE*/
				printf("%sABORT SOCKET\n",RED);
				sock_abort(&sock);						// close the socket
				state = 0;									// go back to the INIT state
				printf("\n");
		}
		status = tcp_tick(&sock);
		
		if(i==0)
		{
			endtime = (float)(MS_TIMER-start)/(float)1000;
			TimePerTrans = endtime/(float)(NUM_OF_SENDS*2);
			TimePerByte	 = TimePerTrans/(float)BUFF_SIZE;
			TimePerBit	 = TimePerByte/(float)8;
			BitsPerTime	 = (float)1/TimePerBit;
			
			printf("%s\nTotal Transfer Time:	%+fsec\n",GREEN,endtime);
			printf("Time per Trans:	%+fsec/trans\n",TimePerTrans);
			printf("Time per Byte:	%+fsec/Byte\n", TimePerByte);
			printf("Time per Bit:	%+fsec/bit\n",  TimePerBit);
			printf("Bits per Time:	%+fbits/sec\n", BitsPerTime);
			i--;
			state = 4;
		}
	}	
}
