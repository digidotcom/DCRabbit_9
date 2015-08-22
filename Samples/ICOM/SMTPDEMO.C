/********************************************************************
   smtpdemo.c
   Z-World, 2000

 	This sample program is used with Intellicom Series and Z-World
 	Demo Board.

   This program uses the SMTP library to send an
   e-mail when a key on the keypad or a switch on
   the demo board is pressed.
********************************************************************/
#class auto					/* Change local var storage default to "auto" */

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
 *
 * It is best to explicitly turn off DHCP in your configuration.  This
 * will allow the console to completely manage DHCP, so that it does
 * not acquire the lease at startup in sock_init() when it may not need
 * to (if the user has not turned on DHCP).
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
 */

//#define SMTP_DOMAIN "mycontroller.mydomain.com"

/*
 *   The SMTP_DEBUG macro logs the communications between the mail
 *   server and your controller.  Uncomment this define to begin
 *   logging
 */

//#define SMTP_DEBUG

/*
 * Also note that you will need to set up the email messages in the
 * main() function.
 */

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"
#use "smtp.lib"

// The structure that holds an email message
typedef struct {
	char *from;
	char *to;
	char *subject;
	char *body;
} Email;

// This value will be used for keys that should be ignored
#define IGNORE 99

///////////////////////////////////////////////////////////////////////

// Initialize the switches
void SwitchInit(void)
{
	// Initialize Port D for Switches 0 and 1
	WrPortI(PDDDR, &PDDDRShadow, 0xf3&PDDDRShadow);		//set bits 2,3 to input
	WrPortI(PDFR, &PDFRShadow, 0xf3&PDFRShadow);			//i/o normal

	// Initialize Port E for Switches 2 and 3
	WrPortI(PEDDR, &PEDDRShadow, 0xf3&PEDDRShadow);		//set bits 2,3 to input
	WrPortI(PEFR, &PEFRShadow, 0xf3&PEFRShadow);			//i/o normal
}

/*
 * Send the mail specified by the key value.  Note that the message
 * number is actually one less than the key number (since key number 0
 * cannot be used).
 */
void SendMail(unsigned key, Email emailArray[])
{
	// Start sending the email
	smtp_sendmail(emailArray[key-1].to, emailArray[key-1].from,
   	           emailArray[key-1].subject, emailArray[key-1].body);

	// Wait until the message has been sent
	while(smtp_mailtick()==SMTP_PENDING)
		continue;

	// Check to see if the message was sent successfully
	if(smtp_status()==SMTP_SUCCESS) {
		dispClear();
		dispPrintf("Message sent\n");
	} else {
		dispClear();
		dispPrintf("Error sending\nmessage\n");
	}
}

/*
 * Check the status of switch 1
 */
cofunc void CheckSwitch1(Email emailArray[])
{
	if (!BitRdPortI(PDDR, 2)) abort;		// if button not down skip out
	waitfor(DelayMs(50));					// wait 50 ms
	if (!BitRdPortI(PDDR, 2)) abort;		// if button not still down exit

	SendMail(13, emailArray);	// send email since button was down 50 ms

	while (1) {
		waitfor(!BitRdPortI(PDDR, 2));	// wait for button to go up
		waitfor(DelayMs(200));				// wait additional 200 ms
		if (!BitRdPortI(PDDR, 2))
			break;		// if button still up break out of while loop
	}
}

/*
 * Check the status of switch 2
 */
cofunc void CheckSwitch2(Email emailArray[])
{
	if (!BitRdPortI(PDDR, 3)) abort;		// if button not down skip out
	waitfor(DelayMs(50));					// wait 50 ms
	if (!BitRdPortI(PDDR, 3)) abort;		// if button not still down exit

	SendMail(14, emailArray);	// send email since button was down 50 ms

	while (1) {
		waitfor(!BitRdPortI(PDDR, 3));	// wait for button to go up
		waitfor(DelayMs(200));				// wait additional 200 ms
		if (!BitRdPortI(PDDR, 3))
			break;		// if button still up break out of while loop
	}
}

/*
 * Check the status of switch 3
 */
cofunc void CheckSwitch3(Email emailArray[])
{
	if (!BitRdPortI(PEDR, 2)) abort;		// if button not down skip out
	waitfor(DelayMs(50));					// wait 50 ms
	if (!BitRdPortI(PEDR, 2)) abort;		// if button not still down exit

	SendMail(15, emailArray);	// send email since button was down 50 ms

	while (1) {
		waitfor(!BitRdPortI(PEDR, 2));	// wait for button to go up
		waitfor(DelayMs(200));				// wait additional 200 ms
		if (!BitRdPortI(PEDR, 2))
			break;		// if button still up break out of while loop
	}
}

/*
 * Check the status of switch 4
 */
cofunc void CheckSwitch4(Email emailArray[])
{
	if (!BitRdPortI(PEDR, 3)) abort;		// if button not down skip out
	waitfor(DelayMs(50));					// wait 50 ms
	if (!BitRdPortI(PEDR, 3)) abort;		// if button not still down exit

	SendMail(16, emailArray);	// send email since button was down 50 ms

	while (1) {
		waitfor(!BitRdPortI(PEDR, 3));	// wait for button to go up
		waitfor(DelayMs(200));				// wait additional 200 ms
		if (!BitRdPortI(PEDR, 3))
			break;		// if button still up break out of while loop
	}
}

void main()
{
	static Email emailArray[16];	// holds the email messages
	auto unsigned key;				// value of key pressed
	auto int i;						// counter

	// Initialize the email array
	for (i = 0; i < 16; i++) {
		emailArray[i].from = "";
	}

	// Define emails here--note that you only need define the emails you
	// will actually use.  Indices 0-11 correspond to the 12 keys on the
	// keypad.  Indices 12-15 refer to the 4 switches on the demo board.
	emailArray[0].from = "tester@foo.bar";
	emailArray[0].to = "manager@foo.bar";
	emailArray[0].subject = "Status OK";
	emailArray[0].body = "All systems functioning correctly.";

	emailArray[1].from = "tester@foo.bar";
	emailArray[1].to = "manager@foo.bar";
	emailArray[1].subject = "System malfunctioning";
	emailArray[1].body = "Systems are malfunctioning!  Help!";

	emailArray[6].from = "tester@foo.bar";
	emailArray[6].to = "manager@foo.bar";
	emailArray[6].subject = "System shut down";
	emailArray[6].body = "The system has been shut down.";

	// These entries refer to the 4 switches on the demo board
	emailArray[12].from = "tester@foo.bar";
	emailArray[12].to = "manager@foo.bar";
	emailArray[12].subject = "Test 1";
	emailArray[12].body = "Test 1 performed.";

	emailArray[13].from = "tester@foo.bar";
	emailArray[13].to = "manager@foo.bar";
	emailArray[13].subject = "Test 2";
	emailArray[13].body = "Test 2 performed.";

	emailArray[14].from = "tester@foo.bar";
	emailArray[14].to = "manager@foo.bar";
	emailArray[14].subject = "Test 3";
	emailArray[14].body = "Test 3 performed.";

	emailArray[15].from = "tester@foo.bar";
	emailArray[15].to = "manager@foo.bar";
	emailArray[15].subject = "Test 4";
	emailArray[15].body = "Test 4 performed.";

	brdInit();		// Initialize the board
	SwitchInit();	// Initialize the switches

	// Configure the upper row of keys on the keypad, in order from left
	// to right
	for (i = 0; i < 6; i++) {
		// Only enable a key if there is a corresponding email message
		if (strcmp(emailArray[i].from, "") != 0) {
			keyConfig ( 5-i, i+1, 0, 0, 0, 0, 0 );
		} else {
			keyConfig ( 5-i, IGNORE, 0, 0, 0, 0, 0 );
		}
	}
	// Configure the lower row of keys on the keypad, in order from left
	// to right
	for (i = 6; i < 12; i++) {
		// Only enable a key if there is a corresponding email message
		if (strcmp(emailArray[i].from, "") != 0) {
			keyConfig ( 19-i, i+1, 0, 0, 0, 0, 0 );
		} else {
			keyConfig ( 19-i, IGNORE, 0, 0, 0, 0, 0 );
		}
	}

	sock_init();	// Initialize networking
	dispClear();	// Clear the LCD

	while (1) {
		// Drive the keypad
		costate {
			keyProcess ();
			waitfor(DelayMs(10));
		}

		// Handle keypresses on the keypad
		costate {
			waitfor(key = keyGet());
			if (key != IGNORE) {
				SendMail(key, emailArray);
			}
		}

		// Handle switch 1
		costate {
			waitfordone { CheckSwitch1(emailArray); }
		}

		// Handle switch 2
		costate {
			waitfordone { CheckSwitch2(emailArray); }
		}

		// Handle switch 3
		costate {
			waitfordone { CheckSwitch3(emailArray); }
		}

		// Handle switch 4
		costate {
			waitfordone { CheckSwitch4(emailArray); }
		}
	}
}

