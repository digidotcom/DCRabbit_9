/**********************************************************
   smtp.c
 	Z-World, 2003

	This program is used with RCM3300 series controllers
	with prototyping boards.

	Description
	===========
   This program uses the SMTP library to send an e-mail
   when a switch on the prototyping board is pressed.

	DS3 and DS4 LED's on the proto-board will light-up
	when sending mail.

   This program is adapted from \Samples\ICOM\smtpdemo.c
   program.

	Test Instructions:
	------------------
	1. Compile and run this program.
	2. Press S2 and S3 on the prototyping board to
		send an email.

***********************************************************/
#use rcm33xx.lib

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6
#define USERLED 0

#define S2  2
#define S3  3

#define ON	1
#define OFF 0

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

#define EMAILTO "me@somewhere.com"
#define EMAILFROM "you@anywhere.com"


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

//#define SMTP_DEBUG

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
	int rc;

	// Start sending the email
	smtp_sendmail(emailArray[email].to, 	  emailArray[email].from,
   	           emailArray[email].subject, emailArray[email].body);

	// Wait until the message has been sent
	while(smtp_mailtick()==SMTP_PENDING)
		continue;

	// Check to see if the message was sent successfully
	if((rc=smtp_status())==SMTP_SUCCESS)
	{
		printf("\n\rMessage sent\n\r");
	}
	else
	{
		printf("\n\rError sending the email message :%x\n\r",rc);
	}
}

/*
 * Check the status of switch 1
 */
cofunc void CheckSwitch1()
{
	if (switchIn(S2))									// wait for switch press
		abort;											// if button not down skip out
	waitfor(DelayMs(50));							// wait 50 ms
	if (switchIn(S2))									// wait for switch press
		abort;											// if button not still down exit

	ledOut(DS3, ON);									// led on
	SendMail(0);										// send email since button was down 50 ms
	ledOut(DS3, OFF);									// led off

	while (1)
	{
		waitfor(switchIn(S2));							// wait for button to go up
		waitfor(DelayMs(200));						// wait additional 200 ms
		if (switchIn(S2))	  							// wait for switch press
			break;										// if button still up break out of while loop
	}
}


/*
 * Check the status of switch 2
 */
cofunc void CheckSwitch2()
{
	if (switchIn(S3))									// wait for switch press
		abort;											// if button not down skip out
	waitfor(DelayMs(50));							// wait 50 ms
	if (switchIn(S3))									// wait for switch press
		abort;											// if button not still down exit

	ledOut(DS4, ON);									// led on
	SendMail(1);										// send email since button was down 50 ms
	ledOut(DS4, OFF);									// led off

	while (1)
	{
		waitfor(switchIn(S3));							// wait for button to go up
		waitfor(DelayMs(200));						// wait additional 200 ms
		if (switchIn(S3))	  							// wait for switch press
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

   #if _USER
   	smtp_setserver(SMTP_SERVER);
   #endif

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