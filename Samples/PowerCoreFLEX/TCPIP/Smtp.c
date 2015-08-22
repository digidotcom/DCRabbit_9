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
   smtp.c

   This program uses the SMTP library to send an e-mail when a switch on
   the PowerCoreFLEX prototyping board is pressed (S2 or S3).

   This program is based on the "samples/icom/smtpdemo.c" program.

	Test Instructions:
	------------------
   1. Set SMTP_SERVER.
   2. Set email addresses and messages in init_emails function.
	3. Compile and run this program.
	4. Press one of the prototyping board switches S2 or S3 to
      send an email.

*******************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"


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
Email emailArray[4];	// holds the email messages

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
 *   The SMTP_SERVER macro tells DCRTCP where your mail server is.  This
 *   mail server MUST be configured to relay mail for your controller.
 *
 *   This value can be the name or the IP address.
 */

#define SMTP_SERVER "10.10.6.176"
//#define SMTP_SERVER "mymailserver.mydomain.com"

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

//#define SMTP_VERBOSE

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
 * Check the status of switch 0
 */
cofunc void CheckSwitch0()
{
	if (digIn(0)) abort;				// if button not down skip out
	waitfor(DelayMs(50));			// wait 50 ms
	if (digIn(0)) abort;				// if button not still down exit

	SendMail(0);						// send email since button was down 50 ms

	while (1) {
		waitfor(digIn(0));			// wait for button to go up
		waitfor(DelayMs(200));		// wait additional 200 ms
		if (digIn(0))
			break;		// if button still up break out of while loop
	}
}

/*
 * Check the status of switch 1
 */
cofunc void CheckSwitch1()
{
	if (digIn(1)) abort;				// if button not down skip out
	waitfor(DelayMs(50));			// wait 50 ms
	if (digIn(1)) abort;				// if button not still down exit

	SendMail(1);						// send email since button was down 50 ms

	while (1) {
		waitfor(digIn(1));			// wait for button to go up
		waitfor(DelayMs(200));		// wait additional 200 ms
		if (digIn(1))
			break;		// if button still up break out of while loop
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
		waitfordone { CheckSwitch0(); }
	}
	costate
	{
		waitfordone { CheckSwitch1(); }
	}
}


void init_emails(void)
{
	// Define emails here
	emailArray[0].from = "tester@foo.bar";
	emailArray[0].to = "name@company.com";
	emailArray[0].subject = "Self-Test status OK";
	emailArray[0].body = "Self-test completed, all tests passed.";

	emailArray[1].from = "tester@foo.bar";
	emailArray[1].to = "name@company.com";
	emailArray[1].subject = "Self-Test failed.";
	emailArray[1].body = "Rocket booster failed";

}


///////////////////////////////////////////////////////////////////////////

void main()
{
	static int counter;

	// Initialize I/O to use PowerCoreFLEX prototyping board
	brdInit();

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
			printf("\rPress SW1 or SW2 to send email, waiting...%d", counter++);
		}
	}
}