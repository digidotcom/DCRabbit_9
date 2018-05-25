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
   userblock_tcpipconsole.c

	This example of a ZCONSOLE.LIB program demonstrates how the
	console configuration information can be stored in the user
	block.  Check the sample program userblock-info.c for information
	about the user block on your board.  If it is a mirrored user
	block, then the contents will be safe even after an ill-timed
	power cycle.

	This sample also demonstrates the use of the extended TCP/IP
	configuration options, including setting multiple name servers,
	enabling DHCP and ping configuration, and handling multiple
	interfaces.

	NOTE!  It can be surprising when the console loads an old
	configuration from the user block after you've supposedly
	changed the compile-time configuration via MY_IP_ADDRESS style
	macros or in TCP_CONFIG.LIB.  If you need to clear the stored
	configuration, you can use the sample program
	SAMPLES\USERBLOCK\USERBLOCK_CLEAR.C .

	version 1.0 :	Initial coding.
	version 1.1 :	Added  #class auto

********************************************************************/
#class auto

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
 * Default SMTP server
 */
#define SMTP_SERVER "10.10.6.1"

/*
 * Size of the buffers for serial port C.  If you want to use
 * another serial port, you should change the buffer macros below
 * appropriately (and change the console_io[] array below).
 */
#define CINBUFSIZE		255
#define COUTBUFSIZE		255

/*
 * Maximum number of TCP sockets this program can use.  The mail
 * client uses one socket and the telnet interface uses 1 socket.
 */
#define MAX_TCP_SOCKET_BUFFERS 2

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
#define CON_VERSION_MESSAGE "TCP/IP User Block Console Version 1.1\r\n"

/*
 * Defines the message that is displayed on all I/O channels when
 * the console starts up.
 */
#define CON_INIT_MESSAGE CON_VERSION_MESSAGE

/*
 * Indicate that we want to use the user block for our configuration
 * information.
 */
#define CON_BACKUP_USER_BLOCK

/*
 * These ximport directives include the help texts for the console
 * commands.  Having the help text in xmem helps save root code
 * space.
 */
#ximport "samples\zconsole\userblock_tcpipconsole_help\help.txt" help_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_help.txt" help_help_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_echo.txt" help_echo_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_set.txt" help_set_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_set_param.txt" help_set_param_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_show.txt" help_show_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_set_mail.txt" help_set_mail_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_set_mail_server.txt" help_set_mail_server_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_set_mail_from.txt" help_set_mail_from_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_mail.txt" help_mail_txt
#ximport "samples\zconsole\userblock_tcpipconsole_help\help_add_nameserver.txt" help_add_nameserver_txt

#memmap xmem

#use "dcrtcp.lib"
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
int my_show(ConsoleState* state);

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
	{ "ADD NAMESERVER", con_add_nameserver, help_add_nameserver_txt },
	{ "SHOW", con_show_multi, help_show_txt },
	{ "SET MAIL", NULL, help_set_mail_txt },
	{ "SET MAIL SERVER", con_set_mail_server, help_set_mail_server_txt },
	{ "SET MAIL FROM", con_set_mail_from, help_set_mail_from_txt },
	{ "MAIL", con_mail, help_mail_txt },
	{ "SET PINGCONFIG", con_set_icmp_config, help_set_txt },
	{ "SET PINGCONFIG RESET", con_set_icmp_config_reset, help_set_txt },
	{ "SET DHCP", con_set_dhcp, help_set_txt },
// The following command allows the debugging level of the TCP/IP stack
// to be changed.  Note that DCRTCP_DEBUG must be defined for this to
// have any effect.
	{ "SET DEBUG", con_set_tcpip_debug, 0 }
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
 * will be saved to the backup area.  Note that if, for example, the
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
	CONSOLE_TCP_MULTI_BACKUP,
	CONSOLE_SMTP_BACKUP
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
	/*
	 * All initialization of TCP/IP, clients, servers, and I/O
	 * must be done by the user prior to using any console
	 * functions.
	 */
	sock_init();

	/*
	 * Initialize the console
	 */
	if (console_init() != 0) {
		printf("Could not load the console configuration!\n");
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
		tcp_tick(NULL);
	}
}