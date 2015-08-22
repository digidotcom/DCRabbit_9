/*******************************************************************************
   pingled.c
   Rabbit, 2007

   This sample program is used with RCM3900 series controllers and RCM3300
   prototyping boards.

   Description
   ===========
   This sample program demonstrates ICMP by pinging a remote host.  On the
   prototyping board, LEDs DS3 and DS4 are flashed when a ping is sent and
   received, respectively.

   This sample program is adapted from ...\Samples\tcpip\ping.c.

   Instructions
   ============
   1. If necessary, make changes below in the configuration section to match
      your requirements.  In particular, you may wish to uncomment and edit the
      PING_WHO macro definition to suit the host you want to ping.  If the
      PING_WHO macro remains commented out then the default gateway is the ping
      target.

   2. If desired, modify the PING_DELAY macro definition to change the amount of
      time, in milliseconds, between the outgoing pings.

   3. Uncomment the VERBOSE define to see received ping information displayed in
      the STDIO window.

   4. Compile and run this program in debug mode.

   5. Our IP address and any errors that are encountered are reported to the
      STDIO window.  If any error is encountered this sample program exits
      (returns control to Dynamic C if running in debug mode, else restarts if
      running in stand-alone mode).

   6. In normal operation, on the prototyping board, DS3 will flash when a ping
      is transmitted and DS4 will flash when a ping is received.

   Quick test with two RCM3900 controllers and RCM3300 prototyping boards:
   a.	Connect a cross-over cable between the two controllers.
   b. Change #define TCPCONFIG 0.
   c. Uncomment both #define MY_IP_ADDRESS and #define PING_WHO.
   d. Compile and run this program on one controller.
   e. Swap the values of MY_IP_ADDRESS and PING_WHO.
   f. Compile and run this program on the second controller.
   g. On each prototyping board, DS3 will flash when a ping is sent and DS4 will
      flash when a ping is received.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

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
/*  Leave commented out to retrieve default gateway and ping that. */
//#define PING_WHO			"10.10.6.1"

//for two controller test, see description above.
//#define MY_IP_ADDRESS 	"10.10.6.2"

/** How often to send PING's (in milliseconds): **/
#define PING_DELAY		500

/** Uncomment to display ping received messages to STDIO: **/
//#define VERBOSE

#if !TCPCONFIG
	#define MY_NETMASK "255.255.255.0"
	#define USE_ETHERNET 1
	#define IFCONFIG_ETH0 \
	        IFS_IPADDR,aton(MY_IP_ADDRESS), \
	        IFS_NETMASK,aton(MY_NETMASK), \
	        IFS_UP
#endif	// !TCPCONFIG

#memmap xmem
#use "dcrtcp.lib"

/**********************************************************
	Routines to change the LEDs.
	The pingleds_setup routine turns off LED's.  The
	pingoutled routine toggles DS3. The pinginled routine
	toggles DS4.
***********************************************************/

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6
#define USERLED 0

#define LEDON	1
#define LEDOFF	0

int ledstatus;

void pingoutled(int onoff)
{
	ledOut(DS3, onoff);
}

void pinginled(int onoff)
{
	ledOut(DS4, onoff);
}

void main(void)
{
	// auto variables in main() permanently consume precious stack space
	static longword seq, ping_who, tmp_seq, time_out;
	static char buffer[100];

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	seq=0;

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
			waitfor(IntervalMs(PING_DELAY));
			_ping(ping_who,seq++);
			pingoutled(LEDON);					// flash transmit LED
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

				pinginled(LEDON);					// flash receive LED
				waitfor(DelayMs(50));
				pinginled(LEDOFF);
			}
		}
	}
}

