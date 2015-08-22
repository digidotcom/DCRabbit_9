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
/*****

static web server over PPP and/or ethernet.
This demonstrates the new multiple-interface functionality.

Once connected via PPP (serial port C by default), the server will
print outs it's IP address on STDIO.  The static ethernet address is
also printed out.

You can then access the static page from either of the addresses
printed out at the start of execution.

If you #define MODEM_STUFF, then you may need to change the
send/expect sequence at the beginning of the program

********/

#define TCPCONFIG		4		// 4 = ethernet + serial PPP.

// PPP/networking debugging options
//#define PPP_VERBOSE
//#define PPP_DEBUG
//#define NET_DEBUG
//#define NET_VERBOSE

// Define the following to include modem control if e.g. you are dialling
// an ISP.  Otherwise, if you have a dedicated serial link to a PC, and no
// modem, you should not define this.
//#define MODEM_STUFF

//Macros for standard login procedure. You may still need to alter expected strings below
//Don't forget the carriage returns '\r'
#define DIALUP_DIALSTRING "ATDT5558000\r"
#define DIALUP_NAME "name\r"
#define DIALUP_PASSWORD "password\r"



#define CINBUFSIZE 31
#define COUTBUFSIZE 31

#define PPP_RTSPORT PBDR
#define PPP_RTSSHADOW PBDRShadow
#define PPP_RTSPIN 7
#define PPP_CTSPORT PBDR
#define PPP_CTSPIN 0

#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2

#memmap xmem
#use "dcrtcp.lib"
#ifdef MODEM_STUFF
	#use "modem.lib"
#endif
#use "http.lib"


#define TIMEZONE        -8

#define LCP_TIMEOUT 5000



//#define FRAGSUPPORT		//make sure FRAGSUPPORT is on

/*
 *  ximport is a Dynamic C language feature that takes the binary image
 *  of a file, places it in extended memory on the controller, and
 *  associates a symbol with the physical address on the controller of
 *  the image.
 *
 */

#ximport "samples/tcpip/http/pages/static.html"    index_html
#ximport "samples/tcpip/http/pages/rabbit1.gif"    rabbit1_gif

/*
 *  http_types gives the HTTP server hints about handling incoming
 *  requests.  The server compares the extension of the incoming
 *  request with the http_types list and returns the second field
 *  as the Content-Type field.  The third field defines a custom
 *  function to handle that mime type.
 *
 *  You can get a list of mime types from Netscape's browser in:
 *
 *  Edit->Preferences->Navigator->Applications
 *
 */

/* the default mime type for '/' must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif")
SSPEC_MIMETABLE_END


// The static resource table is initialized with these macros...
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/index.html", index_html),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif)
SSPEC_RESOURCETABLE_END


void main()
{
	auto unsigned long t;
	auto char buffer[100];
	auto word i;

	debug_on = 1;

#ifdef MODEM_STUFF
	if(ModemOpen(PPP_SPEED) == 0)
	{
		printf("No modem detected\n");
		exit(-1);
	}
	if(ModemInit())
	{
		printf("Modem Initialized\n");
	}
	else
	{
		printf("Could not init modem\n");
		exit(-1);
	}

	ModemSend(DIALUP_DIALSTRING);
	if(ModemExpect("ONNECT", 30000) == 0)
	{
		printf("Expected 'CONNECT'\n");
		exit(-1);

	}

	if(ModemExpect("ogin:", 10000) == 0)
	{
		printf("Expected 'Login:'\n");
		exit(-1);
	}
	ModemSend(DIALUP_NAME);
	if(ModemExpect("word:", 2000) == 0)
	{
		printf("Expected Password\n");
		exit(-1);
	}
	ModemSend(DIALUP_PASSWORD);		//secret password, erase later
	if(ModemExpect("PPP", 5000) == 0)
	{
		printf("Didn't get PPP session\n");
		exit(-1);
	}
	ModemClose();
#endif

	sock_init();

#ifdef MODEM_STUFF
	PPPflowcontrolOn();
#endif

	//PPPnegotiateIP(inet_addr("10.1.10.1"), inet_addr("10.1.10.2"));
	//PPPsetAuthenticator("name", "password");
	//PPPsetAuthenticatee("mageeol", "blah");

	ip_print_ifs();
	router_printall();

	http_init();

/*
 *  tcp_reserveport causes the web server to maintain pending requests
 * whenever there is not a listen socket available
 *
 */

   tcp_reserveport(80);

/*
 *  http_handler needs to be called to handle the active http servers.
 */

   while (1) {
      http_handler();
   }

}

