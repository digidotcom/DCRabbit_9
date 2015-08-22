/*******************************************************************************
   smtp.c
   Z-World, 2002

   This program demonstrates a basic Smart Star system using the SMTP library
   to send an e-mail when a keypress is detected on a display/keypad module.

   This program is based on the "samples/icom/smtpdemo.c" program.

	Test Instructions:
	------------------
	1. Make all necessary changes in the configuration section.
	2. Compile and run this program.
	3. A code execution counter will be displayed.
	4. Press the up, down, right and left arrows on the keypad to send an email.

*******************************************************************************/
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
 * Initialize display/keypad module. Default to 6x8 font size.
 */
 
fontInfo fi6x8;
windowFrame wholewindow;

void initdkm()
{
	glInit();
	glBlankScreen();
	glBackLight(1);
	keyInit();
	keyConfig (  0,'0',0, 0, 0,  0, 0 );
	keyConfig (  1,'1',0, 0, 0,  0, 0 );
	keyConfig (  2,'2',0, 0, 0,  0, 0 );
	keyConfig (  3,'3',0, 0, 0,  0, 0 );
	keyConfig (  4,'4',0, 0, 0,  0, 0 );
	keyConfig (  5,'5',0, 0, 0,  0, 0 );
	keyConfig (  6,'6',0, 0, 0,  0, 0 );
	
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);
	TextWindowFrame(&wholewindow, &fi6x8, 0, 0, 122, 32);
	
	glBlankScreen();
	glBackLight(1);
}


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
	if (smtp_status()==SMTP_SUCCESS)
	{
		//printf("\n\rMessage sent\n\r");
		TextGotoXY(&wholewindow,0,2);
		TextPrintf(&wholewindow, "Message sent");
	}
	else
	{
		//printf("\n\rError sending the email message\n\r");
		TextGotoXY(&wholewindow,0,2);
		TextPrintf(&wholewindow, "Error sending the email message");
	}
}

/*
 * Check the status of switch 0
 */
void CheckSwitch0()
{	
	/*
	 Before sending an email, this example code portion could switch
	 on relay channel 0, in slot 0.
	 
	 relayOut(ChanAddr(0,0),1);
	*/
	
	SendMail(0);	
}

/*
 * Check the status of switch 1
 */
void CheckSwitch1()
{	
	/*
	Before sending an email, this example code portion could read
	analog channel 1, in slot 1.
	
	int value;
	value = anaIn(ChanAddr(1,1));
	if (value > 2047)
		emailArray[1].body = "AD Channel 1 detects decrease";
	else
	 	emailArray[1].body = "AD Channel 1 detects increase";
	*/
	
	SendMail(1);	

}

/*
 * Check the status of switch 2
 */
void CheckSwitch2()
{	
	/*
 	Before sending an email, this example code portion could set
	digital output channel 2, in slot 2.

	digOut(ChanAddr(2,2), 1);
	*/
	
	SendMail(2);	
}

/*
 * Check the status of switch 3
 */
void CheckSwitch3()
{	
	/*
	 This example code portion could switch off relay channel 0, in slot 0
	 relayOut(ChanAddr(0,0),0);
	*/
	
	SendMail(3);	
}


/*
 *		
 *		Check if any of the switches have been pressed
 *    if so then a send email message.
 *
 */
void check_switch_status(int keypress)
{
	switch(keypress)
	{
		case '0':
			CheckSwitch0();
			break;
		case '1':
			CheckSwitch1();
			break;
		case '2':
			CheckSwitch2();
			break;
		case '3':
			CheckSwitch3();
			break;
		default:
			break;
	}
}


void init_emails(void)
{
	// Define emails here
	emailArray[0].from = "tester@foo.bar";
	emailArray[0].to = EMAILTO;
	emailArray[0].subject = "System ON";
	emailArray[0].body = "Relay 0 switched on";

	emailArray[1].from = "tester@foo.bar";
	emailArray[1].to = EMAILTO;
	emailArray[1].subject = "Temperature Change";
	emailArray[1].body = "AD Channel 1 detects change";

	emailArray[2].from = "tester@foo.bar";
	emailArray[2].to = EMAILTO;
	emailArray[2].subject = "Auto Cooling Enabled";
	emailArray[2].body = "Output channel 2 set";

	emailArray[3].from = "tester@foo.bar";
	emailArray[3].to = EMAILTO;
	emailArray[3].subject = "System OFF";
	emailArray[3].body = "Relay 0 switched off";
}


///////////////////////////////////////////////////////////////////////////
void main()
{
	static int counter, keypress;

	// Initialize the controller
	brdInit();
	initdkm();
	sock_init();

	// initialize the email structure array with user defined emails
	init_emails();

	counter = 0;
	while(1)
	{
		costate
		{
			keyProcess ();
			waitfor(DelayMs(10));
		}
		
		costate
		{  // check if user has activated a switch to send an email 
			if ((keypress = keyGet()) != 0)
			{
				check_switch_status(keypress);
			}
		}
		costate
		{
			//printf("\rCode execution counter = %d", counter++);
			TextGotoXY(&wholewindow,0,0);
			TextPrintf(&wholewindow, "Code execution counter = %d", counter++);
		}
	}
}
