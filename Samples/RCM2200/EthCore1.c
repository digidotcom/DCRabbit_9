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

        RCM2200\EthCore1.c

        A basic contoller. This creates two 'devices' (lights), and two
        buttons to toggle them. Users can browse to the device, and change
        the status of the lights.  If you have the core plugged into the
        development board in the MASTER slot, the lights on the board will
        track the ones on the web page.  LED's are based on silkscreen labels.

        See "Samples\ZConsole\TcpConsole.c" for example of maintaining state
        across power-cycles.

        This program is based on the "samples/tcpip/http/ssi.c" and 
        "samples/RCM2100/EthCore1.c" sample programs.
        
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
//#define REDIRECTHOST	"proxy.domain.com:1212"


/********************************
 * End of configuration section *
 ********************************/

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.  
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""


#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  Notice that we have ximported in the source code for
 *  this program.  This allows us to <!--#include file="ethcore1.c"-->
 *  in the pages/showsrc.shtml.
 *
 */

#ximport "samples/RCM2200/pages/ethcore1.shtml" index_html
#ximport "samples/RCM2200/pages/rabbit1.gif"    rabbit1_gif
#ximport "samples/RCM2200/pages/ledon.gif"      ledon_gif
#ximport "samples/RCM2200/pages/ledoff.gif"     ledoff_gif
#ximport "samples/RCM2200/pages/button.gif"     button_gif
#ximport "samples/RCM2200/pages/showsrc.shtml"  showsrc_shtml
#ximport "samples/RCM2200/ethcore1.c"           ethcore1_c

/*
 *  In this case the .html is not the first type in the 
 *  type table.  This causes the default (no extension)
 *  to assume the shtml_handler.
 *
 */

/* the default for / must be first */
const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".cgi", "", NULL},                     // cgi
   { ".gif", "image/gif", NULL}
};

/*
 *  Each ledx points to a text string that is either
 *  "ledon.gif" or "ledoff.gif"  This string is toggled
 *  each time the ledxtoggle.cgi is requested from the 
 *  browser.
 *
 */
const char led_on_gif[] = { "ledon.gif" };
const char led_off_gif[] = { "ledoff.gif" };

char led2[15];
char led3[15];

/*
 *  Instead of sending other text back from the cgi's
 *  we have decided to redirect them to the original page.
 *  the cgi_redirectto forms a header which will redirect
 *  the browser back to the main page.
 *
 */
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

const HttpSpec http_flashspec[] = 
{
   { HTTPSPEC_FILE,  "/",              index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.shtml",   index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/showsrc.shtml", showsrc_shtml, NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/rabbit1.gif",   rabbit1_gif,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledon.gif",     ledon_gif,     NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledoff.gif",    ledoff_gif,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/button.gif",    button_gif,    NULL, 0, NULL, NULL},

   { HTTPSPEC_FILE,  "ethcore1.c",  ethcore1_c,    NULL, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "led2",  0,    led2, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led3",  0,    led3, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "time",  0,  & SEC_TIMER, PTR16, "%ld", NULL},

   { HTTPSPEC_FUNCTION, "/led2tog.cgi",   0, led2toggle, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led3tog.cgi",   0, led3toggle, 0, NULL, NULL},
};

/*  Update all LED's to reflect string values. */
void update_outputs()
{
	int  value;

	/*  update DS2 */
	value = (1<<1);		/* PE1 */
	if( !strcmp(led2,led_on_gif) ) {
		value = 0;
   }
   WrPortI(PEB1R, NULL, value);

   /* update DS3 */
   value = (1<<7);		/* PE7 */;
	if( !strcmp(led3,led_on_gif) ) {
		value = 0;
   }
	WrPortI(PEB7R,NULL,value);
}  /* end update_outputs() */


main()
{
	/* Set both LED's initially ON.  When update_outputs() is called, the
	 * human will see it this way.
	 */
   strcpy(led2,led_on_gif);
   strcpy(led3,led_on_gif);

   sock_init();
   http_init();
   tcp_reserveport(80);

	/*  Configure the I/O ports.  Disable slave port which makes
	 *  Port A an output, and PORT E not have SCS signal.
	 *  Read shadow and set PE1 and PE7 as normal I/O.
	 *  LED's are controlled by PE1 and PE7, so make them outputs.
	 */
	WrPortI(SPCR, NULL, 0x84);
	WrPortI(PEFR,  & PEFRShadow,  ~((1<<7)|(1<<1)) & PEFRShadow);
	WrPortI(PEDDR, & PEDDRShadow, (1<<7)|(1<<1));

   while (1) {
   	update_outputs();
      http_handler();
   }
}

#nodebug
