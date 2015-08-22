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
        ucos3.c

        A sample combining use of the TCP/IP stack (specificly, the web
        server and SMTP client) with uC/OS. This creates three tasks; one
        counts from 0 to 15 in binary on the demo-board LEDs, another that runs
        the web server that allows you to inspect the current state of the LEDs
        and submit a form to send an e-mail, and a third that actually sends
        the e-mail.
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

//#define SMTP_DEBUG 1

/*
 * You must change the following values to whatever
 * your local IP address and netmask are. Contact your
 * network administrator for these numbers.
 */

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
 * only one server is needed for a reserved port
 */
#define HTTP_MAXSERVERS 1

/*
 * Set the maximum number of TCP sockets to use.  We have set the HTTP
 * server to use 1 socket, and the SMTP client uses 1 socket.
 */
#define MAX_TCP_SOCKET_BUFFERS 2

/*
 * SMTP (e-mail) configuration
 */

/*
 * This is the address of your SMTP (e-mail) server
 */
#define SMTP_SERVER	"mail.somewhere.com"

/*
 * Enter a valid return address here - if something goes wrong, this
 * address might receive a note about it.
 */
#define MAIL_FROM		"rabbit@somewhere.com"

/*
 * The subject line that is stamped on all outgoing e-mails.
 */
#define MAIL_SUBJECT	"Sent from uC/OS!"

/********************************
 * End of configuration section *
 ********************************/

/* MCOS configuration */
#define OS_MAX_TASKS           3  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_MAX_EVENTS			 4			// MAX_TCP_SOCKET_BUFFERS + MAX_UDP_SOCKET_BUFFERS	+ 2
#define OS_SEM_EN					 1			// Enable semaphores
#define OS_TIME_DLY_HMSM_EN	 1
#define STACK_CNT_256			 2			// LED update task + main()
#define STACK_CNT_1K         	 2			// TCP/IP needs a 2K stack

#memmap xmem
#use "ucos2.lib"
#use "dcrtcp.lib"
#use "http.lib"
#use "smtp.lib"

/*
 * web page & graphics, for use in the web server
 */
#ximport "samples/tcpip/http/pages/ucos2.shtml"    index_html
#ximport "samples/tcpip/http/pages/ledon.gif"      ledon_gif
#ximport "samples/tcpip/http/pages/ledoff.gif"     ledoff_gif
#ximport "samples/tcpip/http/pages/form2.html"      reg_form
#ximport "samples/tcpip/http/pages/showsrc.shtml"  showsrc_shtml
#ximport "samples/rtdk/ucos3.c"					src_code
#ximport "samples/tcpip/http/pages/rabbit1.gif"    rabbit1_gif

/*
 * A structure to hold the data retrieved from the http POST command.
 */
#define MAX_FORMSIZE	64
typedef struct {
	char *name;
	char value[MAX_FORMSIZE];
} FORMType;
FORMType FORMSpec[2];

/* flag to indicate if we have something to send */
char email_pending;

/* take an ascii character, and return its value, in hex */
char gethexval(char c)
{
	switch(c) {
	case '0' : return 0x0;
	case '1' : return 0x1;
	case '2' : return 0x2;
	case '3' : return 0x3;
	case '4' : return 0x4;
	case '5' : return 0x5;
	case '6' : return 0x6;
	case '7' : return 0x7;
	case '8' : return 0x8;
	case '9' : return 0x9;
	case 'a' : case 'A' : return 0xa;
	case 'b' : case 'B' : return 0xb;
	case 'c' : case 'C' : return 0xc;
	case 'd' : case 'D' : return 0xd;
	case 'e' : case 'E' : return 0xe;
	case 'f' : case 'F' : return 0xf;
	}
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

/*
 * submit.cgi function - parses the POST data, and signals an e-mail to be sent.
 */
int submit(HttpState* state)
{
	int i;
	
	if(state->length) {
		/* buffer to write out */
		if(state->offset < state->length) {
			state->offset += sock_fastwrite(&state->s,
					state->buffer + (int)state->offset,
					(int)state->length - (int)state->offset);
		} else {
			state->offset = 0;
			state->length = 0;
		}
	} else {
		switch(state->substate) {
		case 0:
			strcpy(state->buffer, "HTTP/1.0 200 OK\r\n");
			break;

		case 1:
			/* init the FORMSpec data */
			FORMSpec[0].value[0] = '\0';
			FORMSpec[1].value[0] = '\0';
			state->p = state->buffer;

			parse_post(state);
			
			state->substate++;
			return 0;

		case 2:
			http_setcookie(state->buffer, FORMSpec[0].value);
			break;

		case 3:
			strcpy(state->buffer, "\r\n\r\n<html><head><title>Results</title></head><body>\r\n");
			break;
			
		case 4:
			sprintf(state->buffer, "<p>Sending to: %s<p>\r\n<p>The message to send: %s<p>\r\n",
				FORMSpec[0].value, FORMSpec[1].value);
			email_pending = 1;
			break;

		case 5:
			strcpy(state->buffer, "<p><hr>Sending mail...                           <br>\n");
			break;

		case 6:
			if(email_pending) return 0;
			if(smtp_status()==SMTP_SUCCESS)
				strcpy(state->buffer, "Message sent\n");
			else
				strcpy(state->buffer, "Error sending message\n");
			break;

		case 7:
			strcpy(state->buffer, "<p>Go <a href=\"/\">home</a></body></html>\r\n");
			break;

		default:
			state->substate = 0;
			return 1;
		}

		state->length = strlen(state->buffer);
		state->offset = 0;
		state->substate++;
	}

	return 0;
}

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

   { HTTPSPEC_FILE,  "/rabbit1.gif",   rabbit1_gif,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/showsrc.shtml", showsrc_shtml, NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  	"ssi.c",       src_code,      NULL, 0, NULL, NULL},

   { HTTPSPEC_FILE, 	"/register.html",	reg_form,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FUNCTION,	"/submit.cgi",		 0,  submit, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "led1",  0,    led1, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led2",  0,    led2, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led3",  0,    led3, PTR16,   "%s", NULL},
   { HTTPSPEC_VARIABLE, "led4",  0,    led4, PTR16,   "%s", NULL}
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
 * Send the e-mail submitted in the web form, when signaled through 'email_pending'
 */
void send_email(void *ptr)
{
	for(;;) {
		if(email_pending) {
			//printf("Sending mail...\n");
			smtp_sendmail(FORMSpec[0].value, MAIL_FROM, MAIL_SUBJECT, FORMSpec[1].value);

			while(smtp_mailtick()==SMTP_PENDING)
				continue;
			email_pending = 0;
		} else {
			OSTimeDlyHMSM(0, 0, 2, 0);
		}
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

#if 0
		if(email_pending) {
			/* give the e-mail task some CPU time */
			OSTimeDly(10);
		}
#endif
	}
}

main()
{
	INT8U error;

	email_pending = 0;
	/* init FORM searchable names - must init ALL FORMSpec structs! */
	FORMSpec[0].name = "email_addy";
	FORMSpec[1].name = "email_message";

	OSInit();

	io_init();			// init the LEDs
	sock_init();		// init the TCP/IP stack
	http_init();		// init the web server
	tcp_reserveport(80);
	
	/* shut off all LEDs (set count to 0, and display it in binary) */
	counter = 0;
	update_LEDs();
	
	/* Build the tasks */
	error = OSTaskCreate(blink_lights,	NULL, 256, 0);
	error = OSTaskCreate(web_server, 	NULL, 1024, 2);
	error = OSTaskCreate(send_email,		NULL, 1024, 1);

	/* start it all running */
	OSStart();
}
