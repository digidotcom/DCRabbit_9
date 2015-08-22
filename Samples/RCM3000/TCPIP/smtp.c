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
   smtp.c

	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	The sample library, rcm3000.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
   This program uses the SMTP library to send an e-mail
   when a switch on the prototyping board is pressed.

	DS1 and DS2 LED's on the proto-board will light-up
	when sending mail.
	
   This program is adapted from \Samples\ICOM\smtpdemo.c
   program.

	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press any DS1 or DS2 on the prototyping board to
		send an email.

***********************************************************/
#class auto
#use rcm3000.lib 	//sample library used for this demo

#define DS1 6		//led, port G bit 6
#define DS2 7		//led, port G bit 7
#define S2  1		//switch, port G bit 1
#define S3  0		//switch, port G bit 0

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

// The structure that holds an email message
typedef struct {
	char *from;
	char *to;
	char *subject;
	char *body;
} Email;
Email emailArray[2];	// holds the email messages

/*
 *	Change email to address
 */
#define EMAILTO "manager@zworld.com"

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

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
 *   The SMTP_DEBUG macro logs the communications between the mail
 *   server and your controller.  Uncomment this define to begin
 *   logging
 */

#define SMTP_DEBUG

/********************************
 * End of configuration section *
 ********************************/


#memmap xmem
#use dcrtcp.lib
#use smtp.lib

/*
 * Send the email that corresponding to the switch that was
 * pressed.
 *
 */
 
void SendMail(int email)
{
	// Start sending the email
	smtp_sendmail(emailArray[email].to, 	  emailArray[email].from,
   	           emailArray[email].subject, emailArray[email].body);

	// Wait until the message has been sent
	while(smtp_mailtick()==SMTP_PENDING)
		continue;

	// Check to see if the message was sent successfully
	if(smtp_status()==SMTP_SUCCESS)
	{
		printf("\n\rMessage sent\n\r");
	}
	else
	{
		printf("\n\rError sending the email message\n\r");
	}
}

/*
 * Check the status of switch 1
 */
cofunc void CheckSwitch1()
{	
	if (BitRdPortI(PGDR, S2))						// wait for switch S2 press
		abort;											// if button not down skip out
	waitfor(DelayMs(50));							// wait 50 ms
	if (BitRdPortI(PGDR, S2))						// wait for switch S2 press
		abort;											// if button not still down exit

	BitWrPortI(PGDR, &PGDRShadow, 0, DS1);		// DS1 on
	SendMail(0);										// send email since button was down 50 ms
	BitWrPortI(PGDR, &PGDRShadow, 1, DS1);		// DS1 off

	while (1)
	{
		waitfor(BitRdPortI(PGDR, S2));			// wait for button to go up
		waitfor(DelayMs(200));						// wait additional 200 ms
		if (BitRdPortI(PGDR, S2))					// wait for switch S2 press
			break;										// if button still up break out of while loop
	}
}

/*
 * Check the status of switch 2
 */
cofunc void CheckSwitch2()
{	
	if (BitRdPortI(PGDR, S3))						//wait for switch S3 press
		abort;											// if button not down skip out
	waitfor(DelayMs(50));							// wait 50 ms
	if (BitRdPortI(PGDR, S3))						//wait for switch S3 press
		abort;											// if button not still down exit

	BitWrPortI(PGDR, &PGDRShadow, 0, DS2);		// DS2 on
	SendMail(1);										// send email since button was down 50 ms
	BitWrPortI(PGDR, &PGDRShadow, 1, DS2);		// DS2 off

	while (1)
	{
		waitfor(BitRdPortI(PGDR, S3));			// wait for button to go up
		waitfor(DelayMs(200));						// wait additional 200 ms
		if (BitRdPortI(PGDR, S3))					//wait for switch S3 press
			break;										// if button still up break out of while loop
	}
}


/*
 *		
 *		Check if any of the switches have been pressed
 *    if so then a send email message.
 *
 */
void check_switch_status(void)
{
	costate
	{
		waitfordone { CheckSwitch1(); }
	}
	costate
	{
		waitfordone { CheckSwitch2(); }
	}
}


void init_emails(void)
{
	// Define emails here
	emailArray[0].from = "tester@foo.bar";
	emailArray[0].to = EMAILTO;
	emailArray[0].subject = "Self-Test status OK";
	emailArray[0].body = "Self-test completed, all tests passed.";

	emailArray[1].from = "tester@foo.bar";
	emailArray[1].to = EMAILTO;
	emailArray[1].subject = "System malfunction!";
	emailArray[1].body = "Temperature Sensor #3 failed.";
}


///////////////////////////////////////////////////////////////////////////
main()
{
	static int counter;

 	
	brdInit();			//initialize board for this demo
	
	sock_init();

	// initialize the email structure array with user defined emails
	init_emails();

	counter = 0;
	while(1)
	{
		costate
		{  // check if user has activated a switch to send an email 
			check_switch_status();
		}
		costate
		{
			printf("\rCode execution counter = %d", counter++);
		}
	}
}
///////////////////////////////////////////////////////////////////////////