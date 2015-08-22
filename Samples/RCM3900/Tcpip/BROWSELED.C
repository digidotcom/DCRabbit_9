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
   browseled.c

   This sample program is used with RCM3900 series controllers and RCM3300
   prototyping boards.

   Description
   ===========
   This sample program demonstrates a basic controller running a web page.  Two
   "device LEDs" are created with two buttons to toggle them.  Users can browse
   to the controller's web page and change the status of the lights by clicking
   on the buttons.  The LEDs on the prototyping	board are updated to match the
   ones on the web page.

   This sample program is adapted from ...\Samples\tcpip\ssi.c.

   Instructions
   ============
   1. If necessary, make changes below in the configuration section to match
      your requirements.

   2. Compile and run this program.

   3. Using your PC's web browser, access the web page running on the
      controller.

   4. View the LEDS on the web page and on the prototyping board, DS3 and DS4,
      to see that they match up when changing them via clicking the web page's
      control buttons.

   5. Click the "source code" link on the controller's web page to see this
      sample program's #ximported file content.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6
#define USERLED 0
#define ON 1
#define OFF 0

/***********************************
 * Configuration Section           *
 * ---------------------           *
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
 * Define the number of HTTP servers and socket buffers.
 * With tcp_reserveport(), fewer HTTP servers are needed.
 */
#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2

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
#define REDIRECTTO myurl()

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  Notice that we have ximported in the source code for
 *  this program.  This allows us to <!--#include file="ssi.c"-->
 *  in the pages/showsrc.shtml.
 *
 */

#ximport "pages/browseled.shtml" index_html
#ximport "pages/rabbit1.gif"     rabbit1_gif
#ximport "pages/ledon.gif"       ledon_gif
#ximport "pages/ledoff.gif"      ledoff_gif
#ximport "pages/button.gif"      button_gif
#ximport "pages/showsrc.shtml"   showsrc_shtml
#ximport "browseled.c"           browseled_c

/*
 *  In this case the .html is not the first type in the
 *  type table.  This causes the default (no extension)
 *  to assume the shtml_handler.
 *
 */
/* the default for / must be first */
SSPEC_MIMETABLE_START
   SSPEC_MIME_FUNC( ".shtml", "text/html", shtml_handler), // ssi
   SSPEC_MIME( ".html", "text/html"),                      // html
   SSPEC_MIME( ".cgi", ""),                                // cgi
   SSPEC_MIME( ".gif", "image/gif")
SSPEC_MIMETABLE_END

/*
 *  Each ledx contains a text string that is either
 *  "ledon.gif" or "ledoff.gif"  This string is toggled
 *  each time the ledxtoggle.cgi is requested from the
 *  browser.
 *
 */
char led1[15];
char led2[15];

/*
 *  Instead of sending other text back from the cgi's
 *  we have decided to redirect them to the original page.
 *  the cgi_redirectto forms a header which will redirect
 *  the browser back to the main page.
 *
 */
char *myurl(void)
{
   static char URL[64];
   auto char tmpstr[32];
   auto long ipval;

   ifconfig(IF_DEFAULT, IFG_IPADDR, &ipval, IFS_END);
   sprintf(URL, "http://%s/index.shtml", inet_ntoa(tmpstr, ipval));
   return URL;
}

int led1toggle(HttpState* state)
{
   if (strcmp(led1, "ledon.gif") == 0) {
      strcpy(led1, "ledoff.gif");
   } else {
      strcpy(led1, "ledon.gif");
   }
   cgi_redirectto(state, REDIRECTTO);
   return 0;
}

int led2toggle(HttpState* state)
{
   if (strcmp(led2, "ledon.gif") == 0) {
      strcpy(led2, "ledoff.gif");
   } else {
      strcpy(led2, "ledon.gif");
   }
   cgi_redirectto(state, REDIRECTTO);
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

   SSPEC_RESOURCE_XMEMFILE("browseled.c", browseled_c),

   SSPEC_RESOURCE_ROOTVAR("led1", led1, PTR16, "%s"),
   SSPEC_RESOURCE_ROOTVAR("led2", led2, PTR16, "%s"),

   SSPEC_RESOURCE_FUNCTION("/led1tog.cgi", led1toggle),
   SSPEC_RESOURCE_FUNCTION("/led2tog.cgi", led2toggle)
SSPEC_RESOURCETABLE_END

void update_outputs(void)
{
   /* update O0 */
   if (strcmp(led1, "ledon.gif")) {
      ledOut(DS3, OFF);
   } else {
      ledOut(DS3, ON);
   }
   /* update O1 */
   if (strcmp(led2, "ledon.gif")) {
      ledOut(DS4, OFF);
   } else {
      ledOut(DS4, ON);
   }
}

void main(void)
{
   // it's just good practice to initialize Rabbit's board-specific I/O
   brdInit();

   strcpy(led1, "ledon.gif");
   strcpy(led2, "ledoff.gif");

   sock_init();
   http_init();
   tcp_reserveport(80);

   while (1) {
      update_outputs();
      http_handler();
   }
}

