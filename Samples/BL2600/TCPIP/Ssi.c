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

	ssi.c

   This program demonstrates using a WEB page to control LED's from the BL2600
   controller board. Users can browse to the controller, and change the status
   of the LEDs to match the indicators displayed on the WEB page.

   Connections:
	------------
	1. DEMO board jumper settings:
			- H1 remove all jumpers
			- H2 jumper pins 3-5
              jumper pins 4-6

	2. Connect a wire from the controller J12 GND, to the DEMO board
	   J1 GND.

	3. Connect a wire from the controller J12 +PWR to the DEMO board
	   J1 +K.

   4. Connect the following wires from the controller J1 to the DEMO
      board J1:

      From DIO00 to LED1
      From DIOO1 to LED2
      From DIO02 to LED3
      From DIO03 to LED4


   Program Instructions
   --------------------
   1. Compile and run this program.
   2. With your WEB browser access the WEB page running on the controller.
   3. View LEDS on Web page and the controller to see that they match-up
      when changing them via the WEB page control button.

*******************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


// MACROS for the LED's
#define ON     1
#define OFF    0

#define LED0   0
#define LED1   1
#define LED2   2
#define LED3   3


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
 * TCP/IP modification - reduce TCP socket buffer
 * size, to allow more connections. This can be increased,
 * with increased performance, if the number of sockets
 * are reduced.  Note that this buffer size is split in
 * two for TCP sockets--1024 bytes for send and 1024 bytes
 * for receive.
 */
#define TCP_BUF_SIZE 2048

/*
 * Web server configuration
 */

/*
 * only one socket and server are needed for a reserved port
 */
#define HTTP_MAXSERVERS 1
#define MAX_TCP_SOCKET_BUFFERS 1


/*
 * Our web server as seen from the clients.
 * This should be the address that the clients (netscape/IE)
 * use to access your server. Usually, this is your IP address.
 * If you are behind a firewall, though, it might be a port on
 * the proxy, that will be forwarded to the Rabbit board. The
 * commented out line is an example of such a situation.
 */

#define REDIRECTHOST  _PRIMARY_STATIC_IP
//#define REDIRECTHOST	"proxy.domain.com:1212"

/********************************
 * End of configuration section *
 ********************************/

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */

#define REDIRECTTO  "http://" REDIRECTHOST "/index.shtml"

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  Notice that we have ximported in the source code for
 *  this program.  This allows us to <!--#include file="ssi.c"-->
 *  in the pages/showsrc.shtml.
 *
 */

#ximport "samples/BL2600/tcpip/pages/ssi.shtml"       index_html
#ximport "samples/BL2600/tcpip/pages/rabbit1.gif"     rabbit1_gif
#ximport "samples/BL2600/tcpip/pages/ledon.gif"       ledon_gif
#ximport "samples/BL2600/tcpip/pages/ledoff.gif"      ledoff_gif
#ximport "samples/BL2600/tcpip/pages/button.gif"      button_gif
#ximport "samples/BL2600/tcpip/pages/showsrc.shtml"   showsrc_shtml
#ximport "samples/BL2600/tcpip/ssi.c"                 ssi_c

/*
 *  In this case the .html is not the first type in the
 *  type table.  This causes the default (no extension)
 *  to assume the shtml_handler.
 *
 */

/* the default for / must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END


/*
 *  Each ledx contains a text string that is either
 *  "ledon.gif" or "ledoff.gif"  This string is toggled
 *  each time the ledxtoggle.cgi is requested from the
 *  browser.
 *
 */

char led_LED0[15];
char led_LED1[15];
char led_LED2[15];
char led_LED3[15];

/*
 *  Instead of sending other text back from the cgi's
 *  we have decided to redirect them to the original page.
 *  the cgi_redirectto forms a header which will redirect
 *  the browser back to the main page.
 *
 */

int led_toggle0(HttpState* state)
{
   if (strcmp(led_LED0,"ledon.gif")==0)
   {
      strcpy(led_LED0,"ledoff.gif");
      digOut(0, 1);
   }
   else
   {
      strcpy(led_LED0,"ledon.gif");
      digOut(0, 0);
   }
   cgi_redirectto(state,REDIRECTTO);
   return 0;
}

int led_toggle1(HttpState* state)
{
   if (strcmp(led_LED1,"ledon.gif")==0)
   {
      strcpy(led_LED1,"ledoff.gif");
      digOut(1, 1);
   }
   else
   {
      strcpy(led_LED1,"ledon.gif");
      digOut(1, 0);
   }
   cgi_redirectto(state,REDIRECTTO);
   return 0;
}

int led_toggle2(HttpState* state)
{
   if (strcmp(led_LED2,"ledon.gif")==0)
   {
      strcpy(led_LED2,"ledoff.gif");
      digOut(2, 1);
   }
   else
   {
      strcpy(led_LED2,"ledon.gif");
      digOut(2, 0);
   }
   cgi_redirectto(state,REDIRECTTO);
   return 0;
}

int led_toggle3(HttpState* state)
{
   if (strcmp(led_LED3,"ledon.gif")==0)
   {
      strcpy(led_LED3,"ledoff.gif");
      digOut(3, 1);
   }
   else
   {
      strcpy(led_LED3,"ledon.gif");
      digOut(3, 0);
   }
   cgi_redirectto(state,REDIRECTTO);
   return 0;
}


SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", index_html),
	SSPEC_RESOURCE_XMEMFILE("/index.shtml", index_html),
	SSPEC_RESOURCE_XMEMFILE("/showsrc.shtml", showsrc_shtml),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif),
	SSPEC_RESOURCE_XMEMFILE("/ledon.gif", ledon_gif),
	SSPEC_RESOURCE_XMEMFILE("/ledoff.gif", ledoff_gif),
	SSPEC_RESOURCE_XMEMFILE("/button.gif", button_gif),
	SSPEC_RESOURCE_XMEMFILE("/ssi.c", ssi_c),
	SSPEC_RESOURCE_ROOTVAR("led_LED0", led_LED0, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led_LED1", led_LED1, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led_LED2", led_LED2, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led_LED3", led_LED3, PTR16, "%s"),
	SSPEC_RESOURCE_FUNCTION("/led_LED0.cgi", led_toggle0),
	SSPEC_RESOURCE_FUNCTION("/led_LED1.cgi", led_toggle1),
	SSPEC_RESOURCE_FUNCTION("/led_LED2.cgi", led_toggle2),
	SSPEC_RESOURCE_FUNCTION("/led_LED3.cgi", led_toggle3)
SSPEC_RESOURCETABLE_END



void main()
{
   // initialize the controller
   brdInit();	// Initialize the controller

   // Configure IO channels DIO00 - DIO03 as digital outputs (sinking type outputs)
   digOutConfig(0x000F);

   sock_init();
   http_init();
   tcp_reserveport(80);

   // set the initial state of the LED's
   strcpy(led_LED0,"ledon.gif");   digOut(0, 0);
   strcpy(led_LED1,"ledon.gif");   digOut(1, 0);
   strcpy(led_LED2,"ledon.gif");   digOut(2, 0);
   strcpy(led_LED3,"ledon.gif");   digOut(3, 0);

   // process WEB page requests and update the LED's
   while (1)
   {
      http_handler();
   }
}

