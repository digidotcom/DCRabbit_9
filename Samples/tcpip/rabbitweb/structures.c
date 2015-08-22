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
        Samples\TcpIp\RabbitWeb\structures.c

        Demonstrates the use of structures in the RabbitWeb HTTP enhancements.
        See also the

        samples\tcpip\rabbitweb\pages\structures.zhtml

        page that demonstrates the corresponding ZHTML scripting features for
        structures.

*******************************************************************************/

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

/********************************
 * End of configuration section *
 ********************************/

/*
 * This is needed to be able to use the RabbitWeb HTTP enhancements and the
 * ZHTML scripting language.
 */
#define USE_RABBITWEB 1

#memmap xmem

#use "dcrtcp.lib"
#use "http.lib"

/*
 * This page contains the ZHTML portion of the selection variable demonstration
 */
#ximport "samples/tcpip/rabbitweb/pages/structures.zhtml"	structures_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
   // This handler enables the ZHTML parser to be used on ZHTML files...
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END

/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", structures_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", structures_zhtml)
SSPEC_RESOURCETABLE_END

/*
 * Structure variable to be registered.  Note that you can use watch
 * expressions or the evaluate expression feature during runtime to ensure that
 * the variables have properly changed values.
 */

struct foo_struct {
	int a;
	struct {
		int b;
		long c;
	} bar;
	char d;
};

struct foo_struct foo;
struct foo_struct foo2;

/*
 * #web statements
 */

// In the following statement, the entire foo structure is registered at once.
// The guard expression checks all members.
#web foo (($foo.a > 5) && (($foo.d == 'A') || ($foo.d == 'B')) && \
          ($foo.bar.b > 0) && ($foo.bar.c > 30000))
// Below, each member of foo2 is registered separately.  Note that if there
// were a member of foo2 that was not registered, then it would not be
// accessible to the ZHTML scripting language (unlike above, where all members
// would be accessible).
#web foo2.a ($foo2.a > 5)
#web foo2.d (($foo2.d == 'A') || ($foo2.d == 'B'))
#web foo2.bar.b ($foo2.bar.b > 0)
#web foo2.bar.c ($foo2.bar.c > 30000)

void main(void)
{
	// Initialize the #web-registered variables
	foo.a = 10;
	foo.bar.b = 2;
	foo.bar.c = 50000;
	foo.d = 'B';

	foo2.a = 20;
	foo2.bar.b = 4;
	foo2.bar.c = 100000;
	foo2.d = 'A';

	// Initialize the TCP/IP stack and HTTP server
   sock_init();
   http_init();

	// This yields a performance improvement for an HTTP server
	tcp_reserveport(80);

   while (1) {
		// Drive the HTTP server
      http_handler();
   }
}

