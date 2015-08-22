/*******************************************************************************
        ucos.c
        Z-World, 2000

        A sample combining use of the TCP/IP stack (specificly, the web
        server) with uC/OS. This creates two tasks; one counts from 0 to
        15 in binary on the demo-board LEDs, and another that runs the web
        server that allows you to inspect the current state of the LEDs.
*******************************************************************************/
#class auto

/*
 * This sample requires setting up the demo board as well, as it blinks the LEDs.
 * Instructions on how to set this up can be found in samples/tcpip/http/ssi3.c
 */
  
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
 * Only one server is needed for a reserved port
 */
#define HTTP_MAXSERVERS 1
#define MAX_TCP_SOCKET_BUFFERS 1

/********************************
 * End of configuration section *
 ********************************/

/* MCOS configuration */
#define OS_MAX_TASKS           2  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_MAX_EVENTS			 3			// MAX_UDP_SOCKET_BUFFERS + MAX_TCP_SOCKET_BUFFERS + 2
#define OS_SEM_EN					 1			// Enable semaphores
#define OS_TIME_DLY_HMSM_EN	 1			// Enable OSTimeDlyHMSM
#define STACK_CNT_256			 2			// LED update task + main()
#define STACK_CNT_2K         	 1			// TCP/IP needs a 2K stack

/*
 * IMPORTANT NOTE!
 *
 * It is important to note that the TCP/IP task REQUIRES a 2K stack!
 * This is because the function resolve() (a DNS lookup) creates a
 * udp socket on the stack. If DNS lookups are NOT needed, and
 * resolve() is never called, it is possible to run the TCP/IP stack
 * in only a 1K stack.
 *
 * Also note, all TCP/IP stack functions should be call from ONE (1)
 * thread only, or data corruption and/or system instability may result!
 */

#memmap xmem
#use "ucos2.lib"
#use "dcrtcp.lib"
#use "http.lib"

/*
 * web page & graphics, for use in the web server
 */
#ximport "samples/tcpip/http/pages/ucos.shtml"      index_html
#ximport "samples/tcpip/http/pages/ledon.gif"      ledon_gif
#ximport "samples/tcpip/http/pages/ledoff.gif"     ledoff_gif

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
char counter;	/* real counter for our lights */

/* the default for / must be first */
const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".cgi", "", NULL},                     // cgi
   { ".gif", "image/gif", NULL}
};

const HttpSpec http_flashspec[] = 
{
   { HTTPSPEC_FILE,  "/",              index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.shtml",   index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledon.gif",     ledon_gif,     NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/ledoff.gif",    ledoff_gif,    NULL, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "led1",  0,    led1, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led2",  0,    led2, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led3",  0,    led3, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led4",  0,    led4, PTR16,   "%s", NULL},
};

/*
 *	Sets up the IO ports for our use.
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

/*
 *	This function looks at 'counter', decodes it into binary, and
 * updates both the LEDs and the ledx[] arrays, indicating the
 * current status to the web server.
 */
void update_LEDs()
{
	// store the bit-pattern for the number 
	// and update the hardware LEDs
	if(counter & 0x08) {
	   strcpy(led1,"ledon.gif");
		WrPortI(PDDR, &PDDRShadow, 0x01|PDDRShadow);
	} else {
	   strcpy(led1,"ledoff.gif");
		WrPortI(PDDR, &PDDRShadow, 0xfe&PDDRShadow);
	}

	if(counter & 0x04) {
	   strcpy(led2,"ledon.gif");
		WrPortI(PDDR, &PDDRShadow, 0x02|PDDRShadow);
	} else {
	   strcpy(led2,"ledoff.gif");
		WrPortI(PDDR, &PDDRShadow, 0xfd&PDDRShadow);
	}

	if(counter & 0x02) {
	   strcpy(led3,"ledon.gif");
		WrPortI(PBDR, &PBDRShadow, 0x40|PBDRShadow);
	} else {
	   strcpy(led3,"ledoff.gif");
		WrPortI(PBDR, &PBDRShadow, 0xbf&PBDRShadow);
	}

	if(counter & 0x01) {
	   strcpy(led4,"ledon.gif");
		WrPortI(PBDR, &PBDRShadow, 0x80|PBDRShadow);
	} else {
	   strcpy(led4,"ledoff.gif");
		WrPortI(PBDR, &PBDRShadow, 0x7f&PBDRShadow);
	}
}

/*
 *	One of our two tasks. This is a simulation of a real-time
 * thread, manipulating the hardware, and posts data for the
 * other thread to moniter.
 *
 * This task simply updates the hardware and delays for two
 * seconds, to give a nice slow count.
 */
void blink_lights(void *ptr)
{
	for(;;) {
		counter++;
		if(counter > 15)
			counter = 0;

		update_LEDs();

		OSTimeDlyHMSM(0, 0, 2, 0); // delay a bit to slow down the count
	}		
}

/*
 * This task runs the web server, by callin http_hander(). Access to
 * the ledx[] arrays is allready indicated in the http_flashspec structure.
 */
void web_server(void *ptr)
{
	/* All TCP/IP stuff (ie: the web server) needs to be in one task only. */
	/* This is ok, as the only TCP/IP stuff we are doing is running the web server */
	for(;;) {
		http_handler();
	}
}

main()
{
	INT8U error;

	OSInit();

	io_init();			// init the LEDs
	sock_init();		// init the TCP/IP stack
	http_init();		// init the web server
	tcp_reserveport(80);
	
	/* shut off all LEDs (set count to 0, and display it in binary) */
	counter = 0;
	update_LEDs();
	
	/* Build the two tasks */
	error = OSTaskCreate(web_server, 	NULL, 2048, 1);
	error = OSTaskCreate(blink_lights,	NULL, 256, 0);

	/* start it all running */
	OSStart();
}