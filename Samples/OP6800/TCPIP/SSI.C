/*******************************************************************************

        samples\OP6800\TCPIP\ssi.c
        Z-World, 2001

        A basic contoller. This creates four 'devices' (lights), and four 
        buttons to toggle them. Users can browse to the device, and change
        the status of the lights. The lights (LED0 - LED1) on the LCD display
        and/or the DEMO board will match the ones on the web page. (Use the
        DEMO boards for the LEDS if you don't have the LCD module)

        OP6800 DEMO Board setup
        -----------------------
        1. Connect a +12v power source to the OP6800 Demo Board connector
           J3/J5 as follows:

           PS          OP6800 Demo Board
           --          -----------------
           +12v        +RAW Terminal  
           GND         GND Terminal
                       
        Program Instructions
        --------------------
        1. Compile and run this program.
        2. With your WEB browser access the WEB page running on the
           controller.
        3. View LEDS on Web page and the controller to see that they match-up
           when changing them via the WEB page control button.

        This program is based on the "samples/tcpip/ssi.c" program.
        
*******************************************************************************/
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

#define REDIRECTTO  "http://" REDIRECTHOST ""

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  Notice that we have ximported in the source code for
 *  this program.  This allows us to <!--#include file="ssi.c"-->
 *  in the pages/showsrc.shtml.
 *
 */

#ximport "samples/OP6800/tcpip/pages/ssi.shtml"       index_html
#ximport "samples/OP6800/tcpip/pages/rabbit1.gif"     rabbit1_gif
#ximport "samples/OP6800/tcpip/pages/ledon.gif"       ledon_gif
#ximport "samples/OP6800/tcpip/pages/ledoff.gif"      ledoff_gif
#ximport "samples/OP6800/tcpip/pages/button.gif"      button_gif
#ximport "samples/OP6800/tcpip/pages/showsrc.shtml"   showsrc_shtml
#ximport "samples/OP6800/tcpip/ssi.c"                 ssi_c

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
      strcpy(led_LED0,"ledoff.gif");
   else
      strcpy(led_LED0,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle1(HttpState* state)
{
   if (strcmp(led_LED1,"ledon.gif")==0)
      strcpy(led_LED1,"ledoff.gif");
   else
      strcpy(led_LED1,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle2(HttpState* state)
{
   if (strcmp(led_LED2,"ledon.gif")==0)
      strcpy(led_LED2,"ledoff.gif");
   else
      strcpy(led_LED2,"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle3(HttpState* state)
{
   if (strcmp(led_LED3,"ledon.gif")==0)
      strcpy(led_LED3,"ledoff.gif");
   else
      strcpy(led_LED3,"ledon.gif");

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

   { HTTPSPEC_VARIABLE, "led_LED0",  0,    led_LED0, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_LED1",  0,    led_LED1, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_LED2",  0,    led_LED2, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_LED3",  0,    led_LED3, PTR16,   "%s", NULL},

   { HTTPSPEC_FUNCTION, "/led_LED0.cgi",   0, led_toggle0, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_LED1.cgi",   0, led_toggle1, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_LED2.cgi",   0, led_toggle2, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_LED3.cgi",   0, led_toggle3, 0, NULL, NULL},
};

void update_leds()
{
   auto char *ptr[4];
   auto int i;

   ptr[0] = led_LED0;
   ptr[1] = led_LED1;
   ptr[2] = led_LED2;
   ptr[3] = led_LED3;
 
   for(i=0; i< (sizeof(ptr)/2); i++)
   {
       if(strcmp(ptr[i],"ledon.gif") == 0)
       {
           // Control for OP6800 Demo Bd LED DS1-DS4
           digOut(i+7, 0);
           
           // Control for OP6800 Keypad LED's DS1-DS4
           ledOut(i, 1);
       }
       else
       {
           // Control for OP6800 Demo Bd LED DS1-DS4
           digOut(i+7, 1);

           // Control for OP6800 Keypad LED's DS1-DS4
           ledOut(i, 0);
       }
   }
}

main()
{
   // initialize the controller
   brdInit();	// Initialize the controller

   sock_init();
   http_init();
   tcp_reserveport(80);

   // set the initial state of the LED's 
   strcpy(led_LED0,"ledon.gif");  digOut(7, 0); ledOut(0, 1);
   strcpy(led_LED1,"ledon.gif");  digOut(8, 0); ledOut(1, 1);
   strcpy(led_LED2,"ledon.gif");  digOut(9, 0); ledOut(2, 1);
   strcpy(led_LED3,"ledon.gif");  digOut(10, 0); ledOut(3, 1);

      // process WEB page requests and update the LED's
   while (1) {
      update_leds();
      http_handler();
   }
}

