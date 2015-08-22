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
   blinkleds.c

   This sample program is used with RCM3900 series controllers and RCM3300
   prototyping boards.

   Description
   ===========
   This sample program demonstrates a very basic development example using the
   enhanced RabbitWeb HTTP library and web interface to change the rate that the
   LEDs blink on the prototyping board.

   Instructions
   =============
   1. If necessary, make changes below in the configuration section to match
      your requirements.

   2. Compile and run this program.

   3. Observe the LEDs on the prototyping board blinking at different rates.

   4. Using your PC's web browser, access the web page running on the
      controller.

   5. Via the web browser, enter different frequencies and then press the Submit
      button; observe the LEDs blink rate change.
*******************************************************************************/

/*
 * 	By default, have compiler make function variables storage class
 * 	"auto" (allocated on the stack).
 */
#class auto
#use rcm39xx.lib	// sample library to use with this sample program

/*
 *  memmap forces the code into xmem.  Since the typical stack is larger
 *  than the root memory, this is commonly a desirable setting.  Another
 *  option is to do #memmap anymem 8096 which will force code to xmem when
 *  the compiler notices that it is generating within 8096 bytes of the
 *  end.
 */
#memmap xmem

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
 *  Uncomment the following HTTP_DEBUG macro definition to enable debugging of
 *  the HTTP support library code.
 */
//#define HTTP_DEBUG

#define USE_HTTP_BASIC_AUTHENTICATION 0
#define USE_HTTP_DIGEST_AUTHENTICATION 0

/*
 *  The TIMEZONE compiler setting gives the number of hours from
 *  local time to Greenwich Mean Time (GMT).  For pacific standard
 *  time this is -8.  Note:  for the time to be correct it must be set
 *  with tm_rd which is documented in the Dynamic C user manual.
 */

#define TIMEZONE        -8

/********************************
 * End of configuration section *
 ********************************/

/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1
#define USE_ZHTML 1

/*
 *  #use the Dynamic C TCP/IP stack library and the HTTP application library
 */
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  ximport is a Dynamic C language feature that takes the binary image
 *  of a file, places it in extended memory on the controller, and
 *  associates a symbol with the physical address on the controller of
 *  the image.
 *
 */
#ximport "pages/rabbit1.gif"    rabbit1_gif
#ximport "pages/blinkrate.zhtml"	blinkrate_zhtml

/*
 *  http_types gives the HTTP server hints about handling incoming
 *  requests.  The server compares the extension of the incoming
 *  request with the http_types list and returns the second field
 *  as the Content-Type field.  The third field defines a custom
 *  function to handle that mime type.
 *
 *  You can get a list of mime types from Netscape's browser in:
 *
 *  Edit->Preferences->Navigator->Applications
 *
 */
/* the default for / must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif")
SSPEC_MIMETABLE_END

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6

// blink rate in Hz
struct ledControl {
   unsigned int ledBlinkRate[4];
   enum {
      OFF = 0,
      ON = 1
   };
   byte ledStatus[4];
   unsigned long period[4];
} ledControl;

#web debug

#web_groups user,admin

#web ledControl.ledBlinkRate[@] (($ledControl.ledBlinkRate[@] > 0) && ($ledControl.ledBlinkRate[@] <= 35))

/*
 *  http_flashspec assocates the file image we brought in with ximport and
 *  associates it with its name on the webserver.  In this example the file
 *  "pages/blinkrate.zhtml" will be sent to the client when they request either
 *  <http://yoururl.com/> or <http://yoururl.com/index.zhtml>.
 *
 */
SSPEC_RESOURCETABLE_START
   SSPEC_RESOURCE_XMEMFILE("/", blinkrate_zhtml),
   SSPEC_RESOURCE_XMEMFILE("/index.zhtml", blinkrate_zhtml),
   SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif)
SSPEC_RESOURCETABLE_END

void updatefcn(void);

#web_update ledControl.ledBlinkRate[@] updatefcn

void updatefcn(void)
{
   ledControl.period[0] = 500 / ledControl.ledBlinkRate[0];
   ledControl.period[1] = 500 / ledControl.ledBlinkRate[1];
   ledControl.period[2] = 500 / ledControl.ledBlinkRate[2];
   ledControl.period[3] = 500 / ledControl.ledBlinkRate[3];
}

void main(void)
{
   // it's just good practice to initialize Rabbit's board-specific I/O
   brdInit();

   ledControl.ledBlinkRate[0] = 1;
   ledControl.ledBlinkRate[1] = 2;
   ledControl.ledBlinkRate[2] = 15;
   ledControl.ledBlinkRate[3] = 5;

   ledControl.ledStatus[0] = OFF;
   ledControl.ledStatus[1] = ON;
   ledControl.ledStatus[2] = OFF;
   ledControl.ledStatus[3] = ON;
   updatefcn();

   /*
    *  sock_init initializes the TCP/IP stack.
    *  http_init initializes the web server.
    */
   sock_init();
   http_init();

   /*
    *  tcp_reserveport causes the web server to ignore requests when there
    *  isn't an available socket (HTTP_MAXSERVERS are all serving index_html
    *  or rabbit1.gif).  This saves some memory, but can cause the client
    *  delays when retrieving pages.
    */
   tcp_reserveport(80);

   while (1) {
      /*
       *  http_handler needs to be called to handle the active web servers.
       */
      http_handler();

      costate {
         waitfor(DelayMs(ledControl.period[0]));
         ledOut(DS3, ledControl.ledStatus[0]);
         ledControl.ledStatus[0] = !ledControl.ledStatus[0];
      }

      costate {
         waitfor(DelayMs(ledControl.period[1]));
         ledOut(DS4, ledControl.ledStatus[1]);
         ledControl.ledStatus[1] = !ledControl.ledStatus[1];
      }

      costate {
         waitfor(DelayMs(ledControl.period[2]));
         ledOut(DS5, ledControl.ledStatus[2]);
         ledControl.ledStatus[2] = !ledControl.ledStatus[2];
      }

      costate {
         waitfor(DelayMs(ledControl.period[3]));
         ledOut(DS6, ledControl.ledStatus[3]);
         ledControl.ledStatus[3] = !ledControl.ledStatus[3];
      }
   }
}

