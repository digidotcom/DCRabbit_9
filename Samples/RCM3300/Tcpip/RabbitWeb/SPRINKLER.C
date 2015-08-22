/*******************************************************************************
	sprinkler.c
   Z-World 2004

	The following program is used with RCM3300 series controllers with
	prototyping boards.

   Description
   ===========
	This program demonstrates a basic example using the enhanced RabbitWeb
   HTTP library and web interface to schedule times for the prototyping
   board relay and two digital outputs in a 24-hour period.

   The web page has the following layout:

   Station 1 is connected Relay
   Station 2 is connected to OUT00 on J10
   Station 3 is connected to OUT01 on J10

   Zone 1, 2 and 3 are watering areas where stations are turned on or off
   at different intervals.

   See sprinkler.zhtml page that demonstrates the corresponding ZHTML
   scripting features.

   Instructions
   ============
   0. This program assumes that real-time clock time has been previously
   	set on the controller.

	1.	Compile and run this program.
   2. On the web page, enter hour and minutes for the various zones
      and stations. Press the Submit button to update.
   3. LED's such as the Z-World Demo Board can be connected to the relay
   	and two outputs to view on and off intervals.

*******************************************************************************/
#class auto
#use rcm33xx.lib

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


#use "dcrtcp.lib"
#use "http.lib"

/*
 * This page contains the ZHTML portion of the selection variable demonstration
 */
#ximport "samples/rcm3300/tcpip/rabbitweb/pages/sprinkler.zhtml"	sprinkler_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END


/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", sprinkler_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", sprinkler_zhtml)
SSPEC_RESOURCETABLE_END

/*
 * Selection variables to be registered.  Note that you can use watch
 * expressions or the evaluate expression feature during runtime to ensure that
 * the variables have properly changed values.
 */

// This is an integer selection variable.
// This is a long selection variable.  Note that unsigned ints and longs can
// also be used.

struct stationControl
{
	//zone info
   int state[3];		//zone enable/disabled
	int houron[3];
	int houroff[3];
	int minuteon[3];
	int minuteoff[3];
};

struct stationControl station[3];

char currenttime[64];

struct tm		rtm;			// time struct
unsigned long	t0;			// used for date->seconds conversion

void gettime()
{
	//////////////////////////////////////////////////
	// read current date/time

	t0 = read_rtc();							// read time in seconds since 1980
	mktm(&rtm, t0);
/*
	sprintf(currenttime,"%02d/%02d/%04d %02d:%02d:%02d",
			rtm.tm_mon, rtm.tm_mday, 1900+rtm.tm_year,
			rtm.tm_hour, rtm.tm_min, rtm.tm_sec);
*/
	sprintf(currenttime,"%02d:%02d", rtm.tm_hour, rtm.tm_min);
}

/*
 * #web statements
 */

#web debug
#web_groups user,admin

// The following statement registers the global variable
#web 	station[0].houron[@]  (($station[0].houron[@] >= 0) && ($station[0].houron[@] <24))
#web 	station[0].houroff[@]  (($station[0].houroff[@] >= 0) && ($station[0].houroff[@] <24))
#web 	station[0].minuteon[@]  (($station[0].minuteon[@] >= 0) && ($station[0].minuteon[@] <60))
#web 	station[0].minuteoff[@]  (($station[0].minuteoff[@] >= 0) && ($station[0].minuteoff[@] <60))

#web 	station[1].houron[@]  (($station[1].houron[@] >= 0) && ($station[1].houron[@] <24))
#web 	station[1].houroff[@]  (($station[1].houroff[@] >= 0) && ($station[1].houroff[@] <24))
#web 	station[1].minuteon[@]  (($station[1].minuteon[@] >= 0) && ($station[1].minuteon[@] <60))
#web 	station[1].minuteoff[@]  (($station[1].minuteoff[@] >= 0) && ($station[1].minuteoff[@] <60))

#web 	station[2].houron[@]  (($station[2].houron[@] >= 0) && ($station[2].houron[@] <24))
#web 	station[2].houroff[@]  (($station[2].houroff[@] >= 0) && ($station[2].houroff[@] <24))
#web 	station[2].minuteon[@]  (($station[2].minuteon[@] >= 0) && ($station[2].minuteon[@] <60))
#web 	station[2].minuteoff[@]  (($station[2].minuteoff[@] >= 0) && ($station[2].minuteoff[@] <60))

#web currenttime

#web_update currenttime gettime

/////////////////////////////////////////////////////////

void main(void)
{
	auto int i;

   // Initialize protoboard output
   brdInit();
   relayOut(0,1);
   digOut(0,1);
   digOut(1,1);

	// Initialize the global, #web-registered variables.
   for (i=0; i<3; i++)
   {
   	station[0].houron[i] = 0;
      station[0].houroff[i] = 0;
      station[0].minuteon[i] = 0;
      station[0].minuteoff[i] = 0;
   	station[1].houron[i] = 0;
      station[1].houroff[i] = 0;
      station[1].minuteon[i] = 0;
      station[1].minuteoff[i] = 0;
   	station[2].houron[i] = 0;
      station[2].houroff[i] = 0;
      station[2].minuteon[i] = 0;
      station[2].minuteoff[i] = 0;
   }

	// Initialize the TCP/IP stack, HTTP server, and Ethernet-to-serial state
	// machine.
   sock_init();
   http_init();

   gettime();

   while (1)
   {
		// Drive the HTTP server
      http_handler();

	   costate
      {
      	gettime();
         waitfor(DelaySec(60));
      }

      costate
      {	//check zone 1 relay
			for (i=0; i<3;i++)
         {
				if ((station[0].minuteon[i] != 0) || (station[0].houron[i] != 0))
            {
					if ((station[0].minuteon[i] == rtm.tm_min) && (station[0].houron[i] == rtm.tm_hour))
	  	         {
   	           	relayOut(0, 0);  //ncc
     		      }
					if ((station[0].minuteoff[i] == rtm.tm_min) && (station[0].houroff[i] == rtm.tm_hour))
 	         	{
	              	relayOut(0, 1);  //noc
   	  	      }
            }
         }
      }

      costate
      {	//check zone 2 Out00
			for (i=0; i<3;i++)
         {
				if ((station[1].minuteon[i] != 0) || (station[1].houron[i] != 0))
            {
					if ((station[1].minuteon[i] == rtm.tm_min) && (station[1].houron[i] == rtm.tm_hour))
	  	         {
               	digOut(0, 0);  //OUT00 on
      	      }
					if ((station[1].minuteoff[i] == rtm.tm_min) && (station[1].houroff[i] == rtm.tm_hour))
 	         	{
               	digOut(0, 1);   //OUT00 off
      	      }
            }
         }
      }

      costate
      {	//check zone 3 Out01
			for (i=0; i<3;i++)
         {
				if ((station[2].minuteon[i] != 0) || (station[2].houron[i] != 0))
            {
					if ((station[2].minuteon[i] == rtm.tm_min) && (station[2].houron[i] == rtm.tm_hour))
	  	         {
               	digOut(1, 0);  //OUT01 on
      	      }
					if ((station[2].minuteoff[i] == rtm.tm_min) && (station[2].houroff[i] == rtm.tm_hour))
 	         	{
               	digOut(1, 1);  //OUT00 off
      	      }
            }
         }
      }

   }//end while
}