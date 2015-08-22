/*******************************************************************************
        Samples\TcpIp\RabbitWeb\web_error.c
        Z-World, 2004

        Demonstrates the use of the WEB_ERROR() feature, along with displaying
        these errors in web pages, in the RabbitWeb HTTP enhancements.  
        See also the

        samples\tcpip\rabbitweb\pages\web_error.zhtml

        page that demonstrates the corresponding ZHTML scripting features for
        WEB_ERROR() error messages.

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
 * This page contains the ZHTML portion of the WEB_ERROR() demonstration
 */
#ximport "samples/tcpip/rabbitweb/pages/web_error.zhtml"	web_error_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
   // This handler enables the ZHTML parser to be used on ZHTML files...
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html")
SSPEC_MIMETABLE_END

/* Associate the #ximported files with the web server. */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", web_error_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", web_error_zhtml)
SSPEC_RESOURCETABLE_END

// This variable will hold a temperature value that must be between the values
// 50 and 90.
int temperature;

// The following #web registration ensures that temperature is 50 or greater,
// and sets an error message of "too low" if it is not.
#web temperature (($temperature >= 50)?1:WEB_ERROR("too low"))
// The following #web registration ensures that temperature is 90 or less,
// and sets an error message of "too high" if it is not.  Note that if a
// variable is registered more than once, then all guards for that variable are
// checked.
#web temperature (($temperature <= 90)?1:WEB_ERROR("too high"))

void main(void)
{
	// Initialize the temperature
	temperature = 72;

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

