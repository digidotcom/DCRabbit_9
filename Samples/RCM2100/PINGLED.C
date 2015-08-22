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
        pingled.c

        ICMP demonstration, by pinging a remote host.

        This sample program is an adaptation of samples/tcpip/ping.c.  It
        has been modified for the RCM2100 to flash LEDs on the prototyping
        board.  It flashes DS2 when it sends a ping and flashes DS3 when
        it receives a ping response.

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


/** Remote interface to send PING to (passed to resolve()): **/
/*  Undefine to retrieve default gateway and ping that. */
// #define PING_WHO			"10.10.6.1"

/** How often to send PING's (in milliseconds): **/
#define PING_DELAY		500

#memmap xmem
#use "dcrtcp.lib"

/*******************************************************************************
		Routines to change the LEDs.  The DS2, DS3, DS4, and DS5 LEDs are
		on the least significant bits of the A register.  The pingleds_setup
		routine turns off DS2-5.  The pingoutled routine turns DS3 on when what
		is nonzero and off when it is zero.  Similarly, the pinginled routine
		toggles DS4.
*******************************************************************************/

#define LEDON	1
#define LEDOFF	0

int ledstatus;

void pingleds_setup()
{

	WrPortI(SPCR, NULL, 0x84);  // Set Port A to Output

	ledstatus=0x0f;
	WrPortI(PADR,NULL,ledstatus);

}

void pingoutled(int what)
{
	if(what==LEDON)
		ledstatus&=0xfe;
	else
		ledstatus|=0x01;

	WrPortI(PADR,NULL,ledstatus);
}

void pinginled(int what)
{
	if(what==LEDON)
		ledstatus&=0xfd;
	else
		ledstatus|=0x02;

	WrPortI(PADR,NULL,ledstatus);
}


void main()
{
	longword seq,ping_who,tmp_seq,time_out;

	seq=0;

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