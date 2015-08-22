/********************************************************************
   basicconsole.c
   Z-World, 2001

	This program can be used for boards that have a second flash.
	Don't forget to make space in "RabbitBIOS.c"
		#define XMEM_RESERVE_SIZE  0x6000L  //  Amount of space in the first flash ..

	This sample program could serve as a skeleton for a new program
	that uses ZCONSOLE.LIB.  Only a "HELLO WORLD" and "ECHO" commands
	are implemented.

	version 1.0 :	Initial coding.
	version 1.1 :	Added  #class auto

********************************************************************/
#class auto

/*
 * Size of the buffers for serial port C.  If you want to use
 * another serial port, you should change the buffer macros below
 * appropriately (and change the console_io[] array below).
 */
#define CINBUFSIZE		1023
#define COUTBUFSIZE		255

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
#define FS_MAX_FILES 2

/*
 * Log(base 2) of the desired Logical Sector size.
 */
#define MY_LS_SHIFT 12	// 2^12 == 4096

/*
 *	Uncomment the following to use a filesystem in the first
 * flash (128Kb by default, but the minimum with XMEM_RESERVE_SIZE
 * in RABBITBIOS is taken--make sure XMEM_RESERVE_SIZE is set).
 */
//#define FS2_USE_PROGRAM_FLASH	128

/*
 * The number of console I/O streams that this program supports.
 * Since we are only supporting serial port C, then there is only
 * one I/O stream.
 */
#define NUM_CONSOLES 1

/*
 * Defines the message that is displayed on all I/O channels when
 * the console starts up.
 */
#define CON_INIT_MESSAGE "BasicConsole Version 1.1\r\n"

#memmap xmem

#use "fs2.lib"

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
	CONSOLE_IO_SERC(57600)
};

/*
 * This array defines the commands that are available in the console.
 * The first parameter for the ConsoleCommand structure is the
 * command specification--that is, the means by which the console
 * recognizes a command.  The second parameter is the function
 * to call when the command is recognized.  The third parameter is
 * the location of the #ximport'ed help file for the command.
 */
const ConsoleCommand console_commands[] =
{
	{ "HELLO WORLD", hello_world, 0 },
	{ "ECHO", con_echo, 0 }
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
	CONSOLE_BASIC_BACKUP
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

	// Some boards required brdInit() .
	// This is necessary for initializing RS232 functionality
	// of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

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
	 * Initialize the file system.
	 */
	fs_init(0, 0);

	/*
	 * Initialize the console
	 */
	if (console_init() != 0) {
		printf("Console did not initialize!\n");
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
	}
}
