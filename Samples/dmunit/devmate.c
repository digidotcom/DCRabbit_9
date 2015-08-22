/**********************************************************************
    devmate.c
    Z-World, 2001
   
    This program is intended to be run on a DeviceMate.  It runs a web
    server and most DeviceMate subsystems, demonstrating how several
    subsystems can be integrated together in one DeviceMate program.
   
    Note that the web page demonstrating the variables is available on
    the web server as "var.shtml".
   
    This program can be run with all Target Processor sample programs
    except for the filesystem sample programs and ALL.C (which includes
    the filesystem subsystem).
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
 * DeviceMate, even if DEVMATE_VAR_MAXVARS on the target processor
 * is larger.  Note that files are also included in SSPEC_MAXSPEC.
 */
#define SSPEC_MAXSPEC	10

/*
 * This defines the size in bytes of the xmem buffer in which to
 * store the variables.
 */
#define TARGETPROC_VAR_BUFFERSIZE	1024

/*
 * Number of socket buffers
 */
#define MAX_TCP_SOCKET_BUFFERS 8
#define MAX_UDP_SOCKET_BUFFERS 2

/*
 * Indicate that we are using xmem for storing the logs
 */
#define LOG_USE_XMEM

/*
 * Maximum xmem buffer (quota) for the FS2 log
 */
#define LOG_XMEM_SIZE	500

#memmap xmem

/*
 * These libraries must be included to use the web server.
 */
#use "dcrtcp.lib"
#use "http.lib"

/*
 * Choose the subsystems.
 */ 
#define USE_TC_VAR
#define USE_TC_TCPIP
#define USE_TC_WD
#define USE_TC_LOG
#use "tc_conf.lib"

/*
 * Import the web page.  In an actual application, this page may
 * be loaded from the target processor via the file system
 * DeviceMate feature.
 */
#ximport "samples\dmunit\pages\devmate_var.shtml" var_html

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
	{ HTTPSPEC_FILE,	"/var.shtml",		var_html, NULL, 0, NULL, NULL}
};

void main(void)
{
	LogEntry loginfo;
	int status;
	char buf[200];

	/*
	 * This call is necessary to initialize target communications
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
	 * Log an initial entry.
	 */
#define LOG_TEST_STRING "~~~{ Started test run. }~~~"
	status = log_put(LOG_MAKEPRI(2,LOG_INFO), 0, LOG_TEST_STRING, strlen(LOG_TEST_STRING));
	if (status != 0) {
		printf("Failed to add 1st message: %d\n", status);
	}
	
	/*
	 * Drive the target communications and the web server continuously.
	 * This is all that is necessary as the main part of the program.
	 */
	for (;;) {
		targetproc_tick();
		http_handler();
	}
}
