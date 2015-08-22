/********************************************************************
	ssi2.c
   Z-World, 2002

	This program demonstrates a basic Smart Star system running
	a WEB page.  It assumes you have Relay, ADC and DAC Voltage cards.

	The WEB page consists of three switches to open and close relays
	with indicator lights, three sets of up and down arrows to increase
	or	decrease analog output, and a monitor of three analog inputs.
		
  	This program is based on the "samples/tcpip/ssi.c" program.
        
	System setup
   ------------
   1. Install relay, DAC and ADC cards into Slots 0, 1, 2.

   2. Connect wires from DAC output channels to ADC input channels.
   
			DAC 0 <---> ADC 0      
			DAC 1 <---> ADC 1      
			DAC 2 <---> ADC 2      

   3. Connect V_USER to +RAW on backplane.
           
	Program Instructions
   --------------------
	1. Make all necessary changes in the configuration section.
   2. Compile and run this program.
	3. With your WEB browser access the WEB page running on the
      controller.
	4.	Select the slots where each card occupies and submit selections.
	5. Click a switch.  You should hear a relay switch contact.
	6. Click an arrow button to increase or decrease analog output.
		You should see the output monitored by the analog input.
		
********************************************************************/
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
#define FORM_ERROR_BUF 256

/*
 * Web server configuration
 */

/*
 * only one socket and server are needed for a reserved port
 */
#define HTTP_MAXBUFFER 512
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

#ximport "samples/smrtstar/tcpip/pages/ssi2.shtml"		index_html
#ximport "samples/smrtstar/tcpip/pages/rabbit1.gif"   rabbit1_gif
#ximport "samples/smrtstar/tcpip/pages/ledon.gif"     ledon_gif
#ximport "samples/smrtstar/tcpip/pages/ledoff.gif"    ledoff_gif
#ximport "samples/smrtstar/tcpip/pages/button.gif"    button_gif
#ximport "samples/smrtstar/tcpip/pages/uparrow.gif"   uparrow_gif
#ximport "samples/smrtstar/tcpip/pages/dnarrow.gif"   dnarrow_gif
#ximport "samples/smrtstar/tcpip/pages/showsrc.shtml" showsrc_shtml
#ximport "samples/smrtstar/tcpip/ssi2.c"           	ssi_c


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
char relaycard[15];
char adccard[15];
char daccard[15];
char rcselected[15];
char dcselected[15];
char acselected[15];
char rcslotvalue[15];
char dcslotvalue[15];
char acslotvalue[15];
int relayslot, dacslot, adcslot;
int initdacflag, initadcflag, initrelayflag;
float dac0value, dac1value, dac2value;
float adc0value, adc1value, adc2value;
static char* const slotoptions[] =
{
	"Slot 0",
	"Slot 1",
	"Slot 2"
};

#define MAX_FORMSIZE	16
typedef struct {
	char name[MAX_FORMSIZE];
	char value[MAX_FORMSIZE];
} FORMType;
#define MAXSLOTS 3
FORMType FORMSpec[MAXSLOTS];

/*
 *  Instead of sending other text back from the cgi's
 *  we have decided to redirect them to the original page.
 *  the cgi_redirectto forms a header which will redirect
 *  the browser back to the main page.
 *
 */

int led_toggle0(HttpState* state)
{
	
	if (initrelayflag)
	{
	   if (strcmp(led_LED0,"ledon.gif")==0)
  		{
      	strcpy(led_LED0,"ledoff.gif");
			relayOut(ChanAddr(relayslot, 0),0);	//off
		}
	   else
   	{
      	strcpy(led_LED0,"ledon.gif");
	     	relayOut(ChanAddr(relayslot, 0),1);	//on
   	}
	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle1(HttpState* state)
{
	
	if (initrelayflag)
	{
	   if (strcmp(led_LED1,"ledon.gif")==0)
	  	{
   	   strcpy(led_LED1,"ledoff.gif");
			relayOut(ChanAddr(relayslot, 1),0);	//off
		}
   	else
	   {
   	   strcpy(led_LED1,"ledon.gif");
     		relayOut(ChanAddr(relayslot, 1),1);	//on
   	}	
	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int led_toggle2(HttpState* state)
{
	
	if (initrelayflag)
	{
	   if (strcmp(led_LED2,"ledon.gif")==0)
	  	{
   	   strcpy(led_LED2,"ledoff.gif");
			relayOut(ChanAddr(relayslot, 2),0);	//off
		}
   	else
	   {
   	   strcpy(led_LED2,"ledon.gif");
     		relayOut(ChanAddr(relayslot, 2),1);	//on
	   }
	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int dac0inc(HttpState* state)
{
	if (initdacflag)
	{
		dac0value+=0.25;
		anaOutVolts(ChanAddr(dacslot, 0), dac0value);
	}  	
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int dac1inc(HttpState* state)
{
	if (initdacflag)
	{
		dac1value+=0.25;
		anaOutVolts(ChanAddr(dacslot, 1), dac1value);
	}  	
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int dac2inc(HttpState* state)
{
	if (initdacflag)
	{
		dac2value+=0.25;
		anaOutVolts(ChanAddr(dacslot, 2), dac2value);
	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int dac0dec(HttpState* state)
{
	if (initdacflag)
	{
		dac0value-=0.25;
		anaOutVolts(ChanAddr(dacslot, 0), dac0value);
  	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int dac1dec(HttpState* state)
{
	if (initdacflag)
	{
		dac1value-=0.25;
		anaOutVolts(ChanAddr(dacslot, 1), dac1value);
	}  	
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int dac2dec(HttpState* state)
{
	if (initdacflag)
	{
		dac2value-=0.25;
		anaOutVolts(ChanAddr(dacslot, 2), dac2value);
	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

/*
 * parse the url-encoded POST data into the FORMSpec struct
 * (ie: parse 'foo=bar&baz=qux' into the struct
 */
int parse_post(HttpState* state)
{
	auto int retval;
	auto int i;

	// state->s is the socket structure, and state->p is pointer
	// into the HTTP state buffer (initially pointing to the beginning
	// of the buffer).  Note that state->p was set up in the submit
	// CGI function.  Also note that we read up to the content_length,
	// or HTTP_MAXBUFFER, whichever is smaller.  Larger POSTs will be
	// truncated.
	retval = sock_aread(&state->s, state->p,
	                    (state->content_length < HTTP_MAXBUFFER-1)?
	                     (int)state->content_length:HTTP_MAXBUFFER-1);
	if (retval < 0) {
		// Error--just bail out
		return 1;
	}

	// Using the subsubstate to keep track of how much data we have received
	state->subsubstate += retval;

	if (state->subsubstate >= state->content_length) {
		// NULL-terminate the content buffer
		state->buffer[(int)state->content_length] = '\0';

		// Scan the received POST information into the FORMSpec structure
		for(i=0; i<(sizeof(FORMSpec)/sizeof(FORMType)); i++) {
			http_scanpost(FORMSpec[i].name, state->buffer, FORMSpec[i].value,
			              MAX_FORMSIZE);
		}

		// Finished processing--returning 1 indicates that we are done
		return 1;
	}
	// Processing not finished--return 0 so that we can be called again
	return 0;
}

int setslot(HttpState* state)
{
	int i, slot, tempslot;

	// This is a simple state machine that allows this function to be
	// called multiple times in processing the POST request.
	switch(state->substate) {
	case 0:
		// Init the FORMSpec data
		strcpy(FORMSpec[0].value, "-1");
		strcpy(FORMSpec[1].value, "-1");
		strcpy(FORMSpec[2].value, "-1");

		state->p = state->buffer;
		state->substate++;
		// Fall through to the next case

	case 1:
		// Process the POST request
		if (parse_post(state)) {
			state->substate++;
		}
		break;

	case 2:
		for (slot=0; slot<(sizeof(FORMSpec)/sizeof(FORMType)); slot++)
		{
			if (strcmp(FORMSpec[slot].value, "-1"))
			{
				tempslot = atoi(FORMSpec[slot].value);
				if (tempslot < 0 || tempslot > 2) {
					// Slot number is out of range
					continue;
				}
				if (!strcmp(FORMSpec[slot].name, "relaycard"))
				{
					relayslot = tempslot;
					strcpy(rcselected, slotoptions[relayslot]);
					strcpy(rcslotvalue, FORMSpec[slot].value);
					initrelayflag=1;
				}

				if (!strcmp(FORMSpec[slot].name, "daccard"))
				{
					dacslot = tempslot;
					strcpy(dcselected, slotoptions[dacslot]);
					strcpy(dcslotvalue, FORMSpec[slot].value);
			   	//get calibration constants
					for (i=0; i<8; i++)
					{
						anaOutEERd(ChanAddr(dacslot, i));
					}	
					anaOutEnable();
					anaOutVolts(ChanAddr(dacslot, 0), dac0value);
					anaOutVolts(ChanAddr(dacslot, 1), dac1value);
					anaOutVolts(ChanAddr(dacslot, 2), dac2value);
					initdacflag=1;
				}

				if (!strcmp(FORMSpec[slot].name, "adccard"))
				{
					adcslot = tempslot;
					strcpy(acselected, slotoptions[adcslot]);
					strcpy(acslotvalue, FORMSpec[slot].value);
		   		//get calibration constants
					for (i=0; i<11; i++)
					{
						anaInEERd(ChanAddr(adcslot, i));
					}	
					initadcflag=1;
				}
			}
		}
		cgi_redirectto(state,REDIRECTTO);
		return 0;

	default:
		// Error!  This case should never happen!
		state->substate = 0;
		return 1;
	}

	// More processing to do--allow us to be called again
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
   { HTTPSPEC_FILE,  "/uparrow.gif",   uparrow_gif,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/dnarrow.gif",   dnarrow_gif,   NULL, 0, NULL, NULL},

   { HTTPSPEC_FILE,  "ssi.c",          ssi_c,         NULL, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "led_LED0",  0,    led_LED0, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_LED1",  0,    led_LED1, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led_LED2",  0,    led_LED2, PTR16,   "%s", NULL},
   { HTTPSPEC_FUNCTION, "/led_LED0.cgi",   0, led_toggle0, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_LED1.cgi",   0, led_toggle1, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led_LED2.cgi",   0, led_toggle2, 0, NULL, NULL},
   
   { HTTPSPEC_VARIABLE, "relaycard",   0,  &relaycard, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "adccard",   0,  &adccard, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "daccard",   0,  &daccard, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "rcselected",   0,  &rcselected, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "dcselected",   0,  &dcselected, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "acselected",   0,  &acselected, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "rcslotvalue",   0,  &rcslotvalue, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "dcslotvalue",   0,  &dcslotvalue, PTR16, "%s", NULL},
   { HTTPSPEC_VARIABLE, "acslotvalue",   0,  &acslotvalue, PTR16, "%s", NULL},
   { HTTPSPEC_FUNCTION, "setslot.cgi",   0, setslot, 0, NULL, NULL},
   
   { HTTPSPEC_VARIABLE,"dac0value",  0,   &dac0value, FLOAT32,   "%.2f", NULL},
   { HTTPSPEC_VARIABLE, "dac1value",  0,   &dac1value, FLOAT32,   "%.2f", NULL},
   { HTTPSPEC_VARIABLE, "dac2value",  0,	 &dac2value, FLOAT32,   "%.2f", NULL},
   { HTTPSPEC_VARIABLE, "adc0value",  0,   &adc0value, FLOAT32,   "%.2f", NULL},
   { HTTPSPEC_VARIABLE, "adc1value",  0,   &adc1value, FLOAT32,   "%.2f", NULL},
   { HTTPSPEC_VARIABLE, "adc2value",  0,	 &adc2value, FLOAT32,   "%.2f", NULL},
   
   { HTTPSPEC_FUNCTION, "/dac0up.cgi",   0, dac0inc, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/dac1up.cgi",   0, dac1inc, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/dac2up.cgi",   0, dac2inc, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/dac0dn.cgi",   0, dac0dec, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/dac1dn.cgi",   0, dac1dec, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/dac2dn.cgi",   0, dac2dec, 0, NULL, NULL},
};


int updateADC(HttpState* state)
{
   if (initadcflag)
   {
	  	adc0value = anaInVolts(ChanAddr(adcslot, 0));
  		adc1value = anaInVolts(ChanAddr(adcslot, 1));
	  	adc2value = anaInVolts(ChanAddr(adcslot, 2));
	}
   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

main()
{
HttpState* state;
	
   // initialize the controller
   brdInit();
	initdacflag=initadcflag=initrelayflag=0;
	dac0value=0.0;
	dac1value=1.25;
	dac2value=2.0;
	adc0value=adc1value=adc2value=0.0;
	
   // set the initial state of the LED's 
   strcpy(led_LED0,"ledoff.gif");  ledOut(LED0, OFF);
   strcpy(led_LED1,"ledoff.gif");  ledOut(LED1, OFF);
   strcpy(led_LED2,"ledoff.gif");  ledOut(LED2, OFF);
	strcpy(rcselected, "Choose");
	strcpy(dcselected, "Choose");
	strcpy(acselected, "Choose");
	strcpy(rcslotvalue, "-1");
	strcpy(dcslotvalue, "-1");
	strcpy(acslotvalue, "-1");

	// set the names of the form variables in which we are interested
	strcpy(FORMSpec[0].name, "relaycard");
	strcpy(FORMSpec[1].name, "daccard");
	strcpy(FORMSpec[2].name, "adccard");

   sock_init();
   http_init();
   tcp_reserveport(80);

   // process WEB page requests and update the LED's
   while (1) {
      //update_leds();
      updateADC(state);
      http_handler();
   }
}

