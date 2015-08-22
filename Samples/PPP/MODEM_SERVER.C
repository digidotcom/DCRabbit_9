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

static web server over PPP
Once connected, the server will print outs it's IP address on STDIO
You can then access the static page from that address: 'http://x.x.x.x/'

You will need to change the default addresses, and
the send/expect sequence at the beginning of the program

********/
#define TCPCONFIG	0
//setup PPP on serial port C
#define USE_PPP_SERIAL 0x04

#define MODEM_DEBUG 1

#define PPP_VERBOSE

//change number, name, and password to reflect your ISP account
#define DIALUP_SENDEXPECT "ATDT5551212 CONNECT '' ogin: username word: password PPP"

#define CINBUFSIZE 31
#define COUTBUFSIZE 31

#define TW_TO 0		//eliminate 2MSL delay
#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2

#memmap xmem
#use "dcrtcp.lib"
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
const HttpType http_types[] =
{
   { ".html", "text/html", NULL},
   { ".gif", "image/gif", NULL}
};

/*
 *  http_flashspec assocates the file image we brought in with ximport
 *  and associates it with its name on the webserver.  In this example
 *  the file "samples/http/pages/static.html" will be sent to the 
 *  client when they request either "http://yoururl.com/" or
 *  "http://yoururl.com/index.html"
 *
 */

const HttpSpec http_flashspec[] = 
{
   { HTTPSPEC_FILE,  "/",              index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.html",    index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/rabbit1.gif",   rabbit1_gif,   NULL, 0, NULL, NULL},
};


void main()
{
	auto unsigned long t;
	auto char buffer[100];

	sock_init();

	//configure PPP for dialing in to ISP and bring it up
	ifconfig(IF_PPP2,
				IFS_PPP_INIT,
				IFS_PPP_SPEED, 57600L,
				IFS_PPP_RTSPIN, PBDR, &PBDRShadow, 7,
				IFS_PPP_CTSPIN, PBDR, 0,
				IFS_PPP_FLOWCONTROL, 1,
				IFS_PPP_SENDEXPECT, DIALUP_SENDEXPECT,
				IFS_PPP_HANGUP, "ATH",
				IFS_PPP_MODEMESCAPE, 1,
				IFS_PPP_ACCEPTIP, 1,
				IFS_PPP_ACCEPTDNS, 1,
				IFS_UP,
				IFS_END);

	while(ifpending(IF_PPP2) % 2)
	{
		tcp_tick(NULL);
	}					
	if(ifstatus(IF_PPP2))	
	{
		printf("PPP established\n");
	}
	else
	{
		printf("PPP failed\n");
	}
	printf("IP address is %s\n", inet_ntoa( buffer, gethostid()));

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

	ifconfig(IF_PPP2, IFS_DOWN, IFS_END);
	
	//wait while PPP terminates
	tcp_tick(NULL);
	while(ifpending(IF_PPP2) % 2)
	{
		tcp_tick(NULL);
	}
	
}
