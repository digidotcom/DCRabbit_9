/*****
ppp_answer.c

static web server over PPP, handles peers that call in
Once connected, the server will print outs it's IP address on STDIO
You can then access the static page from that address: 'http://x.x.x.x/'

You will need to change the default addresses, and
the send/expect sequence at the beginning of the program

********/
#define TCPCONFIG	0
//for PPP on port C
#define USE_PPP_SERIAL 0x04

#define PPP_DEBUG

//Uncomment for PPP detail
#define PPP_VERBOSE

#define MODEM_DEBUG 1


#define CINBUFSIZE 31
#define COUTBUFSIZE 31

#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2

#memmap xmem
#use "dcrtcp.lib"
#use "modem.lib"
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

	while(1)
	{
	
			//make sure modem RTS is active low
			BitWrPortI(PBDR, &PBDRShadow, 0, 7);

			//set up to act like an ISP,
			//local address is 10.1.10.1, peer is set to 10.1.10.2
			//IFS_PPP_LOCALAUTH sets up expected name and password
			//Note - SENDEXPECT sets up auto answer on first ring, and waits
			//indefinitely for "CONNECT"
			ifconfig(IF_PPP2,
						IFS_PPP_INIT,
						IFS_PPP_SPEED, 19200L,
						IFS_PPP_USEMODEM, 1,
						IFS_PPP_MODEMESCAPE, 1,
						IFS_PPP_SENDEXPECT, "ATS0=1 &CONNECT",
						IFS_PPP_HANGUP, "ATH",
						IFS_IPADDR, aton("10.1.10.1"),
						IFS_PPP_ACCEPTIP, 0,
						IFS_PPP_SETREMOTEIP, aton("10.1.10.2"),
						IFS_PPP_ACCEPTDNS, 0,
						IFS_PPP_LOCALAUTH, "rabbit", "carrots",
						IFS_UP,
						IFS_END);
						 
			while(ifpending(IF_PPP2) % 2)
			{
				tcp_tick(NULL); //wait for PPP to come up
			}
		
			if(ifstatus(IF_PPP2))
			{
				printf("PPP established\n");
				printf("IP address is %s\n", inet_ntoa( buffer, gethostid()));

				http_init();
			   tcp_reserveport(80);
   
  			 	while (ifstatus(IF_PPP2))
  			 	{
     	 			http_handler();
   			}
   		}
			else
			{
				printf("PPP failed\n");				
			} 

		ifconfig(IF_PPP2, IFS_DOWN, IFS_END);
		while(ifstatus(IF_PPP2))
		{
			tcp_tick(NULL); //wait for PPP to terminate
		}
	}		 	  	 	  	
}
