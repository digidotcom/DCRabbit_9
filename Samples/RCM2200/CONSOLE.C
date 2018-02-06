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
/********************************************************************
   samples\RCM2200\console.c

   This program must be run on the slave!

	This sample program demonstrates many of the features of
	ZCONSOLE.LIB.  See SPCLIENT.C and SERDCLIENT.C for clients
	for this console program.

	Among the features this sample program supports are network
	configuration, uploading web pages, changing variables for use
	with web pages, sending mail, and access to the console through
	a telnet client.  Also of note are the multiple I/O streams
	through which communcation with the console can take place.
	This program can use the slave port, serial port D, or telnet.

	Since the RCM2200 has a single flash, then the filesystem must
	also be located on that flash.  In order to ensure that there
	is enough space for both program and filesystem, a change must
	be made to .\lib\bioslib\memconfig.lib.  Change the XMEM_RESERVE_SIZE
	macro to be large enough for the filesystem (plus one block, so
	that the filesystem can start on a block boundary after rounding
	up to the nearest block boundary).  This sample, as written,
	expects 32K of space, with a 4K block size.  Hence, 36K must be
	reserved, so XMEM_RESERVE_SIZE can be set to 0x9000 (36K).


	This program can communicate over serial port D (which is
	connected between both RCM2200 boards on the proto board).
	However, since writing to the first flash disables interrupts
	for that period of time, then the ISR for the serial port will
	likely not be triggered in time to read any incoming data.  Hence,
	the baud rate for this sample is set to 1200, and even that may
	not be safe for large data transfers (such as with the "PUT"
	command).

********************************************************************/
#class auto

/********************************************************************
	TCP/IP setup
 ********************************************************************/


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
 *
 * It is best to explicitly turn off DHCP in your configuration.  This
 * will allow the console to completely manage DHCP, so that it does
 * not acquire the lease at startup in sock_init() when it may not need
 * to (if the user has not turned on DHCP).
 */
#define TCPCONFIG 1

/*
 * Size of the buffers for serial port C.  If you want to use
 * another serial port, you should change the buffer macros below
 * appropriately (and change the console_io[] array below).
 */
#define DINBUFSIZE		1023
#define DOUTBUFSIZE		255

/*
 * Maximum number of connections to the web server.  This indicates
 * the number of sockets that the web server will use.
 */
#define HTTP_MAXSERVERS 2

/*
 * Maximum number of sockets this program can use.  The web server
 * is taking two sockets (see above), the mail client uses one
 * socket, and the telnet interface uses 1 socket.
 */
#define MAX_TCP_SOCKET_BUFFERS 4

/*
 * All web server content is dynamic, so we do not need the
 * http_flashspec[] array.
 */
#define HTTP_NO_FLASHSPEC

/********************************************************************
	File system setup
 ********************************************************************/

/*
 * The filesystem that the console uses should be located in the
 * first flash (since the RCM2200 has only 1 flash).  Note that
 * this also requires a change to memconfig.lib to reserve enough
 * space for the filesystem.  Edit .\lib\bioslib\memconfig.lib to
 * set an appropriate value to XMEM_RESERVE_SIZE that matches the
 * value you set below.
 */
#define FS2_USE_PROGRAM_FLASH	36			// 36kb = 0x9000

/*
 * Maximum number of entries in the TCP servers' object list (in
 * this case, the total number of variables and files that can
 * be created).
 */
#define SSPEC_MAXSPEC  30

/*
 * Number of logical extents in the filesystem
 */
#define FS_MAX_LX  2

/*
 * Maximum number of files on the filesystem
 */
#define FS_MAX_FILES (SSPEC_MAXSPEC + 2)

/*
 * Log(base 2) of the desired Logical Sector size.
 */
#define MY_LS_SHIFT 12	// 2^12 == 4096

/********************************************************************
	Console setup
 ********************************************************************/

/*
 * The number of console I/O streams that this program supports.
 * Since we are supporting the slave port, serial port D, and
 * telnet, then there are three I/O streams.
 */
#define NUM_CONSOLES 3

/*
 * If this macro is defined, then the version message will be shown
 * with the help command (when the help command has no parameters).
 */
#define CON_HELP_VERSION	// Help by itself shows the version message

/*
 * Defines the version message that will be displayed in the help
 * command if CON_HELP_VERSION is defined.
 */
#define CON_VERSION_MESSAGE "TCP/IP Console Version 1.0\r\n"

/*
 * Defines the message that is displayed on all I/O channels when
 * the console starts up.
 */
#define CON_INIT_MESSAGE CON_VERSION_MESSAGE

/********************************************************************/

/*
 * These ximport directives include the help texts for the console
 * commands.  Having the help text in xmem helps save root code
 * space.
 */
#ximport "samples\rcm2200\console_help\help.txt" help_txt
#ximport "samples\rcm2200\console_help\help_help.txt" help_help_txt
#ximport "samples\rcm2200\console_help\help_echo.txt" help_echo_txt
#ximport "samples\rcm2200\console_help\help_set.txt" help_set_txt
#ximport "samples\rcm2200\console_help\help_set_param.txt" help_set_param_txt
#ximport "samples\rcm2200\console_help\help_set_mail.txt" help_set_mail_txt
#ximport "samples\rcm2200\console_help\help_set_mail_server.txt" help_set_mail_server_txt
#ximport "samples\rcm2200\console_help\help_set_mail_from.txt" help_set_mail_from_txt
#ximport "samples\rcm2200\console_help\help_show.txt" help_show_txt
#ximport "samples\rcm2200\console_help\help_put.txt" help_put_txt
#ximport "samples\rcm2200\console_help\help_get.txt" help_get_txt
#ximport "samples\rcm2200\console_help\help_delete.txt" help_delete_txt
#ximport "samples\rcm2200\console_help\help_list.txt" help_list_txt
#ximport "samples\rcm2200\console_help\help_createv.txt" help_createv_txt
#ximport "samples\rcm2200\console_help\help_putv.txt" help_putv_txt
#ximport "samples\rcm2200\console_help\help_getv.txt" help_getv_txt
#ximport "samples\rcm2200\console_help\help_mail.txt" help_mail_txt
#ximport "samples\rcm2200\console_help\help_reset.txt" help_reset_txt
#ximport "samples\rcm2200\console_help\help_reset_files.txt" help_reset_files_txt
#ximport "samples\rcm2200\console_help\help_reset_variables.txt" help_reset_variables_txt

#memmap xmem

#use "slave_port.lib"
#use "sp_stream.lib"
#use "Fs2.lib"
#use "dcrtcp.lib"
#use "http.lib"
#use "smtp.lib"

#use "ZNETSUPPORT.LIB"
/*
 * Note that all libraries that zconsole.lib needs must be #use'd
 * before #use'ing zconsole.lib .
 */
#use "zconsole.lib"

/*
 * This function prototype is for a custom ZConsole command.  It must be
 * declared before the console_command[] array.
 */
int hello_world(ConsoleState* state);

/*
 * This array defines which I/O streams for which the console will
 * be available.  The streams included below are defined through
 * macros.  Available macros are CONSOLE_IO_SERA, CONSOLE_IO_SERB,
 * CONSOLE_IO_SERC, CONSOLE_IO_SERD, CONSOLE_IO_TELNET, and
 * CONSOLE_IO_SP (for the slave port).  The parameter for the macro
 * represents the initial baud rate for serial ports, the port
 * number for telnet, or the channel number for the slave port.
 * It is possible for the user to define her own I/O handlers and
 * include them in a ConsoleIO structure in the console_io array.
 * Remember that if you change the number of I/O streams here, you
 * should also change the NUM_CONSOLES macro above.
 */
const ConsoleIO console_io[] =
{
	CONSOLE_IO_TELNET(23),
	CONSOLE_IO_SP(0x42),
	CONSOLE_IO_SERD(300)
};

/*
 * This array defines the commands that are available in the console.
 * The first parameter for the ConsoleCommand structure is the
 * command specification--that is, the means by which the console
 * recognizes a command.  The second parameter is the function
 * to call when the command is recognized.  The third parameter is
 * the location of the #ximport'ed help file for the command.
 * Note that the second parameter can be NULL, which is useful if
 * help information is needed for something that is not a command
 * (like for the "SET" command below--the help file for "SET"
 * contains a list of all of the set commands).  Also note the
 * entry for the command "", which is used to set up the help text
 * that is displayed when the help command is used by itself (that
 * is, with no parameters).
 */
const ConsoleCommand console_commands[] =
{
	{ "HELLO WORLD", hello_world, 0 },
	{ "ECHO", con_echo, help_echo_txt },
	{ "HELP", con_help, help_help_txt },
	{ "", NULL, help_txt },
	{ "SET", NULL, help_set_txt },
	{ "SET PARAM", con_set_param, help_set_param_txt },
	{ "SET IP", con_set_ip, help_set_txt },
	{ "SET NETMASK", con_set_netmask, help_set_txt },
	{ "SET GATEWAY", con_set_gateway, help_set_txt },
	{ "SET NAMESERVER", con_set_nameserver, help_set_txt },
	{ "SET MAIL", NULL, help_set_mail_txt },
	{ "SET MAIL SERVER", con_set_mail_server, help_set_mail_server_txt },
	{ "SET MAIL FROM", con_set_mail_from, help_set_mail_from_txt },
	{ "SHOW", con_show, help_show_txt },
	{ "PUT", con_put, help_put_txt },
	{ "GET", con_get, help_get_txt },
	{ "DELETE", con_delete, help_delete_txt },
	{ "LIST", NULL, help_list_txt },
	{ "LIST FILES", con_list_files, help_list_txt },
	{ "LIST VARIABLES", con_list_variables, help_list_txt },
	{ "CREATEV", con_createv, help_createv_txt },
	{ "PUTV", con_putv, help_putv_txt },
	{ "GETV", con_getv, help_getv_txt },
	{ "MAIL", con_mail, help_mail_txt },
	{ "RESET", NULL, help_reset_txt },
	{ "RESET FILES", con_reset_files, help_reset_files_txt },
	{ "RESET VARIABLES", con_reset_variables, help_reset_variables_txt }
};

/*
 * This array sets up the error messages that can be generated.
 * CON_STANDARD_ERRORS is a macro that expands to the standard
 * errors that the built-in commands in zconsole.lib uses.  Users
 * can define their own errors here, as well.
 */
const ConsoleError console_errors[] = {
	CON_STANDARD_ERRORS
};

/*
 * This array defines the information (such as configuration) that
 * will be saved to the filesystem.  Note that if, for example, the
 * HTTP or SMTP related commands are include in the console_commands
 * array above, then the backup information must be included in
 * this array.  The entries below are macros that expand to the
 * appropriate entry for each set of functionality.  Users can also
 * add their own information to be backed up here by adding more
 * ConsoleBackup structures.
 */
const ConsoleBackup console_backup[] =
{
	CONSOLE_BASIC_BACKUP,
	CONSOLE_TCP_BACKUP,
	CONSOLE_HTTP_BACKUP,
	CONSOLE_SMTP_BACKUP
};

/*
 * The following defines the MIME types that the web server will
 * handle.
 */
const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler}, // ssi
   { ".html", "text/html", NULL},           // html
   { ".gif", "image/gif", NULL},
   { ".jpg", "image/jpeg", NULL},
   { ".jpeg", "image/jpeg", NULL},
   { ".txt", "text/plain", NULL}
};

/*
 * This is a custom command.  Custom commands always take a
 * ConsoleState* as an argument (a pointer to the state structure
 * for the given I/O stream), and return an int.  The return value
 * should be 0 when the command wishes to be called again on the
 * next console_tick(), 1 when the command has successfully
 * finished processing, or -1 when the command has finished due
 * to an error.
 */
int hello_world(ConsoleState* state)
{
	state->conio->puts("Hello, World!\r\n");
	return 1;
}

/********************************************************************/

void main(void)
{
	FSLXnum ext1, ext2;
	word newsize;
	longword  smtp_server;
	char		smtp_dotted[16];
	int     rc;

	/* File system setup and partitioning on program flash */
	ext1 = fs_get_flash_lx();
	if (ext1 == 0) {
		printf("No flash available!\n");
		exit(1);
	}

	/*
	 * Partition the filesystem so that 1/2 (0x08000/0x10000) of
	 * the filesystem is reserved for saving the configuration
	 * information.  This is necessary so that the main filesystem
	 * area never takes up so much space that the configuration
	 * information cannot be saved.
	 */
	newsize = 0x8000;

	/*
	 * Partition the filesystem
	 */
	ext2 = fs_setup(ext1, MY_LS_SHIFT, 0, NULL, FS_PARTITION_FRACTION,
	                newsize, MY_LS_SHIFT, 0, NULL);
	if (ext2 == 0) {
		printf("Could not create backup extent!\n");
		exit(1);
	}

	/*
	 * Set the logical extents for the console files and the backup
	 * configuration data.  These are placed in separate logical
	 * extents so that the existence of enough space for the backup
	 * data can be ensured.  These functions must be called before
	 * console_init() so that the console can know where to look for
	 * its backup data.
	 */
	con_set_files_lx(ext1);
	con_set_backup_lx(ext2);

	/*
	 * All initialization of TCP/IP, clients, servers, and I/O
	 * must be done by the user prior to using any console_init().
	 */

	/*
	 * All initialization of TCP/IP, clients, servers, and I/O
	 * must be done by the user prior to using any console
	 * functions.
	 */
	sock_init();

	tcp_reserveport(80);		// Enable SYN-queueing and disable the
									// 2MSL wait for the web server (results
									// in performance improvements).
	http_init();

	/*
	 * Initialize slave port
	 */
	SPinit(0);

	/*
	 * Initialize the file system.
	 */
	fs_init(0, 0);
#ifdef FORMAT
	lx_format(ext1, 0);
	lx_format(ext2, 0);
#endif

	/*
	 * Initialize the console.  If no backup info set, use default router
	 * as the default mail server.
	 */
	if (console_init() != 0) {
		printf("Console did not initialize!\n");
		lx_format(ext1, 0);
		lx_format(ext2, 0);

		/* SMTP server same as default router.. */
		ifconfig( IF_ANY,
					IFG_ROUTER_DEFAULT, & smtp_server,
					IFS_END );
		rc = smtp_setserver( inet_ntoa( smtp_dotted, smtp_server ) );
		if( SMTP_OK != rc ) {
			printf( "ERROR: Can't set SMTP server, rc %d\n" );
			exit(1);
		}

		/*
		 * Save the backup information to the console.
		 */
		con_backup();
	}

	while (1) {
		/*
		 * console_tick() drives the console.
		 */
		console_tick();
		http_handler();
	}
}