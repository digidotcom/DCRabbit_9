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
        Samples\TcpIp\RabbitWeb\web_update.c

        Demonstrates the use of the #web_update feature in the RabbitWeb HTTP
        enhancements, which allows the program to be notified when certain
        variables have acquired new values.  See also the

        samples\tcpip\rabbitweb\pages\web_update.zhtml

        page for the rest of the program.

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
 * This page contains the ZHTML portion of the #web_update demonstration
 */
#ximport "samples/tcpip/rabbitweb/pages/web_update.zhtml"	web_update_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
   // This handler enables the ZHTML parser to be used on ZHTML files...
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END

/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", web_update_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", web_update_zhtml)
SSPEC_RESOURCETABLE_END

// This is a simple variable that will be updated
int foo;
// This structure defines some serial port parameters
typedef struct {
	long baud;
	int databits;
	int stopbits;
} SerPort;
// This array defines three serial ports
SerPort ports[3];

// Registers the foo variable
#web foo ($foo > 10)
// Each port is #web-registered individually so that the #web_update statement
// can be used for each member of the array.  If we #web-registered the array
// all at once (#web ports[@]), then we could only have one #web_update
// statement for the array.  See below for more information.  See the
// ETHERNET_TO_SERIAL.C sample program for another method of handling this
// construct.
#web ports[0] (($ports[0].baud > 0) && \
               (($ports[0].databits == 7) || ($ports[0].databits == 8)) && \
               (($ports[0].stopbits == 1) || ($ports[0].stopbits == 2)))
#web ports[1] (($ports[1].baud > 0) && \
               (($ports[1].databits == 7) || ($ports[1].databits == 8)) && \
               (($ports[1].stopbits == 1) || ($ports[1].stopbits == 2)))
#web ports[2] (($ports[2].baud > 0) && \
               (($ports[2].databits == 7) || ($ports[2].databits == 8)) && \
               (($ports[2].stopbits == 1) || ($ports[2].stopbits == 2)))

// These are function prototypes for the #web_update statements below.
int foo_update(void);
int port0_update(void);
int port1_update(void);
int port2_update(void);

// Associate the foo_update() function with an update of the variable "foo".
// Note that foo_update() will only be called when the value of foo has been
// successfully updated.  That is, if any other variables being updated on the
// same web form is in error, then this function will *not* be called.  The
// function is only called when the committed value of "foo" is changed.
#web_update foo foo_update

// Each of the ports[] array elements gets its own #web_update statement so that
// we can call different update functions for each element.
#web_update ports[0] port0_update
#web_update ports[1] port1_update
#web_update ports[2] port2_update

// This function is called when the value foo is updated.
int foo_update(void)
{
	// Print a
	printf("foo has been updated to %d!\n", foo);
}

// Each of the following functions is called with the corresponding ports[]
// member is updated.  In a real program, the serial port may need to be
// closed and reopened using the new serial port information.
int port0_update(void)
{
	printf("ports[0] has been updated!\n");
}

int port1_update(void)
{
	printf("ports[1] has been updated!\n");
}

int port2_update(void)
{
	printf("ports[2] has been updated!\n");
}

void main(void)
{
	auto int i;

	// Initialize the #web-registered variables
	foo = 15;
	for (i = 0; i < 3; i++) {
		ports[i].baud = 9600;
		ports[i].databits = 8;
		ports[i].stopbits = 1;
	}

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

