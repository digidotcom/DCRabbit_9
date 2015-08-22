/*******************************************************************************
        dlp_web.c
        Z-World, 2004

        A skeleton program outlining a very basic DLP with Web interface.
*******************************************************************************/

/*
 *  Pick the predefined TCP/IP configuration for this sample.  See
 *  LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 *  configuration.
 */
#define TCPCONFIG 1

/*
 *  Include the Web interface for the DLP which can be reached by browsing to
 *  reboot.html.
 */
#define DLP_USE_HTTP_INTERFACE

/*
 *  HTTP_NO_FLASHSPEC is defined since there are no static resources and all DLP
 *  resources are added to the Web server dynamically.
 */
#define HTTP_NO_FLASHSPEC

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use dcrtcp.lib
#use "http.lib"
#use "remoteupload.lib"

SSPEC_MIMETABLE_START
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END

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

	dlp_init();

	/*
	 *  http_handler needs to be called to handle the active http servers.
	 */
	for(;;)
   {
		tcp_tick(NULL);
      http_handler();
		dlp_handler();
   }
}