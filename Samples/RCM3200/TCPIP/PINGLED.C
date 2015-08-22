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
   pingled.c

	This program is used with RCM3200 series controllers
	with prototyping boards.
	
	The sample library, rcm3200.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
   This program demonstrates ICMP by pinging a remote host.
	It will flash LED's, DS1 and DS2 on the prototyping
	board when a ping is sent and received.
	
	This program was adapted from \Samples\TCPIP\ping.c.
	
	Instructions
	============
   1. Modify the MY_IP_ADDRESS, MY_NETMASK, MY_GATEWAY,
      and MY_NAMESERVER to the appropriate settings for
      the network.
      
   2. Change PING_WHO to the host you want to ping. The
      MY_NAMESERVER define is only necessary when you are
      specifing PING_WHO by name instead of by IP address.
      
   3. You may modify the PING_DELAY define to change the
   	amount of time in milliseconds between the outgoing
   	pings.
   	
   4. Uncomment the VERBOSE define to see the incoming
      ping replies.

	5. Compile and run this program.

	6. DS1 will flash when a ping is sent.
		DS2 will flash when a ping is received.
	
**********************************************************/
#class auto

#use rcm3200.lib    		//sample library used for this demo

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

//#define VERBOSE

#define PING_WHO			"10.10.6.1"
#define PING_DELAY		500

#memmap xmem
#use "dcrtcp.lib"

/**********************************************************
	Routines to change the LEDs.
	The port G bit 6 controls DS1 and port G bit 7 controls
	DS2.	The pingleds_setup routine turns off LED's.  The
	pingoutled routine toggles DS1. The pinginled routine
	toggles DS2.
***********************************************************/

#define LEDON	1
#define LEDOFF	0

int ledstatus;

pingleds_setup()
{
	ledstatus=PGDRShadow|0xC0;						//leds initially off
	WrPortI(PGDR, &PGDRShadow, ledstatus);
}

pingoutled(int what)
{
	if (what==LEDON)
		ledstatus&=0xBF;				//turn DS1 on
	else
		ledstatus|=0x40;				//turn DS1 off

	WrPortI(PGDR, NULL, ledstatus);
}

pinginled(int what)
{
	if (what==LEDON)
		ledstatus&=0x7F;				//turn DS2 on
	else
		ledstatus|=0x80;				//turn DS2 off

	WrPortI(PGDR, NULL, ledstatus);
}


main()
{
	longword seq,ping_who,tmp_seq,time_out;

	brdInit();				//initialize board for this demo
	
	seq=0;

	pingleds_setup();
	
	sock_init();
	
	/*
	 *		Get the binary ip address for the target of our 
	 *		pinging.
	 */
	
	ping_who=resolve(PING_WHO);
	if(ping_who==0) {
		printf("unable to resolve %s",PING_WHO);
		return 1;
	}
	
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
			pingoutled(LEDON);					// flash the DS1 LED
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

				pinginled(LEDON);					// flash the DS2 LED
				waitfor(DelayMs(50));
				pinginled(LEDOFF);
			}
		}
	}
}