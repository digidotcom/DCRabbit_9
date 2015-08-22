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
   the demo board is pressed.

   This program is based on the "samples/icom/smtpdemo.c" program.

  	Connections:
	------------
	1. DEMO board jumper settings:
			- H1 remove all jumpers
			- H2 jumper pins 3-5
              jumper pins 4-6

	2. Connect a wire from the controller J12 GND, to the DEMO board
	   J1 GND.

	3. Connect a wire from the controller J12 +PWR to the DEMO board
	   J1 +K.

	4. Connect SW1 from demo board to J1, DIO00 on the controller.
	5. Connect SW2 from demo board to J1, DIO01 on the controller.
	6. Connect SW3 from demo board to J1, DIO02 on the controller.
	7. Connect SW4 from demo board to J1, DIO03 on the controller.


	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press any one of the DEMO board switches SW1 - SW4 to send an email.

*******************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


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
 * Check the status of switch 2
 */
cofunc void CheckSwitch2()
{
	if (digIn(2)) abort;				// if button not down skip out
	waitfor(DelayMs(50));			// wait 50 ms
	if (digIn(2)) abort;				// if button not still down exit

	SendMail(2);						// send email since button was down 50 ms

	while (1) {
		waitfor(digIn(2));			// wait for button to go up
		waitfor(DelayMs(200));		// wait additional 200 ms
		if (digIn(2))
			break;		// if button still up break out of while loop
	}
}

/*
 * Check the status of switch 3
 */
cofunc void CheckSwitch3()
{
	if (digIn(3)) abort;				// if button not down skip out
	waitfor(DelayMs(50));			// wait 50 ms
	if (digIn(3)) abort;				// if button not still down exit

	SendMail(3);						// send email since button was down 50 ms

	while (1) {
		waitfor(digIn(3));			// wait for button to go up
		waitfor(DelayMs(200));		// wait additional 200 ms
		if (digIn(3))
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
	costate
	{
		waitfordone { CheckSwitch2(); }
	}
	costate
	{
		waitfordone { CheckSwitch3(); }
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

	emailArray[2].from = "tester@foo.bar";
	emailArray[2].to = "name@company.com";
	emailArray[2].subject = "System shut down";
	emailArray[2].body = "The system has been shut down.";

	emailArray[3].from = "tester@foo.bar";
	emailArray[3].to = "name@company.com";
	emailArray[3].subject = "System malfunction";
	emailArray[3].body = "Temperature Sensor #3 failed.";
}


///////////////////////////////////////////////////////////////////////////

void main()
{
	static int counter;

	// Initialize the controller
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
			printf("\rCode execution counter = %d", counter++);
		}
	}
}