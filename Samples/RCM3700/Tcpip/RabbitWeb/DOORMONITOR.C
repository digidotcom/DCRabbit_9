/*******************************************************************************
	doormonitor.c
   Z-World, 2004

	The following programs are used with RCM3700 series controllers with
	prototyping boards.

	An optional LCD/Keypad Module is required to run this program.  brdInit()
	in rcm37xx.lib automatically enables external I/O bus for LCD/Keypad
	operations.

   Description
   ===========
	This program demonstrates a basic example using the enhanced RabbitWeb
   HTTP library and web interface to add/change passwords and names.
   Entry and exit times are monitored while passwords are entered on
   the LCD/Keypad.

   See doormonitor.zhtml page that demonstrates the corresponding ZHTML
   scripting features.

   Instructions
   ============
   0. This program assumes that real-time clock time has been previously
   	set on the controller.
   1. The keypad is configured as

           (  1  )(  2	)(	 3  )(  4  )
               (  -  )(  +  )(  0  )

	2.	Compile and run this program.
   3. On the LCD/Keypa, key-in 1 2 3 4 0.  Press "+" to enter.
   4. On the web page, click Update Monitor button.  You should see a new
   	date and time for the Last Entry.
   5. Enter a new password and owner name.  Click Submit Password button.
   6. On the LCD/Keypad, key-in the new password entry. Click Update Monitor
   	button on the web page.

*******************************************************************************/
#class auto
#define PORTA_AUX_IO		//required to run LCD/Keypad for this demo

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

/********************************
 * End of configuration section *
 ********************************/

/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1
#define USE_ZHTML 1

#memmap xmem

#use "dcrtcp.lib"
#use "http.lib"

/*
 * This page contains the ZHTML portion of the selection variable demonstration
 */
#ximport "samples/rcm3700/tcpip/rabbitweb/pages/doormonitor.zhtml"	doormonitor_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END


/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", doormonitor_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", doormonitor_zhtml)
SSPEC_RESOURCETABLE_END

#define MAX_ENTRIES 20

/*
 * Structure variable to be registered.  Note that you can use watch
 * expressions or the evaluate expression feature during runtime to ensure that
 * the variables have properly changed values.
 */

struct {
	char ownerpw[8];
   char ownername[21];
	char entrypw[8];
   char entryname[21];
	char entrytime[64];
	char exittime[64];
} passwords[MAX_ENTRIES];

#web passwords[@].entrypw
#web passwords[@].entryname
#web passwords[@].entrytime
#web passwords[@].exittime


void updateentry(void)
{
	auto int entries;

   for (entries=0; entries<MAX_ENTRIES; entries++)
   {
		strcpy(passwords[entries].ownerpw, passwords[entries].entrypw);
		strcpy(passwords[entries].ownername, passwords[entries].entryname);
   }
}

#web_update passwords[@].entrypw updateentry
#web_update passwords[@].entryname updateentry


void gettime(char *intime)
{
	struct tm		rtm;					// time struct
	unsigned long	t0;			// used for date->seconds conversion

	//////////////////////////////////////////////////
	// read current date/time

	t0 = read_rtc();							// read time in seconds since 1980
	mktm(&rtm, t0);

	sprintf(intime,"%02d/%02d/%04d %02d:%02d:%02d",
			rtm.tm_mon, rtm.tm_mday, 1900+rtm.tm_year,
			rtm.tm_hour, rtm.tm_min, rtm.tm_sec);
}

int confirmentry(char *inputpw, char entrytype)
{
	auto int i, passkey;

   for (i=0, passkey=0; i<MAX_ENTRIES; i++)
   {
		if (!strcmp(passwords[i].ownerpw, inputpw))
	   {
   		passkey = 1;
         strcpy(inputpw, passwords[i].ownername);
         if (entrytype == '+')
	         gettime(passwords[i].entrytime);
         if (entrytype == '-')
	         gettime(passwords[i].exittime);
         break;
	   }
   }

   return passkey;
}

///////////////////////////////////////////////////////////////////////////

fontInfo fi6x8;
windowFrame wholewindow;

void initLcdKeypad()
{
	// Start-up the keypad driver and
	// Initialize the graphic driver
	dispInit();

	// Use default key values along with a key release code
	keyConfig (  0,'1', 0, 0, 0,  0, 0 );
	keyConfig (  1,'2', 0, 0, 0,  0, 0 );
	keyConfig (  2,'3', 0, 0, 0,  0, 0 );
	keyConfig (  3,'4', 0, 0, 0,  0, 0 );
	keyConfig (  4,'-', 0, 0, 0,  0, 0 );
	keyConfig (  5,'+', 0, 0, 0,  0, 0 );
	keyConfig (  6,'0', 0, 0, 0,  0, 0 );

	// Initialize 6x8 font
	glXFontInit(&fi6x8, 6, 8, 32, 127, Font6x8);          // Initialize 6x8 font
	TextWindowFrame(&wholewindow, &fi6x8, 0, 0, LCD_XS, LCD_YS);
	glBlankScreen();
	TextGotoXY(&wholewindow, 0, 0);
	TextPrintf(&wholewindow, "Password");
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void main(void)
{
	auto int wKey;
   char buffer[20];

	auto int i,j;

	brdInit();
   initLcdKeypad();

	// Initialize the #web-registered variables
   strcpy(passwords[0].entrypw, "12340");
   strcpy(passwords[0].entryname, "JGunchy");
   strcpy(passwords[0].entrytime, "no activity");
   strcpy(passwords[0].exittime, "no activity");

   for (i=1; i<MAX_ENTRIES; i++)
   {
		strcpy(passwords[i].entrypw, "Unused");
		strcpy(passwords[i].entryname, "Unused");
	   strcpy(passwords[i].entrytime, "no activity");
   	strcpy(passwords[i].exittime, "no activity");
   }

   updateentry();
   //initialize temporary variables
   j= 10;
	i = 0;

	// Initialize the TCP/IP stack, HTTP server
   sock_init();
   http_init();

   while (1)
   {
		// Drive the HTTP server
      http_handler();

  		costate
		{
			keyProcess();
			wKey = keyGet();
		}

      costate
      {
         switch (wKey)
         {
         	case 0:	break;
            case '+':	//entry
            case '-':	//exit
            		buffer[i]='\0';
		  			  	TextGotoXY(&wholewindow, 0, 1);
         		  	if (confirmentry(buffer, wKey))
            		{
							TextPrintf(&wholewindow, "Accepted");
			  			  	TextGotoXY(&wholewindow, 0, 3);
							TextPrintf(&wholewindow, "Thank you %s", buffer);
            		}
            		else
            		{
							TextPrintf(&wholewindow, "Incorrect password");
            		}
            		i=0;
			        	j=10;
                  waitfor(DelaySec(3));
						glBlankScreen();
				  		TextGotoXY(&wholewindow, 0, 0);
						TextPrintf(&wholewindow, "Password");
                  break;
            default:
            		if (i<7)
                  {
				      	buffer[i] = wKey;
					  		TextGotoXY(&wholewindow, j, 0);
							//TextPrintf(&wholewindow, "%c", wKey);
							TextPrintf(&wholewindow, "%c", '*');
         	   		j++;
            			i++;
                  }
                  break;
         }//end switch
      }//end costate switch

   }
}







