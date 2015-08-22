/*******************************************************************************
        Samples\TCPIP\HTTP\ssi.c
        Z-World, 2000

        A basic contoller. This creates four 'devices' (lights), and four
        buttons to toggle them. Users can browse to the device, and change
        the status of the lights.
*******************************************************************************/
#class auto

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
#define REDIRECTHOST		_PRIMARY_STATIC_IP
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

#ximport "samples/tcpip/http/pages/ssi.shtml"      index_html
#ximport "samples/tcpip/http/pages/rabbit1.gif"    rabbit1_gif
#ximport "samples/tcpip/http/pages/ledon.gif"      ledon_gif
#ximport "samples/tcpip/http/pages/ledoff.gif"     ledoff_gif
#ximport "samples/tcpip/http/pages/button.gif"     button_gif
#ximport "samples/tcpip/http/pages/showsrc.shtml"  showsrc_shtml
#ximport "samples/tcpip/http/ssi.c"                ssi_c

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
char led3[15];
char led4[15];

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

int led3toggle(HttpState* state)
{
   if (strcmp(led3,"ledon.gif")==0)
      strcpy(led3,"ledoff.gif");
   else
      strcpy(led3,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;
}

int led4toggle(HttpState* state)
{
   if (strcmp(led4,"ledon.gif")==0)
      strcpy(led4,"ledoff.gif");
   else
      strcpy(led4,"ledon.gif");

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
	SSPEC_RESOURCE_ROOTVAR("led1", led1, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led2", led2, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led3", led3, PTR16, "%s"),
	SSPEC_RESOURCE_ROOTVAR("led4", led4, PTR16, "%s"),
	SSPEC_RESOURCE_FUNCTION("/led1tog.cgi", led1toggle),
	SSPEC_RESOURCE_FUNCTION("/led2tog.cgi", led2toggle),
	SSPEC_RESOURCE_FUNCTION("/led3tog.cgi", led3toggle),
	SSPEC_RESOURCE_FUNCTION("/led4tog.cgi", led4toggle)
SSPEC_RESOURCETABLE_END


void main()
{
   strcpy(led1,"ledon.gif");
   strcpy(led2,"ledon.gif");
   strcpy(led3,"ledoff.gif");
   strcpy(led4,"ledon.gif");

   sock_init();
   http_init();
	tcp_reserveport(80);

   while (1) {
      http_handler();
   }
}

