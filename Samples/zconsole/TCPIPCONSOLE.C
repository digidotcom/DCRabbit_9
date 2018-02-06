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
   tcpipconsole.c

	This program can be used for boards that have a second flash.

	This sample program demonstrates many of the features of
	ZCONSOLE.LIB with FS2.LIB.

	Among the features this sample program supports is network
	configuration, uploading web pages, changing variables for use
	with web pages, sending mail, and access to the console through
	a telnet client.

	The program uses a large amount of root code.  If you get an out
	of root code space error when compiling this program, you will
	need to change the DATAORG in the BIOS to increase the amount
	of root code space available.

	Open BIOS/RABBITBIOS.C and search for DATAORG.  Change the value
	from 0x6000 to 0x7000.  Close the BIOS and recompile the program.

	version 1.0 :	Initial coding.
	version 1.1 :	Added  #class auto

********************************************************************/
#class auto

/*
 * Uncomment the following line to force the filesystem to be
 * reformatted.
 */
//#define FORMAT

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
 * Define the address of the SMTP server
 */
#define SMTP_SERVER		"10.10.6.1"

/*
 * Size of the buffers for serial port C.  If you want to use
 * another serial port, you should change the buffer macros below
 * appropriately (and change the console_io[] array below).
 */
#define CINBUFSIZE		1023
#define COUTBUFSIZE		255

/*
 * Maximum number of connections to the web server.  This indicates
 * the number of sockets that the web server will use.
 */
#define HTTP_MAXSERVERS 2

/*
 * Maximum number of TCP sockets this program can use.  The web server
 * is taking two sockets (see above), the mail client uses one
 * socket, and the telnet interface uses 1 socket.
 */
#define MAX_TCP_SOCKET_BUFFERS 4

/*
 * All web server content is dynamic, so we do not need the
 * http_flashspec[] array.
 */
#define HTTP_NO_FLASHSPEC

/*
 * Maximum number of entries in the TCP servers' object list (in
 * this case, the total number of variables and files that can
 * be created)
 */
#define SSPEC_MAXSPEC  30

/*
 * Partition the filesystem so that 1/16th (0x01000/0x10000) of
 * the filesystem is reserved for saving the configuration
 * information.  This is necessary so that the main filesystem
 * area never takes up so much space that the configuration
 * information cannot be saved.  For a small filesystem, it may
 * be necessary to increase the following fraction to allow for
 * a minimum of 4 physical sectors in each partition.
 */
#define CONFIG_FRACTION	0x1000

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
#define MY_LS_SHIFT 9	// 2^9 == 512

/*
 *	Uncomment the following to use a filesystem in the first
 * flash (128Kb by default, but the minimum with XMEM_RESERVE_SIZE
 * in RABBITBIOS is taken--make sure XMEM_RESERVE_SIZE is set).
 */
//#define FS2_USE_PROGRAM_FLASH	128

/*
 * Console configuration
 */

/*
 * The number of console I/O streams that this program supports.
 * Since we are supporting serial port C and telnet, then there
 * are two I/O streams.
 */
#define NUM_CONSOLES 2

/*
 * If this macro is defined, then the version message will be shown
 * with the help command (when the help command has no parameters).
 */
#define CON_HELP_VERSION

/*
 * Defines the version message that will be displayed in the help
 * command if CON_HELP_VERSION is defined.
 */
#define CON_VERSION_MESSAGE "TCP/IP Console Version 1.1\r\n"

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
#ximport "samples\zconsole\tcpipconsole_help\help.txt" help_txt
#ximport "samples\zconsole\tcpipconsole_help\help_help.txt" help_help_txt
#ximport "samples\zconsole\tcpipconsole_help\help_echo.txt" help_echo_txt
#ximport "samples\zconsole\tcpipconsole_help\help_set.txt" help_set_txt
#ximport "samples\zconsole\tcpipconsole_help\help_set_param.txt" help_set_param_txt
#ximport "samples\zconsole\tcpipconsole_help\help_set_mail.txt" help_set_mail_txt
#ximport "samples\zconsole\tcpipconsole_help\help_set_mail_server.txt" help_set_mail_server_txt
#ximport "samples\zconsole\tcpipconsole_help\help_set_mail_from.txt" help_set_mail_from_txt
#ximport "samples\zconsole\tcpipconsole_help\help_show.txt" help_show_txt
#ximport "samples\zconsole\tcpipconsole_help\help_put.txt" help_put_txt
#ximport "samples\zconsole\tcpipconsole_help\help_get.txt" help_get_txt
#ximport "samples\zconsole\tcpipconsole_help\help_delete.txt" help_delete_txt
#ximport "samples\zconsole\tcpipconsole_help\help_list.txt" help_list_txt
#ximport "samples\zconsole\tcpipconsole_help\help_createv.txt" help_createv_txt
#ximport "samples\zconsole\tcpipconsole_help\help_putv.txt" help_putv_txt
#ximport "samples\zconsole\tcpipconsole_help\help_getv.txt" help_getv_txt
#ximport "samples\zconsole\tcpipconsole_help\help_mail.txt" help_mail_txt
#ximport "samples\zconsole\tcpipconsole_help\help_reset.txt" help_reset_txt
#ximport "samples\zconsole\tcpipconsole_help\help_reset_files.txt" help_reset_files_txt
#ximport "samples\zconsole\tcpipconsole_help\help_reset_variables.txt" help_reset_variables_txt

#memmap xmem

#use "fs2.lib"
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
 * This function prototype is for a custom command, so it must be
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
	CONSOLE_IO_SERC(57600),
	CONSOLE_IO_TELNET(23)
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

void main(void)
{
	FSLXnum ext1, ext2;
	word newsize;

	/* File system setup and partitioning */

	ext1 = fs_get_flash_lx();
	if (ext1 == 0) {
		printf("No flash available!\n");
		exit(1);
	}
	newsize = CONFIG_FRACTION;
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
	sock_init();
	tcp_reserveport(80);		// Enable SYN-queueing and disable the
									// 2MSL wait for the web server (results
									// in performance improvements).
	http_init();

	/*
	 * Initialize the file system.
	 */
	fs_init(0, 0);
#ifdef FORMAT
	lx_format(ext1, 0);
	lx_format(ext2, 0);
#endif

	/*
	 * Initialize the console
	 */
	if (console_init() != 0) {
		printf("Console did not initialize.\n");
		lx_format(ext1, 0);
		lx_format(ext2, 0);

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