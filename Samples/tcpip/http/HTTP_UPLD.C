/*******************************************************************************
        Samples\TCPIP\HTTP\httpupld.c
        Z-World, 2003

        Demonstrate the HTTP file upload facility.

        The CGI merely dumps the action codes and information which is
        presented by the server.

        After the upload is completed, the CGI switches back to the
        initial form (index.html).

        See the "upld_fat.c" sample for details on using the default
        upload handler CGI, and adding security.

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
 * only one socket and server are needed for a reserved port
 */
#define HTTP_MAXSERVERS 1
#define MAX_TCP_SOCKET_BUFFERS 1


//#define HTTP_VERBOSE
//#define ZSERVER_VERBOSE
//#define HTTP_DEBUG
//#define ZSERVER_DEBUG
//#define DCRTCP_DEBUG


#define HTTP_TIMEOUT		6


/********************************
 * End of configuration section *
 ********************************/

#define USE_HTTP_UPLOAD		// Required for this demo, to include upload code.

#define DISABLE_DNS		// No name lookups required

#memmap xmem

#use "dcrtcp.lib"
#use "http.lib"


#ximport "samples/tcpip/http/pages/upload.html"    index_html

// This table maps file extensions to the appropriate "MIME" type.  This is
// needed for the HTTP server.
SSPEC_MIMETABLE_START
	SSPEC_MIME(".htm", "text/html"),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END


int upload_cgi(HttpState * s)
{
   if (!http_getState(s)) {
   	http_setState(s, 1);
   	printf("First entry:\n");
      printf("  HTTP version=%s\n",
        http_getHTTPVersion(s) == HTTP_VER_09 ? "0.9" :
        http_getHTTPVersion(s) == HTTP_VER_10 ? "1.0" :
        http_getHTTPVersion(s) == HTTP_VER_11 ? "1.1" : "unknown");
      printf("  HTTP method=%s\n",
        http_getHTTPMethod(s) == HTTP_METHOD_GET ? "GET" :
        http_getHTTPMethod(s) == HTTP_METHOD_POST ? "POST" :
        http_getHTTPMethod(s) == HTTP_METHOD_HEAD ? "HEAD" : "unknown");
      printf("  Userid=%d\n", http_getContext(s)->userid);
      printf("  URL=%s\n", http_getURL(s));
   }


	switch (http_getAction(s)) {
   	case CGI_PROLOG:
      	printf("PROLOG \"%s\"\n", http_getData(s));
         break;
   	case CGI_HEADER:
      	printf("HEADER \"%s\"\n", http_getData(s));
         break;
   	case CGI_START:
      	printf("START content_length=%ld\n", http_getContentLength(s));
         printf("  field name=%s\n", http_getField(s));
         printf("  disposition=%d\n", http_getContentDisposition(s));
         printf("  transfer_encoding=%d\n", http_getTransferEncoding(s));
         printf("  content_type=%s\n", http_getContentType(s));
         break;
   	case CGI_DATA:
      	printf("DATA length=%d (total %ld)\n", http_getDataLength(s), http_getContentLength(s));
         break;
   	case CGI_END:
      	printf("END ----------- actual received length=%ld\n", http_getContentLength(s));
         break;
      case CGI_EPILOG:
      	printf("EPILOG \"%s\"\n", http_getData(s));
         break;
      case CGI_EOF:
      	printf("EOF (unused content=%ld) \"%s\"\n", s->content_length, http_getData(s));
         // Since we use switchCGI, there is no need to return CGI_DONE.
   		http_switchCGI(s, "index.html");
         break;
      case CGI_ABORT:
      	printf("ABORT!\n");
         break;
      default:
      	printf("CGI: unknown action code %d\n", http_getAction(s));
         break;
   }
   return 0;
}


// The flash resource table is now initialized with these macros...
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/index.html", index_html),
	SSPEC_RESOURCE_CGI("upload.cgi", upload_cgi)
SSPEC_RESOURCETABLE_END

void main()
{
	char buf[20];

   sock_init();
   http_init();
   tcp_reserveport(80);

   printf("Ready: point your browser to http://%s/\n\n", inet_ntoa(buf, MY_ADDR(IF_DEFAULT)));


   while (1) {
      http_handler();
   }
}


