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
/*****
Tests PPP interface, with modem attached to serial port C (default).
Connects to ISP and sends a test mail.
Brings the PPP interface up and down three times.

You will nedd to change the default addresses and the mail_to[]
and mail_from[] globals.
You will also may need to alter the send/expect sequence at the
beginning of the program (plus DIALUP_NAME, DIALUP_PASSWORD and DIALUP_NUMBER).

This sample assumes you are using hardware modem flow control i.e. CTS and RTS.
The CTS port and bit number are defined in the IFS_PPP_CTSPIN parameter,
defaulting to parallel port B, bit 0.  Likewise, RTS is set via IFS_PPP_RTSPIN,
and defaults to parallel port B, bit 7.

********/

#define TCPCONFIG 0				// No predefined config for this sample

//Use serial port C (IF_PPP2)
#define USE_PPP_SERIAL 0x04

//Uncomment to get PPP detail
//#define PPP_VERBOSE
//#define PPP_DEBUG
//#define PPPLINK_VERBOSE
//#define PPPLINK_DEBUG
//#define SMTP_VERBOSE
//#define SMTP_DEBUG 1
#define CHAT_VERBOSE				// See what's going on.
//#define CHAT_DEBUG

#define DIALUP_NAME "username"
#define DIALUP_PASSWORD "password"
#define DIALUP_NUMBER "5551212"

// Following macro sets up the initial login screen navigation.  This is necessary for access to
// many ISPs, since when you dial in with the modem, the ISP does user authentication via an
// old-fashioned login screen.
// %0 gets userid substituted, %1 is password -- obtained from values passed for IFS_PPP_REMOTEAUTH.
// The string below gets compiled to "ATDT5551212 #CONNECT '' #ogin: %0 #word: %1 ~"
// This is a send/expect sequence used to establish a modem connection to the ISP (ready for PPP to
// take over).  Expanded out:
//  ATZ            - send the modem reset command (usually default config 0).  This helps get the modem
//                   into a known initial state.
//  #ok            - wait for 'ok' string (the leasing '#' means case-insensitive i.e. will accept
//                   'OK', 'Ok' etc.
//  ATDT5551212    - send the dialout command
//  #CONNECT       - wait for 'connect' message
//  ''             - Send nothing (quotes are a place-holder).  Note that the CRLF is appended to any
//                   send-string _except_ an empty string like this (but you can suppress the CRLF by
//                   prefixing the send-string with '#').
//  #ogin:         - Wait for a 'login:' message.  We don't actually look for the initial 'L' since the
//                   first character can sometimes be lost.
//  @%0            - send the userid (i.e. the value supplied by the IFS_PPP_REMOTEAUTH parameter.
//                   If the character-based login prompt expects a different userid than the one passed
//                   to the following PPP authentication phase, then you can't use %0.  In this case,
//                   put the character-based login ID directly in the send/expect string.
//                   The initial '@' character causes us to pause for 1.5 seconds before sending the
//                   string.  This is not theoretically required, however some ISPs have software which
//                   tries to discourage machine-based hacking attempts.  If we respond too quickly, the
//                   ISP thinks we are typing at inhumanly fast rates, and deems us to be a hacker.
//  #word:         - Wait for the 'password' prompt.
//  @%1            - Send the password string.  See considerations above for the logon ID string.
//  ~              - Wait for ascii tilde char (0x7E).  This is handy, since this is the first character sent
//                   by PPP when it starts up on the peer.  Some peers send an ascii 'PPP' string, but this
//                   is not quite so reliable as a means of detecting PPP startup.
#define DIALUP_SENDEXPECT "ATZ #ok ATDT" DIALUP_NUMBER " #CONNECT '' #ogin: @%0 #word: @%1 ~"

#define SMTP_SERVER "smtp.isp.com"


#memmap xmem
#use "dcrtcp.lib"
#use "smtp.lib"


#define TIMEZONE        -8

#define LCP_TIMEOUT 5000

const char mail_to[] = "you@wherever.com";
const char mail_from[] = "rabbit@isp.com";
const char mail_subject[] = "Mail from Rabbit";
const char mail_body[] = "This is a test message sent by a Rabbit through an ISP.";


int main()
{
	auto unsigned long t;
	auto char buffer[100];
	auto int mail_status;
	auto int count;

	sock_init(); //initialize TCP/IP

	//test repeated open/close of modem link
	for(count = 0; count < 3; count++)
	{

	   //configure PPP for dialing in to ISP and bring it up
	   ifconfig(IF_DEFAULT,
	            IFS_PPP_SPEED, 57600L,
	            IFS_PPP_RTSPIN, PBDR, NULL, 7,   // Note: the NULL is for the shadow register.  From DC9.0,
	                                             // this is computed automatically - the value passed here
	                                             // for the shadow register is ignored.
	            IFS_PPP_CTSPIN, PBDR, 0,
	            IFS_PPP_FLOWCONTROL, 1,
	            IFS_PPP_SENDEXPECT, DIALUP_SENDEXPECT,
	            IFS_PPP_HANGUP, "ATH #ok",
	            IFS_PPP_MODEMESCAPE, 1,
	            IFS_PPP_ACCEPTIP, 1,
	            IFS_PPP_ACCEPTDNS, 1,
	            IFS_PPP_REMOTEAUTH, DIALUP_NAME, DIALUP_PASSWORD,
	            IFS_UP,
	            IFS_END);

	   while(ifpending(IF_DEFAULT) & 1)
	      tcp_tick(NULL);

	   if(ifstatus(IF_DEFAULT))
	      printf("PPP established\n");
	   else
	      printf("PPP failed\n");

	   ifconfig(IF_DEFAULT, IFG_IPADDR, &t, IFS_END);
	   printf("IP address is %s\n", inet_ntoa( buffer, t));

	   smtp_sendmail(mail_to, mail_from, mail_subject, mail_body);

	   while (SMTP_PENDING == (mail_status = smtp_mailtick()));

	   if(mail_status == SMTP_SUCCESS)
	      printf("Message sent\n");
	   else
	      printf("Failed to send message\n");

	   ifconfig(IF_DEFAULT, IFS_DOWN, IFS_END);

	   //wait while PPP terminates
	   while(ifpending(IF_DEFAULT) & 1)
	      tcp_tick(NULL);

	} //end of for loop

   return 0;
}