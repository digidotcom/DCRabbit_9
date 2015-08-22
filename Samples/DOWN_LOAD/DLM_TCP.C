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
/***********************************************************
	SAMPLES\DOWN_LOAD\dlm_tcp.c

	THIS PROGRAM IS PROVIDED AS A SAMPLE ONLY!

	It is intended for use ONLY with Rabbit-based boards with one or two
	flash chips in conjunction with the sample program dlp_tcp.c.  This
	version uses TCP FTP to upload the new application image.  It has been
	tested using Ethernet, but not with serial PPP.

	TO DO :
			(1) Store password into flash so it can be updated.

		*			*			*			*			*			*			*

	IMPORTANT:  Ensure that the FTP program's binary image transfer mode
		is enabled when sending the DLP to the DLM!

	IMPORTANT:  System ID Block must be version 2 or greater.

	IMPORTANT:  Test your DLP first as an ordinary program.  If that works,
		then test the DLM hooked up with the programming cable so you can
		see any error messages.  If both those work, you're ready to deploy!

		*			*			*			*			*			*			*

	TWO 256K FLASH MEMORIES OVERVIEW:
	=================================

	The download manager (DLM) is loaded as the primary program in the
	first flash memory of the Rabbit, so it becomes active after reset.
	The downloaded program (DLP) is stored as the secondary program in the
	second flash, so it becomes inactive after reset.

	The DLM uses the first 256K flash and (by default) the first half of
	SRAM.  TCP uses SRAM (xmemory) for its buffers.  This program does not
	use the File System.  Both DLM and DLP have full root memory; it's just
	the amount of xcode and (by default) xdata that is half of normal for
	the board.

	When the DLM is running, the ID/User Blocks are accessible in the MB0CR
	quadrant.  The downloaded program (DLP) is stored in the second 256K
	flash.  To start the DLP, the DLM remaps the second flash into the
	MB0CR quadrant and then executes the DLP beginning at address 0.

	To compile the 2 * 256K flash version of the DLM, ensure that the
	Project Options' Defines box contains the following macros:

	COMPILE_PRIMARY_PROG ; DONT_SPLIT_RAM

	Note that the DONT_SPLIT_RAM macro should be omitted if the DLM and DLP
	have SRAM variables that must be preserved from each other.

	Also ensure that the Project Options' Defines box does not contain any
	of these macros:

	COMPILE_SECONDARY_PROG ; ROUTE_A18_TO_FLASH_A17 ; USE_2NDFLASH_CODE


	ONE 512K FLASH MEMORY OVERVIEW:
	===============================

	The download manager (DLM) is loaded as the primary program in the
	flash memory of the Rabbit, so it becomes active after reset.  The
	downloaded program (DLP) is stored as the secondary program, so it
	becomes inactive after reset.

	The DLM uses the first 256K of flash and (by default) the first half of
	SRAM.  TCP uses SRAM (xmemory) for its buffers.  This program does not
	use the File System.  Both DLM and DLP have full root memory; it's just
	the amount of xcode and (by default) xdata that is half of normal for
	the board.

	When the DLM is running, the ID/User Blocks are accessible in the MB1CR
	quadrant.  The downloaded program (DLP) is stored in the second 256K of
	flash, less the amount of flash memory that is reserved for the ID/User
	Blocks.  To start the DLP, the DLM remaps the second half of flash into
	the MB0CR quadrant and then executes the DLP beginning at address 0.

	To compile the 1 * 512K flash version of the DLM, ensure that the
	Project Options' Defines box contains the following macros:

	COMPILE_PRIMARY_PROG ; DONT_SPLIT_RAM

	Note that the DONT_SPLIT_RAM macro should be omitted if the DLM and DLP
	have SRAM variables that must be preserved from each other.

	Also ensure that the Project Options' Defines box does not contain any
	of these macros:

	COMPILE_SECONDARY_PROG ; ROUTE_A18_TO_FLASH_A17 ; USE_2NDFLASH_CODE


	ONE 256K FLASH MEMORY OVERVIEW:
	===============================

	WARNING #1:  The TCP DLM and DLP are large progams once compiled.  With
		a single split 256K flash, most of it is taken up by the TCP stack,
		FTP and Telnet.  This leaves very little room for an application.
		Customers should consider using either two 256K flashes or one 512K
		flash in their solution.

	WARNING #2:  Since the TCP DLP is a tight fit into only 128K of flash
		(when splitting a 256K flash), the amount of memory given to the
		ID/User Blocks must be reduced.  Because the BIOS searches for ID
		Blocks on 8K boundaries, the ID/User Blocks must be version 2, which
		is a non-mirrored type.  Because version 2 ID blocks don't have a
		backup image in flash, they are not as robust as mirrored versions.
		If sudden power outages are possible, avoid version 2 user blocks.
		Customers should consider using either two 256K flashes or one 512K
		flash in their solution.

	WARNING #3:  The single split 256K flash versions of the DLM and DLP
		require a board-level hardware modification.  The Rabbit CPU's A18
		output must be re-routed to connect to the flash's A17 input.  Many
		Z-World boards have jumper pads with zero ohm resistors installed
		for this purpose.

	The download manager (DLM) is loaded as the primary program in the
	flash memory of the Rabbit, so it becomes active after reset.  The
	downloaded program (DLP) is stored as the secondary program, so it
	becomes inactive after reset.

	The DLM uses the first 128K of flash and (by default) the first half of
	SRAM.  TCP uses SRAM (xmemory) for its buffers.  This program does not
	use the File System.  Both DLM and DLP have full root memory; it's just
	the amount of xcode and (by default) xdata that is half of normal for
	the board.

	When the DLM is running, the ID/User Blocks are accessible in the MB1CR
	quadrant.  The downloaded program (DLP) is stored in the second 128K of
	flash, less the amount of flash memory that is reserved for the ID/User
	Blocks.  To start the DLP, the DLM remaps the second half of flash into
	the MB0CR quadrant and then executes the DLP beginning at address 0.

	To compile the 1 * 256K flash version of the DLM, ensure that the
	Project Options' Defines box contains the following macros:

	COMPILE_PRIMARY_PROG ; DONT_SPLIT_RAM ; ROUTE_A18_TO_FLASH_A17

	Note that the DONT_SPLIT_RAM macro should be omitted if the DLM and DLP
	have SRAM variables that must be preserved from each other.

	Also ensure that the Project Options' Defines box does not contain any
	of these macros:

	COMPILE_SECONDARY_PROG ; USE_2NDFLASH_CODE

		*			*			*			*			*			*			*

	PROJECT OPTIONS' DEFINES BOX MACRO EXPLANATIONS:
	================================================

	COMPILE_PRIMARY_PROG selects, via conditional compilation, the code
	necessary for a DLM (primary) program to run, especially regarding
	memory mapping and ID/User Blocks access.

	COMPILE_SECONDARY_PROG selects, via conditional compilation, the code
	necessary for a DLP (secondary) program to run, especially regarding
	memory mapping and ID/User Blocks access.

	DONT_SPLIT_RAM disables the default setting, via conditional macro
	redefinitions, of the division of SRAM between the DLM (lower half) and
	the DLP (upper half).  When this macro is defined, the DLM and the DLP
	will trample each others' SRAM variables, but this doesn't matter if
	neither has vital data that must be preserved in SRAM.

	Note:  INVERT_A18_ON_PRIMARY_FLASH is obsolete, and no longer has
	effect even if it is defined.

	ROUTE_A18_TO_FLASH_A17 selects, via conditional compilation, the code
	necessary to set up the unusual memory mapping and ID/User Blocks
	access that is required required when the single split-256K flash
	hardware modification is done.

	USE_2NDFLASH_CODE  is not related to putting a secondary program in the
	second flash, and MUST be commented out when compiling this program.

		*			*			*			*			*			*			*

	SINGLE 256K FLASH NOTES:
	========================

	The single split 256K flash version requires a hardware board
	modification.  The Rabbit CPU's A18 output must be re-routed to the
	flash chip's A17 input.  On many Z-World boards, there are zero ohm
	resistors (jumpers) to accomplish this.  Add ROUTE_A18_TO_FLASH_A17 in
	the Project Options' Defines box for both the DLM and the DLP.  If
	neither the DLM nor the DLP have SRAM variables that must be protected
	from the other, then also define DONT_SPLIT_RAM there.

	If you get the compiler error message:

		Out of xmem code space, use a larger ROM/RAM.
	..or..
		Origin xmemcode collides with origin watcode starting at physical address 0xfdc00.

	you may need to decrease the total size of the ID/User Blocks to free
	up flash space for more XMEM code.

	IMPORTANT:  Carefully consider the (negative) effects of altering the
	ID/User Blocks, particularly if your board has stored factory
	calibration constants.  Do not alter the ID/User Blocks if you are
	unsure that it is OK to do so!

	Use the write_idblock.c utility (available in the TN224.ZIP file from
	Z-World's web site) to write an 8K version 2 ID Block.  Change the
	constants in write_idblock.c from 0x4000 to 0x2000 (16K to 8K), and
	force a version 2 ID Block.  (If you've performed the single split 256K
	flash hardware modification, compile and run write_idblock.c as a
	split-256K flash DLM program to avoid undoing and re-doing the hardware
	modification.)  Then, in .\lib\bioslib\StdBIOS.c, define
   MAX_USERBLOCK_SIZE to be 0x2000 and recompile the BIOS and then the DLM.
   (The BIOS's	MAX_USERBLOCK_SIZE macro reserves that amount of space for
   the ID/User	Blocks.)

		*			*			*			*			*			*			*

	COMPILING THE DOWNLOADED PROGRAMS:
	==================================

	Before compiling the DLM to the target, first compile the DLP to a BIN
	file.  Z-World provides dlp_tcp.c, dlp_serial_2_flash.c and
	dlp_256kflash.c as sample downloaded application programs.  Follow the
	instructions in the appropriate downloaded program source to compile a
	DLP that is suitable for your purpose and compatible with the target
	Rabbit board.

	After you've compiled the DLP to a BIN file, if necessary, transfer it
	to a computer with an FTP client.  (If you do this often, make a little
	batch file that copies the file somewhere, renaming it to "image.bin"
	along the way.)

	The dlm_tcp.c DLM's download image is a binary file.  While the serial DLM
	requires an Intel Hex dump file, the TCP DLM's FTP server reads the binary
	file directly.

		*			*			*			*			*			*			*

	COMMON MODIFIABLE CONSTANTS:
	============================

	Here are some constants you might want to check out and modify.

	TCP/IP Things:
	--------------
	* MY_IP_ADDRESS, MY_NETMASK, MY_NETMASK, MY_NETMASK
		Typically identify and network navigation values.  If you're using
		DHCP, these values become fallbacks.
	* FTP_CMDPORT
		Overrides default port of 21 for FTP.  Same as USE_TCP_PORT in DLP.
	* USE_DHCP, DHCP_CHECK
		Have the DLM query for its IP address.

	Timeouts:
	---------
	* IDLE_TIMEOUT_SECS
		How long the DLM waits before rebooting.  If an FTP connection does
		something of significance then the timeout time restarts.  Set high
		for DLM_debugging, and low (maybe 14) while in the field.

	User Identification/Authentication:
	-----------------------------------
	* USER_NAME, USER_PASSWORD
		Identification used by FTP server to determine a valid user.

		*			*			*			*			*			*			*

	COMPILING THE DOWNLOAD MANAGER:
	===============================

	Connect the programming cable to the Rabbit target board.  Ensure that
	the Project Options' Defines box contains the appropriate macros for
	the particular 2* 256K, 1 * 512K, or 1 * 256K flash equipped Rabbit
	board, as listed above in the MEMORY OVERVIEWs.

	You can change the username and/or password.  To use the standard FTP
	port, comment out the line:
					#define FTP_CMDPORT  9011

	The DLM can also use DHCP.  Below, uncomment USE_DHCP and DHCP_CHECK.
	When the DLM boots up it will then ask for its IP address.  Configure
	your DHCP server to provide leases that never expire, otherwise you
	might not be able to reach your board again!

	Compile the dlm_tcp.c program to the target, then disconnect the
	programming cable and reboot the board.  The download manager will now
	be running and listening on the (non-standard) FTP port.  (The above
	steps need only be done once; thereafter the DLM runs after a hardware
	reset or power cycle.)


	DOWNLOAD MANAGER OPERATION:
	===========================

	When the DLM boots up, it listens on an FTP port.  Only the user-
	password combination defined by USER_NAME and USER_PASSWORD (see defines
	below) is valid.  Security by obfuscation is to use a non-standard
	FTP port.  In this case port #9011 is used.  To connect with a command
	line system, try this:

				C:> ftp
				ftp> open targetsys 9011
				Connected to targetsys.factory.com.
				220 Hello! Welcome to ZWorld TinyFTP!
				Name (unknown):

	Normally FTP is invoked with a target host name.  However, if you don't
	specify a host, FTP goes into interactive mode.  The "open" command
	connects to a remote host uses a specific port number.  Normal login
	occurs at this point (tested under Microsoft NT 4.0 and Linux 2.2).
	If you use the standard FTP port, then invoking "ftp targetsys" is OK.

	Once authenticated, the FTP client will find these files:
		* "image.bin"  --  Place to upload the application image (.BIN file).
								The size is neveer correct (ignore it).
		* "reboot.me"  --  Reading here gives lastest upload image size and
								CRC values.  Writing causes DLM to boot into the
								application
		* "SysIDBlock.bin"  --  Reveals the system ID block as a read-only
								file.
		* "status" -- Results of last download attempt.

	(These names can be #define'd to different strings.)

	When transfering files, be sure you're in BINARY mode.  Most FTP clients
	default to ASCII mode, which translates newline character.  After logging
	in, the user should issue "bin" (for binary mode transfers) followed by
	"put image.bin".  You'll need a small file named "reboot.me" to upload
	and boot the user application.  One way is to "get reboot.me" followed
	by "put reboot.me".  The DLM will then close all FTP connections (you'll
	see "421 Service not avaialble, remote server has closed connection")
	and turn off the Ethernet chip.

	Note, the user application is uploaded directly to flash.  If the network
	upload is interrupted midway, then part of the flash will be updated.
	The other half will be the previous contents.  This is bad.  Restart the
	DLM and upload again.

	After the FTP server is shut down, the DLM calls tcp_tick() for two
	more seconds before turing off the Ethernet controller (pd_shutdown()
	added in 7.20).  Booting into the user application requires remapping
	the flash chips.  You can't be executing code in flash when this
	happens.  So, a small assem routine is copied into a RAM `char'
	buffer (Chip_SW_Func[]) to manipulate the MBnCR registers (Memory Bank
	#n Control Reg).

		*			*			*			*			*			*			*

	REBOOTING THE USER APPLICATION:
	===============================

	The source file dlp_tcp.c contains code to listen on a TCP port for a
	reboot command.  If some user can prove they know the password, or get
	the right answer to the puzzle, it will remap the flash and boot into
	the DLM.

	The DLP image is a binary file, which the FTP server reads directly.
	Note that if you upload the new application in ASCII mode, newline
	translation is done.  Thus, the CRC will be correct but the loaded
	image is wrong.  Then, when the DLM tries to run the DLP the Rabbit may
	hang.  In this case, you'll need to *manually reset* the board to
	regain control.  If you want better control, try the DeviceMate product
	from Z-World.

	USE OF FTP_SERVER.LIB:
	======================

	This application uses the FTP server library to provide a well-known
	transfer protocol.  By default, the FTP server library handles read-only
	files.  To allow writing to flash and rebooting the DLM, the operations
	of writing to FTP files is extended by functions in this application.
	The functions are named my_ftp_open(), my_ftp_write() and my_ftp_close().
	Because they replace pieces of the library, the are given specific
	parameters.  Some parameters are unused if they're aren't applicable
	to the specific usage, e.g. directories.

	EXTENSIONS:
	===========

	If the target Rabbit board has 512K SRAM, DONT_SPLIT_RAM is not defined
	in the Project Options' Defines box, and you want more SRAM memory for
	the DLP, you could reuse some from the DLM.  The idea is to have the
	DLM xalloc() some of its unused xmemory, and then leave a pointer in
	the User Block.  When the user application runs, it reads out the
	xmemory pointer.  However, the address is to SRAM that was in MB2CR
	quadrant, but now has been mapped to MB3CR quadrant!  Therefore, the
	DLP must subtract 0x40000uL from the xmemory pointer before using it.


	NOTES:
	======

	The function ProcessProgramByte() assumes that the sector size of the
	second flash is the same as the sector size of the primary flash.
	This may not always be the case, but it should still work if it is
	not.  Flash writing speed would be decreased if the primary flash has
	a smaller sector size.

	This program just barely fits into 128K of code (half of a 256K flash
	memory chip).  To achieve this, make a few adjustments.  In the Project
	Options' Compiler tab, turn off all runtime checking and optimize for
	size, not speed.


************************************************************/
#if (_USER)
	#error This sample is not compatible with RabbitSys - RabbitSys includes
   #error native support for remote firmware update.
#endif

//#class auto

#ifndef ROUTE_A18_TO_FLASH_A17
// Only single split-256K flash has too little xmem code space!
#memmap xmem
#endif

#if !FLASH_COMPILE
#error "Must compile a DLM to flash, to run in flash!"
#error "Incompatible with RAM_COMPILE and FAST_RAM_COMPILE modes."
#endif

#ifndef COMPILE_PRIMARY_PROG
#error "Must compile A DLM as a primary program."
#error "Put COMPILE_PRIMARY_PROG in the Project Options' Defines box."
#error "Must recompile the BIOS also.  Hit Ctrl-Y then recompile."
#endif

#ifdef COMPILE_SECONDARY_PROG
#error "Cannot compile a DLM as a secondary program."
#error "Remove COMPILE_SECONDARY_PROG from the Project Options' Defines box."
#error "Ensure the Advanced Compiler Options' \"Include BIOS\" is selected."
#error "Must recompile the BIOS also.  Hit Ctrl-Y then recompile."
#endif

#if  __SEPARATE_INST_DATA__
#ifndef  DONT_SPLIT_RAM
#error "Separate I&D space is incompatible with split RAM."
#error "Put DONT_SPLIT_RAM in the Project Options' Defines box."
#error "Must recompile the BIOS also.  Hit Ctrl-Y then recompile."
#endif
#endif

#ifdef ROUTE_A18_TO_FLASH_A17
#if _FLASH_SIZE_ == 0x80
#error "ROUTE_A18_TO_FLASH_A17 is incompatible with a single 512K flash."
#error "Remove ROUTE_A18_TO_FLASH_A17 from the Project Options' Defines box."
#error "Must recompile the BIOS also.  Hit Ctrl-Y then recompile."
#endif
#endif

//*** If sector writable 2nd flash, should be equal to the maximum
//***  sector size of the 2nd flash for efficiency. If byte writable,
//***  512 is a good number. Be sure to increase this if a
//***  sector-writable flash with a larger sector might be used
#define MAXSECTORSIZE   1024

#if _BOARD_TYPE_ == RCM2200
//  Define to flash DS3 on RCM2200 while until the restart.
// #define RCM2200_FLASH_LED
#endif

/**
 * 	Turn on a bunch of printf() calls.  Useful while debugging, but not
 * 	during actual deployment since no STDOUT console is available.
 */
// #define DLM_VERBOSE

/*  Debug or not to debug? */
//#define  DLM_debug	debug
//
#define DLM_debug  nodebug


/**********************************************************************
**********************************************************************/
// The physical address of the second flash
#define DLP_PHYSICAL_ADDR 0x40000ul

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
 * configuration.  DHCP is enabled by selecting the appropriate TCPCONFIG.
 */
#define TCPCONFIG 1


/*
 * 	(OVERRIDE) Listen on port for FTP server.  Commands and download image
 * 	all occur on this port (passive).  Delete to use standard FTP port.
 */
#define  FTP_CMDPORT		9011

/*
 * All web server content is dynamic, so we do not need
 * http_flashspec[].  Try to minimize footprint.
 */
#define HTTP_NO_FLASHSPEC

/**
 * 		Make sure we're unique in the intranet.
 */
#define DHCP_CHECK

/**
 * 	(OVERRIDE) Min DHCP retry interval (secs).
 * 	If not defined, defaults to 60.
 */
#define DHCP_MINRETRY 6

//
// This optional string will be sent to the DHCP server as the
// 'class identifier' of this host.  This allows the server to
// "know what it's dealing with".  There is no particular standard
// for this string, but for ease of management Z-World suggests
// colon-separated fields in the form "hardware platform:vendor:
// firmware identifier:version number".  Your DHCP server can be
// configured to recognise these strings in order to select
// appropriate parameters. [NB: it is expected that this string
// be the same for all clones, i.e. not customised for each unit
// of the same product -- the ethernet hardware address is unique
// for each unit and thus identifies particular 'individuals' to
// the DHCP server.  Think of class ID as identifying the "what" but
// not the "who"].
//
#define DHCP_CLASS_ID "Rabbit-TCPIP:Z-World:DHCP-Test:1.0.0"

/**
 * 	This macro causes the MAC address to be used as a unique client
 * 	identifier.
 */
#define DHCP_CLIENT_ID_MAC


/*********************************************************************
 * Network and FTP-Server Configuration
 *********************************************************************/

#undef HTTP_MAXSERVERS
#undef SMTP_SERVER


/**
 * 	Have FTP Server allowing O_WRONLY and list files with "w" attributes.
 * 	NB:  This method is deprecated as of Dynamic C 8.5, but this sample
 * 	     continues to use it for this release in order to ease migration of
 * 	     pre-existing TCP Down Load Manager applications.  The deprecation
 * 	     warning message can be safely ignored.
 */
#define ENABLE_DLM_TCP_SUPPORT  1

/*  ==OVERRIDE==  (default 10) */
#define SAUTH_MAXUSERS	3

/* define a meaningful SERVER groups label */
#define ALL_GROUPS 0xFFFF


/**
 * 	Names for the files FTP presense.  IMAGE_FILENAME connects to the
 * 	second flash (second quadrant).  Writing to REBOOT_FILENAME reboots
 * 	into the application; reading it gives latest size and CRC values.
 * 	The file SYSBLOCK_FILENAME reveals the SystemID block as a read-only file.
 * 	Latest download status stored in STATUS_FILENAME.
 */
#define  IMAGE_FILENAME 	"image.bin"
#define  REBOOT_FILENAME	"reboot.me"
#define  SYSBLOCK_FILENAME	"SysIDBlock.bin"
#define  README_FILENAME	"README"
#define  STATUS_FILENAME	"status"

/**
 * 	After some time of no activity, we either run the DLP or reboot
 *  	ourselves.  Variable 'gRebootTime' is time seconds when that decision
 * 	gets made.  (Use maybe 12 to 20 for in-the-field deployment; use 1500
 * 	for DLM_debugging.)
 */
#define  IDLE_TIMEOUT_SECS 	150


/**********************************************************************
 Password configuration
**********************************************************************/
/*
 * 	Describe the proper user name and password.  Program reverts to
 * 	these when the username is blank.
 */
#define  USER_NAME		"Rabbit"
#define  USER_PASSWORD	"123"


#define DLM_MAX_PW_LEN   12       // max password length
#define DLM_MIN_PW_LEN    0  	 	 // min password length
#define DLM_PASSWORD_LVL  1  	 	 // 0 = runtime changeable

#define USER_ID_BLOCK_PW_OFFSET 0 // Offset into the User Block of where
                                  //  password is stored. The other
                                  //  information kept in the block
                                  //  follows the password immediately.

//*** Used for first time initialization. If this is found in the User ID
//***  Block, then the password should be there too.
#define INITIALIZATION_FLAG  "DLM/TCP/Z-World/Rabbit Semiconductor"


/********************************
 * End of configuration section *
 ********************************/

// #define ZSERVER_DEBUG
// #define FTP_DEBUG
// #define DHCP_VERBOSE
// #define BOOTP_VERBOSE

#use "dcrtcp.lib"
#use "ftp_server.lib"


//  Choose correct flash writing routine.
#ifdef SPLIT_FLASH_PRIMARY_PROG
	/* Single flash, be careful on the writes: */
	#define  zapWriteFlash(dst,src,siz)  WriteFlash(dst,src,siz)
#else
	/* Write to the second flash chip: */
	#define  zapWriteFlash(dst,src,siz)  WriteFlash2(dst,src,siz)
#endif


/* -------------------------------------------------------------------- */

/**
 *  This string is available as the file README_FILENAME (#define'd symbol).
 *  It provides a DLM version number and instructions.
 */
const char  help_text[] = {
	"This is the Rabbit TCP Download Manager, v1.25.\r\n"	\
	"Upload a new DLP image as '" IMAGE_FILENAME "', then write to the file\r\n"	\
	"'" REBOOT_FILENAME "'.  The connection will close and the Rabbit board\r\n"	\
	"will restart your application.  The file '" STATUS_FILENAME "' contains\r\n" \
	"the results (status information) about the most recent download.\r\n"
};


/*
 * This array sets up the error messages that can be generated.
 * CON_STANDARD_ERRORS is a macro that expands to the standard
 * errors that the built-in commands in zconsole.lib uses.  Users
 * can define their own errors here, as well.
 */
/*  These errors correspond to ErrMessage[]: */
#define ERR_CHECKSUM       101
#define INVALID_RECORD_START 102
#define UNKNOWN_RECORD_TYPE  103
#define UNKNOWN_STATE  		  104
#define WRITE_LAST_SECT_ERR  105
#define ERR_WRITE_FLASH      106
#define DLM_OVERLAP_ERR      107
#define CRC_ERROR            108
#define AUTHORIZATION_ERROR  109
#define TIMEOUT_ERROR        110
#define USER_IDBLOCK_IO_ERROR 111


struct hexfileState
{
	unsigned				CRC;
	unsigned long		Size;
	unsigned long		ProgramStartAddr;
	unsigned long		lpStore;
	unsigned long		sectorOffset;
	char  				error;
	char  				nDataBytes;
};
struct hexfileState gDownloadState;

struct _storedDLP
{
	unsigned  		CRC;
	unsigned long	ProgramStartAddr;
	unsigned long	Size;
};
struct _storedDLP storedDLP;

/*  Where in user block (offset) is "storedDLP" kept. */
#define  USERBLK_OFF_STATE 		(USER_ID_BLOCK_PW_OFFSET+DLM_MAX_PW_LEN)


/* -------------------------------------------------------------------- */

/*  SSPEC index where information about image located (see IMAGE_FILENAME). */
int 	gImageIndex;
/*  SSPEC file descriptor for open IMAGE_FILENAME. */
int	fd_gImageIndex;

/*  SSPEC index of writing file to cause reset (see REBOOT_FILENAME). */
int 	gProgParamsIndex;
/*  SSPEC file descriptor for open REBOOT_FILENAME. */
int	fd_gProgParamsIndex;

/*  User ID of user who can download a new application image. */
int 	gUpdaterID;

/**
 * 	Status (error messages) from last download.  All but last char is
 * 	available through the FTP server.  String always ends with CR-NL-NUL.
 * 	As message changes, the ROOTFILE is resized.  There is no overflow
 * 	checking.
 */
char	gStatusFile[ 120 ];
int 	gStatusID;


/* -------------------------------------------------------------------- */

/**
 * 	After some time of no activity, want to call ShutdownAndRunDLP().
 * 	Can't use a virtual watchdog cuz that reboots into same program.
 * 	We want a reboot to the download program.  Hit "watchdog" by
 * 	adding IDLE_TIME_SECS to it.  If "gRebootTime" ever arrives, then
 * 	call ShutdownAndRunDLP().
 */
unsigned long	gRebootTime;


/* -------------------------------------------------------------------- */


/**
 * 	A message was copied into the status file buffer.  Make sure it ends
 * 	with a CR-NL-NUL byte trio.  If there is much space between the end
 * 	of string and end of gStatusFile[], then install a line terminator
 * 	there.  Change size of "file" to match content size.
 */
DLM_debug void
status_update()
{
	gStatusFile[ sizeof(gStatusFile)-3 ] = '\0';
	strcat( gStatusFile, "\r\n" );
	sspec_resizerootfile( gStatusID, strlen(gStatusFile) );
}   /* end status_redo() */


/**
 * 	Install a new message into our "status" file.  A CR-NL-NUL will be
 * 	appended and the "file size" adjusted.
 */
DLM_debug void
status_set( /*const*/ char * mesg )
{
	strncpy( gStatusFile, mesg, sizeof(gStatusFile)-3 );
	status_update();
}   /* end status_set() */


DLM_debug void
status_clear(void)
{
	status_set( "idle" );
}   /* end status_clear() */


/* -------------------------------------------------------------------- */

char 	sectorBuff[MAXSECTORSIZE];
char 	scrBuff[MAXSECTORSIZE]; 		// Re-used for DLM-DLP switch code.
int 	gSectorSize;
int	gFirstSector;


/**
 * 	Before downloading we need to erase the whole second flash chip.
 * 	Hope there is no file system lurking there, cuz it all goes bye-bye...
 */
DLM_debug root void
prepWriteFlash(void)
{

#ifndef SPLIT_FLASH_PRIMARY_PROG
// Avoid attempt at erasing ID/User Blocks in 2nd half of double-mapped
//  split-flash system!
// But, it's OK to erase the entire 2nd flash in a two-flash system.
	hitwd();
	#asm
		ld		bc, 0
	#if __SEPARATE_INST_DATA__
		ld		hl, _EraseFlashChip2
		lcall	_CallRAMFunction
	#else
		call	_EraseFlashChip2
	#endif
	#endasm
#endif //SPLIT_FLASH_PRIMARY_PROG

	// CRC and offset start both init 0.
	memset( & gDownloadState, 0, sizeof(gDownloadState) );

	gFirstSector = TRUE;
	gSectorSize =  _FlashInfo.sectorSize > MAXSECTORSIZE ?
						    MAXSECTORSIZE : _FlashInfo.sectorSize;
}   /* end prepWriteFlash() */


/**********************************************************************
int ProcessProgramByte(char ch);

Description:
	This function processes bytes received from the DLM hex file
	processing state machine and writes them to the 2nd flash. It
	waits until a full sector has been received before writing to
	the flash. Before it writes to the flash, it tells the host
	to pause transmission so data is not lost.

Assumption:
	The program will be received in one contiguous block starting on a
	sector boundary.

Input:
	ch - the byte to be processed

Return value:
	0 == everthing good so far.
	!0 == a write to flash fails in which case the return value of the
			failed call to zapWriteFlash() is returned.

**********************************************************************/

DLM_debug
int ProcessProgramByte(char ch)
{
	static char *sPtr;			/* "static" to save value. */
	auto int 	retval;
#ifdef ROUTE_A18_TO_FLASH_A17
	auto char 		saveMB1CR;
#endif

	//**** If we're starting a sector, reset ptr into temp buffer.
	if(gDownloadState.sectorOffset==0)
	{
		sPtr = sectorBuff;
	}

	retval = 0;

	//**** keep a count program size
	gDownloadState.Size++;
	gDownloadState.sectorOffset++;

	//**** load byte to sector buffer
	*sPtr++ = ch;
	gDownloadState.CRC = getcrc(&ch, 1, gDownloadState.CRC);

	//**** if we have a sector full of data, write it
	if(gDownloadState.sectorOffset == gSectorSize)
	{
#ifdef DLM_VERBOSE
			printf( "Writing offset %06lx  \r", gDownloadState.lpStore );
#endif
#ifndef ROUTE_A18_TO_FLASH_A17
			/*
			 *  Let FLASHWR.LIB know we zapped the flash.  zapWriteFlash() will
			 *  reset this back to FALSE after every call.
			 */
			_assumeF2_erased = TRUE;
#endif

			retval = zapWriteFlash(gDownloadState.lpStore+DLP_PHYSICAL_ADDR,
											sectorBuff, gSectorSize);
			if(gFirstSector)
			{
					//*** gDownloadState.ProgramStartAddr is the start of the program
					gFirstSector = FALSE;
					gDownloadState.ProgramStartAddr = gDownloadState.lpStore+DLP_PHYSICAL_ADDR;
			}
			memset( scrBuff, 0x55, gSectorSize ); 		/* DEBUG */

			//*** read back data to double check
#ifdef ROUTE_A18_TO_FLASH_A17
			// Map MB1CR memory quadrant to first flash (/CS0)
			saveMB1CR = MB1CRShadow;
			WrPortI( MB1CR, & MB1CRShadow, FLASH_WSTATES | 0x00 );

			xmem2root( scrBuff, gDownloadState.lpStore+DLP_PHYSICAL_ADDR, gSectorSize);

			//  Map MB1CR memory quadrant back to original location
			WrPortI( MB1CR, & MB1CRShadow, saveMB1CR );
#else
			xmem2root( scrBuff, gDownloadState.lpStore+DLP_PHYSICAL_ADDR, gSectorSize);
#endif
			if(retval || memcmp(scrBuff, sectorBuff, gSectorSize))
			{
					sprintf( gStatusFile, "memcmp() failed at $%lx ! RETVAL=%d",
									gDownloadState.lpStore+DLP_PHYSICAL_ADDR, retval );
					status_update();
					gDownloadState.error = ERR_WRITE_FLASH;
					retval = FTP_ERR_WRITING;
			}

			gDownloadState.sectorOffset = 0;
			gDownloadState.lpStore += gSectorSize;
	}   /* if got a sector full.. */

	return retval;
}   /* end ProcessProgramByte() */


/**********************************************************************
SYNOPSIS:
	Called after entire program received to see if a partial sector was
	received last.  Updates "storedDLP" with correct values.  Caller must
	write this to persistent storage.
RETURN:	0 == OK,  <0 error values from zapWriteFlash().

**********************************************************************/
DLM_debug
int WriteLastSector(void)
{
	auto int retval;
#ifdef ROUTE_A18_TO_FLASH_A17
	auto char 		saveMB1CR;
#endif

	if(gDownloadState.sectorOffset!=0)
	{
#ifndef ROUTE_A18_TO_FLASH_A17
		_assumeF2_erased = 1;
#endif

		/*  Store to flash.. */
		retval = zapWriteFlash(gDownloadState.lpStore+DLP_PHYSICAL_ADDR,
										sectorBuff,
										(int)gDownloadState.sectorOffset+1);

		/*  Read back to ensure it stuck. */
#ifdef ROUTE_A18_TO_FLASH_A17
		// Map MB1CR memory quardrant to first flash (/CS0)
		saveMB1CR = MB1CRShadow;
		WrPortI( MB1CR, & MB1CRShadow, FLASH_WSTATES | 0x00 );

		xmem2root( scrBuff,
					  gDownloadState.lpStore+DLP_PHYSICAL_ADDR,
					 (int)gDownloadState.sectorOffset+1 );

		//  Map MB1CR memory quadrant back to original location
		WrPortI( MB1CR, & MB1CRShadow, saveMB1CR );
#else
		xmem2root( scrBuff,
					  gDownloadState.lpStore+DLP_PHYSICAL_ADDR,
					 (int)gDownloadState.sectorOffset+1 );
#endif

		if(retval || memcmp(scrBuff,sectorBuff,(int)gDownloadState.sectorOffset+1))
		{
			gDownloadState.error = WRITE_LAST_SECT_ERR;
			return retval;
		}
	}

	/*
	 *  Update "storedDLP" in RAM. Caller must store it.
	 */
	storedDLP.CRC = gDownloadState.CRC;
	storedDLP.ProgramStartAddr = gDownloadState.ProgramStartAddr;
	storedDLP.Size = gDownloadState.Size;

	return 0;
}   /* end WriteLastSector() */


/*********************************************************************
 * int DLP_CRC_OK()
 * Return Value: -1 not done downloading, 0 CRC doesn't match, 1 CRC good.
 *********************************************************************/
int   				computedCRC;		// External for error reporting.
struct _storedDLP UBstoredDLP;

DLM_debug
int DLP_CRC_OK()
{
	auto char prog[0x100];   			// Read chunks of flash memory into here.
	auto unsigned long Size,i;
#ifdef ROUTE_A18_TO_FLASH_A17
	auto char 		saveMB1CR;
#endif

	//**** If download was successful, these were changed ****
	if( !readUserBlock(&UBstoredDLP, USERBLK_OFF_STATE, sizeof(UBstoredDLP)) )
	{
	   if((UBstoredDLP.ProgramStartAddr!=0xfffffffful) && (UBstoredDLP.Size!=0xfffffffful))
   	{
#ifdef ROUTE_A18_TO_FLASH_A17
			// Map memory quardrant 2 to first flash (/CS0)
			saveMB1CR = MB1CRShadow;
			WrPortI( MB1CR, & MB1CRShadow, FLASH_WSTATES | 0x00 );
#endif
		   Size = UBstoredDLP.Size;
   		computedCRC = 0;
	   	//*** CRC function can only handle 255 bytes at a time.
		   for(i=0;i<Size-0xff;i+=0xff)
   		{
	   		xmem2root(prog,UBstoredDLP.ProgramStartAddr+i,(int)0xff);
		   	computedCRC = getcrc(prog, 0xff, computedCRC);
   		}
	   	xmem2root(prog,UBstoredDLP.ProgramStartAddr+i,(int)0xff);
   		computedCRC = getcrc(prog, (char)(Size-i), computedCRC);

#ifdef ROUTE_A18_TO_FLASH_A17
			//  Map memory quadrant 2 back to original location
			WrPortI( MB1CR, & MB1CRShadow, saveMB1CR );
#endif
#ifdef DLM_VERBOSE
	   	if( computedCRC == UBstoredDLP.CRC ) {
		   	printf( "CRC_OK -- yes\n" );
   		} else {
	   		printf( "CRC_OK -- no.\n" );
	   	}
#endif
	   	if( computedCRC == UBstoredDLP.CRC )
		   	return 1;
   		else
	   		return 0;
		}
	}
	return -1;
}   /* end DLP_CRC_OK() */


/* -------------------------------------------------------------------- */

/**
 * 	Restart this DLM.  Could be cuz of bad download and the DLP is no
 * 	longer coherent, or a timeout during the FTP session.
 */
DLM_debug root void
RestartDLM()
{
#asm
	ipset 3          ; turn off interrupts
	call 0x0000
#endasm
}

/*  Array MUST be in RAM cuz flash memories are getting remapped! */
/* (Re-use the sector buffer. It's big and not needed anymore) */
#define Chip_SW_Func 	scrBuff

void  _SwitchFlashChip(), _EndSwitchFlash();   // assem labels
#define  SWITCH_FUNC_SIZE	((int)((unsigned)_EndSwitchFlash - (unsigned)_SwitchFlashChip)+1)

void  CopyChipSWtoRAM();
void  SwitchChip(int CSOEWE);


DLM_debug
void ShutdownAndRunDLP()
{
	auto unsigned 	tim;

	// Close all sockets first.  All IP stack time to response for TCP close.
	ftp_shutdown(FALSE);
	for( tim = (int)(SEC_TIMER + 3) ; tim != (int)SEC_TIMER ; ) {
		tcp_tick(NULL);
	}
#if CC_VER >= 0x0730
	ifdown(IF_ETH0);		// ifdown() added in 7.30
#else
	pd_powerdown(0);
#endif

#if __SEPARATE_INST_DATA__
	//*** put flash switching function in data RAM
	xmem2xmem(paddrDS(Chip_SW_Func), (unsigned long) _SwitchFlashChip,
	          (unsigned) SWITCH_FUNC_SIZE);
#else
	// Put flash switching function into RAM
	memcpy( Chip_SW_Func, _SwitchFlashChip, SWITCH_FUNC_SIZE );
#endif

	// ...and call it
	SwitchChip(2|0);  // CS2/OE0/WE0
}

/**
 *  This code is copied into "root data" area and then executed there
 *  (which is third quadrant physically).
 * 	ENTRY: 	HL = Chip Selects for MB0 (ignored).
 */
#asm nodebug root
_SwitchFlashChip::
#if __SEPARATE_INST_DATA__
	ex		de, hl		; shuffle Chip Selects for MB0 back into HL
#endif
	ipset 3							  // turn off interrupts
	ld   a,0x51 						// Silence the watchdog.
	ioi  ld (WDTTR),a
	ld   a,0x54
	ioi  ld (WDTTR),a

#ifdef SPLIT_FLASH_PRIMARY_PROG
	//  Single (256K or 512K) flash, only have to invert A18.
	ld 	A, 0x58  				// Invert A18, WriteProtect,/OE0,/WE0,/CS0
	ioi ld	(MB0CR), A
#else
	//  Two flash, swap which gets the "CS" strobe.
	ld   a,(MB0CRShadow)         // get shadow reg
   and  0x38                    // mask out wait-states, CS/WE/OE bits
   or   0x40 | CS_FLASH2  		  		  // set to new CS,WE,OE
   ioi  ld (MB0CR),a            // load MMU reg

   ld 	a, (MB1CRShadow)
   and 	0xF8
   ld 	L, CS_FLASH
   or 	L
   ioi  ld (MB1CR), A
#endif

	ld   hl,0
	jp   (hl)
_EndSwitchFlash::
#endasm

DLM_debug root
void SwitchChip(int CSOEWE)
{
#asm
#if __SEPARATE_INST_DATA__
		; HL holds CS/WE/OE bits... passed into switch code.
		ex		de, hl	; shuffle chip selects for MB0 into DE
		ld		hl, Chip_SW_Func
		lcall	_CallRAMFunction
#else
		; HL holds CS/WE/OE bits... passed into switch code.
		call	Chip_SW_Func
#endif
#endasm
}


/*********************************************************************
UserTaskTick()

Called every pass through the main loop. Control tasks that must be
performed while the DLM runs should handled here.

Return Value: none
*********************************************************************/
DLM_debug
void UserTaskTick(void)
{
	;
}

/* -------------------------------------------------------------------- */

int 	last_access;

/**
 * 	Override so we can postpone a watchdog reset while the user does
 * 	FTP directory listings.
 */
DLM_debug int
my_ftp_list(int item, char *line, int listing, int uid, int cwd)
{
	gRebootTime = SEC_TIMER + IDLE_TIMEOUT_SECS; 	// Hit watchdog!
	return ftp_dflt_list(item, line, listing, uid, cwd, &ftp_servers[0]);
}   /* end my_ftp_list() */


/**
 * 	Verify access rights and maybe open file for remote FTP client.  Once
 * 	opened, handle special files.  If the reboot file is being written to,
 * 	then make sure CRC is OK, then do reboot.
 */
DLM_debug int
my_ftp_open( char *name, int options, int uid, int cwd )
{
	auto int 	fd;

	gRebootTime = SEC_TIMER + IDLE_TIMEOUT_SECS; 	// Hit watchdog!

   if (((0 <= fd_gImageIndex) && !strcmp(name, IMAGE_FILENAME)) ||
       ((0 <= fd_gProgParamsIndex) && !strcmp(name, REBOOT_FILENAME)))
   {
		sprintf(gStatusFile, "ERROR: Can't reopen %s", name);
		status_update();
		return FTP_ERR_WRITING;
   }
	fd = ftp_dflt_open( name, options, uid, cwd, &ftp_servers[0] );
	if (!strcmp(name, IMAGE_FILENAME)) {
   	fd_gImageIndex = fd;	// mark this "special" file as open
   	if (O_WRONLY == options) {
			prepWriteFlash();
			status_set( "ready" );
      }
	} else
	if (!strcmp(name, REBOOT_FILENAME)) {
   	fd_gProgParamsIndex = fd;	// mark this "special" file as open
		switch (options) {
		case O_WRONLY :
			if ((fd = DLP_CRC_OK()) <= 0) {
				if (fd < 0) {
					sprintf( gStatusFile, "ERROR: DLM can't read User Block" );
				} else {
					sprintf( gStatusFile, "ERROR: CRC bad (%d): %u : %u",
									fd, computedCRC, UBstoredDLP.CRC );
				}
				status_update();
				return FTP_ERR_WRITING;
			}
			/*  User cleared thru voice-print identification, so reboot machine... */
			ShutdownAndRunDLP();
			/* Shouldn't ever get here.. */
			return -1;

		case O_RDONLY :
			readUserBlock(&storedDLP, USERBLK_OFF_STATE, sizeof(storedDLP));
			break;
		}
	}

	return fd;
}   /* end my_ftp_open() */


/**
 * 	Accumulate more bytes for flash image update.  Assumes all writes
 * 	are contiguous (ie, "offset" increases by "len" from previous call).
 * 	If not our file, pass to default handler.
 *
 * 	Hits our watchdog to prevent timeout/reboot during transfer.  This could
 * 	leave the second flash in a mixed up state, A Bad Thing.  However, if
 * 	packet within TIMEOUT, then we will reboot back into DLM.
 *
 * 	RETURN:	>= 0	number of bytes copied.
 *					-1 failure.
 */
DLM_debug int
my_ftp_write( int fd, char *buf, long offset, int len )
{
	auto char * p;
	auto int 	orig_len;

	if (fd != fd_gImageIndex) {
		return ftp_dflt_write(fd, buf, offset, len, &ftp_servers[0]);
	}

	gRebootTime = SEC_TIMER + IDLE_TIMEOUT_SECS; 	// Hit watchdog!

	orig_len = len;
	gDownloadState.lpStore = offset & ~(gSectorSize-1L);	/* Allow restarting of xfer (desired?) */

	for (p = buf; --len >= 0; ++p) {
		if (ProcessProgramByte(*p) != 0) {
			return -1;
		}
	}

	return orig_len;
}   /* end my_ftp_write() */


/**
 * 	Close a file.  Access to it is no longer needed.
 * 	If not our file, pass to default handler.
 * 	RETURN:	0 == OK.
 */
DLM_debug int
my_ftp_close(int fd)
{
	auto int 	rc;

	if (fd == fd_gProgParamsIndex) {
   	fd_gProgParamsIndex = -1;	// mark this "special" file as closed
		if (last_access != O_WRONLY) {
			return ftp_dflt_close(fd, &ftp_servers[0]);
		}
	}
	else
	if (fd == fd_gImageIndex) {
   	fd_gImageIndex = -1;	// mark this "special" file as closed
		if (0 == gDownloadState.error) {
			/*  Issue with using FTP server: after CLOSE, if write fails can't signal user. */
			rc = WriteLastSector();
#ifdef DLM_VERBOSE
			printf( "WriteLastSector() --> %d\n", rc );
#endif
			if ( 0 != rc) {
				return FTP_ERR_UNAVAIL;
			}

			//*** store "storedDLP" info in UID block
			//*** store "storedDLP" info in flash constants
			if ((rc = writeUserBlock(USERBLK_OFF_STATE, &storedDLP, sizeof(storedDLP))))
			{
				sprintf( gStatusFile, "ERROR: can't update UserBlock %d.\n", rc );
				status_update();
				return FTP_ERR_UNAVAIL;
			} else {
				/* life is good. */
				sprintf(gStatusFile, "GOOD: %ld bytes received",
								gDownloadState.sectorOffset + gDownloadState.lpStore);
				status_update();
			}
			return 0;
		}
	}

	return ftp_dflt_close(fd, &ftp_servers[0]);
}   /* end my_ftp_close() */


/**
 * 	Return size of some "file object".  "fd" is usually an SSPEC index.
 * 	In version 7.30 + called for directory listing! (but not before)
 *
 * 	RETURN: 	>= 0 size of object, when constant
 *					-1 size cannot be determined.
 */
DLM_debug long
my_ftp_getfilesize( int fd )
{

	if( fd == fd_gImageIndex ) {
#ifdef ROUTE_A18_TO_FLASH_A17
		/* Half of 256K is 128K: */
		return 0x20000L;
#else
	#if _FLASH_SIZE_ == 0x80
   	/* Half of 512K is 256K: */
   	return 0x40000L;
	#else
		/* Size is 4K blocks in second flash: */
		return (long) SysIDBlock.flash2Size << 12L;
	#endif
#endif
	}
	return ftp_dflt_getfilesize(fd, &ftp_servers[0]);
}   /* end my_ftp_getfilesize() */


/*
 *  (If the ftp_dflt_XXXX functions are unknown, you must upgrade to at
 *   least DCR 7.20.  The FTP server was improved then.)
 */
const FTPhandlers myFtpHandlers = {
	my_ftp_open,
   NULL, // use default read handler
	my_ftp_write,
   my_ftp_close,
	my_ftp_getfilesize,
   my_ftp_list,
	/* Following added in 7.20: */
	NULL, // use default cd handler
	NULL	// use default pwd handler
#ifdef FTP_EXTENSIONS
	, NULL,	// use default mdtm handler
	NULL		// use default delete handler
#endif
};


/* -------------------------------------------------------------------- */

/**
 * 	Initialize (maybe) DHCP and IP/socket library.  If there are abnormal
 * 	exits here in main(), then there is an error in your code and/or
 * 	network environment.
 */
int 	itemp;

void main()
{
#ifdef DLM_VERBOSE
	auto char 	ipstr[20];
#endif
	auto int 	retval;

#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	// Necessary for initializing RS232 functionality of LP35XX boards.
	brdInit();
#endif

	if( SWITCH_FUNC_SIZE > sizeof(Chip_SW_Func) ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: Flash switch routine too big for root area.\n" );
#endif
		exit(2);
	}

	memset(&gDownloadState,0,sizeof(gDownloadState)); // initialize the data structure

   retval = sock_init();
   switch (retval) {
   	default:
   		break;
   	case 1:
#ifdef DLM_VERBOSE
   		printf("Could not initialize packet driver.\n");
#endif
   		exit(1);
   }

#ifdef DLM_VERBOSE
	printf( "my ip: %s\n", inet_ntoa( ipstr, MY_ADDR(IF_DEFAULT) ) );
#endif

   /*
    *  Create an "authorized user" into the system.  To update the file,
    *  external clients must use this name/password.
    */
	gUpdaterID = sauth_adduser(USER_NAME, USER_PASSWORD,
                              SERVER_FTP | SERVER_WRITABLE);
	if( gUpdaterID < 0 ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: Can't add FTP user \"" USER_NAME "\"!\n" );
#endif
		exit(2);
	}
   retval = sauth_setusermask(gUpdaterID, ALL_GROUPS, NULL);
	retval = sauth_setwriteaccess( gUpdaterID, SERVER_FTP );

	/*
	 * 	Enter the application image area as a named file.  An illusion.
	 * 	SSPEC requires the length be the first 4 bytes.  But it ain't.
	 * 	Therefore, we must override FTP get-file-size.
	 */
	gImageIndex = sspec_addxmemfile( IMAGE_FILENAME, DLP_PHYSICAL_ADDR,
												SERVER_FTP | SERVER_WRITABLE );
	if( gImageIndex < 0 ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: can't add '" IMAGE_FILENAME "'!\n" );
#endif
		exit(2);
	}
   /*
    * 	Mark the application image file as NOT opened.
    */
	fd_gImageIndex = -1;	// impossible SSPEC file descriptor value
	retval = sspec_setuser( gImageIndex, gUpdaterID );

	/*
	 * 	Add System ID block as read-only file.  Don't need its ID around.
	 * 	Besides, FTP will access RAM copy of block.
	 */
	retval = sspec_addrootfile( SYSBLOCK_FILENAME,
											(char *) & SysIDBlock, sizeof(SysIDBlock),
											SERVER_FTP );
	if( retval < 0 ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: can't add '" SYSBLOCK_FILENAME "'!\n" );
#endif
		exit(2);
	}
	retval = sspec_setuser( retval, gUpdaterID );

	/*
	 * 	"status" holds results of last download.  It either contains "OK"
	 * 	on success, or the description of the error on failure.  The
	 * 	string always ends with a CR-NL pair, then a NUL byte.  The size
	 * 	is readjusted as needed.
	 */
	gStatusID = sspec_addrootfile( STATUS_FILENAME,
											(char *) & gStatusFile, 0,
											SERVER_FTP );
	if( gStatusID < 0 ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: can't add '" STATUS_FILENAME "'!\n" );
#endif
		exit(2);
	}
	retval = sspec_setuser( gStatusID, gUpdaterID );
	status_clear();
	gDownloadState.error = 0;

	/*
	 * 	Add a README file (online documentation).  Don't need its ID around.
	 */
	retval = sspec_addrootfile( README_FILENAME,
											(char *) help_text, sizeof(help_text)-1,
											SERVER_FTP );
	if( retval < 0 ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: can't add '" README_FILENAME "'!\n" );
#endif
		exit(2);
	}
	retval = sspec_setuser( retval, gUpdaterID );


	/*  Should really read these values from flash */
	storedDLP.CRC = ~0;
	storedDLP.ProgramStartAddr = ~0UL;
	storedDLP.Size = ~0UL;
	gProgParamsIndex = sspec_addrootfile( REBOOT_FILENAME,
												(char *) & storedDLP, sizeof(storedDLP),
												SERVER_FTP | SERVER_WRITABLE );
	if( gProgParamsIndex < 0 ) {
#ifdef DLM_VERBOSE
		printf( "ERROR: can't add '" REBOOT_FILENAME "'!\n" );
#endif
		exit(2);
	}
   /*
    * 	Mark the reboot file as NOT opened.
    */
	fd_gProgParamsIndex = -1;	// impossible SSPEC file descriptor value

	retval = sspec_setuser( gProgParamsIndex, gUpdaterID );
   if (retval < 0) {
#ifdef DLM_VERBOSE
   	printf("Error, sspec_setuser failure!  (Invalid sspec or user ID?)\n");
#endif
		exit(2);
   }

	retval = sspec_addrule("/", NULL, ALL_GROUPS, ALL_GROUPS, SERVER_FTP, 0, NULL);
   if (retval < 0) {
#ifdef DLM_VERBOSE
   	printf("Error, sspec_addrule failure!  (Rule table full?)\n");
#endif
		exit(2);
   }

	retval = sspec_addrule("/image.bin", NULL, ALL_GROUPS, ALL_GROUPS, SERVER_FTP, 0, NULL);
   if (retval < 0) {
#ifdef DLM_VERBOSE
   	printf("Error, sspec_addrule failure!  (Rule table full?)\n");
#endif
		exit(2);
   }

	retval = sspec_addrule("/reboot.me", NULL, ALL_GROUPS, ALL_GROUPS, SERVER_FTP, 0, NULL);
   if (retval < 0) {
#ifdef DLM_VERBOSE
   	printf("Error, sspec_addrule failure!  (Rule table full?)\n");
#endif
		exit(2);
   }

   retval = sauth_getwriteaccess(gUpdaterID);
   if (retval < 0) {
#ifdef DLM_VERBOSE
   	printf("Error, sauth_getwriteaccess failure!  (Invalid user ID?)\n");
#endif
		exit(2);
   }
   else if (!(retval & SERVER_FTP)) {
#ifdef DLM_VERBOSE
   	printf("Error, no write access to the FTP server!\n");
#endif
		exit(2);
   }

	ftp_init( & myFtpHandlers );

#ifdef RCM2200_FLASH_LED
	/*  1. Convert the I/O ports.  Disable slave port which makes
	 *     Port A an output, and PORT E not have SCS signal.
	 */
	WrPortI(SPCR, & SPCRShadow, 0x84);

	/*  2. Read function reg shadow and set PE1 and PE7 as normal I/O.
	 *     LED's are connected to PE1 and PE7, make them outputs.
	 *     Using shadow register preserves setup of other Port E bits.
	 */
	WrPortI(PEFR,  & PEFRShadow,  ~((1<<7)|(1<<1)) & PEFRShadow);
	WrPortI(PEDDR, & PEDDRShadow, (1<<7)|(1<<1));

	/*  3. Turn on DS2 (0 -> PE1) and turn off DS3 (1 -> PE7).
	 */
	WrPortI(PEDR, & PEDRShadow, (1<<7));
#endif

	/*
	 *  Count down time when there is no FTP connection.  While connected
	 *  to an FTP client and receiving file commands, the timeout is reset.
	 */
	gRebootTime = SEC_TIMER + IDLE_TIMEOUT_SECS;

	while( (long)(gRebootTime - SEC_TIMER) >= 0 ) {
		ftp_tick();
		UserTaskTick();
#ifdef RCM2200_FLASH_LED
		retval++;
		if( 0 == (retval & 0x03FF) ) {
			if( 0 != (retval & 0x0400) ) {
				WrPortI(PEB7R, NULL, 0xff);  // turn LED DS3 off
			} else {
				WrPortI(PEB7R, NULL, 0);     // turn LED DS3 on
			}
		}
#endif
	}


	/*  Well, waited long enough.  Let's just run the DLP. */
	if( DLP_CRC_OK() == 1 ) {
		strcpy( gStatusFile, "GOOD: DLP's CRC is OK, starting the DLP." );
		status_update();
		ShutdownAndRunDLP();
	}

	/*  Bad CRC ... Leave an error message in battery-backed SRAM */
	strcpy( gStatusFile, "ERROR: CRC bad, so doing DLM restart..." );
	status_update();
	RestartDLM();
	/*NOTREACHED*/
} 	/* end main() */

