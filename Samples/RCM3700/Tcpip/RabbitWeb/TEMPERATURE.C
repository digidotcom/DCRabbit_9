/*******************************************************************************
	temperature.c
   Z-World 2004

	The following program is used with RCM3700 series controllers with
	prototyping boards.

   Description
   ===========
   This program demonstrates using the enhanced RabbitWeb library and web
   interface using analog input THERM_IN7 and a thermistor to calculate
   temperature. It also demonstrates some simple #web variable registration
   along with the authentication features.

   An email message will be sent if the current temperature exceeds the
   minimum and maximum range set by the user.

   See temperature.zhtml and temp_admin.zhtml pages that demonstrates
   the corresponding ZHTML scripting features.


	Thermistor Instructions
	=======================
   This example assumes that the thermistor you are using is the one
   included in the RCM3700 Development Kit. Values for beta, series
   resistor and resistance at standard temperature are given per part
   specification.

	Given:
	Beta = 3965
	Series resistor on protoboard = 1k ohm
	Resistance at 25 degree C = 3k ohm

	1. Install thermistor leads into J7 found on the prototyping board.


   Configuration Instructions
   ==========================
   1. In the configuration section below, make changes to suit your
   	application as needed.


	Instructions
   ============
	1.	Compile and run this program.
   2  When accessing the administration page, note that the username is
      "foo" and the password is "bar".
   3. On the web page, the current temperature will be displayed.
      Choose "Change the temperature setting" to change temperature
      settings and email notification and notify interval.
   4. Press Submit and Return back to Current Temperature.
	5. Apply heat and cold air to the thermistor to observe change in
   	temperature.  An status message will be displayed in STDIO window
      when an email message is sent.


*******************************************************************************/
#class auto
#memmap xmem
// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

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

#define EMAILTO "you@somewhere.com"
#define EMAILFROM "tester@foo.bar"

/*
 *   The SMTP_SERVER macro tells DCRTCP where your mail server is.  This
 *   mail server MUST be configured to relay mail for your controller.
 *
 *   This value can be the name or the IP address.
 */

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
 *   The SMTP_DEBUG macro logs the communications between the mail
 *   server and your controller.  Uncomment this define to begin
 *   logging
 */

#define SMTP_DEBUG

/********************************
 * End of configuration section *
 ********************************/

/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1
#define USE_ZHTML 1

#use "dcrtcp.lib"
#use "http.lib"
#use "smtp.lib"

/*
 * This page contains the ZHTML portion of the selection variable demonstration
 */
#ximport "samples/rcm3700/tcpip/rabbitweb/pages/temperature.zhtml"	monitor_zhtml
#ximport "samples/rcm3700/tcpip/rabbitweb/pages/temp_admin.zhtml"	admin_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END


/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", monitor_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", monitor_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/admin/index.zhtml", admin_zhtml)
SSPEC_RESOURCETABLE_END

// The following line defines an "admin" group, which will be used to protect
// certain variables.  This must be defined before we register the variables
// below.
#web_groups admin

// The #web lines below register C variables with the web server, such that
// they can be used with the HTTP server enhancements.

// The following lines creates a "curtemperature" variable to keep the current
// temperature value, and allows read-only access to it.
float curtemperature;
#web curtemperature groups=all(ro)

// This creates minimum and maximum temperature alarm variables, which can be
// modified by the admin group, but is only readable by everybody else.
float maxtemp_alarm, mintemp_alarm;
#web mintemp_alarm (($mintemp_alarm>=-40.0) && ($mintemp_alarm<=70.0)) groups=all(ro),admin
#web maxtemp_alarm (($maxtemp_alarm>=-40.0) && ($maxtemp_alarm<=70.0)) groups=all(ro),admin

// The following two sets of registrations are for an alarm interval variable
// and an alarm email address.
int alarm_interval;
char alarm_email[50];
#web alarm_interval (($alarm_interval>0) && ($alarm_interval < 30000)) \
                    groups=all(ro),admin
#web alarm_email groups=all(ro),admin

#web debug


///////////////////////////////
// the next set of functions configures and sends
// email messages
///////////////////////////////

// The structure that holds an email message
typedef struct {
	char *from;
	char *to;
	char *subject;
	char *body;
} Email;
Email emailArray[2];	// holds the email messages


void init_emails(void)
{
	// Define emails here
	emailArray[0].from = EMAILFROM;
	emailArray[0].to = alarm_email;
	emailArray[0].subject = "Minimum temperature reached\0";
	emailArray[0].body = "Heater switching on in 60 seconds\0";

	emailArray[1].from = EMAILFROM;
	emailArray[1].to = alarm_email;
	emailArray[1].subject = "Maximum temperature reached\0";
	emailArray[1].body = "Heater switching off in 60 seconds\0";
}

void SendMail(int email)
{
	// Start sending the email
	smtp_sendmail(emailArray[email].to, emailArray[email].from,
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


///////////////////////////////
// getTemp() gets the raw analog input and converts the value
// to both Celcius and Fahrenheit.
///////////////////////////////

////constants
#define Tkstd  298.15			//25.0 + 273.15;	//convert to Kelvins
#define Bt 3965.0					//thermistor beta
#define Rs 1000.0					//series resistor
#define Rtstd 3000.0				//standard temp resistance
#define Dmax 2047.0				//max value on ADS7870
#define Gain 1.0					//actual gain multiplier

int getTemp(float *Tc, float *Tf)
{
	auto float  Tk,		//calculated temperature kelvins
					Draw;		//raw data value
   auto int i;

 	Draw = anaIn(7, SINGLE, GAIN_1);

	//calculate temperature in kelvins
	Tk = (Bt*Tkstd) / ( Tkstd * ( log( fabs( (-Draw*Rs) / (Rtstd*(Draw-(Dmax*Gain))) ) ) ) + Bt );

	*Tc = Tk - 273.15;				//convert to celcius
	*Tf = 1.8*(Tk - 255.37);		//calculate fahrenheit

	return 0;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void main(void)
{
	auto int i, userid;
   auto float tcelcius, tfahrenheit;

   // Initialize protoboard output
   brdInit();

	//first-time call of this function charges up the cap for 1 sec
	//use single-ended and gain of 1
 	anaIn(7, SINGLE, GAIN_1);

   //initialize email messages
	init_emails();

	// Initialize the global, #web-registered variables.
   mintemp_alarm =-20.0;
   maxtemp_alarm = 40.0;
	alarm_interval = 60;
	strcpy(alarm_email, EMAILTO);

	// Initialize the TCP/IP stack, HTTP server, and Ethernet-to-serial state
	// machine.
   sock_init();
   http_init();

   // If the browser specifies a directory (instead of a proper resource name)
   // default to using "index.zhtml" in that directory, if it exists.
   // If we don't use this function, the default is "index.html" which won't
   // work for this sample.
   http_set_path("/", "index.zhtml");

	tcp_reserveport(80);

	// The following line limits access to the "/admin" directory to the admin
	// group.  It also requires basic authentication for the "/admin"
	// directory.
   sspec_addrule("/admin", "Admin", admin, admin, SERVER_ANY,
                 SERVER_AUTH_BASIC, NULL);
	// The following two lines create an "admin" user and adds it to the admin
	// group.
   userid = sauth_adduser("foo", "bar", SERVER_ANY);
   sauth_setusermask(userid, admin, NULL);

   // get current temperature but use celcius parameter
	getTemp(&curtemperature, &tfahrenheit);

   while (1)
   {
		// Drive the HTTP server
      http_handler();

	   costate
      {
      	getTemp(&curtemperature, &tfahrenheit);
         waitfor(DelaySec(10));
      }

      costate
      {
			if (curtemperature <= mintemp_alarm)
         {
				SendMail(0);
	         waitfor(DelaySec(alarm_interval));
         }
			if (curtemperature >= maxtemp_alarm)
         {
			  	SendMail(1);
		      waitfor(DelaySec(alarm_interval));
         }
      }

   }//end while
}