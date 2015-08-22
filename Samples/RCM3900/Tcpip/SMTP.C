/*******************************************************************************
   smtp.c
   Rabbit, 2007

   This sample program is used with RCM3900 series controllers and RCM3300
   prototyping boards.

   Description
   ===========
   This sample program uses the SMTP library to send an e-mail when a switch on
   the prototyping board is pressed.  On the prototyping board, the DS3 or DS4
   LED will light up when sending an e-mail.

   This sample program is adapted from the ...\Samples\ICOM\smtpdemo.c sample
   program.

   Instructions
   ============
   1. Edit the email configuration macros below as necessary to suit your
      requirements.

   2. Compile and run this program in debug mode.

   3. Press S2 or S3 on the prototyping board to send an email.

   4. Check your destination email account for new emails and observe the status
      messages printed in the STDIO window as well as the DS3 and DS4 LEDs on
      the prototyping board.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

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
#define EMAILFROM "mycontroller@somewhere.com"

/*
 *   The SMTP_SERVER macro tells DCRTCP where your mail server is.  This
 *   mail server MUST be configured to relay mail for your controller.
 *
 *   This value can be the name or the IP address.
 */
#define SMTP_SERVER "mymailserver.somewhere.com"

/*
 *   The SMTP_DOMAIN should be the name of your controller.  e.g.
 *   "mycontroller.somewhere.com"  Many SMTP servers ignore this
 *   value, but some SMTP servers use this field.  If you have
 *   problems, turn on the SMTP_VERBOSE macro and see were it is
 *   bombing out.  If it is in the HELO command consult the
 *   person in charge of the mail server for the appropriate value
 *   for SMTP_DOMAIN. If you do not define this macro it defaults
 *   to the value in MY_IP_ADDRESS.
 *
 */
//#define SMTP_DOMAIN "mycontroller.somewhere.com"

/*
 *   The SMTP_VERBOSE macro logs the communications between the mail server and
 *   your controller.  Uncomment this #define to begin logging.
 */
//#define SMTP_VERBOSE

/*
 *   The USE_SMTP_AUTH macro enables SMTP Authentication, a method
 *   where the client authenticates with the server before sending
 *   a message.  Call smtp_setauth() before smtp_sendmail() to set
 *   the username and password to use for authentication.
 */
//#define USE_SMTP_AUTH

/*
 *   If the following macro is defined, then if SMTP authentication
 *   fails, the library will NOT attempt non-authenticated SMTP.
 */
//#define SMTP_AUTH_FAIL_IF_NO_AUTH

/*
 * IF USE_SMTP_AUTH is defined then the following SMTP User name and password
 * macro definitions are required and must be set appropriately for the User's
 * SMTP account.
 */
#define MY_SMTP_AUTH_USERNAME "mycontroller@somewhere.com"
#define MY_SMTP_AUTH_PASSWORD "Secret!Secret!"

/********************************
 * End of configuration section *
 ********************************/

// The structure that holds an email message
typedef struct {
	char *from;
	char *to;
	char *subject;
	char *body;
} Email;
Email emailArray[2];	// holds the email messages

#memmap xmem
#use dcrtcp.lib
#use smtp.lib

/*
 * Send the email that corresponds to the switch that was pressed.
 */
void SendMail(int email)
{
	auto int rc;

	// Start sending the email
	smtp_sendmail(emailArray[email].to,      emailArray[email].from,
	              emailArray[email].subject, emailArray[email].body);

	// Wait until the message has been sent
	while (smtp_mailtick() == SMTP_PENDING);

	// Check to see if the message was sent successfully
	if ((rc = smtp_status()) == SMTP_SUCCESS) {
		printf("\n\rMessage sent.\n\r");
	} else {
		printf("\n\rError sending the email message :%x.\n\r",rc);
	}
}

/*
 * Check the status of switch 1
 */
cofunc void CheckSwitch1(void)
{
	waitfor(!switchIn(S2));			// wait for switch press
	waitfor(DelayMs(50));			// wait 50 milliseconds more
	if (switchIn(S2)) abort;		// if button not still down, exit

	ledOut(DS3, ON);					// LED on
	SendMail(0);						// send email since button down for >= 50 mS
	ledOut(DS3, OFF);					// LED off

	while (1) {
		waitfor(switchIn(S2));		// wait for switch release
		waitfor(DelayMs(200));		// wait additional 200 milliseconds
		if (switchIn(S2)) break;	// if button still up, break out of while loop
	}
}

/*
 * Check the status of switch 2
 */
cofunc void CheckSwitch2(void)
{
	waitfor(!switchIn(S3));			// wait for switch press
	waitfor(DelayMs(50));			// wait 50 milliseconds more
	if (switchIn(S3)) abort;		// if button not still down, exit

	ledOut(DS4, ON);					// LED on
	SendMail(1);						// send email since button down for >= 50 mS
	ledOut(DS4, OFF);					// LED off

	while (1) {
		waitfor(switchIn(S3));		// wait for switch release
		waitfor(DelayMs(200));		// wait additional 200 milliseconds
		if (switchIn(S3)) break;	// if button still up, break out of while loop
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
	costate {
		waitfordone { CheckSwitch1(); }
	}
	costate {
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

void main(void)
{
	static unsigned long counter;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	sock_init();

	// initialize the email structure array with user defined emails
	init_emails();

#ifdef USE_SMTP_AUTH
	smtp_setauth(MY_SMTP_AUTH_USERNAME, MY_SMTP_AUTH_PASSWORD);
#endif

	counter = 0;
	while (1) {
		costate {
			// check if user has activated a switch to send an email
			check_switch_status();
		}
		costate {
			printf("\rCode execution counter = %lu.", counter++);
		}
	}
}

