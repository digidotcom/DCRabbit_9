/**********************************************************
   browseled.c
   Z-World, 2003

	This program is used with RCM3700 series controllers
	with prototyping boards.

	Description
	===========
	This program demonstrates a basic controller running a
	WEB page. Two "device LED's" are created with two
	buttons to toggle	them.  Users can browse to the device
	and change the	status of the lights. The LED's on the
	prototyping	board will match the ones on the web page.

   This program is adapted from \Samples\TCPIP\ssi.c.

	Instructions
	============
	1. Make changes below in the configuration section to
	   match your application.
	2. Compile and run this program.
   3. With your WEB browser access the WEB page running on
   	the controller.
   4. View LEDS on Web page and the prototyping board to
   	see that	they match-up when changing them via the
   	WEB page	control button.

**********************************************************/
#class auto

#define DS1 0x40	//led, port F bit 6 bitmask
#define DS2 0x80	//led, port F bit 7 bitmask

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

/*
 * Our web server as seen from the clients.
 * This should be the address that the clients (netscape/IE)
 * use to access your server. Usually, this is your IP address.
 * If you are behind a firewall, though, it might be a port on
 * the proxy, that will be forwarded to the Rabbit board. The
 * commented out line is an example of such a situation.
 */
#define REDIRECTHOST		_PRIMARY_STATIC_IP
// #define REDIRECTHOST	"my.host.com:8080"

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

#define REDIRECTTO 		"http://" REDIRECTHOST "/index.shtml"

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  Notice that we have ximported in the source code for
 *  this program.  This allows us to <!--#include file="ssi.c"-->
 *  in the pages/showsrc.shtml.
 *
 */

#ximport "samples/rcm3700/tcpip/pages/browseled.shtml" 	index_html
#ximport "samples/rcm3700/tcpip/pages/rabbit1.gif"    	rabbit1_gif
#ximport "samples/rcm3700/tcpip/pages/ledon.gif"      	ledon_gif
#ximport "samples/rcm3700/tcpip/pages/ledoff.gif"     	ledoff_gif
#ximport "samples/rcm3700/tcpip/pages/button.gif"     	button_gif
#ximport "samples/rcm3700/tcpip/pages/showsrc.shtml"  	showsrc_shtml
#ximport "samples/rcm3700/tcpip/browseled.c"          	browseled_c

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

char led1[15];
char led2[15];

/*
 *  Instead of sending other text back from the cgi's
 *  we have decided to redirect them to the original page.
 *  the cgi_redirectto forms a header which will redirect
 *  the browser back to the main page.
 *
 */

int led1toggle(HttpState* state)
{
   if (strcmp(led1,"ledon.gif")==0)
      strcpy(led1,"ledoff.gif");
   else
      strcpy(led1,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;
}

int led2toggle(HttpState* state)
{
   if (strcmp(led2,"ledon.gif")==0)
      strcpy(led2,"ledoff.gif");
   else
      strcpy(led2,"ledon.gif");

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
	SSPEC_RESOURCE_XMEMFILE("browseled.c", browseled_c),
	SSPEC_RESOURCE_ROOTVAR("led1", led1, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led2", led2, PTR16, "%s"),
	SSPEC_RESOURCE_FUNCTION("/led1tog.cgi", led1toggle),
	SSPEC_RESOURCE_FUNCTION("/led2tog.cgi", led2toggle),
SSPEC_RESOURCETABLE_END


void update_outputs()
{
	auto int value;

	value=PFDRShadow&0x3F;   //on state for leds

	/* update O0 */
	if (strcmp(led1,"ledon.gif"))
		value|=DS1;

	/* update O1 */
	if (strcmp(led2,"ledon.gif"))
		value|=DS2;

	WrPortI(PFDR, &PFDRShadow, value);
}

main()
{

	brdInit();				//initialize board for this demo

   strcpy(led1,"ledon.gif");
   strcpy(led2,"ledoff.gif");

   sock_init();
   http_init();
   tcp_reserveport(80);

   while (1)
   {
   	update_outputs();
      http_handler();

	}
}

#nodebug