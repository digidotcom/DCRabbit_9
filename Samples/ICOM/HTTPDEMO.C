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
/********************************************************************
	Samples\ICOM\httpdemo.c

 	This sample program is used with Intellicom Series and Z-World
 	Demo Board.
	
	This program allows a web browser to view and change the state of
	the Intellicom controller.  The LEDs can be toggled.  The current
	message of the LCD is displayed and can be changed.  The current
	state of the switches on the demo board and keys on the keypad
	are displayed.  Pressing Reload in your browser will update the
	web page to the current status.  Connections to each board
	should be set up as follows:
        
    Power Supply     Intellicom Series Board
	 ============		=======================
         12V  +-+---------+  PWR
                |      
         GND  +-|----+----+  GND
                |    | 
                |    |       485+
    Demo Board  |    | 
    ==========  |    |       485-
                |    | 
        GND   +-|----+       GND
                |              
        SW1   +-|-------+    TX
                |       |
        SW2   +-|-----+ |    RX
                |     | |
        SW3   +-|---+ | +-+  I0
                |   | |
        SW4   +-|-+ | +---+  I1
                | | |  
        +5V   +-+ | +-----+  I2
                  |
        K         +-------+  I3

        LED1  +-----------+  O0

        LED2  +-----------+  O1

        LED3  +-----------+  O2

        LED4  +-----------+  03

        BUZZER

********************************************************************/
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
 *
 * It is best to explicitly turn off DHCP in your configuration.  This
 * will allow the console to completely manage DHCP, so that it does
 * not acquire the lease at startup in sock_init() when it may not need
 * to (if the user has not turned on DHCP).
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
 * the proxy, that will be forwarded to the controller board. The
 * commented out line is an example of such a situation.
 */
#define REDIRECTHOST		_PRIMARY_STATIC_IP
//#define REDIRECTHOST	"myproxy.mydomain.com:8080"

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 * REDIRECTTO is used by each LedxToggle cgi's to tell the
 * browser which page to hit next.  The default REDIRECTTO
 * assumes that you are serving a page that does not have
 * any address translation applied to it.  
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""

/*
 * Import the files and images into memory
 */

#ximport "samples/icom/pages/httpdemo.shtml"    index_html
#ximport "samples/icom/pages/ledon.gif"         ledon_gif
#ximport "samples/icom/pages/ledoff.gif"   ledoff_gif
#ximport "samples/icom/pages/button.gif"   button_gif

/*
 * In this case the .html is not the first type in the 
 * type table.  This causes the default (no extension)
 * to assume the shtml_handler.
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
 * This structure is used to parse the returned FORM data, using the
 * POST method.
 */
#define MAX_FORMSIZE 200
typedef struct {
	char *name;
	char value[MAX_FORMSIZE];
} FORMType;
FORMType FORMSpec[1];

/*
 * Each led entry contains a text string that is either
 * "ledon.gif" or "ledoff.gif"  This string is toggled
 * each time the ledxtoggle.cgi is requested from the 
 * browser.
 */
char led[4][15];
char display[4][21];	 // LCD display buffer
char switch_status[4][15];	 // Current status of switches
char key[12][15];		 // Current status of keys

char displayBuf[200]; // Temporary display buffer

/*
 * Initialize the LCD
 */
void LcdInit(void)
{
	int i;
	for (i = 0; i < 4; i++) {
		display[i][0] = '\0';
	}
	dispClear();
}

/*
 * Strip the encoding from the POST info
 */
void StripEncoding(char *dest, char *src, int len)
{
	int i;
	int j;
	char upper;
	char lower;
	char value;

	i = 0;
	j = 0;
	while( (i < len) && (src[i] != '\0') ) {
		if (src[i] == '+') {
			dest[j] = ' ';
		} else if (src[i] == '%') {
			upper = toupper(src[++i]);
			lower = toupper(src[++i]);
			
			if ((upper >= '0') && (upper <= '9')) {
				upper = upper - '0';
			} else if ((upper >= 'A') && (upper <= 'F')) {
				upper = upper - 'A' + 10;
			}
			if ((lower >= '0') && (lower <= '9')) {
				lower = lower - '0';
			} else if ((lower >= 'A') && (lower <= 'F')) {
				lower = lower - 'A' + 10;
			}

			value = (upper * 16) + lower;
			dest[j] = value;
		} else {
			dest[j] = src[i];
		}
		i++;
		j++;
	}
	dest[j] = '\0';
}

/*
 * Parse one token 'foo=bar', matching 'foo' to the name field in
 * the struct, and storing 'bar' into the value
 */
void ParseToken(HttpState* state)
{
	int i, len;

	for(i=0; i<HTTP_MAXBUFFER; i++) {
		if(state->buffer[i] == '=')
			state->buffer[i] = '\0';
	}
	state->p = state->buffer + strlen(state->buffer) + 1;
	
	for(i=0; i<(sizeof(FORMSpec)/sizeof(FORMType)); i++) {
		if(!strcmp(FORMSpec[i].name,state->buffer)) {
			len = (strlen(state->p)>MAX_FORMSIZE) ? MAX_FORMSIZE - 1: strlen(state->p);
			strncpy(FORMSpec[i].value,state->p,1+len);
			FORMSpec[i].value[MAX_FORMSIZE - 1] = '\0';
		}
	}
}

/*
 * parse the url-encoded POST data into the FORMSpec struct
 * (ie: parse 'foo=bar&baz=qux' into the struct)
 */
int ParsePost(HttpState* state)
{
	int retval;

	while(1) {
		retval = sock_fastread(&state->s, state->p, 1);
		if(0 == retval) {
			*state->p = '\0';
			ParseToken(state);
			return 1;
		}

		if((*state->p == '&') || (*state->p == '\r') || (*state->p == '\n')) {
			/* found one token */
			*state->p = '\0';
			ParseToken(state);
			
			state->p = state->buffer;
		} else {
			state->p++;
		}
			
		if((state->p - state->buffer) > HTTP_MAXBUFFER) {
			/* input too long */
			return 1;
		}
	}
}

/*
 * Update the LCD
 */
void DispUpdateWindow(void)
{
	int i;

	dispClear();
	for (i = 0; i < 4; i++) {
		dispGoto(0, i);
		dispPrintf(display[i]);
	}
}

/*
 * Set a new message on the LCD
 */
int SetNewMessage(HttpState *state)
{
	int i;
	int line;
	int charNum;
	
	switch(state->substate) {
	case 0:
		/* Init the FORMSpec data */
		FORMSpec[0].value[0] = '\0';

		state->p = state->buffer;
		state->substate++;
		break;

	case 1:
		if(ParsePost(state)) {
			// Strip the encoding from the new LCD message
			StripEncoding(displayBuf, FORMSpec[0].value, MAX_FORMSIZE);

			// Format the new message into the display buffer
			line = 0;
			charNum = 0;
			for (i = 0; i < strlen(displayBuf); i++) {
				if (displayBuf[i] == '\r') {
					// skip this character
				} else if ((line < 4) && (displayBuf[i] == '\n')) {
					display[line][charNum] = '\0';
					line++;
					charNum = 0;
				} else if ((line < 4) && (charNum < 20)) {
					display[line][charNum] = displayBuf[i];
					charNum++;
				} else if ((line < 4) && (charNum == 20)) {
					display[line][charNum] = '\0';
				}
			}
			if (line < 4) {
				display[line][charNum] = 0;
			}
			for (i = line+1; i < 4; i++) {
				display[i][0] = '\0';
			}

			// Print the new message on the LCD		
			DispUpdateWindow();

			// Reload the web page so that the new message is displayed			
			cgi_redirectto(state, REDIRECTTO);
			state->substate++;
		} else {
		}
		break;

	default:
		state->substate = 0;
		return 1;
	}
	return 0;
}

/*
 * Instead of sending other text back from the cgi's
 * we have decided to redirect them to the original page.
 * the cgi_redirectto forms a header which will redirect
 * the browser back to the main page.
 */
int Led1Toggle(HttpState* state)
{
   if (strcmp(led[0],"ledon.gif")==0)
      strcpy(led[0],"ledoff.gif");
   else
      strcpy(led[0],"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int Led2Toggle(HttpState* state)
{
   if (strcmp(led[1],"ledon.gif")==0)
      strcpy(led[1],"ledoff.gif");
   else
      strcpy(led[1],"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int Led3Toggle(HttpState* state)
{
   if (strcmp(led[2],"ledon.gif")==0)
      strcpy(led[2],"ledoff.gif");
   else
      strcpy(led[2],"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

int Led4Toggle(HttpState* state)
{
   if (strcmp(led[3],"ledon.gif")==0)
      strcpy(led[3],"ledoff.gif");
   else
      strcpy(led[3],"ledon.gif");

   cgi_redirectto(state,REDIRECTTO);
   return 0;      
}

/*
 *		This function sets up the LEDs and switches
 */
void IOInit()
{
	// Initialize port B for leds 3 and 4
	WrPortI(PBDR, &PBDRShadow, 0x3f&PBDRShadow);			//set outputs 2 and 3

	// Initialize port D for leds 1 and 2
	WrPortI(PDDDR, &PDDDRShadow, 0x03|PDDDRShadow);		//set bits 0,1 to output
	WrPortI(PDDCR, &PDDCRShadow, 0xfc&PDDCRShadow);		//outputs driven hi/lo
	WrPortI(PDFR, &PDFRShadow, 0xf0&PDFRShadow);			//i/o normal
	WrPortI(PDDR, &PDDRShadow, 0xfc&PDDRShadow);			//set outputs 0 and 1

	// Initialize port D for switches 1 and 2
	WrPortI(PDDDR, &PDDDRShadow, 0xf3&PDDDRShadow);		//set bits 2,3 to input

	// Initialize port E for switches 3 and 4
	WrPortI(PEDDR, &PEDDRShadow, 0xf3&PEDDRShadow);		//set bits 2,3 to input
	WrPortI(PEFR, &PEFRShadow, 0xf3&PEFRShadow);			//i/o normal
}

/*
 * Update the LEDs
 */
void UpdateOutputs()
{
	// Update LED 1
	if(strcmp(led[0],"ledon.gif"))
		WrPortI(PDDR, &PDDRShadow, 0xfe&PDDRShadow);
	else 
		WrPortI(PDDR, &PDDRShadow, 0x01|PDDRShadow);

	// Update LED 2
	if(strcmp(led[1],"ledon.gif"))
		WrPortI(PDDR, &PDDRShadow, 0xfd&PDDRShadow);
	else 
		WrPortI(PDDR, &PDDRShadow, 0x02|PDDRShadow);

	// Update LED 3
	if(strcmp(led[2],"ledon.gif"))
		WrPortI(PBDR, &PBDRShadow, 0xbf&PBDRShadow);
	else 
		WrPortI(PBDR, &PBDRShadow, 0x40|PBDRShadow);

	// Update LED 4
	if(strcmp(led[3],"ledon.gif"))
		WrPortI(PBDR, &PBDRShadow, 0x7f&PBDRShadow);
	else 
		WrPortI(PBDR, &PBDRShadow, 0x80|PBDRShadow);
}

/*
 * Check the status of the switches
 */
void CheckSwitches(int vSwitch[]) {
	int i;
	int swTemp;
	for (i = 0; i < 4; i++) {
		if ((i == 0) || (i == 1)) {
			swTemp = BitRdPortI(PDDR, i+2);
		} else {
			swTemp = BitRdPortI(PEDR, i);
		}
		if (swTemp && (vSwitch[i] == 0)) {
			strcpy(switch_status[i], "o");
			vSwitch[i] = 1;
		} else if (!swTemp && (vSwitch[i] == 1)) {
			strcpy(switch_status[i], "x");
			vSwitch[i] = 0;
		}
	}
}

/*
 * Process a key event (a press or a release) on the keypad
 */
void ProcessKeyEvent(unsigned wKey, int vKey[])
{
	// Process a key press
	if (wKey <= 12) {
		if (vKey[wKey-1] == 0) {
			strcpy(key[wKey-1], "o");
			vKey[wKey-1] = 1;
		}
	} else {
	// Process a key release
		if (vKey[wKey-12-1] == 1) {
			strcpy(key[wKey-12-1], "x");
			vKey[wKey-12-1] = 0;
		}
	}
}

const HttpSpec http_flashspec[] = 
{
   { HTTPSPEC_FILE,  "/",              index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.shtml",   index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledon.gif",     ledon_gif,     NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledoff.gif",    ledoff_gif,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/button.gif",    button_gif,    NULL, 0, NULL, NULL},

	{ HTTPSPEC_VARIABLE, "led1",   0,    led[0],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "led2",   0,    led[1],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "led3",   0,    led[2],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "led4",   0,    led[3],     PTR16,   "%s", NULL},

	{ HTTPSPEC_VARIABLE, "sw1",    0,    switch_status[0],  PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "sw2",    0,    switch_status[1],  PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "sw3",    0,    switch_status[2],  PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "sw4",    0,    switch_status[3],  PTR16,   "%s", NULL},

	{ HTTPSPEC_VARIABLE, "key1",   0,    key[0],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key2",   0,    key[1],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key3",   0,    key[2],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key4",   0,    key[3],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key5",   0,    key[4],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key6",   0,    key[5],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key7",   0,    key[6],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key8",   0,    key[7],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key9",   0,    key[8],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key10",  0,    key[9],     PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key11",  0,    key[10],    PTR16,   "%s", NULL},
	{ HTTPSPEC_VARIABLE, "key12",  0,    key[11],    PTR16,   "%s", NULL},
	
   { HTTPSPEC_VARIABLE, "line1",  0,    display[0], PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "line2",  0,    display[1], PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "line3",  0,    display[2], PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "line4",  0,    display[3], PTR16,   "%s", NULL},

   { HTTPSPEC_FUNCTION, "/newmessage.cgi", 0, SetNewMessage, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led1tog.cgi",    0, Led1Toggle,    0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led2tog.cgi",    0, Led2Toggle,    0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led3tog.cgi",    0, Led3Toggle,    0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/led4tog.cgi",    0, Led4Toggle,    0, NULL, NULL}
};

main()
{
	int i;				// a counter
	int vSwitch[4];	// holds the status of the switches
	int vKey[12];		// holds the status of the keys
	unsigned wKey;		// a key event

	// Initialize the statuses of the switches and LEDs
	for (i = 0; i < 4; i++) {
		vSwitch[i] = 0;
		strcpy(switch_status[i],"x");
		strcpy(led[i],"ledon.gif");
	}

	// Initialize the FORMSpec structure
	FORMSpec[0].name = "message";

	// Initialize the board
	brdInit();

	// Configure the upper row of keys on the keypad, in order from left
	// to right
	for (i = 0; i < 6; i++) {
		keyConfig(5-i, i+1, i+12+1, 0, 0, 0, 0);
		vKey[i] = 0;
		strcpy(key[i],"x");
	}
	// Configure the lower row of keys on the keypad, in order from left
	// to right
	for (i = 6; i < 12; i++) {
		keyConfig(19-i, i+1, i+12+1, 0, 0, 0, 0);
		vKey[i] = 0;
		strcpy(key[i],"x");
	}

	IOInit();		// Initialize the LEDs and switches
   sock_init();	// Initialize networking
   http_init();	// Initialize the HTTP server
   tcp_reserveport(80);
   LcdInit();		// Initialize the LCD

   while (1) {
   	// Drive the web server and keep the LEDs up to date
   	costate {
      	http_handler();
      	UpdateOutputs();
      }

      // Periodically check the status of the switches
      costate {
      	CheckSwitches(vSwitch);
      	waitfor(DelayMs(10));
      }

      // Drive the keypad
      costate {
      	keyProcess();
      	waitfor(DelayMs(10));
      }

      // Handle key presses and releases
      costate {
      	waitfor(wKey = keyGet());
      	ProcessKeyEvent(wKey, vKey);
      }
   }
}
