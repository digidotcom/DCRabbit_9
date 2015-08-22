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
        Samples\low power\LP_PING.c

        This program demonstrates conditional usage of TCP/IP
        networking in Low Power modes. It responds to TCP/IP ping
        packets and also demonstrates pinging a remote host. It
        prints a message when the ping response arrives here.
        If PING_WHO is not defined, then it pings the default
        gateway.

        When it is run, this program will start off in Full power mode
        until Switch S2 (or "S1" on RCM36/37xx) on the prototyping board
        is pressed by the user.

        Each time the switch is pressed, the CPU changes to a lower power
        consumption mode until networking is disabled. After that, power
        consumption will progressively get reduced with each Switch press
        until a minimum CPU speed of ~2kHz is reached.  The next Switch press
        will return the board to full power consumption mode with networking
        re-enabled.

        Here is the sequence of power modes for each time the user presses
        the swtich:


      HIGH POWER CONSUMPTION(FAST)                    LOW POWER CONSUMPTION(SLOW)
Mode:     1      2     3     4     5     6     7     8     9     10
        Ping responses get slower---->   |
          					           |     |
                             Network Disabled/Debugging no longer possible by mode 5.

       For a detailed description of the above power modes, see function
       description for set_cpu_power_mode() in LOW_POWER.LIB.

     Note: The LED light is programmed to blink at differnt rates for
       each lower power mode, to indicate that the board is still running.
       This program will not run in debugging mode, with program cable, when
       switching to low power mode..
*******************************************************************************/
#class auto

/* Define HIT_WATCHDOG if watchdog is enabled (default) while using
*  power modes 6 through 10 (ie when periodic interrupt is disabled).
*/
#define HIT_WATCHDOG

// Debugging option
#define LOWPOWER_VERBOSE

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

/** Remote interface to send PING to (passed to resolve()): **/
/*  Undefine to retrieve default gateway and ping that. */
// #define PING_WHO			"10.10.6.1"


/********************************
 * End of configuration section *
 ********************************/

/* The following defines the macro SWITCH_PRESSED(), based on which Rabbit prototyping
 * board is being used. SWITCH_PRESSED() returns 1 if the switch "S2" (or "S1" on the RCM36/37xx) is pressed
 * on the prototyping board and 0 otherwise.
 */
#if ((_BOARD_TYPE_ & 0xFF00) == (RCM3700A & 0xFF00))
   // We use Switch 1 (S1), if using the RCM36/37XX Prototyping Board.
   #define  SWITCH_PRESSED()   !BitRdPortI(PFDR, 4)
   #define  SET_LED(on)		    BitWrPortI(PFDR, &PFDRShadow, !on, 6)
#elif (_BOARD_TYPE_ & 0xFF00) == (RCM3000 & 0xFF00) || \
      (_BOARD_TYPE_ & 0xFF00) == (RCM3200 & 0xFF00) || \
      _BOARD_TYPE_ == RCM3209 || _BOARD_TYPE_ == RCM3229
 #if _BOARD_TYPE_ == RCM3220A || _BOARD_TYPE_ == RCM3229
   #fatal "This board does not have Ethernet capability."
 #endif
   // We use Switch 2 (S2), if using the RCM30/31/32xx Prototyping Board.
   #define  SWITCH_PRESSED()   !BitRdPortI(PGDR, 1)
   #define  SET_LED(on)		    BitWrPortI(PGDR, &PGDRShadow, !on, 6)
#elif (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || \
      _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
   #use "rcm33xx.lib"
   // We use Switch 2 (S2), if using the RCM3300 Prototyping Board.
   #define  SWITCH_PRESSED()   !switchIn(2)
   #define  SET_LED(on)        ledOut(3, on)
#elif (_BOARD_TYPE_ == RCM3400A || _BOARD_TYPE_ == RCM3410A)
   // We use Switch 2 (S2), if using the RCM3400 Prototyping Board.
   #define  SWITCH_PRESSED()  !BitRdPortI(PDDR, 5)
   #define  SET_LED(on)       BitWrPortI(PDDR, &PDDRShadow, !on, 6)
#elif (_BOARD_TYPE_ >= RCM3800 && _BOARD_TYPE_ <= RCM3800+0x00FF)
   #use "powercoreflex.lib"
   // We use Switch 2 (S2), if using the RCM3800 Prototyping Board.
   #define  SWITCH_PRESSED()   !digIn(0)
   #define  SET_LED(on)        ledOut(1, on)
#elif _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910
   #use "rcm39xx.lib"
   // We use Switch 2 (S2), if using the RCM3300 Prototyping Board.
   #define  SWITCH_PRESSED()   !switchIn(2)
   #define  SET_LED(on)        ledOut(3, on)
#else
   #error "This Board type does not have a known switch."
#endif

#memmap xmem
#use "dcrtcp.lib"
#use "low_power.lib"

int main()
{
	longword seq,ping_who,tmp_seq,time_out;
	char	buffer[100];
	auto int powerMode;
   auto unsigned long delayStart;
   auto char ledState;

   brdInit();

   ledState = 0; //used to blink LED light on and off
   powerMode = 1;

	sock_init();
	// Wait for the interface to come up
	while (ifpending(IF_DEFAULT) == IF_COMING_UP) {
		tcp_tick(NULL);
	}

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
		printf("ERROR: unable to resolve IF_ROUTER_DEFAULT\n");
		return 1;
	}
#endif

	seq=0;
	for(;;) {

		costate
		{
			if (!SWITCH_PRESSED())		//wait for switch S2 press (S1 on RCM36/37xx boards)
				abort;

         while(SWITCH_PRESSED())  //wait for release
         	yield;

			powerMode++;						//set next power mode
         if(powerMode > NUM_POWER_MODES)
         {
         	powerMode = 1;
            set_cpu_power_mode(powerMode, CLKDOUBLER_ON, SHORTCS_OFF);

				/* CPU speed is adequate for ethernet,
             * power the Ethernet interface up... */
            pd_powerup(IF_ETH0);

            //wait 1 second for ethernet to go back up
            delayStart = MS_TIMER;
            while(MS_TIMER - delayStart < 1000L);

            //Re-enable TCP/IP interface.
            ifconfig(IF_ETH0, IFS_UP, IFS_END);
				// Wait for the interface to come back up
  				while (ifpending(IF_DEFAULT) == IF_COMING_UP) {
					tcp_tick(NULL);
  				}
            abort;
         }
			else if(powerMode == 6) //turn off networking at 5 and over
         {
         	/* CPU speed is inadequate for ethernet...
             * So, disable TCP/IP... */
         	ifconfig(IF_ETH0, IFS_DOWN, IFS_END);
            //...and power down Ethernet interface.
            pd_powerdown(IF_ETH0);
         }

         set_cpu_power_mode(powerMode, CLKDOUBLER_ON, SHORTCS_OFF);

		} //end costate

      /* Only handle tcp events & pings if we are in a
       * power mode that allows for network activity. */
		if(powerMode < 6)
      {
	      /*
	       *    It is important to call tcp_tick here because
	       *    ping packets will not get processed otherwise.
	       *
	       */
	      tcp_tick(NULL);

	      /*
	       *    Send one ping per second.
	       */

	      costate {
	         waitfor(DelaySec(1));
	         _ping(ping_who,seq++);
	      }

	      /*
	       *    Has a ping come in?  time_out!=0xfffffff->yes.
	       */

	      time_out=_chk_ping(ping_who,&tmp_seq);
	      if(time_out!=0xffffffff)
	         printf("\nreceived ping:  %ld", tmp_seq);
      }


      /* Blink the LED light according to which power mode
       * we are in. Notice for power modes 1-5 we are in the
       * MHZ range, so a delay is introduced. */
      if(powerMode <= 5)  //Power uses main osc. , so add a delay
      {
	      costate {
         	waitfor(DelayMs(10L + powerMode));
	         ledState = !ledState;
	         SET_LED(ledState);
	      }
      }
      else //CPU is running in 2-32kHZ range here
      {
	   	ledState = !ledState;
	   	SET_LED(ledState);
      }

   /* For cases where the watchdog timer is enabled, and
    * we are operating in a power mode where periodic interrupts
    * are disabled, we also need to hit the watchdog periodically */
#ifdef HIT_WATCHDOG
  		if(powerMode > 5)
   	{
          //hit watchdog
  			hitwd();
         /* If we also use timers, use this opportunity to update
          * timers, such as MS_TIMER, etc.  Uncomment if using timers
          * during low power mode (modes 6-10)  */
        	//updateTimers();
		}
#endif

	} //end for(;;) loop
}