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
        ssl_zimport.c

        This program uses the ZIMPORT.LIB library to compress web pages
        that are served by the HTTP server.  It demonstrates a couple of
        different ways in which compressed files can be used with the
        HTTP server.

		  This program has been modified to use SSL via HTTPS. See the
        SSL comments below for more information.

        ***NOTE*** This sample will NOT compile without first creating a
        certificate with the name and path specified in the #ximport line
        ("#ximport 'cert\mycert.dcc' SSL_CERTIFICATE") below. See the SSL
        Walkthrough, Section 4.1 in the SSL User's Manual for information
        on creating certificates for SSL programs.

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
 * only one socket and server are needed for a reserved port,
 * but to use HTTP and HTTPS on the same device, 2 servers
 * are needed, one for HTTP and one for HTTPS. To use only
 * HTTPS, change HTTP_MAXSERVERS to match HTTP_SSL_SOCKETS.
 */
#define HTTP_MAXSERVERS 2
#define MAX_TCP_SOCKET_BUFFERS 2

// This macro determines the number of HTTP servers that will use
// SSL (HTTPS servers). In this case, we have 2 total servers, and
// this defines one of them to be HTTPS
#define HTTP_SSL_SOCKETS 1

/*
 * INPUT_COMPRESSION_BUFFERS must be defined to be at least as large as
 * HTTP_MAXSERVERS.  Each server instance needs its own compression buffer,
 * because up to HTTP_MAXSERVERS compressed files can be simultaneously
 * uncompressed.  Note that if you are also uncompressing file outside of
 * the web server, you need to account for this in the number of
 * INPUT_COMPRESSION_BUFFERS.
 */
#define INPUT_COMPRESSION_BUFFERS 2

// SSL Stuff
// This macro tells the HTTP library to use SSL
#define USE_HTTP_SSL

// Import the certificate
#ximport "cert\mycert.dcc" SSL_CERTIFICATE

/********************************
 * End of configuration section *
 ********************************/

/*
 * Note that zimport.lib must be #use'd before http.lib (and zserver.lib
 * if you have explicity #use'd it).
 */
#use "zimport.lib"
#use "dcrtcp.lib"
#use "http.lib"

#zimport "pages/zimport.shtml"			zimport_shtml
#zimport "pages/alice.html"				alice_html
#zimport "pages/alice-rabbit.jpg"		alice_jpg

/* the default for / must be first */
const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".jpg", "image/jpeg", NULL}            // jpeg
};

/*
 * Compressed files, when included in the http_flashspec[] structure, will
 * be automatically uncompressed when they are sent to the client.
 */
const HttpSpec http_flashspec[] =
{
   { HTTPSPEC_FILE,  "/",              zimport_shtml, NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.shtml",   zimport_shtml, NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,	"/alice.html",		alice_html,		NULL,	0,	NULL,	NULL}
};

/*
 * The following variables will be used to hold the sizes of the compressed
 * text and image files, respectively.
 */
long text_size;
long image_size;

void main(void)
{
	/*
    * Get the compressed sizes of the files
    */
	xmem2root(&text_size, alice_html, 4);
	text_size &= ZIMPORT_MASK;
	xmem2root(&image_size, alice_jpg, 4);
	image_size &= ZIMPORT_MASK;

	/*
    * When compressed files are included via the sspec_addxmemfile() function,
    * they must be marked as such with SERVER_COMPRESSED in the servermask
    * bitfield.  Otherwise, the file will be sent to the client in compressed
    * form.
    *
    * Note, however, that image files do not generally compress well (or at
    * all).  Hence, it is best to leave image files uncompressed.
    */
	sspec_addxmemfile("/alice.jpg", alice_jpg, SERVER_HTTPS | SERVER_COMPRESSED);
	sspec_addvariable("text_size", &text_size, INT32, "%ld", SERVER_HTTPS);
   sspec_addvariable("image_size", &image_size, INT32, "%ld", SERVER_HTTPS);

   sock_init();
   http_init();

   while (1) {
   	http_handler();
   }
}

