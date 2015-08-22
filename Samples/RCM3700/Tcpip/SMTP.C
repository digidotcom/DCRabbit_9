/**********************************************************
   smtp.c
 	Z-World, 2003

	This program is used with RCM3700 series controllers
	with prototyping boards.

	Description
	===========
   This program uses the SMTP library to send an e-mail
   when a switch on the prototyping board is pressed.

	DS1 LED on the proto-board will light-up
	when sending mail.

   This program is adapted from \Samples\ICOM\smtpdemo.c
   program.

   Note:  Port pin PB7 is connected as both switch S2 and
   external I/O bus on the proto-board.  S2 should not be used
   with ethernet operations.

	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press DS1 on the prototyping board to
		send an email.

***********************************************************/
#define DS1(x) (BitWrPortI(PFDR, &PFDRShadow, x, 6))	//led, port F bit 6
#define DS2(x) (BitWrPortI(PFDR, &PFDRShadow, x, 7))	//led, port F bit 7
#define S1  (BitRdPortI(PFDR, 4)) 							//switch, port F bit 4
#define ON	0
#define OFF 1

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
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1


/*
 *   These macros need to be changed to the appropriate values.
 */

#define EMAILTO "you@anywhere.com"
#define EMAILFROM "tester@foo.bar"


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
	if (S1)												// wait for switch S1 press
		abort;											// if button not down skip out
	waitfor(DelayMs(50));							// wait 50 ms
	if (S1)												// wait for switch S1 press
		abort;											// if button not still down exit

	DS1(ON);												// DS1 on
	SendMail(0);										// send email since button was down 50 ms
	DS1(OFF);											// DS1 off

	while (1)
	{
		waitfor(S1);									// wait for button to go up
		waitfor(DelayMs(200));						// wait additional 200 ms
		if (S1)											// wait for switch S1 press
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
}


void init_emails(void)
{
	// Define emails here
	emailArray[0].from = EMAILFROM;
	emailArray[0].to = EMAILTO;
	emailArray[0].subject = "Self-Test status OK";
	emailArray[0].body = "Self-test completed, all tests passed.";

	emailArray[1].from = EMAILFROM;
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