/************************************************************
	SAMPLES\DOWN_LOAD\dlp_tcp.c

	Z-World, 2002

	This program is provided as a Sample ONLY. It is intended for use
	on Rabbit based boards with 1 or 2 flash chips.  It will listen
	on a TCP telnet port for a reset indication.

	Use this dlp_tcp.c sample in conjunction with one of the dlm_tcp.c,
	dlm_serial_2_flash.c, or dlm_256kflash.c sample programs for 1 or 2
	flash boards, as appropriate.  It is just a small test program to use
	as a downloaded program.

		*			*			*			*			*			*			*

	IMPORTANT:  Ensure that the FTP program's binary image transfer mode
		is enabled when sending the DLP to the DLM!

	IMPORTANT:  System ID Block must be version 2 or greater.

	IMPORTANT:  Test your DLP first as an ordinary program.  If that works,
		then test the DLM hooked up with the programming cable so you can
		see any error messages.  If both those work, you're ready to deploy!

		*			*			*			*			*			*			*

	TCP - telnet NOTES:
	===================

	See dlm_tcp.c for TCP and telnet details.  See Tech Note 224 for
	additional information.

		*			*			*			*			*			*			*

	TWO 256K FLASH MEMORIES OVERVIEW:
	=================================

	The download manager (DLM) is loaded as the primary program in the
	first flash memory of the Rabbit, so it becomes active after reset.
	The downloaded program (DLP) is stored as the secondary program in the
	second flash, so it becomes inactive after reset.

	The DLP uses the second 256K flash and (by default) the second half of
	SRAM.  TCP uses SRAM (xmemory) for its buffers.  This program does not
	use the File System.  Both DLM and DLP have full root memory; it's just
	the amount of xcode and (by default) xdata that is half of normal for
	the board.

	To start the DLP, the DLM remaps the second flash into the MB0CR
	quadrant and then executes the DLP beginning at address 0.  The DLP's
	BIOS maintains the second flash memory mapping as set up by the DLM.
	When the DLP is running, the ID/User Blocks are accessible in the MB1CR
	quadrant.

	To compile the 2 * 256K flash version of the DLP, ensure that the
	Project Options' Defines box contains the following macros:

	COMPILE_SECONDARY_PROG ; DONT_SPLIT_RAM

	Note that the DONT_SPLIT_RAM macro can be omitted if neither the DLM
	nor the DLP have SRAM variables that must be preserved from each other.
	Also ensure that the Project Options' Defines box does not contain any
	of these macros:

	COMPILE_PRIMARY_PROG ; ROUTE_A18_TO_FLASH_A17 ; USE_2NDFLASH_CODE


	ONE 512K FLASH MEMORY OVERVIEW:
	===============================

	The download manager (DLM) is loaded as the primary program in the
	flash memory of the Rabbit, so it becomes active after reset.  The
	downloaded program (DLP) is stored as the secondary program, so it
	becomes inactive after reset.

	The DLP uses the second 256K of flash, less the amount of flash memory
	that is reserved for the ID/User Blocks, and (by default) the second
	half of SRAM.  TCP uses SRAM (xmemory) for its buffers.  This program
	does not use the File System.  Both DLM and DLP have full root memory;
	it's just the amount of xcode and (by default) xdata that is half of
	normal for the board.

	To start the DLP, the DLM remaps the second half of flash into the
	MB0CR quadrant and then executes the DLP beginning at address 0.
	Initially, the DLP's BIOS maintains the flash memory mapping as set up
	by the DLM.  When the DLP is running in flash, the ID/User Blocks are
	accessible in the MB0CR quadrant.

	To compile the 1 * 512K flash version of the DLP, ensure that the
	Project Options' Defines box contains the following macros:

	COMPILE_SECONDARY_PROG ; DONT_SPLIT_RAM

	Note that the DONT_SPLIT_RAM macro can be omitted if neither the DLM
	nor the DLP have SRAM variables that must be preserved from each other.
	Also ensure that the Project Options' Defines box does not contain any
	of these macros:

	COMPILE_PRIMARY_PROG ; ROUTE_A18_TO_FLASH_A17 ; USE_2NDFLASH_CODE


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

	The DLP uses the second 128K of flash, less the amount of flash memory
	that is reserved for the ID/User Blocks, and (by default) the second
	half of SRAM.  TCP uses SRAM (xmemory) for its buffers.  This program
	does not use the File System.  Both DLM and DLP have full root memory;
	it's just the amount of xcode and (by default) xdata that is half of
	normal for the board.

	To start the DLP, the DLM remaps the second half of flash into the
	MB0CR quadrant and then executes the DLP beginning at address 0.
	The DLP's BIOS maintains the flash memory mapping as set up by the DLM.
	When the DLP is running, the ID/User Blocks are accessible in the MB0CR
	quadrant.

	To compile the 1 * 256K flash version of the DLP, ensure that the
	Project Options' Defines box contains the following macros:

	COMPILE_SECONDARY_PROG ; DONT_SPLIT_RAM ; ROUTE_A18_TO_FLASH_A17

	Note that the DONT_SPLIT_RAM macro can be omitted if neither the DLM
	nor the DLP have SRAM variables that must be preserved from each other.
	Also ensure that the Project Options' Defines box does not contain any
	of these macros:

	COMPILE_PRIMARY_PROG ; USE_2NDFLASH_CODE

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
	modification.)  Then, in .\lib\bioslib\StdBIOS.lib, define
   MAX_USERBLOCK_SIZE to be 0x2000 and recompile the BIOS and then the DLP.
   (The BIOS's	MAX_USERBLOCK_SIZE macro reserves that amount of space for
   the ID/User	Blocks.)

		*			*			*			*			*			*			*

	CONFIGURATION:
	==============

	USE_TELNET_PORT		Which TCP port to listen on.

	STDIO_DEBUG_SERIAL	You cannot have the programming cable attacked
								while running the DLP.  However, you can redirect
								STDOUT to a serial port.  Define this with serial
								control register for output, or undef for normal
								operation.

		*			*			*			*			*			*			*

	COMPILING THE DOWNLOADED PROGRAM:
	=================================

	Ensure that the Project Options' Defines box contains the appropriate
	macros for the particular 2* 256K, 1 * 512K, or 1 * 256K flash equipped
	Rabbit board, as listed above in the MEMORY OVERVIEWs.

	In the "Options | Define Target Configuration.." dialog, please select
	the board type, or the specific board in the Board ID pull-down.  Then
	hit OK to register it.

	Compile this program, including the BIOS, to a BIN file.  To do so,
	select the target board type from the Project Options' Targetless list,
	then choose the "Compile > Compile to .bin File > Compile to Flash" menu
	item.  A BIN file will be automatically generated.

	If necessary, copy the BIN file to the machine where the telnet client
	will be run.  This might be a laptop used in the field.

	Compile the DLM program to the Rabbit target.  See the instructions in
	the appropriate one of dlm_tcp.c, dlm_serial_2_flash.c or
	dlm_256kflash.c for details on compiling and running the DLM.

	The DLP image is a binary file, which the FTP server reads directly.
	Note that if you upload the new application in ASCII mode, newline
	translation is done.  Thus, the CRC will be correct but the loaded
	image is wrong.  Then, when the DLM tries to run the DLP the Rabbit may
	hang.  In this case, you'll need to *manually reset* the board to
	regain control.  If you want better control, try the DeviceMate product
	from Z-World.

	Some telnet clients do not echo by default.  This program does not do
	telnet protocol negotiation.

	REBOOTING TO DLM USING telnet:
	==============================

	To reboot the user application (DLP) back to the DLM using telnet,
	you need to connect and answer a question.  This is security so only
	authorized people/programs can reboot the working system.  Connect
	to telnet port 9013 ("unix> telnet targetsys 9013").  The Rabbit will
	present an 8 digit hex value to you.  This is the puzzle; it is
	expecting an answer.  Currently, type back the same number and hit
	<ENTER>.   The target will disconnect, and if the puzzle is solved,
	the board will reboot into the DLM.

	The puzzle strategy uses a Challenge-Response style of proving
	identification.  It assumes only authorized people can solve the
	puzzle, and other people would have a difficult time solving it.
	For each connection the Rabbit generates a value.  It encrypts the
	number and displays it.  Whoever connnected, must decrypt the value
	(thus recovering the original value), apply some algorithm to it,
	encrypting again and send it back.  In the code below, encryption
	is simple XOR, and the algorithm is to change nothing.  Usually, the
	algorthm is simple like adding one.  See ALGO() and CRYPT() macros.

	The Challenge-Response style isn't very strong, but passphrases are
	never sent in cleartext.  If you have some kind of algorithm, then
	writing a program to connect, take in the value, apply alogrithm,
	and then send back the result is the best approach.  A program is
	faster at manipulating bits than a human.

	If the puzzle isn't solved quickly enough, the DLP will drop the
	connect, ignoring the request to reboot.  (See PUZZLE_MASTER_TIMEOUT
	setting below.)

****************************************************************/
#if (_USER)
	#error This sample is not compatible with RabbitSys - RabbitSys includes
   #error native support for remote firmware update.
#endif

#class auto

#memmap	xmem

/** DEBUG **/
// #define STDIO_DEBUG_SERIAL  SBDR
/** DEBUG **/

/*  Which port to listen onto (overrides normal port 23): */
#define  USE_TELNET_PORT	9013

#if !FLASH_COMPILE
#error "Must compile a DLP to flash, to run in flash!"
#error "Incompatible with RAM_COMPILE and FAST_RAM_COMPILE modes."
#endif

#ifdef  COMPILE_PRIMARY_PROG
#error "Cannot compile a DLP as a primary program."
#error "Remove COMPILE_PRIMARY_PROG from the Project Options' Defines box."
#error "Make sure the correct target configuration has been selected."
#endif

#ifndef  COMPILE_SECONDARY_PROG
#error "Must compile a DLP as a secondary program."
#error "Put COMPILE_SECONDARY_PROG in the Project Options' Defines box."
#error "Ensure the Advanced Compiler Options' \"Include BIOS\" is selected."
#endif

#if  __SEPARATE_INST_DATA__
#ifndef  DONT_SPLIT_RAM
#error "Separate I&D space is incompatible with split RAM."
#error "Put DONT_SPLIT_RAM in the Project Options' Defines box."
#endif
#endif

#ifdef ROUTE_A18_TO_FLASH_A17
#if _FLASH_SIZE_ == 0x80
#error "ROUTE_A18_TO_FLASH_A17 is incompatible with single 512K flash."
#error "Remove ROUTE_A18_TO_FLASH_A17 from the Project Options' Defines box."
#endif
#endif

/**
 * 	Turn on a bunch of printf() calls.  Useful while debugging, but not
 * 	during actual deployment since no STDOUT console is available.
 */
// #define DLP_VERBOSE


/* -------------------------------------------------------------------- */


/*  Serial port B can be use for debugging output. */
#ifdef STDIO_DEBUG_SERIAL
  #define STDIO_DEBUG_BAUD	57600L
  #define BOUTBUFSIZE	511
  #define BINBUFSIZE	 31
#endif


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
 * 	Listen on port for telnet connections.  Commands occur on this port.
 */
#define  DLM_PORT		(USE_TELNET_PORT)


/**
 * 	Number of seconds to wait for remote to solve the puzzle.  Take
 * 	into account network latency, especially if satelites are involved.
 */
#define  PUZZLE_MASTER_TIMEOUT 	12


/**
 * 	Encryption (reversable) and the transform algorithm (can be one-way)
 * 	performed by the remote peer.
 * 	If ALGO(x) is "(x)" (no change) then host must respond with
 * 	value sent to them; good for testing.
 */
#define  CRYPT(x) 	((x) ^ 0x98765432L)
#define  ALGO(x)		 (x)
/*  These for testing. Client must respond with 1 added to value. */
// #define  CRYPT(x) 	 (x)
// #define  ALGO(x)		((x)+1)


/********************************
 * End of configuration section *
 ********************************/


/*  Grab that awesome TCP library from Z-World! */
#use dcrtcp.lib


/*
 *		DLMState contains all of the information necessary to manage the
 *		TCP connection.  The state is one of the connection states.
 * 	The buffer is used to temporarily hold the request or header
 * 	lines.  The offset and length are used to send the response in
 * 	buffer[].  The offset is the offset within the buffer[] and the
 * 	length is where the text ends.  The socket structure is used
 * 	internally by DCRTCP to manage its state and buffers.
 */
struct DLMState {
	int 		do_reboot;
	int 		state;
#define DLMS_INIT					0		/* closed or never opened */
#define DLMS_LISTEN				1		/* waiting for a connection */
#define DLMS_AWAITING_RESPONSE	2		/* get the http request line */
#define DLMS_CHECK_RESPONSE	3		/* get any http headers */
#define DLMS_SENDRSP				4		/* send the canned page */
#define DLMS_ABORT 				5 		/* We're closing the connection */
#define DLMS_WAITCLOSE			6		/* wait for the connection to be torn down */

	long 		connect_timeout;	/* when to give up on remote (millisecs) */

	unsigned long 		puzzle;  	/* secret value! */
	tcp_Socket	socket;
	char 		buffer[120];
	int 		offset;
	int 		length;
};

struct DLMState	myState;


/* -------------------------------------------------------------------- */

#if !(_RK_FIXED_VECTORS)
#rvarorg flashdriver resume
#endif
char  Chip_SW_Func [40]; 		// Place in root "memory" to hold switch code.
#if !(_RK_FIXED_VECTORS)
#rvarorg rootdata resume
#endif

void  CopyChipSWtoRAM();
void  _SwitchFlashChip(), _EndSwitchFlash();   // assem labels
#define  SWITCH_FUNC_SIZE	((int)((unsigned)_EndSwitchFlash - (unsigned)_SwitchFlashChip)+1)

void  SwitchChip(int CSOEWE);
void ProcessRestartCommand(char ch);

void RestartDLM(void);
int vwd;


/* -------------------------------------------------------------------- */


/**
 * 	Compute an initial puzzle value and encrypt it.  Format as string
 * 	into buffer in DLMState structure.  Sets up 'offset' and 'length'
 * 	in prepartion for transmission of puzzle.
 */
nodebug void
DLM_generate_challenge( struct DLMState * state )
{
	auto unsigned long 	enc;

	memset( state->buffer, 0, sizeof(state->buffer) );

	state->puzzle = MS_TIMER * 210757UL; 		// Randomize.
	state->do_reboot = FALSE;

	enc = CRYPT( state->puzzle );
	sprintf( state->buffer, "%04x,%04x\r\n",
					(unsigned) (enc >> 16), (unsigned) (enc) );

	state->offset = 0;
	state->length = strlen( state->buffer );
}   /* end DLM_generate_challenge() */


/**
 * 	Puzzle response has arrived and needs to be checked.  Convert the
 * 	string in 'buffer' then decrypt it (encryption algo is two-way).
 * 	Then apply magically algo to our stored puzzle value.  If we solved
 * 	the puzzle same as client, then response is good.  If thre is a
 * 	different response, it's not solved.
 *
 * 	RETURN: (bool) TRUE if same, else FALSE if not solved.
 */
nodebug int
DLM_check_response( struct DLMState * state )
{
	auto unsigned long	enc;
	auto unsigned char *	p;
	auto int 	ch;
	auto int 	same;

	/*
	 *  Convert hex digits to 32-bit value.  Skip punctuation and whitespace.
	 *  Ie, like ".,' "
	 */
	for( enc = 0, p = state->buffer; *p ; ++p ) {
		ch = toupper(*p);
		if( ispunct(ch) || isspace(ch) ) {
			continue;
		}
		if( '0' <= ch && ch <= '9' ) {
			enc = (enc * 16) + (ch - '0');
		} else
		if( 'A' <= ch && ch <= 'F' ) {
			enc = (enc * 16) + (ch - 'A' + 10);
		} else {
			/*  Found a non-hex, non-punct char... that ends it! */
			break;
		}
	}	/*  while hex digits available for conversion.. */

	/*  Now decode the response. */
	enc = CRYPT( enc );

	same = (enc == ALGO(state->puzzle));
#ifdef DLP_VERBOSE
	printf( "check = %d\n", same );
#endif
	return( same );
}   /* end DLM_check_response() */


/**
 *		This function sets the DLMState.state to the initial
 *		state so it will be initialized on the first pass
 *		through the handler.
 *
 */
nodebug size void
DLM_init(struct DLMState* state)
{
	state->state = DLMS_INIT;
	state->length = 0;
}

/**
 * 	State machine to handle telnet-style connections.  If time to reboot,
 * 	then restart the DLM.  If the state buffer holds something (ie,
 * 	0 < length < offset) then sock_fastwrite() as much as we can.
 * 	Otherwise the state machine.  Listen for connection.  Once connected,
 * 	generate a new puzzle and send to client.  If time out, then abort
 * 	connection; else if got a line of input, advance to check response.
 * 	Check response, and it matching, indicate a reboot to the DLM should
 * 	occur.  Abort the TCP connection.
 */
nodebug void
DLM_handler(struct DLMState * state)
{
	auto tcp_Socket* socket;
	auto int bytes_written;

	socket=&state->socket;

	/*
	 *		was the connection reset?  Should we reboot to DLM?
	 */
	if(state->state!=DLMS_INIT && tcp_tick(socket)==0) {
		if( state->do_reboot == TRUE ) {
#if CC_VER >= 0x0730
			ifdown(IF_ETH0);		/* Added in DCR 7.30 */
#else
			pd_powerdown(0);
#endif
			RestartDLM();
			/*NOTREACHED*/
		}
		state->state=DLMS_INIT;
	}

	/*
	 *  If a message is lingering (ie, TCP window prevented complete
	 *  transmission of string), then try to send it out again.
	 *  When "state->length == 0", we can proceed to the next state.
	 *  (See the HTTP form programs for more samples.)
	 */
	if( state->length > 0 ) {
		/* buffer to write out */
		if(state->offset < state->length) {
			state->offset += sock_fastwrite(socket,
									state->buffer + (int)state->offset,
									(int)state->length - (int)state->offset);
		} else {
			state->offset = 0;
			state->length = 0;
		}
		return;
	}

	switch(state->state)
	{
		case DLMS_INIT:
			/*
			 *		passive open on the socket port:  USE_TELNET_PORT
			 */
			if( ! tcp_listen(socket, DLM_PORT, 0L, 0, NULL, 0) ) {
#ifdef DLP_VERBOSE
		    	printf( "ERROR: can't listen on port %d\n", USE_TELNET_PORT );
		    	exit(2);
#endif
				state->state = DLMS_ABORT;
				break;
		   }

			state->state=DLMS_LISTEN;
			state->puzzle = 0;		/* Clear out previous value */
#ifdef DLP_VERBOSE
			printf("\nInitializing socket\n");
#endif
			break;

		case DLMS_LISTEN:
			/*
			 *		wait for a connection.  Once got it, generate a challenge
			 *    and send it off.
			 */
			if(sock_established(socket)) {
				sock_mode(socket,TCP_MODE_ASCII);
#ifdef DLP_VERBOSE
				printf("New Connection\n");
#endif

				/*
				 *  make a challenge.  sets "offset" and "length" so we'll
				 *  transmit the next time we're called.  Once it is
				 */
				DLM_generate_challenge(state);
				state->state=DLMS_AWAITING_RESPONSE;
				state->connect_timeout = SEC_TIMER + PUZZLE_MASTER_TIMEOUT;
			}
			break;

		case DLMS_AWAITING_RESPONSE:
			/*
			 *		Wait for response from challenge.  Timeout if the wait is
			 *    too long...  Response must be at least 4 chars.
			 */
			if( (long)(SEC_TIMER - state->connect_timeout) >= 0 ){
				strcpy( state->buffer, "timeout" );
				state->state = DLMS_ABORT;
				break;
			}

			if(sock_bytesready(socket) >= 0) {
				state->buffer[sizeof(state->buffer)-1] = '\0';
				sock_gets(socket,state->buffer,sizeof(state->buffer)-1);
				if( strlen( state->buffer ) >= 4 ) {
					state->state=DLMS_CHECK_RESPONSE;
#ifdef DLP_VERBOSE
					printf("Response: %s\n",state->buffer);
#endif
				}
			}

			break;

		case DLMS_CHECK_RESPONSE:
			/*
			 *		process the response in "state->buffer". Because we read in
			 * 	ASCII mode, there is no trailing NEWLINE char(s).
			 */
			if( DLM_check_response( state ) ) {
				strcpy( state->buffer, "YES" );
				state->state = DLMS_ABORT;
				state->do_reboot = TRUE;
			} else {
//				strcpy( state->buffer, "didn't solve puzzle" );
				state->state = DLMS_ABORT;
			}
			break;

		case DLMS_ABORT :
			/*
			 * 	Time to force a close up.  Pass "state->buffer" as reason.
			 */
			state->connect_timeout = SEC_TIMER + PUZZLE_MASTER_TIMEOUT/2;
			sock_close(socket);
			state->state=DLMS_WAITCLOSE;
#ifdef DLP_VERBOSE
			printf("Closing Socket: %s\n", state->buffer);
#endif
			break;

		case DLMS_WAITCLOSE:
			/*
			 * 	Come return here until the connection actually closes.
			 * 	"if(state->state!=DLMS_INIT && tcp_tick(socket)==0)"
			 * 	will notice the socket disconnected and reset the state
			 * 	machine.
			 *    If that doesn't happen soon, then force it and re-init.
			 */
			if( (long)(SEC_TIMER - state->connect_timeout) >= 0 ){
				state->state = DLMS_INIT;
#ifdef DLP_VERBOSE
				printf("Abort Socket: forced close.\n");
#endif
				break;
			}
			break;

		default:
			/*
			 *		how did we get here?  programming error?
			 */
			state->state=DLMS_INIT;
			break;
	}
}   /* end DLM_handler() */


/* -------------------------------------------------------------------- */


/**
 * 	Initialize (maybe) DHCP and IP/socket library.  If there are abnormal
 * 	exits here in main(), then there is an error in your code and/or
 * 	network environment.
 */
void main()
{

	/* ASSERT: Make sure buffer is big enough. (remove once working) vvv */
	if( sizeof(Chip_SW_Func) < SWITCH_FUNC_SIZE ) {
		printf( "ERROR: Flash Chip switch code too big.\n" );
		exit(99);
	}
	/* Make sure buffer is big enough. (remove once working) ^^^ */

	sock_init();
	DLM_init( & myState );

	/*
	 *  Timeouts are handled within DLM_handler().  No virtual watchdog is
	 *  used since we don't want user's application resetting itself.
	 *  If host doesn't respond in time, just shut the socket.
	 */
	for(;;) {
		tcp_tick(NULL);
		DLM_handler( & myState );
	}

}  /* end TCP main() */


/* -------------------------------------------------------------------- */


/**
 *  This code is copied into "root data" area and then executed there
 *  (which is MB2CR quadrant physically).
 *
 *  If separate I&D space,
 *	   ENTRY: 	DE = Chip Selects for MB0.
 *  Otherwise (combined I&D space),
 * 	ENTRY: 	HL = Chip Selects for MB0.
 */
#asm nodebug root
_SwitchFlashChip::
#if _RK_FIXED_VECTORS
	ex		de, hl		; shuffle Chip Selects for MB0 back into HL
#endif
	ipset 3							  // turn off interrupts
	ld   a,0x51
	ioi  ld (WDTTR),a
	ld   a,0x54
	ioi  ld (WDTTR),a

#ifdef SPLIT_FLASH_SECONDARY_PROG
	//  Single (256K or 512K) flash, only have to un-invert A18.
	ld   a,0x48
   ioi  ld (MB0CR),a    	// 2 WS, un-invert A18, write protect MB0
#else
	ld   a,(MB0CRShadow)         // get shadow reg
   and  0xf0                    // mask out CS/WE/OE bits
   or 	L							  // set to new CS,WE,OE (HL = parameter #1)
   ioi  ld (MB0CR),a            // load MMU reg
#endif

	ld   hl,0
	jp   (hl)
_EndSwitchFlash::
#endasm

nodebug
void SwitchChip(int CSOEWE)
{
#asm
#if _RK_FIXED_VECTORS
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


/**
 * 	Do a restart into the DLM in quadrant 0 flash.  If TCP used, then
 * 	should shutdown all those services and call tcp_tick() to let FIN
 * 	packets be ACK'ed, then shutdown the ethernet chip.  Since we're
 * 	fiddling with Flash, where code normally lies, we need the switch
 * 	code in RAM.  Do this by copying code into a RAM char array, then
 * 	jump to it.'  The RAM array will appear in root data section.
 *
 * 	But to jump there need a little assembly routine, which we pass
 * 	the MB0CR value.  Can't use CS_FLASH (from bios) cuz that's been
 * 	redefined to indicate the DLP Flash!
 */
nodebug void
RestartDLM()
{

#if CC_VER >= 0x0730
	ifdown(IF_ETH0);		/* Added in DCR 7.30 */
#else
	pd_powerdown(0);
#endif

#if  _RK_FIXED_VECTORS
	//*** put flash switching function in data RAM
	xmem2xmem(paddrDS(Chip_SW_Func), (unsigned long) _SwitchFlashChip,
	          SWITCH_FUNC_SIZE);
#else
	// Put flash switching function in RAM
	memcpy( Chip_SW_Func, _SwitchFlashChip, SWITCH_FUNC_SIZE );
#endif

	/*
	 *  ...and call it.  Do no use CS_FLASH here, since that will select
	 * 	the flash were the DLP lives.  It doesn't select Flash #1.
	 * 	Therefore, explicitly pass in the chip selects _we_ want.
	 */
	SwitchChip(0|0);  // CS0/OE0/WE0 (appears as HL in assem code)
}

