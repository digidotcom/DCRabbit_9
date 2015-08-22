/*******************************************************************************
        INTEGRATION.C
        Rabbit, 2007

        ** NOTE ** This is the SECOND part of a 2-part sample. You will need
        to run the INTEGRATION_FAT_SETUP.C sample program before this program
        can run. See the section on the FAT Filesystem below for more
        information.

		  This sample program demonstrates the use of ZSERVER.LIB and FAT
		  filesystem functionality, with RabbitWeb dynamic HTML content,
		  all secured using SSL. This sample also supports dynamic updates
        of both the application and its resources using the Rabbit
        Download Manager (DLM) and HTTP upload capability, respectively
        (neither of these currently supports SSL security).

        This sample will only run on the RCM3300 family, RCM3700, RCM3720 and
        PowerCoreFLEX modules with serial flash.

        The DLM portion of this sample will only work on RCM3300 family boards
        that are equipped with on-board serial flash.

        ** NOTE **
        THIS IS AN ADVANCED SAMPLE THAT COMBINES MANY SOFTWARE ELEMENTS FROM
        VARIOUS MODULES. YOU WILL NEED TO HAVE ALL THE MODULES LISTED ABOVE,
        OR THIS SAMPLE WILL NOT COMPILE.

        Before running this sample, you should look at and run samples for
        ZServer, the FAT, RabbitWeb, SSL, the DLM, and HTTP upload to become
        familiar

        Upon running the sample (using Dynamic C or the DLM on an RCM 3300 to
        upload the program), the humidity monitor will be displayed. This
        page is accessible via plain HTTP or over SSL-secured HTTPS. Clicking
        on the administrator link will bring up the admin page, which is
        automatically secured using SSL and a username and password. The
        following are used for the username and password by default:
	        USERNAME: myadmin PASSWORD: myadmin
        The admin page demonstrates some RabbitWeb capabilities, and provides
        access to the HTTP upload page. Clicking the upload link will bring
        up the HTTP upload page, which allows you to choose new files for
        both the humidity monitor and the admin page. Your browser may prompt
        you again for your username and password, these are the same as above.

        Note that the upload page is a static page included in the program
        flash and can only be updated by recompiling and downloading the
        application. This is so that you cannot accidentally change the upload
        page, possibly restricting yourself from performing future updates.

        To try out rhe update capability, click the upload link on the admin
        page and choose a simple tezt file to replace monitor.ztm. Open another
        browser window and load the main page. You will see your text file has
        replaced the humidity monitor. To restore the monitor, go back to the
        other window, click back to go to the upload page again, and choose
        HUMIDITY_MONITOR.ZHTML to replace monitor.ztm and click Upload.
        Refreshing the other window will now show that the page has been
        restored. You have successfully updated and restored your application's
        files remotely!

        ** NOTE ** This sample, since it uses the FAT, has a special shutdown
        procedure that should be followed before powering off to prevent
        potential corruption of FAT files. This procedure involves pressing
        and holding a switch on the protoboard (S2 on the 3300, S1 on the 3700)
        for a specified number of seconds. Once the shutdown procedure has run,
        an LED will blink rapidly, indicating that it is now safe to turn the
        device off. This procedure can be modified by the user to provide any
        application-specific shutdown tasks desired.

        ** NOTE ** HTTP upload and SSL currently do not work together, so
        compiling this program will generate a serious warning. However, we
        are not using HTTP upload over SSL, so we can ignore the warning. For
        convenience, we have included a macro, HTTP_UPLOAD_SSL_SUPRESS_WARNING,
        which will supress the warning message.

        THE FOLLOWING SECTIONS DESCRIBE MODULE-SPECIFIC ISSUES

		  --== RabbitWeb ==--
        The basic sample is modified from a RabbitWeb sample based on a
        humidity monitor. It demonstrates some simple #web variable
        registration along with some authentication features.

        Note that the RabbitWeb ZHTML pages are stored in the 8.3 FAT format
		  with the extension .ZTM. See the pages "pages\humidity_monitor.zhtml"
        and "pages\humidity_admin.zhtml" for the corresponding ZHTML pages.

        --== FAT Filesystem ==--
		  ** NOTE ** This is the second of a 2-part sample.  The other part
        (run first) is INTEGRATION_FAT_SETUP.C, which will setup the
        appropriate files for this sample (copies #ximported files into
        the FAT).

        This sample uses the FAT filesystem for serving up the RabbitWeb
        pages. Using the FAT also allows us to remotely update these
        resources using the HTTP upload capability of the HTTP server.

        --== ZServer ==--
        All resources are accessed using ZServer functionality, which provides
        a friendly, uniform interface to file systems and resources. ZServer
        also provides authentication for protecting those resources.

        --== SSL ==--
		  This program uses SSL via HTTPS. See the SSL comments below for
        more information.

	     See the SSL Walkthrough for information on creating certificates
        for SSL programs.

        --== HTTP Upload ==--
		  This sample supports remote updating of resources (files) via the web.
        It uses the default HTTP upload CGI (HTTP.LIB) along with a custom
        upload page (pages/UPLOAD.HTML). The upload page is accessible from
        the admin page.

        --== DLM Support ==--
        If you are using an RCM33xx that's equipped with on-board serial flash,
        you can make this sample into a DLP for use with the Remote Application
        Update DLM. This is simply done by uncommenting a macro define below.
        Search for the comment DLP SPECIFIC CHANGE to see the portions of code
        added when DLP support is enabled.

*******************************************************************************/

#memmap xmem

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section are  *
 * customizable by the user.       *
 ***********************************/

/*
 *  This value is the number of seconds that the shutdown switch must be
 *  held to enter the maintenance shutdown mode (S2 on the 3300, S1 on the 3700)
 */
#define SHUTDOWN_DELAY 3

/*
 *  Uncomment the next line to add support to make this sample a DLP
 *  for use with the Rabbit Semiconductor Download Manager
 *  ** NOTE ** This will only work on an RCM3300
 */
//#define INTEGRATION_USE_DLM

/*
 *  Add this to supress the warning that HTTP upload and SSL will not
 *  work together. We are using SSL and HTTP upload in the same sample,
 *  but we are not using upload over SSL, so it is OK to ignore the
 *  warning generated by HTTP.LIB
 */
//#define HTTP_UPLOAD_SSL_SUPRESS_WARNING


 // Necessary for zserver and FAT to work together
#define FAT_USE_FORWARDSLASH
#define FAT_BLOCK

/*
 *  Pick the predefined TCP/IP configuration for this sample.  See
 *  LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 *  configuration.
 */
#define TCPCONFIG 1

/*
 *  DLP SPECIFIC CHANGE: Include both the Web and Telnet interfaces for the DLP.
 *  The Web interface can be reached by browsing to the reboot.html.
 */
#ifdef INTEGRATION_USE_DLM
	#define DLP_USE_HTTP_INTERFACE
#endif

/********************************
 * End of configuration section *
 ********************************/

#ifdef INTEGRATION_USE_DLM
 #if _BOARD_TYPE_ != RCM3300A && _BOARD_TYPE_ != RCM3310A && \
     _BOARD_TYPE_ != RCM3305 && _BOARD_TYPE_ != RCM3315 && \
     _BOARD_TYPE_ != RCM3309 && _BOARD_TYPE_ != RCM3319 && \
     _BOARD_TYPE_ != RCM3350A
	#error "The DLM requires an RCM33xx board with on-board serial flash."
 #endif
#endif

/*
 *  Board-specific defines
 */
#define ON 1
#define OFF 0

#if (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || \
    _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
	#use rcm33xx.lib     //sample library to use with this application
	#define DS3 3
	#define SWITCH2  2
#elif _BOARD_TYPE_ == RCM3700A || _BOARD_TYPE_ == RCM3700H || \
      _BOARD_TYPE_ == RCM3720A || _BOARD_TYPE_ == RCM3720H || \
      _BOARD_TYPE_ == RCM3750 || _BOARD_TYPE_ == RCM3760 || \
      _BOARD_TYPE_ == RCM3700LP
	#define DS2 7		 //led, port F bit 7
	#define SWITCH1  4 //switch, port F bit 4
#elif POWERCOREFLEX_SERIES
 #if _DF_TYPE_SER
	#use PowerCoreFLEX.lib     //sample library to use with this application
	#define DS3 1
	#define SWITCH2  2
 #else
	// PowerCoreFLEX modules are configurable and may not have serial flash
	#fatal "This PowerCoreFLEX Module does not have serial flash."
 #endif
#else
	// This program is so large that it will only run on boards with 512K flash
   // and 512K RAM, with a 3000A for SSL support, and a serial flash
	#error "Must use one of an RCM 3300, 3305, 3309, 3310, 3315, 3319, 3350,"
	#error " 3700, 3720, 3750, 3760 or PowerCoreFlex (with serial flash) for the"
	#fatal " integration sample (the 3710 does not have enough flash or RAM)."
#endif


/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1

/*
 * This is needed to be able to use HTTP Upload capability
 */
#define USE_HTTP_UPLOAD

/*
 * only one socket and server are needed for a reserved port,
 * but to use HTTP and HTTPS on the same device, 2 servers
 * are needed, one for HTTP and one for HTTPS. To use only
 * HTTPS, change HTTP_MAXSERVERS to match HTTP_SSL_SOCKETS.
 */
#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2

/*
 *  This macro determines the number of HTTP servers that will use
 *  SSL (HTTPS servers). In this case, we have 2 total servers, and
 *  this defines one of them to be HTTPS
 */
#define HTTP_SSL_SOCKETS 1

/*
 * SSL Stuff
 * This macro tells the HTTP library to use SSL
 */
#define USE_HTTP_SSL

/*
 * Import the certificate (use default of SSL_CERTIFICATE, used automatically
 * by the HTTP library, no API calls necessary)
 */
#ximport "cert\mycert.dcc" SSL_CERTIFICATE

/*
 *  Define this because we are using a static rule table.  Flash rules, OK?
 */
#define SSPEC_FLASHRULES

/*
 *  Include our libraries
 */
#use "dcrtcp.lib"
#use "fat.lib"		// Must use FAT before http/zserver
#use "http.lib"

#ifdef INTEGRATION_USE_DLM
// DLP SPECIFIC CHANGE: explicity use the DLP library
#use "remoteupload.lib"
#endif

/*
 *  For file upload, include the upload page (accessed from the Admin page)
 *  This file is not stored in the FAT because it is a special file. Uploading
 *  a new upload.html could potentially result in an inability to upload
 *  *anything*. By including it in the application itself, we can guarantee that
 *  it can only be changed by changing the entire application (and possibly
 *  the associated CGI, which would be needed if upload.html is modified).
 */
#ximport "pages/upload.html" upload_html

// The default mime type for '/' must be first
// FAT only allows 3-character extensions, so for ZHTML, use "ztm"
SSPEC_MIMETABLE_START
   // This handler enables the ZHTML parser to be used on ZHTML files...
	SSPEC_MIME_FUNC(".ztm", "text/html", zhtml_handler),
	SSPEC_MIME(".htm", "text/html"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END

// Define groups for permissions
#define WORLD_GROUP  0xFFFF // Everyone

// The following line defines an "admin" group, which will be used to protect
// certain variables and resources.  This must be defined before we register
// the variables below. This group is also used to protect upload resources in
// the access permissons "rule" table below.
#web_groups admin

// This is the access permissions "rule" table.  It associates userid
// information with files in the FAT filesystem.  This is necessary because
// FAT filesystems do not support the concept of owner userids.  Basically,
// this is a simple prefix-matching algorithm.
SSPEC_RULETABLE_START
	// You need to be in user group "admin" to write resources in the "/A/admin/"
   // directory, and any files starting with "monitor", i.e. monitor.ztm. The
   // 2nd parameter is used as the resource realm, which your browser will use
   // to prompt you for a userid and password.  Only the servers specified can
   // access the resources (both HTTP and HTTPS can access "monitor" but only
   // HTTPS servers can access the "/A/admin/" directory, for now, however, we
   // need to allow access through the HTTP server for HTTP upload to work on
   // the admin page). Basic (i.e. plaintext, but secured using HTTPS)
   // user/password authentication is specified for access. Everyone in the
   // WORLD_GROUP (0xFFFF) can read these pages, but only users in the "admin"
   // group can write them. Below we will set up an "anonymous" user so that a
   // username and password are not required for read access to certain files.
	SSPEC_MM_RULE("/A/monitor", "MonitorPages", WORLD_GROUP, admin,
                 SERVER_HTTP | SERVER_HTTPS, SERVER_AUTH_NONE, NULL),
	SSPEC_MM_RULE("/A/admin/", "AdminPages", admin, admin,
                 SERVER_HTTP | SERVER_HTTPS, SERVER_AUTH_BASIC, NULL)
SSPEC_RULETABLE_END

// The flash resource table is now initialized with these macros...
SSPEC_RESOURCETABLE_START
// "/upload.html" - This is the upload page. Note that it is the only static
// resource (page) used by this application. All other resources are stored
// in the FAT so that they can be updated.
SSPEC_RESOURCE_XMEMFILE("/upload.htm", upload_html),
// "upload.cgi" - resource name as specified in the HTML for handling the POST
//                data
// http_defaultCGI - name of the default upload CGI handler in HTTP.LIB
// "AdminPages" - the realm associated with upload.cgi
// admin - read permission must be limited to this group.  The CGI itself
//         only needs read permission, but it should be the same group
//         bits as specified for the write permission for the target
//         resource files (see ruletable entry above).
// 0x0000 - no write permission.  Writing to a CGI does not make sense, only
//          writing to files.
// SERVER_HTTP - only the HTTP server can access the CGI. (HTTP upload is not
//					  currently compatible with SSL, or this would be SERVER_HTTPS
// 				  for SSL security)
// SERVER_AUTH_BASIC - should use the same authentication technique as for
//                     the files.
SSPEC_RESOURCE_P_CGI("upload.cgi", http_defaultCGI, "AdminPages", admin, 0x0000,
                     SERVER_HTTP, SERVER_AUTH_BASIC)
SSPEC_RESOURCETABLE_END

// The #web lines below register C variables with the web server, such that
// they can be used with RabbitWeb.

// The following lines creates a "hum" variable to keep the current humidity
// value, and allows read-only access to it.
int hum;
#web hum groups=all(ro)

// The following are strings representing the respective addresses for the
// HTTP (port 80) and HTTPS (port 443) addresses. These will be inserted into
// the pages to dynamically create links. These are normally the same, but we
// provide this in case you have a situation (such as the device being behind
// a proxy) where the addresses for HTTP and HTTPS may differ. These may be
// either the IP address or Fully Qualified Domain Name of the device. See
// the accompanying ZHTML files to see where these variables are used.
char http_addr[50];
char https_addr[50];
#web http_addr groups=all(ro)
#web https_addr groups=all(ro)

// This creates a humidity alarm variable, which can be modified by the admin
// group, but is only readable by everybody else.
int hum_alarm;
#web hum_alarm ((0 < $hum_alarm) && ($hum_alarm <= 100)) groups=all(ro),admin

// The following two sets of registrations are for an alarm interval variable
// and an alarm email address.
int alarm_interval;
char alarm_email[50];
#web alarm_interval ((0 < $alarm_interval) && ($alarm_interval < 30000)) \
                    groups=all(ro),admin
#web alarm_email groups=all(ro),admin

void shutdown(void);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// M A I N /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
   auto int userid, anon, uid;
   auto long length;
   auto int rc;
	auto int sw1, sw2, led1, led2;
	auto long shutdown_time;
   auto char sd_timer_running;

	brdInit();				 //initialize board for this demo

	led1 = led2 = OFF;	 //initialize leds to off value
	sw1 = sw2 = 0;			 //initialize switches to false value
   sd_timer_running = 0; // Flag for checking shutdown delay

	// Initialize the values
	hum = 50;
	hum_alarm = 75;
	alarm_interval = 60;
	strcpy(alarm_email, "somebody@nowhere.org");

	// Initialize the TCP/IP stack and HTTP server (and SSL)
   sock_init();

   // Mount all appropriate file systems
   rc = sspec_automount(SSPEC_MOUNT_ANY, NULL, NULL, NULL);
   if (rc) {
   	printf("Failed to initialize, rc=%d\nProceeding anyway...\n", rc);
   }

   // Initialize HTTP server and reserve the HTTP (80) and HTTPS (443) ports
   http_init();
	tcp_reserveport(80);
	tcp_reserveport(443);

#ifdef INTEGRATION_USE_DLM
   // DLP SPECIFIC CHANGE: The DLP must be initialized in remoteupload.lib
   dlp_init();
#endif

   // If the browser specifies a directory (instead of a proper resource name)
   // default to using "montior.ztm" in that directory, if it exists.
   // If we don't use this function, the default is "index.html" which won't
   // work for this sample.
   http_set_path("/", "A/monitor.ztm");

	// The following line limits access to the "/admin" directory to the admin
	// group.  It also requires basic authentication for read access to the
   // "/admin" directory.
   sspec_addrule("/admin", "AdminPages", admin, admin, SERVER_ANY,
                 SERVER_AUTH_BASIC, NULL);

	// The following creates an "admin" user and adds it to the admin group.
   // Administrator resources are also protected using SSL over HTTPS so that
   // admin passwords are not sent plaintext, and all transactions on admin
   // pages (except for uploads - for now) are also protected using HTTPS.
   userid = sauth_adduser("myadmin", "myadmin", SERVER_ANY);
   if(userid < 0) {
   	printf("Failed to create admin userid, rc=%d\nProceeding anyway...\n", rc);
   }
   else {
   	// assign user mask and individual write access.
	   sauth_setusermask(userid, admin, NULL);
	   sauth_setwriteaccess(userid, SERVER_HTTPS | SERVER_HTTP);
	}

	// The following creates an "anonymous" user for read access to all publicly
   // available resources. Note the bitmask in sauth_setusermask of
   // WORLD_GROUP & (!admin), which indicates anonymous access for all resources
   // in groups *other than* admin. The anonymous user can choose to user either
   // plain HTTP or SSL-secured HTTPS to view pages.
	anon = sauth_adduser("anonymous", "", SERVER_HTTP | SERVER_HTTPS);
   if(anon < 0) {
   	printf("Failed to create anonymous userid, rc=%d\nProceeding anyway...\n", rc);
   }
   else {
   	// Setup anonymous read access for all resources in groups other than
      // the "admin" group
	   sauth_setusermask(anon, WORLD_GROUP & (!admin), NULL);
   	http_set_anonymous(anon);
   }

   // Set up our address for use by the ZHTML scripts in our pages
   // (These may be changed to reflect your specific setup)
   inet_ntoa(http_addr, MY_ADDR(IF_DEFAULT));
   inet_ntoa(https_addr, MY_ADDR(IF_DEFAULT));

	// This drives the HTTP server, the shutdown monitor, and (optionally)
   // the DLP handler
   while(1) {
   	// HTTP handler costate
		costate {
	     http_handler();
      }

      // Shutdown monitor costate
		costate {
      	//wait for switch press
#if (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || POWERCOREFLEX_SERIES || \
    _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
			if (switchIn(SWITCH2))
#else // 3700
			if (BitRdPortI(PFDR, SWITCH1))
#endif
			{
	 			abort;
         }

         // See if we need to set the shutdown timer
         if(!sd_timer_running) {
         	// Only set shutdown time if timer is not yet running
				shutdown_time = SEC_TIMER + SHUTDOWN_DELAY;

            // Timer is running
            sd_timer_running = 1;
         }

			// Switch press detected if got to here, wait a second (or 25 ms)
			waitfor(DelayMs(25));

         // If we have waited long enough, shut it down
         if((long)(SEC_TIMER - shutdown_time) > 0) {
	         shutdown();
         }

			// See if we get a switch release, the user backed out (released the
         // button)
#if (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || POWERCOREFLEX_SERIES || \
    _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
			if (switchIn(SWITCH2))
#else // 3700
			if (BitRdPortI(PFDR, SWITCH1))
#endif
			{
            sw1 = !sw1;				 // set valid switch
            sd_timer_running = 0; // Reset flag

				abort;
			}
		}

#ifdef INTEGRATION_USE_DLM
  		costate {
	      // DLP SPECIFIC CHANGE: call the DLP handler.
   	   dlp_handler();
      }
#endif
   }
}

////////////////////////////////////////////////////////////////////////////////
/// provides milli-second delay for blinking lights
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

////////////////////////////////////////////////////////////////////////////////
// Shutdown routine which should be called before the Rabbit device is
// powered down. This is specifically so we can unmount the FAT flash
// filesystem to prevent possible corruption. Any other user shutdown
// tasks (such as writing variables to non-volitile memory) can be
// placed here as well
void shutdown(void) {

	auto int i;
   auto fat_part* part;
   auto int led1;

   led1 = OFF;

   // User shutdown routines can be called here

   // Cycle through all partitions and unmount as needed
   for(i = 0; i < SSPEC_MAX_PARTITIONS; i++) {
   	// See if the partition is registered
	   part = sspec_fatregistered(i);
    	if(part) {
      	// The partition was registered, lets unmount it
			fat_UnmountDevice(part->dev);
      }
   }

   // All done, infinite loop with blinking LED to show we are done
   while(1) {
#if (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || POWERCOREFLEX_SERIES || \
    _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
		ledOut(DS3, led1=led1?0:1);
#else // 3700
		BitWrPortI(PFDR, &PFDRShadow, led1=led1?0:1, DS2);
#endif

      msDelay(100);
	}
}