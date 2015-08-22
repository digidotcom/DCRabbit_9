/*******************************************************************************

        Samples\BL2000\tcpip\ssi.c
        Z-World, 2001

        A basic contoller. This creates five 'devices' (lights), and five
        buttons to toggle them. Users can browse to the device, and change
        the status of the lights. The lights on the controller(DS4 - DS8)
        will match the ones on the web page.

        This program is based on the "samples/tcpip/ssi.c" program.
        
*******************************************************************************/
#class auto


// MACROS for the LED's
#define ON     1
#define OFF    0

#define DS4    0
#define DS5    1
#define DS6    2
#define DS7    3
#define DS8    4


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
 *  this program.  This allows us to <!--#include file="ssi.c"-->
 *  in the pages/showsrc.shtml.
 *
 */

#ximport "samples/bl2000/tcpip/pages/ssi.shtml"       index_html
#ximport "samples/bl2000/tcpip/pages/rabbit1.gif"     rabbit1_gif
#ximport "samples/bl2000/tcpip/pages/ledon.gif"       ledon_gif
#ximport "samples/bl2000/tcpip/pages/ledoff.gif"      ledoff_gif
#ximport "samples/bl2000/tcpip/pages/button.gif"      button_gif
#ximport "samples/bl2000/tcpip/pages/showsrc.shtml"   showsrc_shtml
#ximport "samples/bl2000/tcpip/ssi.c"                 ssi_c

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

char led_DS4[15];
char led_DS5[15];
char led_DS6[15];
char led_DS7[15];
char led_DS8[15];


/*
 *  Instead of sending other text back from the cgi's
 *  we have decided to redirect them to the original page.
 *  the cgi_redirectto forms a header which will redirect
 *  the browser back to the main page.
 *
 */

int led_toggle4(HttpState* state)
{
   if (strcmp(led_DS4,"ledon.gif")==0)
      strcpy(led_DS4,"ledoff.gif");
   else
      strcpy(led_DS4,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle5(HttpState* state)
{
   if (strcmp(led_DS5,"ledon.gif")==0)
      strcpy(led_DS5,"ledoff.gif");
   else
      strcpy(led_DS5,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle6(HttpState* state)
{
   if (strcmp(led_DS6,"ledon.gif")==0)
      strcpy(led_DS6,"ledoff.gif");
   else
      strcpy(led_DS6,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle7(HttpState* state)
{
   if (strcmp(led_DS7,"ledon.gif")==0)
      strcpy(led_DS7,"ledoff.gif");
   else
      strcpy(led_DS7,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle8(HttpState* state)
{
   if (strcmp(led_DS8,"ledon.gif")==0)
      strcpy(led_DS8,"ledoff.gif");
   else
      strcpy(led_DS8,"ledon.gif");

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

   { HTTPSPEC_FILE,  "ssi.c",          ssi_c,         NULL, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "led_DS4",  0,    led_DS4, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_DS5",  0,    led_DS5, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_DS6",  0,    led_DS6, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_DS7",  0,    led_DS7, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_DS8",  0,    led_DS8, PTR16,   "%s", NULL},

   { HTTPSPEC_FUNCTION, "/led_DS4.cgi",   0, led_toggle4, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_DS5.cgi",   0, led_toggle5, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_DS6.cgi",   0, led_toggle6, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_DS7.cgi",   0, led_toggle7, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_DS8.cgi",   0, led_toggle8, 0, NULL, NULL},
};

void update_leds()
{
   auto char *ptr[5];
   auto int i;

   ptr[0] = led_DS4;
   ptr[1] = led_DS5;
   ptr[2] = led_DS6;
   ptr[3] = led_DS7;
   ptr[4] = led_DS8;

   for(i=0; i<5; i++)
   {
       if(strcmp(ptr[i],"ledon.gif") == 0)
       {
           ledOut(i, 1);
       }
       else
       {
           ledOut(i, 0);
       }
   }
}

void main()
{
   // initialize the controller
   brdInit();
   sock_init();
   http_init();
   tcp_reserveport(80);

   // set the initial state of the LED's 
   strcpy(led_DS4,"ledon.gif");  ledOut(DS4, ON);
   strcpy(led_DS5,"ledon.gif");  ledOut(DS5, ON);
   strcpy(led_DS6,"ledoff.gif"); ledOut(DS6, OFF);
   strcpy(led_DS7,"ledon.gif");  ledOut(DS7, ON);
   strcpy(led_DS8,"ledon.gif");  ledOut(DS8, ON);

   // process WEB page requests and update the LED's
   while (1) {
      update_leds();
      http_handler();
   }
}
