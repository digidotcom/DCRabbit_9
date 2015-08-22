/*******************************************************************************
        LP_SMTP.c
        Rabbit, 2007

        This program demonstrates Low Power mode while
        using TCP/IP networking to send e-mails upon request.

        When it is run, it will start off in minimum power (Mode 1)
        until Switch S2 (or "S1" on the RCM36/37xx) is pressed by the user.

        When the switch is pressed, the board switches to full power
        (Mode 10), with CPU working at full speed, and an e-mail message
        is sent. After the message has been sent, the board switches back
        to minimal power usage (Mode 1) with the CPU running at only 2kHz.

      For a detailed description of the above power modes, see function
      description for set_cpu_power_mode() in LOW_POWER.LIB.

        Note: When running in Low power mode, LED is programmed to blink
         to indicate that board is still operating.  This program will not
         run in debugging mode with program cable is connected.
*******************************************************************************/
#class auto

#define LOWPOWER_VERBOSE // Debugging option

/* Define HIT_WATCHDOG if watchdog is enabled (default) while using
*  power modes 6 through 10 (ie when periodic interrupt is disabled).
*/
#define HIT_WATCHDOG

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

/*
 *   These macros need to be changed to the appropriate values or
 *   the smtp_sendmail(...) call in main() needs to be changed to
 *   reference your values.
 */

#define FROM     "myaddress@mydomain.com"
#define TO       "myaddress@mydomain.com"
#define SUBJECT  "You've got mail!"
#define BODY     "Visit the Rabbit Semiconductor web site.\r\n" \
	"There you'll find the latest news about Dynamic C."

/*
 *   The SMTP_SERVER macro tells DCRTCP where your mail server is.  This
 *   mail server MUST be configured to relay mail for your controller.
 *
 *   This value can be the name or the IP address.
 */

/*
 *   The SMTP_SERVER macro tells DCRTCP where your mail server is.  This
 *   mail server MUST be configured to relay mail for your controller.
 *
 *   This value can be the name or the IP address.
 */

#define SMTP_SERVER "mymailserver.mydomain.com"

/*
 *   The SMTP_DOMAIN should be the name of your controller.  i.e.
 *   "somecontroller.somewhere.com"  Many SMTP servers ignore this
 *   value, but some SMTP servers use this field.  If you have
 *   problems, turn on the SMTP_DEBUG macro and see were it is
 *   bombing out.  If it is in the HELO command consult the
 *   person in charge of the mail server for the appropriate value
 *   for SMTP_DOMAIN. If you do not define this macro it defaults
 *   to the value in MY_IP_ADDRESS.
 *
 */

//#define SMTP_DOMAIN "mycontroller.mydomain.com"

/*
 *   The SMTP_VERBOSE macro logs the communications between the mail
 *   server and your controller.  Uncomment this define to begin
 *   logging
 */

// #define SMTP_VERBOSE

/********************************
 * End of configuration section *
 ********************************/

/* The following defines the macros SWITCH_PRESSED() and SET_LED(), based on which Rabbit prototyping
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
#use dcrtcp.lib
#use smtp.lib
#use "low_power.lib"

void main()
{
	auto int powerMode;
   auto unsigned long delayStart;
   auto char ledState;

   brdInit();

	ledState = 0; //used to blink LED light on and off

	sock_init();
	// Wait for the interface to come up
	while (ifpending(IF_DEFAULT) == IF_COMING_UP) {
		tcp_tick(NULL);
	}

   //Bring down TCP/IP interface in preparation for Low Power mode.
   ifconfig(IF_ETH0, IFS_DOWN, IFS_END);
   //...and power down Ethernet controller.
   pd_powerdown(IF_ETH0);
   /* Set Rabbit to work under minimum power consumption mode (Mode 10)
    * until user presses Switch 2 (Switch 1 on RCM36/37xx boards) */
   powerMode = 10;
   set_cpu_power_mode(powerMode, CLKDOUBLER_OFF, SHORTCS_OFF);

   while(1)
   {
 		costate
		{
			if (!SWITCH_PRESSED())	 //wait for switch S2 press (S1 on RCM36/37xx boards)
				abort;

         while(SWITCH_PRESSED())  //wait for release
         	yield;

			powerMode = 1;	 //set power mode to work at full cpu speed
         set_cpu_power_mode(powerMode, CLKDOUBLER_ON, SHORTCS_OFF);

         //power up Ethernet controller
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
		} //end costate

 	   if(powerMode == 1)
      {
	      //switch press detected if got here, send an email
	      smtp_sendmail(TO, FROM, SUBJECT, BODY);

	      while(smtp_mailtick()==SMTP_PENDING)
	         continue;

	      if(smtp_status()==SMTP_SUCCESS)
	         printf("Message sent\n");
	         else
	      printf("Error sending message\n");

         //Switch back to lowest power consumption mode after sending e-mail
         //Bring down TCP/IP interface.
         ifconfig(IF_ETH0, IFS_DOWN, IFS_END);
         //...and power down Ethernet controller.
         pd_powerdown(IF_ETH0);
         //Set power back to 10 (Minimum power consumption)
         powerMode = 10;
         set_cpu_power_mode(powerMode, CLKDOUBLER_OFF, SHORTCS_OFF);
      }

      /* CPU is running in 2-32kHZ range here, so
   	 * blink the LED light to indicate that we are
       * still running. */
      if(powerMode > 5)
      {
	   	ledState = !ledState;
	   	SET_LED(ledState);
      }

   /* For cases where the watchdog timer is enabled, and
    * we are operating in a power mode where periodic interrupts
    * are disabled, we also need to hit the watchdog promptly */
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
   } //end loop
}