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
	sprinkler.c

	The following program is used with RCM3700 series controllers with
	prototyping boards.

   Description
   ===========
	This program demonstrates a basic example using the enhanced RabbitWeb
   HTTP library and web interface to schedule times for the prototyping
   board and two digital outputs in a 24-hour period. The next step to
   enhance this program would be to add the ability to add and remove
   stations. The ability to retain schedule settings across power loss
   would be another improvement.

   The web page has the following layout:

   Station 1 is connected JP4-1
   Station 2 is connected JP4-2

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
      The RTC can be set from the browser also
   3. LED's such as the Z-World Demo Board can be connected to the
   	two outputs, JP4-1 and JP4-2 to view on and off intervals.

*******************************************************************************/

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

#memmap xmem

// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

/********************************
 * End of configuration section *
 ********************************/

/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1


#use "dcrtcp.lib"
#use "http.lib"

// This page contains the ZHTML portion of the selection variable demonstration
#ximport "/pages/sprinkler.zhtml"	sprinkler_zhtml

// The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END

// Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", sprinkler_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", sprinkler_zhtml)
SSPEC_RESOURCETABLE_END


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
struct tm  rtm;			// time struct
int setclockmode;
void settime();
int RTChour, RTCmin;

// #web statements
#web_groups user,admin

/*
 * Selection variables to be registered.  Note that you can use watch
 * expressions or the evaluate expression feature during runtime to ensure that
 * the variables have properly changed values.
 */
#web 	station[0].houron[@]  (($station[0].houron[@] >= 0) && ($station[0].houron[@] <24))
#web 	station[0].houroff[@]  (($station[0].houroff[@] >= 0) && ($station[0].houroff[@] <24))
#web 	station[0].minuteon[@]  (($station[0].minuteon[@] >= 0) && ($station[0].minuteon[@] <60))
#web 	station[0].minuteoff[@]  (($station[0].minuteoff[@] >= 0) && ($station[0].minuteoff[@] <60))
#web 	station[1].houron[@]  (($station[1].houron[@] >= 0) && ($station[1].houron[@] <24))
#web 	station[1].houroff[@]  (($station[1].houroff[@] >= 0) && ($station[1].houroff[@] <24))
#web 	station[1].minuteon[@]  (($station[1].minuteon[@] >= 0) && ($station[1].minuteon[@] <60))
#web 	station[1].minuteoff[@]  (($station[1].minuteoff[@] >= 0) && ($station[1].minuteoff[@] <60))
#web 	RTChour (($RTChour >= 0) && ($RTChour <24))
#web 	RTCmin  (($RTCmin >= 0) &&  ($RTCmin <60))
#web currenttime
#web setclockmode

// settime is callback function called when seclockmode changes
#web_update setclockmode settime

#define CONFIG		0x06	   //configure JP4-1 and 2 as outputs

void gettime()
{
	// read current date/time
	mktm(&rtm, read_rtc());  // load tm structure
	sprintf(currenttime,"%02d:%02d", rtm.tm_hour, rtm.tm_min);
   RTChour = (int)rtm.tm_hour;
   RTCmin = (int)rtm.tm_min;
}

void settime()
{
   // call back function called when setclockmode changed browser
	if(setclockmode==2){
     setclockmode = 0;
  	  rtm.tm_hour = (char)RTChour;
	  rtm.tm_min =  (char)RTCmin;
	  write_rtc(mktime(&rtm));
	  sprintf(currenttime,"%02d:%02d", rtm.tm_hour, rtm.tm_min);
   }
}

void main(void)
{
	auto int i;

   // Initialize protoboard output
   brdInit();
	//intialize output values
   digOut(1,0);			//output low
   digOut(2,0);      	//output low
	digConfig(CONFIG);  	//configure
   setclockmode = 0;

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
   }

	// Initialize the TCP/IP stack, HTTP server
   sock_init();
   http_init();

   gettime();  // init. time display

   while (1)
   {
		// Drive the HTTP server
      http_handler();

	   costate
      {
      	gettime();
         waitfor(DelaySec(30));
      }

      costate
      {
         // Check station 1 zones
			for (i=0; i<3;i++)
         {
				if ((station[0].minuteon[i] != 0) || (station[0].houron[i] != 0))
            {
					if ((station[0].minuteon[i] == rtm.tm_min) && (station[0].houron[i] == rtm.tm_hour))
	  	         {
               	digOut(1, 0);
     		      }
					if ((station[0].minuteoff[i] == rtm.tm_min) && (station[0].houroff[i] == rtm.tm_hour))
 	         	{
               	digOut(1, 1);
   	  	      }
            }
         }
      }

      costate
      {	// Check station 2 zones
			for (i=0; i<3;i++)
         {
				if ((station[1].minuteon[i] != 0) || (station[1].houron[i] != 0))
            {
					if ((station[1].minuteon[i] == rtm.tm_min) && (station[1].houron[i] == rtm.tm_hour))
	  	         {
               	digOut(2, 0);
      	      }
					if ((station[1].minuteoff[i] == rtm.tm_min) && (station[1].houroff[i] == rtm.tm_hour))
 	         	{
               	digOut(2, 1);
      	      }
            }
         }
      }

   }//end while
}