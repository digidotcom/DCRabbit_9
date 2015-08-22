/*******************************************************************************
        Samples\RemoteFirmwareUpdate\dlp_static.c
        Z-World, 2004

        This program is the same as Samples\TcpIp\HTTP\static.c except that it
        has been turned into a DLP and must be compiled using
        DownloadImageGenerator.exe before it is uploaded to the DLM.  Where this
        sample has changed for the DLP it is marked with a "DLP SPECIFIC CHANGE"
        comment.

        A very basic example using the HTTP library. This program
        completely intilizes the library, outputing a basic static
        web page.
*******************************************************************************/
/*
 * 	By default, have compiler make function variables storage class
 * 	"auto" (allocated on the stack).
 */
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
 *  The TIMEZONE compiler setting gives the number of hours from
 *  local time to Greenwich Mean Time (GMT).  For pacific standard
 *  time this is -8.  Note:  for the time to be correct it must be set
 *  with tm_rd which is documented in the Dynamic C user manual.
 */

#define TIMEZONE        -8

/*
 *  DLP SPECIFIC CHANGE: Include the Web interface for the DLP.  The Web
 *  interface can be reached by browsing to reboot.html.
 */
#define DLP_USE_HTTP_INTERFACE

/********************************
 * End of configuration section *
 ********************************/

/*
 *  memmap forces the code into xmem.  Since the typical stack is larger
 *  than the root memory, this is commonly a desirable setting.  Another
 *  option is to do #memmap anymem 8096 which will force code to xmem when
 *  the compiler notices that it is generating within 8096 bytes of the
 *  end.
 *
 *  #use the Dynamic C TCP/IP stack library and the HTTP application library
 */
#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"
// DLP SPECIFIC CHANGE: explicity use the DLP library
#use "remoteupload.lib"

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
	SSPEC_MIME(".gif", "image/gif"),
   SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END

/*
 *  http_flashspec assocates the file image we brought in with ximport
 *  and associates it with its name on the webserver.  In this example
 *  the file "samples/http/pages/static.html" will be sent to the
 *  client when they request either "http://yoururl.com/" or
 *  "http://yoururl.com/index.html"
 *
 */

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", index_html),
	SSPEC_RESOURCE_XMEMFILE("/index.html", index_html),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif)
SSPEC_RESOURCETABLE_END

void main()
{

	/*
	 *  sock_init initializes the TCP/IP stack.
	 *  http_init initializes the web server.
	 */

   sock_init();
   http_init();

	/*
	 *  tcp_reserveport causes the web server to ignore requests when there
	 *  isn't an available socket (HTTP_MAXSERVERS are all serving index_html
	 *  or rabbit1.gif).  This saves some memory, but can cause the client
	 *  delays when retrieving pages.
	 */

   tcp_reserveport(80);

   // DLP SPECIFIC CHANGE: The DLP must be initialized in remoteupload.lib
   dlp_init();

	/*
	 *  http_handler needs to be called to handle the active http servers.
	 */

   while (1) {
      http_handler();
      // DLP SPECIFIC CHANGE: call the DLP handler.
      dlp_handler();
   }
}