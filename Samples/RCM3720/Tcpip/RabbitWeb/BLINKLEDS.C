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

   The following programs are used with RCM3700 series controllers with
	prototyping boards.

   Description
   ===========
	This program demonstrates a very basic development example using the
	enhanced RabbitWeb HTTP library and web interface to change the rate
   that the LED's	blink on the prototyping board.

   Instructions
   =============
   1.  Configure IP address in TCPIP_CONFIG.LIB,
       Compile and run this program.
   2.  Observe the LED's on the prototyping board blink at different rates.
   3.  Open browser with address http://xxx.xxx.xxx.xxx where xxx.. is the
       IP address.
   4.  Enter different frequencies and press Submit button to observe rate
   	 change.

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

/*
 * This is needed to be able to use the HTTP enhancements and the ZHTML
 * scripting language.
 */
#define USE_RABBITWEB 1

// include libraries
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  ximport is a Dynamic C language feature that takes the binary image
 *  of a file, places it in extended memory on the controller, and
 *  associates a symbol with the physical address on the controller of
 *  the image.
 *
 */
#ximport "/pages/rabbit1.gif"       rabbit1_gif
#ximport "/pages/blinkrate.zhtml"	blinkrate_zhtml

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
 * the default for / must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif")
SSPEC_MIMETABLE_END

/*
 *  http_flashspec assocates the file image we brought in with ximport
 *  and associates it with its name on the webserver.  In this example
 *  the file "samples/http/pages/static.html" will be sent to the
 *  client when they request either "http://yoururl.com/" or
 *  "http://yoururl.com/index.html"
 *
 */

//blink rate in HZ
struct ledControl
{
	unsigned int ledBlinkRate[4];
   enum {
   	OFF = 0,
      ON = 1
   };
   byte ledStatus[4];
   unsigned long period[4];
}ledControl;

// define structure member as web variable
#web ledControl.ledBlinkRate[@] (($ledControl.ledBlinkRate[@] > 0) && \
         ($ledControl.ledBlinkRate[@] <= 35))

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", blinkrate_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", blinkrate_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif)
SSPEC_RESOURCETABLE_END


void updatefcn(void);

#web_update ledControl.ledBlinkRate[@] updatefcn

void updatefcn(void)
{
	ledControl.period[0] = 500/ledControl.ledBlinkRate[0];
	ledControl.period[1] = 500/ledControl.ledBlinkRate[1];
}


#define DS1 1			//led, port F bit 6
#define DS2 2			//led, port F bit 7

// output function to control protoboard LED's
void pbledOut(int channel, int onoff)
{
	// works for port F bits 6 and 7 only
	BitWrPortI(PFDR, &PFDRShadow, onoff, channel+5);
}

void main()
{
	float period;

	ledControl.ledBlinkRate[0] = 15;
	ledControl.ledBlinkRate[1] = 5;

   ledControl.ledStatus[0] = OFF;
	ledControl.ledStatus[1] = ON;
 	updatefcn();

   brdInit();				//initialize board for this demo

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
  	   // http_handler needs to be called to handle the active http servers.
      http_handler();

	  	costate
      {
      	waitfor(DelayMs(ledControl.period[0]));
	      pbledOut(DS1, ledControl.ledStatus[0]);
         ledControl.ledStatus[0] = !ledControl.ledStatus[0];
      }

	  	costate
      {
      	waitfor(DelayMs(ledControl.period[1]));
	      pbledOut(DS2, ledControl.ledStatus[1]);
         ledControl.ledStatus[1] = !ledControl.ledStatus[1];
       }
   }
}




