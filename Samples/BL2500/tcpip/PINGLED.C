/*******************************************************************************
   pingled.c
   Z-World, 2002

	This program is used with BL2500 series controllers.
	
	Description
	===========
   This program demonstrates ICMP by pinging a remote host.
	It will flash LED's, DS3 and DS4 on the board when a
	ping is sent and received.

	This program was adapted from \Samples\RCM2100\pingled.c.
	
	Instructions
	============
   1. Change PING_WHO to the host you want to ping.
      
   2. You may modify the PING_DELAY define to change the
   	amount of time in milliseconds between the outgoing
   	pings.
   	
   3. Uncomment the VERBOSE define to see the incoming
      ping replies.

	4. Compile and run this program.

	5. DS3 will flash when a ping is sent.
		DS4 will flash when a ping is received.


	Quick test with two BL2500 controllers
	======================================
	1.	Connect a cross-over cable to each controller.
	2. Change #define TCPCONFIG 0.
	3. Uncomment #define MY_IP_ADDRESS and #define PING_WHO.
	4. Compile and run this program on one controller.
	5. Switch the value of MY_IP_ADDRESS and PING_WHO.
	6. Compile and run this program on the second controller.
	7. On each board:
		DS3 will flash when a ping is sent.
		DS4 will flash when a ping is received.
	
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
 *
 * It is best to explicitly turn off DHCP in your configuration.  This
 * will allow the console to completely manage DHCP, so that it does
 * not acquire the lease at startup in sock_init() when it may not need
 * to (if the user has not turned on DHCP).
 */
#define TCPCONFIG 1

/** Remote interface to send PING to (passed to resolve()): **/
/*  Undefine to retrieve default gateway and ping that. */
//#define PING_WHO			"10.10.6.2"

//#define MY_IP_ADDRESS 	"10.10.6.1"			//for two controller test, see description above.

/** How often to send PING's (in milliseconds): **/
#define PING_DELAY		500

//#define VERBOSE

#memmap xmem
#use "dcrtcp.lib"

/*******************************************************************************
		Routines to change the LEDs.  The DS3, DS4 LEDs are
		on the least significant bits of the A register.  The pingleds_setup
		routine turns off DS3 and DS4.  The pingoutled routine turns DS3 on when what
		is nonzero and off when it is zero.  Similarly, the pinginled routine
		toggles DS4.
*******************************************************************************/
#define DS3 2
#define DS4 3

#define LEDON	1
#define LEDOFF	0

int ledstatus;

void pingleds_setup()
{
	ledOut(DS3, LEDOFF);
	ledOut(DS4, LEDOFF);
}

void pingoutled(int what)
{
	ledOut(DS3, what);
}

void pinginled(int what)
{
	ledOut(DS4, what);
}


void main()
{
	longword seq,ping_who,tmp_seq,time_out;
	char	buffer[100];


	brdInit();
	seq=0;

	pingleds_setup();
	
	sock_init();
	
	/* Print who we are... */
	printf( "My IP address is %s\n\n", inet_ntoa(buffer, gethostid()) );

	/*
	 *		Get the binary ip address for the target of our 
	 *		pinging.
	 */
	
#ifdef PING_WHO	
	/* Ping a specific IP addr: */
	ping_who=resolve(PING_WHO);
	if(ping_who==0) {
		printf("ERROR: unable to resolve %s\n",PING_WHO);
		exit(2);
	}
#else
	/* Examine our configuration, and ping the default router: */
	tmp_seq = ifconfig( IF_ANY, IFG_ROUTER_DEFAULT, & ping_who, IFS_END );
	if( tmp_seq != 0 ) {
		printf( "ERROR: ifconfig() failed --> %d\n", (int) tmp_seq );
		exit(2);
	}
	if(ping_who==0) {
		printf("ERROR: unable to resolve IFG_ROUTER_DEFAULT\n");
		exit(2);
	}
#endif
	
	for(;;) {
		/*
		 *		It is important to call tcp_tick here because
		 *		ping packets will not get processed otherwise.
		 *
		 */

		tcp_tick(NULL);
		
		/*
		 *		Send one ping every PING_DELAY ms.
		 */
		
		costate {
			waitfor(DelayMs(PING_DELAY));
			_ping(ping_who,seq++);
			pingoutled(LEDON);					// flash the DS3 LED
			waitfor(DelayMs(50));
			pingoutled(LEDOFF);
		}
		
		/*
		 *		Has a ping come in?  time_out!=0xfffffff->yes.
		 */

		costate {
			time_out=_chk_ping(ping_who,&tmp_seq);
			if(time_out!=0xffffffff) {

#ifdef VERBOSE			
				printf("received ping:  %ld\n", tmp_seq);
#endif

				pinginled(LEDON);					// flash the DS4 LED
				waitfor(DelayMs(50));
				pinginled(LEDOFF);
			}
		}
	}
}