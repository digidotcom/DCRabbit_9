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
    devmate_zconsole.c

    This program is intended to be run on a DeviceMate.  It runs a web
    server and all DeviceMate subsystems.  It also maintains a console
    that is available via the telnet port.  In addition, with some
    simple modifications, the console can be made available via serial
    port A (that is, the programming port).  However, this means that
    you cannot both use the console and debug the DeviceMate at the
    same time.

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
#define TARGETPROC_FS_USELX	(fs_ext)

#define TARGETPROC_FS_ENABLE_ZCONSOLE

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
 * Console configuration
 */

/*
 * Size of the buffers for serial port A.  If you want to use
 * another serial port, you should change the buffer macros below
 * appropriately (and change the console_io[] array below).
 */
//#define AINBUFSIZE		127
//#define AOUTBUFSIZE		127

/*
 * The number of console I/O streams that this program supports.
 * Since we are supporting serial port C and telnet, then there
 * are two I/O streams.
 */
#define NUM_CONSOLES 1

/*
 * If this macro is defined, then the version message will be shown
 * with the help command (when the help command has no parameters).
 */
#define CON_HELP_VERSION

/*
 * Defines the version message that will be displayed in the help
 * command if CON_HELP_VERSION is defined.
 */
#define CON_VERSION_MESSAGE "DeviceMate Console Version 1.0\r\n"

/*
 * Defines the message that is displayed on all I/O channels when
 * the console starts up.
 */
#define CON_INIT_MESSAGE CON_VERSION_MESSAGE

/*
 * These ximport directives include the help texts for the console
 * commands.  Having the help text in xmem helps save root code
 * space.
 */
#ximport "samples\dmunit\console_help\help.txt" help_txt
#ximport "samples\dmunit\console_help\help_help.txt" help_help_txt
#ximport "samples\dmunit\console_help\help_echo.txt" help_echo_txt
#ximport "samples\dmunit\console_help\help_set.txt" help_set_txt
#ximport "samples\dmunit\console_help\help_set_param.txt" help_set_param_txt
#ximport "samples\dmunit\console_help\help_show.txt" help_show_txt

/*
 * Choose the subsystems.
 */
#define USE_TC_VAR
#define USE_TC_TCPIP
#define USE_TC_WD
#define USE_TC_FS
#define USE_TC_LOG
#use "tc_conf.lib"

/*
 * These libraries must be included to use the web server
 */
#use "dcrtcp.lib"
#use "http.lib"

#use REMOTEUPLOADDEFS.LIB

#use "zconsole.lib"

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

/*
 * Define the I/O methods available for the console
 */
const ConsoleIO console_io[] =
{
//	CONSOLE_IO_SERA(2400),	// If adding this, change NUM_CONSOLES above
	CONSOLE_IO_TELNET(23)
};

/*
 * Define the available commands
 */
int devmate_con_show(ConsoleState* state);

const ConsoleCommand console_commands[] =
{
	{ "ECHO", con_echo, help_echo_txt },
	{ "HELP", con_help, help_help_txt },
	{ "", NULL, help_txt },
	{ "SET", NULL, help_set_txt },
	{ "SET PARAM", con_set_param, help_set_param_txt },
	{ "SET IP", con_set_ip, help_set_txt },
	{ "SET NETMASK", con_set_netmask, help_set_txt },
	{ "SET GATEWAY", con_set_gateway, help_set_txt },
	{ "SET NAMESERVER", con_set_nameserver, help_set_txt },
	{ "SHOW", devmate_con_show, help_show_txt }
};

/*
 * Use the standard error codes and messages
 */
const ConsoleError console_errors[] = {
	{ CON_ERR_TIMEOUT, "Timed out." },
	{ CON_ERR_BADCOMMAND, "Unknown command." },
	{ CON_ERR_BADPARAMETER, "Bad or missing parameter." },
	{ CON_ERR_COMMANDTOOLONG, "Command too long." },
	{ CON_ERR_BADIPADDRESS, "Bad IP address." }
};

long lookup_gateway(void)
{
	auto int i;
	auto ATHandle ath;

	/* find the default gateway */
	for (i = 0; i < ARP_ROUTER_TABLE_SIZE; i++) {
		// Fixme: need to make documented interface for this data
		ath = ATH2INDEX(_arp_gate_data[i].ath);
		if (ath > ARP_TABLE_SIZE)
			continue;

		// found it
		return _arp_data[ath].ip;
	}
}
const ConsoleBackup console_backup[] =
{
	CONSOLE_BASIC_BACKUP,
	CONSOLE_TCP_BACKUP,
	CONSOLE_TARGETPROC_FS_BACKUP
};

/*
 * Custom "show" command
 */
nodebug int devmate_con_show(ConsoleState* state)
{
	if (state->conio->wrUsed() != 0) {
		return 0;
	}
	switch (state->substate) {
	case 0:
		state->conio->puts("Current configuration:");
		state->conio->puts("\r\n\tI/O Parameter:\t");
		sprintf(state->buffer, "%ld", console_backup_info.param[state->console_number]);
		state->conio->puts(state->buffer);
		break;
	case 1:
		state->conio->puts("\r\n\tIP Address:\t");
		state->conio->puts(inet_ntoa(state->buffer, my_ip_addr));
		break;
	case 2:
		state->conio->puts("\r\n\tNetmask:\t");
		state->conio->puts(inet_ntoa(state->buffer, sin_mask));
		break;
	case 3:
		state->conio->puts("\r\n\tGateway:\t");
		state->conio->puts(inet_ntoa(state->buffer, lookup_gateway()));
		break;
	case 4:
		state->conio->puts("\r\n\tName Server:\t");
		state->conio->puts(inet_ntoa(state->buffer, servlist_first(&_dns_server_table,0,NULL)));
		state->conio->puts("\r\n");
		return 1;
	}
	state->substate++;
	return 0;
}

void main(void)
{
	FSLXnum backup_ext;
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
	 * Partition the filesystem, always allocating 1/8 for the backup partition.
	 */
	backup_ext = fs_setup(fs_ext, MY_LS_SHIFT, 0, NULL, FS_PARTITION_FRACTION,
	                      0x2000, MY_LS_SHIFT, 0, NULL);
	if (backup_ext == 0) {
		printf("Could not create backup extent!\n");
		exit(2);
	}

	lxsize = fs_get_lx_size(fs_ext, 1, MY_LS_SHIFT);
	lxsize = fs_get_lx_size(backup_ext, 1, MY_LS_SHIFT);

	/*
	 * Set the logical extents for the console files and the backup
	 * configuration data.  These are placed in separate logical
	 * extents so that the existence of enough space for the backup
	 * data can be ensured.  These functions must be called before
	 * console_init() so that the console can know where to look for
	 * its backup data.
	 */
	con_set_files_lx(fs_ext);
	con_set_backup_lx(backup_ext);

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
	 * Initialize the console
	 */
	if (console_init() != 0) {
		printf("Console could not load configuration--starting from scratch!\n");
		lx_format(fs_ext, 0);
		lx_format(backup_ext, 0);

		/*
		 * Save the backup information to the console.
		 */
		con_backup();
	}

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
		console_tick();
		http_handler();
	}
}