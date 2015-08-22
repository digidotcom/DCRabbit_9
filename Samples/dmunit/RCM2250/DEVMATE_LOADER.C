/**********************************************************************
    devmate_2flash.c
    Z-World, 2001
   
    This program is intended to be run on a DeviceMate.  It runs a web
    server and all DeviceMate subsystems. 
   
    Note that the web page demonstrating the variables is available on
    the web server as "var.shtml".
   
    This version places the filesystem in the 2nd flash device, and
    as such requires a board with two flash devices installed, such
    as the RCM2250.
   
    This program can be run with all Target Processor sample programs.
**********************************************************************/

#define TC_I_AM_DEVMATE		/* necessary for all DeviceMates */

/*
 * Comment out the following line to prevent the filesystem from
 * being reformatted.
 */
#define FORMAT

/*
 * Necessary to define the network parameters.
 */
#define TCPCONFIG 1

/*
 * This limits the total number of variables that can be on the
 * DeviceMate, even if DEVMATE_VAR_MAXVARS on the target processor
 * is larger.  Note that files are also included in SSPEC_MAXSPEC.
 */
#define SSPEC_MAXSPEC	30

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
 * Number of logical extents in the filesystem
 */
#define FS_MAX_LX  3

/*
 * Maximum number of files on the filesystem
 */
#define FS_MAX_FILES 20

/*
 * Log(base 2) of the desired Logical Sector size.
 */
#define MY_LS_SHIFT 9	// 2^9 == 512

/*
 * Indicate that we want the SPEC table backed up
 */
#define TARGETPROC_FS_ENABLE_BACKUP

/*
 * This library must be used for filesystem support
 */
#memmap xmem
#use "fs2.lib"

/*
 * The Logical Extent (LX) that the target can access in FS2.
 * This MUST be defined! As the actuall LX number is
 * determined at run time, a variable will be used. It will be
 * filled in before the target communications starts!
 */
FSLXnum	fs_ext;
FSLXnum	backup_ext;
#define TARGETPROC_FS_USELX	(fs_ext)
#define TARGETPROC_FS_USEBACKUPLX	(backup_ext)

/*
 * Indicate that we are using FS2 for storing the logs
 */
#define LOG_USE_FS2

/*
 * Maximum file size (quota) for the FS2 log
 */
#define LOG_FS2_SIZE(strm)		1000	// Maximum file size (quota) for FS2 log

/*
 * Logical extent to use for storing logs
 */
#define LOG_FS2_DATALX(strm)	(fs_ext)

/*
 * Logical extent to use for storing the metadata for the logs
 */
#define LOG_FS2_METALX(strm)	(fs_ext)

/*
 * Define to reset all logs when started
 */
//#define RESET_ALL_LOGS

/*
 * These libraries must be included to use the web server
 */
#use "dcrtcp.lib"
#use "http.lib"

/*
 * Choose the subsystems.
 */ 
#define USE_TC_VAR
#define USE_TC_TCPIP
#define USE_TC_WD
#define USE_TC_FS
#define USE_TC_LOG
#define USE_TC_LOADER
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
	long lxsize;
	LogEntry loginfo;
	int status;
	char buf[200];

	/* File system setup and partitioning */
	fs_ext = fs_get_flash_lx();
	if (fs_ext == 0) {
		printf("No flash available!\n");
		exit(1);
	}

	/*
	 * Get the size of the entire flash with the given sector size
	 */
	lxsize = fs_get_lx_size(fs_ext, 1, MY_LS_SHIFT);
	/*
	 * Partition the filesystem - always give 1/8 to the backup partition, as we
	 * have room to spare.
	 */
	backup_ext = fs_setup(fs_ext, MY_LS_SHIFT, 0, NULL, FS_PARTITION_FRACTION,
	                      0x2000, MY_LS_SHIFT, 0, NULL);
	if (backup_ext == 0) {
		printf("Could not create backup extent!\n");
		exit(2);
	}

	lxsize = fs_get_lx_size(fs_ext, 1, MY_LS_SHIFT);
	lxsize = fs_get_lx_size(backup_ext, 1, MY_LS_SHIFT);

	if (fs_init(0, 0) != 0) {
		printf("Filesystem failed to initialize!\n");
		exit(3);
	}
#ifdef FORMAT
	if (lx_format(fs_ext, 0) != 0) {
		printf("Filesystem failed to format!\n");
		exit(4);
	}
	if (lx_format(backup_ext, 0) != 0) {
		printf("Backup area failed to format!\n");
		exit(5);
	}
#endif

	fs_set_lx(fs_ext, fs_ext);

	/*
	 * Reset all logs if requested.
	 */
#ifdef RESET_ALL_LOGS
	log_open(LOG_DEST_ALL, 1);
#endif

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
	 * Print out previous log entries
	 */
	if (!log_seek(LOG_DEST_FS2, 0)) {
		printf("Scanning previous log entries, oldest first...\n");
		for (;;) {
			if (log_next(LOG_DEST_FS2, &loginfo) < 0)
				break;
			printf("%s\n", log_format(&loginfo, buf, sizeof(buf), 1));
		}
		printf("End of messages.\n");
	}

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
