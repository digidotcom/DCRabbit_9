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
        Samples\TCPIP\HTTP\cgi.c

        A simple web-counter implemented in CGI functions. This
        demonstrates how to add basic dynamic functionality to
        your pages.
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

/* where to send the user after the CGI finishes */
#define REDIRECTTO 		"http://" REDIRECTHOST "/index.html"

#define TIMEZONE        -8

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

#ximport "samples/tcpip/http/pages/cgi.html"    	index_html
#ximport "samples/tcpip/http/pages/rabbit1.gif"    rabbit1_gif


/*
 *  SSPEC_MIMETABLE_* gives the HTTP server hints about handling incoming
 *  requests.  The server compares the extension of the incoming
 *  request with this list and returns the second field
 *  as the Content-Type field.
 *
 *  The default mime type for '/' must be first
 *
 *  Notice that cgi doesn't have a MIME type or a handler.
 *  The handler for this type of entity is make the the HttpSpec
 *  structure.
 *
 */
SSPEC_MIMETABLE_START
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END



/*
 *  there is a new entry in the http_flashspec:  "/test.cgi"
 *
 *  This entry includes the online name of CGI routine and the
 *  internal routine that it maps to.
 *
 */

int test_cgi(HttpState* state);

// The static resource table is initialized with these macros...
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/index.html", index_html),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif),
	SSPEC_RESOURCE_FUNCTION("/test.cgi", test_cgi)
SSPEC_RESOURCETABLE_END


void main()
{
   sock_init();
   http_init();

   tcp_reserveport(80);

/*
 *  http_handler needs to be called to handle the active http servers.
 */

   while (1) {
      http_handler();
   }
}

/*
 *  test string what is served by test_cgi.  It is used
 *  as one of the parameters to sprintf() to build a buffer
 *  that is later sent.  The format of the first three lines
 *  is important.
 *
 *  HTTP/1.0 200 OK\r\n
 *  Date: <current date/time from http_date_str()>\r\n
 *  Content-Type: text/html\r\n\r\n
 *
 *  The first line tells the internet browser that the content
 *  was found and will be returned in the body.  The next line
 *  is the date which is followed by the mime type of the body.
 *  Notice the \r\n\r\n.  If you leave out the second \r\n the
 *  browser will assume that the next line is part of the header.
 *  The document follows.
 *
 *  There are many books and online references for the format
 *  of CGI responses.
 *
 */

const char teststr[] =
	"HTTP/1.0 200 OK\r\n" \
	"Date: %s\r\n" \
	"Content-Type: text/html\r\n\r\n" \
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD W3 HTML//EN\">\r\n" \
	"<HTML>" \
	"<HEAD><TITLE><cgi.c></TITLE></HEAD>" \
	"<BODY><H1>cgi.c, hit count %d</H1>" \
	"      <BR><BR><BR><A HREF=\"%s\">home</A>" \
	"</BODY>" \
	"</HEAD>";

/*
 *  This is the internal cgi routine.  It uses teststr to build
 *  a buffer which it uses to respond to the request.
 *
 *  This routine increments the hit count value and uses the
 *  cgi_sendstring routine to send the page.  The cgi_sendstring
 *  routine immediately returns and the string is sent the next
 *  time this server gets a tick.  If you want the server to
 *  immediately close the connection return a one otherwise you
 *  should return a zero.
 *
 */

int test_cgi(HttpState* state)
{
	static char date[30];
	static char buffer[512];
	static int hitcount;

	#GLOBAL_INIT { hitcount=0; }

	hitcount++;
	http_date_str(date);

	sprintf(buffer,teststr,date,hitcount,REDIRECTTO);

	cgi_sendstring(state,buffer);
   return 0;
}