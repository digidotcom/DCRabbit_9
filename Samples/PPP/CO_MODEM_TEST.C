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
NOTE:
	The new (DC 7.30+) drivers for PPP and modem control are
non-blocking. Therefore, the PPP and modem cofunctions in this
sample are obsolete. Please refer to MODEM_TEST.C for an example
of the current API.


Tests modem.lib
Trys to connect to mother.com

Using cofunctions from modem.lib and ppp.lib

Be sure to change the default addresses, and the mail_to[], mail_from[] globals
You will also need to alter the Send/Expect sequence at the beginning of the program

********/
#define TCPCONFIG		0
//Use serial port C (IF_PPP2)
#define USE_PPP_SERIAL 0x04

//Macros for standard login procedure. You may still need to alter expected strings below
//Don't forget the carriage returns '\r'
#define DIALUP_DIALSTRING "ATDT5552222\r"
#define DIALUP_NAME "username\r"
#define DIALUP_PASSWORD "password\r"

#define SMTP_SERVER "smtp.yourisp.com"

#define SMTP_DEBUG 1
#define MODEM_DEBUG 1

//Uncomment to turn on PPP detail
//#define PPP_VERBOSE

#define CINBUFSIZE 31
#define COUTBUFSIZE 31

#memmap xmem
#use "dcrtcp.lib"
#use "modem.lib"
#use "smtp.lib"


#define TIMEZONE        -8

#define LCP_TIMEOUT 5000

//#define FRAGSUPPORT		//make sure FRAGSUPPORT is on


const char mail_to[] = "rabbit@yourisp.com";
const char mail_from[] = "you@wherever.com";
const char mail_subject[] = "Mail from Rabbit";
const char mail_body[] = "This is a test message sent by a Rabbit through an ISP.";


void main()
{
	auto unsigned long t;
	auto char buffer[100];
	auto int mail_status;
	auto int status;
	auto int run_flag;

	run_flag = 1;

	while(run_flag)
	{
		costate
		{
			if(ModemOpen(57600) == 0)
			{
				printf("No modem detected\n");
			}
			wfd { status = CofModemInit(); }
			if(status)
			{
				printf("Modem Initialized\n");
			}
			else
			{
				printf("Could not init modem\n");
				exit(-1);
			}

			wfd { CofModemSend("ATM0\r"); }
         wfd { status = CofModemExpect("OK", 2000); }
			if( status == 0)
			{
				printf("Expected 'OK'\n");
				exit(-1);
			}

         wfd { CofModemSend(DIALUP_DIALSTRING); }
			wfd { status = CofModemExpect("ONNECT", 40000); }
			if( status == 0)
			{
				printf("Expected 'CONNECT'\n");
				exit(-1);
			}

			wfd { status = CofModemExpect("ogin:", 10000); }
			if(status == 0)
			{
				printf("Expected 'Login:'\n");
				exit(-1);
			}
			wfd { CofModemSend(DIALUP_NAME); }
			wfd { status = CofModemExpect("word:", 2000); }
			if( status == 0)
			{
				printf("Expected Password\n");
				exit(-1);
			}
			wfd { CofModemSend(DIALUP_PASSWORD); }		//secret password, erase later
			wfd { status = CofModemExpect("PPP", 5000); }
			if( status == 0)
			{
				printf("Didn't get PPP session\n");
				exit(-1);
			}
			ModemClose();

			sock_init();

			//Need to have modem RTS pin be active low
			BitWrPortI(PBDR, &PBDRShadow, 0, 7);

			//set up options for dialing in to an ISP and start up
			//interface
			ifconfig(IF_PPP2,
						IFS_PPP_INIT,
						IFS_PPP_SPEED, 57600L,
						IFS_PPP_ACCEPTIP, 1,
						IFS_PPP_ACCEPTDNS, 1,
						IFS_PPP_REMOTEAUTH, DIALUP_NAME, DIALUP_PASSWORD,
						IFS_UP,
						IFS_END );

			while(ifpending(IF_PPP2) % 2)
			{
				tcp_tick(NULL);
				yield; //wait for interface to try and come up
			}
			if(ifstatus(IF_PPP2))
			{
				printf("PPP established\n");
			}
			else
			{
				printf("PPP failed\n");
			}
			printf("IP address is %s\n", inet_ntoa( buffer, gethostid()));

			smtp_sendmail(mail_to, mail_from, mail_subject, mail_body);
			mail_status = SMTP_PENDING;
			while(mail_status == SMTP_PENDING)
			{
				mail_status = smtp_mailtick();
				yield;
			}
			if(mail_status == SMTP_SUCCESS)
			{
				printf("Message sent\n");
			}
			else
			{
				printf("Failed to send message\n");
			}
			ifconfig(IF_PPP2, IFS_DOWN, IFS_END);
			while(ifpending(IF_PPP2) % 2)
			{
				tcp_tick(NULL);
				yield; //wait for PPP to terminate
			}
			ModemOpen(57600);
			wfd { status = CofModemHangup(); }
			ModemClose();
			run_flag = 0;
		}

		costate
		{
			//do somthing else to prove that we can
			WrPortI(SPCR, NULL, 0x84); //set port A to output
			BitWrPortI(PADR, &PADRShadow, 1, 0);
			waitfor(DelayMs(500));
			BitWrPortI(PADR, &PADRShadow, 0, 0);
			waitfor(DelayMs(500));
		}
	}

}