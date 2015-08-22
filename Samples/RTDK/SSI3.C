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
        ssi3.c

        A basic contoller. This creates four 'devices' (lights), and four
        buttons to toggle them. Users can browse to the device, and change
        the status of the lights.

        This program has some incremental changes over ssi.c.  It is set up
        to change the lights on the demo board matching what is displayed in
        the web browser.  Set up the development board as follows.  

    Power Supply      TCP/IP Dev Board

         12V  +-------+--+  PWR
                      |
         GND  +----+--|--+  GND
                   |  |
                   |  |     485+
    Demo Board     |  |
                   |  |     485-
                   |  |
        GND     +--+  |     GND
                      |        
        SW1           |     TX
                      |
        SW2           |     RX
                      |
        SW3           |     I0
                      |
        SW4           |     I1
                      |
        +5V     +-----+     I2

        K                   I3

        LED1    +--------+  O0

        LED2    +--------+  O1

        LED3    +--------+  O2

        LED4    +--------+  03

        BUZZER

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
 * only one server and socket are needed for a reserved port
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

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.  
 *
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""

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
#ximport "samples/rtdk/ssi3.c"               ssi3_c

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

const HttpSpec http_flashspec[] = 
{
   { HTTPSPEC_FILE,  "/",              index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.shtml",   index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/showsrc.shtml", showsrc_shtml, NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/rabbit1.gif",   rabbit1_gif,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledon.gif",     ledon_gif,     NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledoff.gif",    ledoff_gif,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/button.gif",    button_gif,    NULL, 0, NULL, NULL},

   { HTTPSPEC_FILE,  	"ssi.c",       ssi3_c,        NULL, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "led1",  0,    led1, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led2",  0,    led2, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led3",  0,    led3, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led4",  0,    led4, PTR16,   "%s", NULL},

   { HTTPSPEC_FUNCTION, "/led1tog.cgi",   0, led1toggle, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led2tog.cgi",   0, led2toggle, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led3tog.cgi",   0, led3toggle, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led4tog.cgi",   0, led4toggle, 0, NULL, NULL},
};

/*
 *		io_init() this function sets up the B and D I/O ports.
 */

void io_init()
{
	// initialize port b
	WrPortI(PBDR, &PBDRShadow, 0x3f&PBDRShadow);			//set outputs 0 and 1

	// initialize port d
	WrPortI(PDDDR, &PDDDRShadow, 0x03|PDDDRShadow);		//set bits 0,1 to output
	WrPortI(PDDCR, &PDDCRShadow, 0xfc&PDDCRShadow);		//outputs driven hi/lo
	WrPortI(PDFR, &PDFRShadow, 0xf0&PDFRShadow);			//i/o normal
	WrPortI(PDDR, &PDDRShadow, 0xfc&PDDRShadow);			//set outputs 2 and 3
}

void update_outputs()
{
	/* update O0 */
	if(strcmp(led1,"ledon.gif"))
		WrPortI(PDDR, &PDDRShadow, 0xfe&PDDRShadow);
	else 
		WrPortI(PDDR, &PDDRShadow, 0x01|PDDRShadow);

	/* update O1 */
	if(strcmp(led2,"ledon.gif"))
		WrPortI(PDDR, &PDDRShadow, 0xfd&PDDRShadow);
	else 
		WrPortI(PDDR, &PDDRShadow, 0x02|PDDRShadow);

	/* update O2 */
	if(strcmp(led3,"ledon.gif"))
		WrPortI(PBDR, &PBDRShadow, 0xbf&PBDRShadow);
	else 
		WrPortI(PBDR, &PBDRShadow, 0x40|PBDRShadow);

	/* update O3 */
	if(strcmp(led4,"ledon.gif"))
		WrPortI(PBDR, &PBDRShadow, 0x7f&PBDRShadow);
	else 
		WrPortI(PBDR, &PBDRShadow, 0x80|PBDRShadow);
}

main()
{
   strcpy(led1,"ledon.gif");
   strcpy(led2,"ledon.gif");
   strcpy(led3,"ledoff.gif");
   strcpy(led4,"ledon.gif");

   io_init();
   sock_init();
   http_init();
   tcp_reserveport(80);

   while (1) {
      http_handler();
      update_outputs();
   }
}

#nodebug