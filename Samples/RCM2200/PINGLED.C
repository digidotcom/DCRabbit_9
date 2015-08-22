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
        Samples/RCM2200/pingled.c

        ICMP demonstration, by pinging a remote host.

        This sample program is an adaptation of samples/tcpip/ping.c.  It
        has been modified for the RCM2200 to flash LEDs on the prototyping
        board.  It flashes DS2 when it sends a ping and flashes DS3 when
        it receives a ping response.  The MASTER core controls the LEDs.

        To use this program modify the MY_IP_ADDRESS, MY_NETMASK, MY_GATEWAY,
        and MY_NAMESERVER to the appropriate settings for the network.  Then
        change PING_WHO to the host you want to ping.  The MY_NAMESERVER
        define is only necessary when you are specifing PING_WHO by name
        instead of by IP address.

        You can modify the PING_DELAY define to change the amount of time
        in milliseconds between the outgoing pings.

        If you would like to see the incoming ping replies, uncomment the
        VERBOSE define.
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


/** Define causes printf() when response arrives **/
//#define VERBOSE

/** Remote interface to send PING to (passed to resolve()): **/
/*  Undefine to retrieve default gateway and ping that. */
// #define PING_WHO			"10.10.6.1"

/** How often to send PING's (in milliseconds): **/
#define PING_DELAY		2000

#memmap xmem
#use "dcrtcp.lib"


/*******************************************************************************
		Routines to change the LEDs.  The DS2 and DS3 LEDs are on two bits of
		the E register, PE1 and PE7.  The pingleds_setup() routine turns off
		DS2 and DS3.  The pingoutled() routine turns DS2 on when what is
		nonzero and off when it is zero.  Similarly, the pinginled() routine
		toggles DS3.
*******************************************************************************/

#define LEDON	1
#define LEDOFF	0


void pingoutled(int what)
{
	/*  Control DS2.  Output 0 to turn LED on. */
	WrPortI(PEB1R, NULL, (what != LEDOFF) ? 0 : (1<<1));
}

void pinginled(int what)
{
	/*  Control DS3.  Output 0 to turn LED on. */
	WrPortI(PEB7R, NULL, (what != LEDOFF) ? 0 : (1<<7));
}

void pingleds_setup()
{
	/*  Convert the I/O ports.  Disable slave port which make
	 *  Port A an output, and PORT E not have SCS signal.
	 *  Read shadow and set PE1 and PE7 as normal I/O.
	 *  LED's are connted to PE1 and PE7, make them outputs.
	 */
	WrPortI(SPCR, & SPCRShadow, 0x84);
	WrPortI(PEFR,  & PEFRShadow,  ~((1<<7)|(1<<1)) & PEFRShadow);
	WrPortI(PEDDR, & PEDDRShadow, (1<<7)|(1<<1));

	pingoutled(LEDOFF);
	pinginled(LEDOFF);
}


int main()
{
	longword seq,ping_who,tmp_seq,time_out;

	pingleds_setup();
	
	sock_init();
	
	/*
	 *		Get the binary ip address for the target of our 
	 *		pinging.
	 */
#ifdef PING_WHO	
	/* Ping a specific IP addr: */
	ping_who=resolve(PING_WHO);
	if(ping_who==0) {
		printf("ERROR: unable to resolve %s\n",PING_WHO);
		return 1;
	}
#else
	/* Examine our configuration, and ping the default router: */
	tmp_seq = ifconfig( IF_ANY, IFG_ROUTER_DEFAULT, & ping_who, IFS_END );
	if( tmp_seq != 0 ) {
		printf( "ERROR: ifconfig() failed --> %d\n", (int) tmp_seq );
		return 1;
	}
	if(ping_who==0) {
		printf("ERROR: unable to resolve IFG_ROUTER_DEFAULT\n");
		return 1;
	}
#endif
	
	seq=0;
	for(;;) {
		/*
		 *		It is important to call tcp_tick() here because
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
	}   /* end for-ever... */
}
