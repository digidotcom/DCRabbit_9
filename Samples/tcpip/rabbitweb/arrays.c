/*******************************************************************************
        Samples\TcpIp\RabbitWeb\\arrays.c
        Z-World, 2004

        Demonstrates the use of arrays in the RabbitWeb HTTP enhancements.  
        See also the

        samples\tcpip\rabbitweb\pages\arrays.zhtml

        page that demonstrates the corresponding ZHTML scripting features for
        array variables.

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
#ximport "samples/tcpip/rabbitweb/pages/arrays.zhtml" arrays_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
   // This handler enables the ZHTML parser to be used on ZHTML files...
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END

/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", arrays_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", arrays_zhtml)
SSPEC_RESOURCETABLE_END

/*
 * Array variables to be registered.  Note that you can use watch
 * expressions or the evaluate expression feature during runtime to ensure that
 * the variables have properly changed values.
 */

int array1[5];
int array2[2][2];
int array3[3];

/*
 * #web statements
 */

// The following statement registers the entire array1[] array.  The '@'
// wildcard symbol indicates that all elements in the array must be larger than
// 0.
#web array1[@] ($array1[@] > 0)
// This registers the multidimensional array array2[].  Since there are multiple
// '@' wildcards, then '@[0]' and '@[1]' are used to refer to the indices.
#web array2[@][@] (($array2[@[0]][@[1]] > 0) && ($array2[@[0]][@[1]] <= 10))
// Note that it is also possible to register array elements individually.
#web array3[0]	($array3[0] > 1)
#web array3[1] ($array3[1] > 2)
#web array3[2] ($array3[2] > 3)

void main(void)
{
	auto int i;

	// Initialized the #web-registered variables.
	for (i = 0; i < 5; i++) {
		array1[i] = i + 1;
	}
	array2[0][0] = 2;
	array2[0][1] = 3;
	array2[1][0] = 4;
	array2[1][1] = 5;
	for (i = 0; i < 3; i++) {
		array3[i] = i + 5;
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

