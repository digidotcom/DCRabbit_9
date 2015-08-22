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

   This sample program is used with RCM3900 series controllers and RCM3300
   prototyping boards.

   Description
   ===========
   This sample program demonstrates using the enhanced RabbitWeb HTTP library
   and web interface to schedule on and off times in a 24-hour period for the
   prototyping board's relay and two digital outputs.

   The web page has the following layout:
      Station 1 is connected to the Relay on J17.
      Station 2 is connected to OUT00 on J10.
      Station 3 is connected to OUT01 on J10.

   Zones 1, 2 and 3 are watering areas where stations are turned on or off at
   different times.

   See also the Pages/sprinkler.zhtml page that demonstrates the corresponding
   ZHTML scripting features.

   Instructions
   ============
   0. This program assumes that the controller's real-time clock time has been
      previously set.

   1. If necessary, make changes below in the configuration section to match
      your requirements.

   2. Compile and run this program.

   3. Using your PC's web browser, access the web page running on the
      controller.

   4. On the web page, enter hour and minutes for the various zones and
      stations, then press the Submit button to update.

   5. Note:  LEDs such as those on the Z-World Demo Board can be connected to
      the prototyping board's relay and two outputs to view the on and off
      intervals.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

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
 * Define APP_VERBOSE to 0 or nonzero to disable or enable STDIO output,
 * respectively.
 */
#define APP_VERBOSE 0

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
#ximport "pages/sprinkler.zhtml"	sprinkler_zhtml

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

struct stationControl {
   // 3 zones' on / off information per station
   int houron[3];
   int houroff[3];
   int minuteon[3];
   int minuteoff[3];
};

struct stationControl station[3];

char currenttime[64];
struct tm rtm;			// time struct
unsigned long	t0;	// used for date->seconds conversion

void gettime(void)
{
   //////////////////////////////////////////////////
   // read current date/time

   t0 = read_rtc();	// read time in seconds since 1980
   mktm(&rtm, t0);
/*
   sprintf(currenttime, "%02d/%02d/%04d %02d:%02d:%02d",
           rtm.tm_mon, rtm.tm_mday, 1900+rtm.tm_year,
           rtm.tm_hour, rtm.tm_min, rtm.tm_sec);
*/
   sprintf(currenttime, "%02d:%02d", rtm.tm_hour, rtm.tm_min);
}

/*
 * #web statements
 */
#web debug
#web_groups user,admin

// The following statement registers the global variable
#web station[0].houron[@] (($station[0].houron[@] >= 0) && ($station[0].houron[@] < 24))
#web station[0].houroff[@] (($station[0].houroff[@] >= 0) && ($station[0].houroff[@] < 24))
#web station[0].minuteon[@] (($station[0].minuteon[@] >= 0) && ($station[0].minuteon[@] < 60))
#web station[0].minuteoff[@] (($station[0].minuteoff[@] >= 0) && ($station[0].minuteoff[@] < 60))

#web station[1].houron[@] (($station[1].houron[@] >= 0) && ($station[1].houron[@] < 24))
#web station[1].houroff[@] (($station[1].houroff[@] >= 0) && ($station[1].houroff[@] < 24))
#web station[1].minuteon[@] (($station[1].minuteon[@] >= 0) && ($station[1].minuteon[@] < 60))
#web station[1].minuteoff[@] (($station[1].minuteoff[@] >= 0) && ($station[1].minuteoff[@] < 60))

#web station[2].houron[@] (($station[2].houron[@] >= 0) && ($station[2].houron[@] < 24))
#web station[2].houroff[@] (($station[2].houroff[@] >= 0) && ($station[2].houroff[@] < 24))
#web station[2].minuteon[@] (($station[2].minuteon[@] >= 0) && ($station[2].minuteon[@] < 60))
#web station[2].minuteoff[@] (($station[2].minuteoff[@] >= 0) && ($station[2].minuteoff[@] < 60))

#web currenttime
#web_update currenttime gettime

void main(void)
{
   static int i, j;

   // it's just good practice to initialize Rabbit's board-specific I/O
   brdInit();

   // Initialize our prototyping board outputs.
   relayOut(0, 0);
   digOut(0, 0);
   digOut(1, 0);

   // Initialize the global, #web-registered variables.
   for (j = 0; j < 3; ++j) {
      for (i = 0; i < 3; ++i) {
         station[j].houron[i] = 0;
         station[j].houroff[i] = 0;
         station[j].minuteon[i] = 0;
         station[j].minuteoff[i] = 0;
      }
   }

   // Initialize the TCP/IP stack, HTTP server, and Ethernet-to-serial state
   //  machine.
   sock_init();
   http_init();

   while (1) {
      // drive the HTTP server
      http_handler();

      costate {
         gettime();
         waitfor(IntervalSec(60L));
      }

      // check stations 1 (RELAY), 2 (OUT00) and 3 (OUT01)
      for (j = 0; j < 3; ++j) {
         // check each station's zones 1, 2 and 3
         for (i = 0; i < 3; ++i) {
            if (station[j].minuteon[i] || station[j].houron[i]) {
#if APP_VERBOSE
               printf("Station %d zone %d on at %02d:%02d.\n", j + 1, i + 1,
                      station[j].houron[i], station[j].minuteon[i]);
               if (station[j].minuteoff[i] || station[j].houroff[i]) {
                  printf("Station %d zone %d off at %02d:%02d.\n", j + 1, i + 1,
                         station[j].houroff[i], station[j].minuteoff[i]);
               }
#endif
               if ((station[j].minuteon[i] == rtm.tm_min) &&
                   (station[j].houron[i] == rtm.tm_hour))
               {
                  if (0 == j) {
                     relayOut(j, 1);	// RELAY NOx <--> COMx closed
                  } else {
                     digOut(j - 1, 1);	// OUT00 or OUT01 on (active high)
                  }
               }
               if ((station[j].minuteoff[i] == rtm.tm_min) &&
                   (station[j].houroff[i] == rtm.tm_hour))
               {
                  if (0 == j) {
                     relayOut(j, 0);	// RELAY NCx <--> COMx closed
                  } else {
                     digOut(j - 1, 0);	// OUT00 or OUT01 off (inactive low)
                  }
               }
            }
         }
      }

   }	// end while
}

