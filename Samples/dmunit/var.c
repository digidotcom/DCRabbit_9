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
/**********************************************************************
    var.c
   
    This program is intended to be run on a DeviceMate.  It runs a web
    server, and will accept variable updates from a target processor.
    These variables are included in .shtml pages that the DeviceMate
    serves.  Hence, these variables allow a simple means of having
    dynamic content in web pages.  See the .shtml file
    devmate_var.shtml for how these variables are included in a
    web page.
   
    See var.c and var_ucos.c for the Target Processor programs
    intended to be run with this program.
**********************************************************************/

#define TC_I_AM_DEVMATE		/* necessary for all DeviceMates */

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

/*
 * This limits the total number of variables that can be on the
 * DeviceMate, even if DEVMATE_VAR_TABLELEN on the target processor
 * is larger.  Note that files are also included in SSPEC_MAXSPEC.
 */
#define SSPEC_MAXSPEC	10

/*
 * This defines the size in bytes of the xmem buffer in which to
 * store the variables.
 */
#define TARGETPROC_VAR_BUFFERSIZE	1024

#memmap xmem

/*
 * Choose var services.
 */ 
#define USE_TC_VAR
#use "tc_conf.lib"

/*
 * These libraries must be included to use the web server.
 */
#use "dcrtcp.lib"
#use "http.lib"

/*
 * Import the web page.  In an actual application, this page may
 * be loaded from the target processor via the file system
 * DeviceMate feature.
 */
#ximport "samples\dmunit\pages\devmate_var.shtml" index_html

/*
 * Define the HTTP types that we will support.  Note that the .shtml
 * type must be first if the default file "/" includes SSI tags
 * (such as the variable tags).
 */
const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".cgi", "", NULL},                     // cgi
   { ".gif", "image/gif", NULL}
};

/*
 * Include the HTML file in the web server.
 */
const HttpSpec http_flashspec[] = 
{
	{ HTTPSPEC_FILE,	"/",					index_html, NULL, 0, NULL, NULL},
	{ HTTPSPEC_FILE,	"/index.shtml",		index_html, NULL, 0, NULL, NULL}
};

void main(void)
{
	/*
	 * These calls are necessary to initialize target communications
	 * beween the DeviceMate and the target processor.
	 */
	targetproc_init();

	/*
	 * Initialize the TCP/IP stack and the web server.
	 */
	sock_init();
	http_init();

	/*
	 * The following improves interactive performance of the web server.
	 */
	tcp_reserveport(80);
	
	/*
	 * Drive the target communications and the web server continuously.
	 * This is all that is necessary as the main part of the program.
	 * Variable updates will be accepted from the target processor and
	 * reflected in any web pages that reference those variables.
	 */
	for (;;) {
		targetproc_tick();
		http_handler();
	}
}
