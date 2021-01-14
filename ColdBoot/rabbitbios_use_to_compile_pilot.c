/***********************************************************************
* ******************************************************************** *
* *																					 	* *
* *	Universal BIOS for Z-World/Rabbit Semiconductor Rabbit 2000    * *
* *	CPU based boards.                                              * *

   Copyright (c) 2015 Digi International Inc.

   This Source Code Form is subject to the terms of the Mozilla Public
   License, v. 2.0. If a copy of the MPL was not distributed with this
   file, You can obtain one at http://mozilla.org/MPL/2.0/.

* *	Assumes ATMEL compatible flash algorithms                      * *
* *                                                                  * *
* *******************************************************************  *
************************************************************************/

#define	R3000	0x0100

#ifndef __BIOS_ALREADY_COMPILED
	#define __BIOS_ALREADY_COMPILED
#else
	#fatal "Cannot compile the BIOS as a user program!"
#endif

/////// BEGIN USER MODIFIABLE MACROS /////////////////////////////////

// Include the board-specific initialization header
#use BOARDTYPES.LIB

//***** Miscellaneous Information *******************************************
#define	_RK_FIXED_VECTORS	(__SEPARATE_INST_DATA__  && _CPU_ID_ < R3000_R1)

#ifndef TC_SYSBUF_BLOCK_START
	#define TC_SYSBUF_BLOCK_START	(DATAEND-FLASH_BUF_SIZE-TC_SYSBUF_BLOCK) + 1
#endif

#define PRODUCT_NAME	"Universal Rabbit BIOS Version 7.30"

//#define CLOCK_DOUBLED  1			// Set to 1 to double clock if
											//		Rabbit 2000:	crystal <= 12.9024 MHz,
											//		Rabbit 3000:	crystal <= 26.4192 MHz,
											//  or to 0 to always disable clock doubler

#define CS1_ALWAYS_ON  0         // Keeping CS1 active is useful if your
                                 // system is pushing the limits of RAM
                                 // access time. It will increase power
                                 // consumption a little. Set to 0 to
                                 // disable, 1 to enable

//***** Clock Modulator Information ***************************************
#define ENABLE_SPREADER	1			// Set to 0 to disable spectrum spreader,
											// 1 to enable normal spreading, or
											// 2 to to enable strong spreading.

//***** File System Information *******************************************
#define XMEM_RESERVE_SIZE 0x0000L //  Amount of space in the first flash to
                                  //  reserve. Dynamic C will not use this
                                  //  much flash for xmemory code space.
                                  //  This allows the filesystem to be used on
                                  //  the first flash.

#define FS2_RAM_RESERVE 0			//  Number of 4096-byte blocks to reserve for
											//  filesystem II battery-backed RAM extent.
											//  Leave this at zero if not using RAM filesystem.

//******* Target communication configuration ********************************
#define TC_SYSBUF_SIZE		256	// The size of the internal target communication
											// system buffers. The TC_HEADER_RESERVE size is
											// included in this value, so adjust the size of
											// the buffer accordingly.

#define TC_SYSBUF_COUNT		15		// The number of internal target communication
											// system buffers. Each is of the size
											// TC_SYSBUF_SIZE, as defined above.

#define TC_WRITEBLOCK_SIZE	256	// The size of the buffer that WriteBlocks are
											// received into, and therefor the MTU of a
											// write request.

//******* Macro definitions for compile mode ********************************
// These are determined by the BIOS memory setting compiler option
// The xxxx_COMPILE macros should be used instead of the _xxxxx_ compiler
// macros, since they can be combined in #if statements, such as
// #if FLASH_COMPILE || FAST_RAM_COMPILE
// ...
// #endif
#ifdef _FLASH_
	#define FLASH_COMPILE 1
#else
	#define FLASH_COMPILE 0
#endif

#ifdef _RAM_
	#define RAM_COMPILE 1
#else
	#define RAM_COMPILE 0
#endif

#ifdef _FAST_RAM_
	#define FAST_RAM_COMPILE 1
#else
	#define FAST_RAM_COMPILE 0
#endif

//***** Error Logging information *******************************************
//   Extend RAM for the error logging is pre-allocated in chunks of 4096.
//   The default configuration (when ENABLE_ERROR_LOGGING is 1) uses
//   one 4096 chunk.
#define ENABLE_ERROR_LOGGING   0 // Enable error logging, set to 1 to enable
#define ERRLOG_USE_REG_DUMP    1 //  Include register dump in log entries
#define ERRLOG_STACKDUMP_SIZE 16 //  Include stack dump of this size in entries
#define ERRLOG_USE_MESSAGE     0 // Use this to include an 8 byte message in the
                                 //  log entries. The default exception handler
                                 //  currently makes no use of this.

#define ERRLOG_NUM_ENTRIES    78 // For best use of RAM space, choose a value that
                                 //  that will make a log size close to a multiple
                                 //  of 4096
#if RAM_COMPILE
#if (ENABLE_ERROR_LOGGING==1)
#error "Error logging not available when compiling to RAM"
#undef ENABLE_ERROR_LOGGING
#define ENABLE_ERROR_LOGGING 0
#endif
#endif

/***** Cloning Information *******************************************
 When after cloning starts, the LED on the cloning board goes out.
 It begins toggling about every second after the initial loaders
 are transfered and running.
 When cloning is finished, the LED stays on. If any error occurs
 while cloning the LED will start blinking. Possible errors are
 autobaud failed, flash write failed, checksum incorrect.
**********************************************************************/

#define ENABLE_CLONING         0 // Set to 1 to enable cloning.

#define CL_CHECKSUM_ENABLED    1 // Do Internet checksum for cloning.
                                 //  Setting this to 0 will save some code
                                 //  size and a little time. Uses RFC 1071,
                                 //  1624 method.

#define CL_ASSUME_SAME_CRYSTAL 0 // 0 Assumes crystal speed is different on master
                                 //  and slave. Setting to 1 saves code space
                                 //  for the crystal speed measurement and
                                 //  autobauding. Autobauding works by setting
                                 //  the slave and master to the highest baud
                                 //  rate possible, and lowering the master's
                                 //  baud rate until they match. This MUST be
                                 //  set to 0 if the slave's crystal is or
                                 //  might be slower than the master's. If this is
                                 //  set to 1, make sure CL_DOUBLE_CLONE_CLOCK and
                                 //  CLOCK_DOUBLED are set the same if copying
                                 //  to like speed boards, since no autobauding
                                 //  will take place.

#define STERILE_CLONES         0 // Seeting to 1 causes cloned boards to bypass
                                 //  the check for the cloning cable, thereby
                                 //  disabling cloning of clones

#define CL_RUN_AFTER_COPYING   1 // Set to 0 to not run program on slave after
                                 //  cloning.

#define CLONE_WHOLE_FLASH	    0 // Set to 1 to copy entire flash device to
                                 //  excludes system ID block unless
                                 //  CL_INCLUDE_ID_BLOCKS is set to 1 also.
                                 //  Excludes secondary flash device, but
                                 //  but if the program being cloned extends
                                 //  into the second flash, it will be copied
                                 //  regardless of this setting.

#define CL_COPY_2NDFLASH       0 // Set to 1 to clone second flash also. This
                                 //  is separate from USE_2NDFLASH_CODE so that
                                 //  files in the 2nd flash can be copied too if
                                 //  desired. This will cause the second flash
                                 //  to be copied unconditionally even if the
                                 //  the program doesn't span two flashes.

#define CL_INCLUDE_ID_BLOCKS   0 // Set to 1 to copy system ID and user block
                                 //  areas when cloning. Setting this to 1 will
                                 //  cause the ID blocks to be copied
                                 //  unconditionally.

#define CL_DOUBLE_CLONE_CLOCK  0 // If the cloned board has or might have a
                                 //  crystal speed more than 12.9MHz, this
                                 //  be MUST be set to 0. This macro is
                                 //  independent of the CLOCK_DOUBLED macro. It
                                 //  only applies clock doubling WHILE cloning.

#define INITIAL_SLV_BAUD_DIV   0 // If the slave is faster than, or might be
                                 //  faster than the master, increase this to
                                 //  the value needed to to achieve the fastest
                                 //  common baud rate that works on both boards.
                                 //  0 will always work when the slave crystal
                                 //  speed is equal to the master crystal speed.
                                 //  (This is the value that goes in the divisor
                                 //  register, the actual divisor is this plus 1.)
											//  The divisor register value is determined as
											//  follows:
                                 //  divisor reg. value = (crystal frequency in Hz)
                                 //                        -----------------------  - 1
                                 //                           (32 * baud rate)
                                 //  For achievable baud rates, this should within
                                 //  4% of an integral value.
											//
                                 //  Max baud rate = (crystal frequency in Hz) /32
                                 //
                                 //  If CL_DOUBLE_CLONE_CLOCK = 1, the crystal
                                 //  frequency is the double the crystal frequency.
                                 //
                                 //  Use this macro also if mismatched crystal speeds
                                 //  will prevent the master from achieving a common
                                 //  baud rate with the slave if the slave's div. reg.
                                 //  is 0. For example, set to 4 if the master is 12.8MHz
                                 //  and the slave is 9.2MHz.

#if (ENABLE_CLONING == 1)
	#if RAM_COMPILE==1
		#error "Cannot enable cloning when compiling to RAM."
		#undef ENABLE_CLONING
		#define ENABLE_CLONING 0
	#endif
#endif

//***** Timer A prescale information (Rabbit 3000 only) ****************

//#define	USE_TIMERA_PRESCALE	// The Rabbit 3000 has the ability to
											// run the peripheral clock at the
											// same frequency as the CPU clock
											// instead of the standard CPU/2.
											// Uncomment this macro to enable
											// this feature.

//***** Memory mapping information ************************************
//#define USE_2NDFLASH_CODE      // Uncomment this only if you have a
                                 //  a board with 2 256K flashes, and you
                                 //  want to use the second flash for extra
                                 //  code space. The file system is not
                                 //  compatible with using the second flash
                                 //  for code.

//***** MMIDR definition **********************************************

#if __SEPARATE_INST_DATA__
	#if FLASH_COMPILE || FAST_RAM_COMPILE
		#define MMIDR_VALUE	0x29 | (CS1_ALWAYS_ON << 5)
	#endif
	#if RAM_COMPILE==1
		#define MMIDR_VALUE	0x21 | (CS1_ALWAYS_ON << 5)
	#endif
#else
	#define MMIDR_VALUE  CS1_ALWAYS_ON << 5
#endif // end of __SEPARATE_INST_DATA__

//***** DATAORG definition ********************************************
// Defines the beginning logical address for the data segment

#ifndef DATAORG						// Allow user to override in the compiler options.
#if __SEPARATE_INST_DATA__
#define DATAORG      0x3000		// Begining logical address for data seg.
                                 // A user with a lot of root constants
                                 // can increase this in increments of 0x1000 t
                                 // to as high as 0xB000 to allow more
                                 // root constant space.
                                 // OR, root data space can be increased by
                                 // lowering this to as low as 0x1000.

#else
#define DATAORG      0x6000      // Begining logical address for data seg.
                                 // A user with a lot of root code
                                 // and not alot of root data can increase
                                 // this in increments of 0x1000 to as high
                                 // as 0xB000 to allow more root code space.
                                 // OR, root data space can be increased by
                                 // lowering this to as low as 0x3000.
#endif
#endif

//***** Memory Size Information ********************************************
#if (_RAM_SIZE_==0x80) && !FAST_RAM_COMPILE
											// Number of 4k pages of RAM. _RAM_SIZE_
#define RAM_SIZE 0x40            // is defined internally by Dynamic C
#else										// during the coldboot stage, but can
#define RAM_SIZE _RAM_SIZE_      // changed here. If _RAM_SIZE_ is 0x80 (512K)
#endif                           // we reserve the upper half for xalloc
		                           // and map top of DATA to the top of the
                                 // lower half of RAM. The xalloc initialization
                                 // code will recognize that the space is
                                 // available if RAM_SIZE < _RAM_SIZE_.

#define FLASH_SIZE _FLASH_SIZE_  // Number of 4k pages of flash. _FLASH_SIZE_
                                 // is defined internally by Dynamic C during
                                 // the coldboot stage by reading the flash ID
                                 // int from the flash on CS0, but can be changed
			          				   // here.

#define WATCHCODESIZE    0x200   // Number of root RAM bytes for Watch Code

#define ZERO_OUT_STATIC_DATA  0  // Set to 1 to Zero out static data on startup/reset.
                                 //  Not compatible with protected variables.
                                 //  Does not conflict with GLOBAL_INIT.

//***** Debug Kernel Information ******************************************
											// _DK_ENABLE_BREAKPOINTS_ is defined internally
											// by Dynamic C and is set to 1 to enable
											// breakpoint support in debug kernel, 0 to
											// disable support and reclaim code space

//#define DK_ENABLE_BREAKPOINTS			_DK_ENABLE_BREAKPOINTS_
//#define DK_ENABLE_BREAKPOINTS			0

											// _DK_ENABLE_ASMSINGLESTEP_ is defined
											// internally by Dynamic C and is set to 1
											// to enable assembly level single step
											// support in the debug kernel, 0 to
											// disable support and reclaim code space

//#define DK_ENABLE_ASMSINGLESTEP		_DK_ENABLE_ASMSINGLESTEP_
//#define DK_ENABLE_ASMSINGLESTEP		0

											// _DK_ENABLE_WATCHEXPRESSIONS_ is defined
											// internally by Dynamic C and is set to 1
											// to enable watch expressions in the debug
											// kernel, 0 to disable support and reclaim
											// code space

//#define DK_ENABLE_WATCHEXPRESSIONS	_DK_ENABLE_WATCHEXPRESSIONS_
//#define DK_ENABLE_WATCHEXPRESSIONS	0

//********************************************************************
//  Advanced configuration items follow
//  (Primarily of interest to board makers)
//********************************************************************

#define MAX_USERBLOCK_SIZE 0x8000 // This amount of flash gets excluded from
                                  // the available code space for the user
                                  // block and system ID areas. This could
                                  // be reduced to as little 0x1000, but
                                  // future binary compatibility with large
                                  // sector flash types will be compromised
                                  // for programs that use writeUserBlock()
                                  // readUserBlock(). This should be a
                                  // multiple of 0x1000

////////// END OF USER-MODIFIABLE MACROS /////////////////////////////////

#if (MAX_USERBLOCK_SIZE&0x0fff)
   #error "MAX_USERBLOCK_SIZE must multiple of 0x1000"
#endif
#if (MAX_USERBLOCK_SIZE<0x2000)
	/*
	 *  If there are two copies, then each must be min size of a Flash sector.
	 *  With a 4K sector size, the minimum space reserved for user block is 8K.
	 */
   #error "MAX_USERBLOCK_SIZE must at least 0x2000"
#endif

#if (RAMONLYBIOS == 1)
	#if !RAM_COMPILE
		#fatal "Must compile to RAM when using RAM-only BIOS!"
	#endif
#endif

#ifdef USE_TIMERA_PRESCALE
	#if (_CPU_ID_ != R3000)
		#warns "Timer A prescale exists on Rabbit 3000 only; disabling USE_TIMERA_PRESCALE macro."
		#undef USE_TIMERA_PRESCALE
	#endif
#endif

///////////////////////////////////////////////////////////////////////

//**** These are redefinitions which are done if the macro
//****  COMPILE_SECONDARY_PROG is defined. These cause the RAM
//****  to be split, and the top half used, and they map
//****  quadrant 0 to  CS2/OE0/WE0. This preserves all global and
//****  function-static variables.  For RAM reuse (or small RAM systems),
//****  define DONT_SPLIT_RAM then primary and secondary use all RAM.
//****  Battery backup of RAM isn't useful in this case.
//****  This for situations where 2 co-resident programs are used ONLY,
//****  such as a downloader and a downloaded program.
//****  The downloaded program should generally be the secondary program.
#ifdef  COMPILE_SECONDARY_PROG
	#if RAM_COMPILE==1
		#error "Intended for DOWN_LOAD sample only.  Please undefine COMPILE_SECONDARY_PROG."
	#endif
	#ifdef  INVERT_A18_ON_PRIMARY_FLASH
		/* Single flash, split in half */
		#undef  MB0CR_INVRT_A18
		#define MB0CR_INVRT_A18  1
		#if _FLASH_SIZE_ != 0x40
			#error "Inverting A18 only works on single 256K Flash memory!"
		#endif
   	/* The value 0x20 means 128K partition for Secondary (assume 256K flash) */
	   #undef  FLASH_SIZE
      #define FLASH_SIZE 0x20
	   #undef  CS_FLASH
	   #define CS_FLASH	0x00
   #else
   	/* Two flash memory chips; one here, one there */
	   #undef  CS_FLASH
	   #define CS_FLASH	0x02
   #endif
	#ifndef DONT_SPLIT_RAM
	   #if (_RAM_SIZE_==0x80)
		   #undef _RAM_SIZE_
      	#define _RAM_SIZE_ 0x40
   	#else
	      #if (_RAM_SIZE_==0x40)
		   	#undef _RAM_SIZE_
 	   	   #define _RAM_SIZE_ 0x20
   	   #else
	         #if (_RAM_SIZE_==0x20)
 	   	      #undef _RAM_SIZE_
			      #define _RAM_SIZE_ 0x10
	         #else
   	         #error "unknown RAM size"
	         #endif
		  #endif
   	#endif
   	/* Locate secondary's RAM in upper half of it. */
		#undef  RAM_SIZE
	   #define RAM_SIZE _RAM_SIZE_
	   #undef  RAM_START
   	#define RAM_START 0x80+RAM_SIZE
   #endif	// ifndef DONT_SPLIT_RAM
#endif

//**** These are redefinitions which are done if the macro
//****  COMPILE_PRIMARY_PROG is defined.  These can cause the RAM to be
//****  split, and the bottom half used.  This preserves all global and
//****  function-static variables.  For RAM reuse (or small RAM systems),
//****  define DONT_SPLIT_RAM then primary and secondary use all RAM.
//****  Battery backup of RAM isn't useful in this case.
//****  This for situations where 2 co-resident programs are used ONLY,
//****  such as a downloader and a downloaded program.
//****  The downloader program should generally be the primary program.
#ifdef  COMPILE_PRIMARY_PROG
	#if RAM_COMPILE==1
		#error "Intended for DOWN_LOAD sample only.  Please undefine COMPILE_PRIMARY_PROG."
	#endif
	#ifdef  INVERT_A18_ON_PRIMARY_FLASH
		#if _FLASH_SIZE_ != 0x40
			#error "Inverting A18 only works on single 256K Flash memory!"
		#endif
		/* Flash memory holds two programs, assume 256K memory chip. */
   	/* The value 0x20 means 128K partition for Secondary (assume 256K flash) */
	   #undef  FLASH_SIZE
      #define FLASH_SIZE 0x20
   #endif
	#ifndef DONT_SPLIT_RAM
		#if (_RAM_SIZE_==0x80)
		   #undef _RAM_SIZE_
      	#define _RAM_SIZE_ 0x40
	   #else
   	   #if (_RAM_SIZE_==0x40)
	 		   #undef _RAM_SIZE_
   	   	#define _RAM_SIZE_ 0x20
	      #else
   	      #if (_RAM_SIZE_==0x20)
   				#undef _RAM_SIZE_
	      	   #define _RAM_SIZE_ 0x10
			   #else
   		      #error "Unknown RAM size"
      	   #endif
	      #endif
   	#endif
   	#undef RAM_SIZE
	   #define RAM_SIZE _RAM_SIZE_
   #endif	// ifndef DONT_SPLIT_RAM
#endif

/*  DONT_SPLIT_RAM only for primary or secondary system splits. */
#ifndef COMPILE_PRIMARY_PROG
	#ifndef COMPILE_SECONDARY_PROG
		#ifdef DONT_SPLIT_RAM
			#warns "DONT_SPLIT_RAM ignored."
		#endif
	#endif
#endif

///////////////////////////////////////////////////////////////////////
// ***** BIOS data structures *****************************************

#ifdef CC_VER
	#define __DC__		CC_VER
#endif
typedef unsigned char 		uint8;
typedef unsigned short int	uint16;
typedef short int				int16;
typedef unsigned long int	uint32;
typedef unsigned long int  faraddr_t;
#define FARADDR_NULL	0L

struct _regBuffer {
	unsigned _AF;
	unsigned _BC;
	unsigned _DE;
	unsigned _HL;
	unsigned _SP;
	unsigned _PC;
	unsigned _XPC;
	unsigned _IX;
	unsigned _IY;
	unsigned _AFprime;
	unsigned _BCprime;
	unsigned _DEprime;
	unsigned _HLprime;
};

typedef int word;
typedef struct {
	unsigned short addr;  // address
	unsigned char base;   // base (BBR or CBR)
}	ADDR24_S;

typedef union { unsigned long l;    // long for increment/decrement
					struct { ADDR24_S a; // the address itself
								char flags; // flags associated
						 } aaa;
}	ADDR24;

typedef struct
{
	unsigned    char    Size;
	unsigned    char    Type;
	unsigned    long   Mods;
} TypedInfo;

typedef union
{
	unsigned short  Integer;
	unsigned long   Long;
	float               Float;
	ADDR24          Addr;
} TypedValue;

typedef struct
{
	TypedInfo       TypedData;
	TypedValue      Value;  // value pushed without padding
} TypedArg;

struct progStruct {
	ADDR24      RCB,RCE,    // root code (Begin and End)
					XCB,XCE,    // extended code (Begin and End)
					RDB,RDE,    // root data (Begin and End)
					XDB,XDE,		// extended data (RAM) (Begin and End)
					RCDB,RCDE,  // root constant data (Begin and End)
					HPA;			// Highest address of program in flash (max of RCDE, RCE, XCE)
	unsigned short  auxStkB,// aux stack Begin
					auxStkE,    // end
					stkB,       // stack begin
					stkE,       // end
					freeB,      // free begin
					freeE,      // end
					heapB,      // heap begin
					heapE;      // end
};

struct _errLogEntry
{
	long secTimer;
	ADDR24 address;
	char   errType;
#if (ERRLOG_USE_MESSAGE==1)
	char   message[8];
#endif
#if (ERRLOG_USE_REG_DUMP==1)
	struct _regBuffer regDump;
#endif
#if (ERRLOG_STACKDUMP_SIZE!=0)
	char stackDump[ERRLOG_STACKDUMP_SIZE];
#endif
	char  checksum;
};

struct _errLogInfo
{
	char     status;
	unsigned ExceptionIndex;
	unsigned ExceptionIndexMod;
	unsigned errorsSinceDeployment;
	unsigned HWresetsSinceDeployment;
	unsigned WDTOsSinceDeployment;
	unsigned SWresetsSinceDeployment;
	char     headerChecksum;
};

enum OriginType { UNKNOWN_ORG = 0, RCODORG, XCODORG, WCODORG, RVARORG, XVARORG,
						WVARORG, RCONORG, RESVORG };

typedef struct {
	char type;
   char flags;                //
   unsigned long	paddr;      // Physical address
   unsigned		laddr;      	// Logical address
   unsigned long 	usedbytes;  // Actual number of bytes used or reserved starting from paddr/laddr
   unsigned long  totalbytes; // Total space allocated
} _sys_mem_origin_t;

//***** Error Logging Checks ******************************************
#if (ENABLE_ERROR_LOGGING==1)
#define ERRLOG_LOG_SIZE (ERRLOG_NUM_ENTRIES*sizeof(struct _errLogEntry)+sizeof(struct _errLogInfo))
#if (0x10000ul <= ERRLOG_LOG_SIZE)
#error "Error log size too big"
#endif
#else
#define ERRLOG_LOG_SIZE 0
#endif

#if (ENABLE_ERROR_LOGGING==1)
#define ERRLOG_NBLOCKS (1+ERRLOG_LOG_SIZE/4096)
#else
#define ERRLOG_NBLOCKS 0
#endif

//***** Settings for bank control registers ***************************
#if FLASH_COMPILE  // running in flash
  #define MB0CR_SETTING	FLASH_WSTATES | CS_FLASH  | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
  #if (_FLASH_SIZE_ == 0x80)
    #define MB1CR_SETTING	FLASH_WSTATES | CS_FLASH  | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
  #else
    #define MB1CR_SETTING	FLASH_WSTATES | CS_FLASH2 | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
  #endif

	#define MB2CR_SETTING	RAM_WSTATES | CS_RAM | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
	#define MB3CR_SETTING	RAM_WSTATES | CS_RAM | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)
#endif

#if FAST_RAM_COMPILE // compile to flash running in RAM
  #define MB0CR_SETTING	RAM_WSTATES | CS_RAM  | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
  #define MB1CR_SETTING	RAM_WSTATES | CS_RAM  | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
  #if (RUN_IN_RAM_CS == 0x2) // assumes second RAM on CS1
		#define MB2CR_SETTING	RAM2_WSTATES | CS_RAM2 | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
  #else
		#define MB2CR_SETTING	RAM_WSTATES | CS_RAM  | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
  #endif
  #define MB3CR_SETTING	FLASH_WSTATES | CS_FLASH | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)
#endif

#if RAM_COMPILE  // running in RAM
	#define MB0CR_SETTING	RAM_WSTATES | CS_RAM | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
	#define MB1CR_SETTING	RAM_WSTATES | CS_RAM | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
	#define MB2CR_SETTING	RAM_WSTATES | CS_RAM | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
  	#define MB3CR_SETTING	RAM_WSTATES | CS_RAM | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)
#endif

// ***** System Org Definitions ***************************************
#define STACKORG  		0xD000	// Beginning logical address for stack segment
#define ROOTCODEORG  	0x0000	// Beginning logical address for root code
#define XMEMORYORG   	0xE000	// Beginning logical address for Xmem

/* size of the block reserved for the TC system buffers */
#define TC_SYSBUF_BLOCK	((1L*TC_SYSBUF_SIZE*TC_SYSBUF_COUNT)+TC_WRITEBLOCK_SIZE)
#define TC_SYSBUF_BLOCK4K	1

// This should work for separate I&D!
//***** Compute the SEGSIZE value *************************************
#if (RAM_COMPILE)
#if __SEPARATE_INST_DATA__
#define MEMBREAK    0+(STACKORG/256) + (STACKORG/4096)  // SEGSIZE reg value
#endif
#endif

#if FAST_RAM_COMPILE && (RUN_IN_RAM_CS == 0x2)
	#define MEMBREAK    0+(STACKORG/256) + ((STACKORG-BBROOTDATASIZE)/4096)  // SEGSIZE reg value, minus battery-backed size
#endif

#ifndef MEMBREAK
#define MEMBREAK    0+(STACKORG/256) + (DATAORG/4096)  // SEGSIZE reg value
#endif

//***** Compute Code and Data Xmem sizes ******************************
#ifndef FS_RESERVE_SIZE
#define FS_RESERVE_SIZE 0	// default to no space reserved for the FS in the first flash
	#endif

#if FLASH_COMPILE
	#ifdef USE_2NDFLASH_CODE
		#define XMEMORYSIZE2		FLASH_SIZE*4096L
	#endif
	#if __SEPARATE_INST_DATA__
		#define SID_XMEMORYSIZE     0x10000-STACKORG
		#define XMEMORYSIZE FLASH_SIZE*4096L-0x10000-DATAORG-DATAORG-MAX_USERBLOCK_SIZE-XMEM_RESERVE_SIZE
	#else
		#define XMEMORYSIZE FLASH_SIZE*4096L-DATAORG-MAX_USERBLOCK_SIZE-XMEM_RESERVE_SIZE
	#endif
#endif

// Calculate reserved Xmem size for RAM compile modes
#if __SEPARATE_INST_DATA__ && (RAM_COMPILE || FAST_RAM_COMPILE)
		#define SID_XMEMORYSIZE  0x10000-STACKORG-0x200
		#define XMEM_RAM_RESERVE 0x1E000+FS2_RAM_RESERVE*4096L+TC_SYSBUF_BLOCK // 0x1E000 includes minimum 4k for stack
#elif (RAM_COMPILE || FAST_RAM_COMPILE)
		#define XMEM_RAM_RESERVE 0xE000+FS2_RAM_RESERVE*4096L+TC_SYSBUF_BLOCK // 0xE000 includes minimum 4k for stack
#endif

// Calculate the size of Xmem
#if RAM_COMPILE || FAST_RAM_COMPILE
		#define XMEMORYSIZE   	RAM_SIZE*4096L-(XMEM_RAM_RESERVE)
#endif

#if FAST_RAM_COMPILE
	#if __SEPARATE_INST_DATA__
		#define FLASHXMEMSIZE FLASH_SIZE*4096L-0x10000-DATAORG-DATAORG-MAX_USERBLOCK_SIZE-XMEM_RESERVE_SIZE
	#else
		#define FLASHXMEMSIZE FLASH_SIZE*4096L-DATAORG-MAX_USERBLOCK_SIZE-XMEM_RESERVE_SIZE
	#endif
#endif

//***** Compute Code and Data ORGs ************************************
#if __SEPARATE_INST_DATA__
#define WATCHCODEORG 0xe000
#else // not separate I&D space for both RAM and FLASH
#define WATCHCODEORG 	STACKORG-(0x0200+WATCHCODESIZE)-BBROOTDATASIZE   // if present, move down for battery backed area
#endif

#if __SEPARATE_INST_DATA__
#if RAM_COMPILE || FAST_RAM_COMPILE
#define SID_XMEMORYORG XMEMORYORG + 0x200 // add 200 to allow space for watch code
#else
#define SID_XMEMORYORG XMEMORYORG
#endif
#endif

//***** Compute the MMU segment registers *****************************
//***** the 1 represents space for a 4K flash transfer buffer
#if __SEPARATE_INST_DATA__ || (FAST_RAM_COMPILE && RUN_IN_RAM_CS == 0x2)
	#define DATASEGVAL   0x0
#else
	#define DATASEGVAL   RAM_START + RAM_SIZE - (STACKORG>>12) - (1+ERRLOG_NBLOCKS+FS2_RAM_RESERVE)-TC_SYSBUF_BLOCK4K
#endif

#if __SEPARATE_INST_DATA__
        #define SID_XMEMSEGVAL  (0xff & ((STACKORG-0xe000)/4096))
        #if FLASH_COMPILE
                #define XMEMSEGVAL      (0xff & ((0x10000+DATAORG-0xe000)/4096))
        #else
                #define XMEMSEGVAL      (0xff & ((0x10000+STACKORG-BBROOTDATASIZE-0xe000)/4096))
        #endif

        // We have a flash, which can be used for the file system
        #if FAST_RAM_COMPILE
        			#define FLASHXMEMSEGVAL  (0xff & ((0x10000+DATAORG-0xe000)/4096))
        	#endif
#else
        #if (FAST_RAM_COMPILE && (RUN_IN_RAM_CS == 0x2))
                #define XMEMSEGVAL      (0xff & ((STACKORG-BBROOTDATASIZE-0xe000)/4096))
        #else
                #define XMEMSEGVAL      (0xff & ((DATAORG-0xe000)/4096))
        #endif

        #if FAST_RAM_COMPILE
        			#define FLASHXMEMSEGVAL (0xff & ((DATAORG-0xe000)/4096))
        #endif
#endif

#ifdef USE_2NDFLASH_CODE
	#if RAM_COMPILE==1 || FAST_RAM_COMPILE
	#undef USE_2NDFLASH_CODE
	#warnt "USE_2NDFLASH_CODE disabled when compiling to RAM."
	#endif
#endif

#ifdef USE_2NDFLASH_CODE
#define XMEMSEGVAL2 	(0xff & ((-0xE000)/4096 + FLASH_SIZE))
#endif

#if __SEPARATE_INST_DATA__
#define WATCHSEGVAL RAM_START-1
#elif FAST_RAM_COMPILE // not separate I&D space for both RAM and FLASH
#define WATCHSEGVAL 0
#else
#define WATCHSEGVAL DATASEGVAL
#endif


#if (FAST_RAM_COMPILE && (RUN_IN_RAM_CS == 0x2)) 	// Only if we have a battery-backed RAM on CS1
	#define BBROOTDATASIZE		0x1000					// Size of battery-backed data
	#define BBROOTDATAORG		STACKORG-0x1			// battery-backed root data org
	#if __SEPARATE_INST_DATA__
		#define BBDATASEGVAL	  	0
	#else
		#define BBDATASEGVAL   	RAM2_START + RAM2_SIZE - (STACKORG>>12) - (1+ERRLOG_NBLOCKS+FS2_RAM_RESERVE)-TC_SYSBUF_BLOCK4K
	#endif
#else
	#define BBROOTDATASIZE 0								// Default to zero (does not exist)
#endif

#if __SEPARATE_INST_DATA__
	#define ROOTCONSTORG			0x0												// Beginning of constants
	#define ROOTCONSTSIZE		DATAORG  										// Constant space size
	#define ROOTCONSTSEGVAL 	0x10												// Constant segment offset
	#define ROOTCODESIZE			STACKORG											// Size of root code
	#define ROOTDATAORG			STACKORG-BBROOTDATASIZE-0x1				// Beginning of root data (0x1000 for bbram)
#else // not Separate I&D
	#define ROOTCODESIZE			DATAORG									 		// Size of root code
//BRI:  test!	#define ROOTDATAORG			STACKORG-(0x0200+WATCHCODESIZE)-BBROOTDATASIZE-0x1	 // Beginning of root data
//BRI:  test!
	#define ROOTDATAORG			STACKORG-(0x0200+WATCHCODESIZE)-BBROOTDATASIZE-0x2	 // Beginning of root data
//BRI:  test!	#define ROOTDATAORG			0x7FFF	 // Beginning of root data	//BRI:  test!
#endif

//BRI:  test!	#define ROOTDATASIZE			ROOTDATAORG-DATAORG+0x1						// Size of root data
	#define ROOTDATASIZE			0x0800	// Size of root data	//BRI:  test!


/* marker for where the end of XMEM is */
#define END_OF_XMEMORY	(((XMEMSEGVAL<<12L)+(long)XMEMORYORG+(long)XMEMORYSIZE)&0xFFFFF)

#if FAST_RAM_COMPILE
#define END_OF_FLASH_XMEM (((FLASHXMEMSEGVAL<<12L)+(long)XMEMORYORG+(long)FLASHXMEMSIZE)&0xFFFFF)
#endif

//***** This is to let Dynamic C programs know we are running in Flash or RAM.
//***** _RAM_, and _FLASH_ are defined by the compiler.  RUN_IN_RAM
//***** predates these compiler generated macros.
//***** FAST_RAM_COMPILE mode compiles to flash and runs in RAM
#if FLASH_COMPILE
#define RUN_IN_RAM 0
#else
#define RUN_IN_RAM 1		// RAM_COMPILE || FAST_RAM_COMPILE
#endif

//***** Tell the compiler this is a BIOS ******************************
#pragma Rabbit CompileBIOS

#ifdef USE_2NDFLASH_CODE
#if FS_RESERVE_SIZE > 0
#error "Using second flash for both code and a filesystem is not currently supported."
#endif
#endif

/*-------------------------------------------------------------------
**  CODE ORG statements to determine code locations in physical and
**  logical memory space.
**
**  Type Name     Segment       Bottom       Size			  	   Use
--------------------------------------------------------------------*/
#if __SEPARATE_INST_DATA__
#rcodorg rootcode	0x00          ROOTCODEORG  ROOTCODESIZE		ispace apply
#else
#rcodorg rootcode	0x00          ROOTCODEORG  ROOTCODESIZE		apply
#endif


#if __SEPARATE_INST_DATA__
#xcodorg sid_xmemcode (SID_XMEMSEGVAL)  SID_XMEMORYORG	SID_XMEMORYSIZE			apply
#xcodorg xmemcode (XMEMSEGVAL) XMEMORYORG XMEMORYSIZE follows sid_xmemcode
#else
#xcodorg xmemcode	(XMEMSEGVAL)  XMEMORYORG   XMEMORYSIZE       apply
#endif

#ifdef USE_2NDFLASH_CODE
#xcodorg xmemcod2 (XMEMSEGVAL2) XMEMORYORG 	XMEMORYSIZE2 		follows xmemcode // this needs to change for sepid
#endif

#wcodorg	watcode  (WATCHSEGVAL)  WATCHCODEORG WATCHCODESIZE     apply

/*--------------------------------------------------------------------
**  DATA ORG statements to determine data locations in physical and
**  logical memory space.
**
**  Type Name     Segment        Top          Size			  	 Use
-------------------------------------------------------------------*/
#if __SEPARATE_INST_DATA__
	#rvarorg rootdata	(DATASEGVAL) 	ROOTDATAORG  ROOTDATASIZE 	 dspace apply
#else
	#rvarorg rootdata	(DATASEGVAL) 	ROOTDATAORG  ROOTDATASIZE 	 apply
#endif // End of __SEPARATE_INST_DATA__

#if BBROOTDATASIZE > 0
	#rvarorg bbrootdata (BBDATASEGVAL) BBROOTDATAORG BBROOTDATASIZE batterybacked apply
#endif

#if __SEPARATE_INST_DATA__
#rconorg rootconst (ROOTCONSTSEGVAL) ROOTCONSTORG ROOTCONSTSIZE apply
#endif


//**** internal and external interrupt vector table locations *****************
#define XINTVEC_BASE  STACKORG-0x100
#define INTVEC_BASE STACKORG-0x200

#if __SEPARATE_INST_DATA__
#undef XINTVEC_BASE
#undef INTVEC_BASE

#define XINTVEC_BASE xintvec_relay
#define INTVEC_BASE intvec_relay

#define SID_XINTVEC_BASE STACKORG-0x100
#define SID_INTVEC_BASE	STACKORG-0x200

#if FLASH_COMPILE
#define SID_DATAORG RAM_START
#endif
#if RAM_COMPILE || FAST_RAM_COMPILE
#define SID_DATAORG RAM_START+0x10
#endif

#define INTVEC_RELAY_SETUP(x) $\
		lcall SID_DATAORG-(0xe-(0xf&((x)>>12))), 0xE000 | ((x) & 0x0FFF) $\
		jp sid_intvec_cleanup

// Macro to load 16 bit values into a physical address
#define	PHY_LOAD_VALUE(paddr, value) $\
	ld    a, paddr >> 16 $\
	ld		hl, value $\
	ldp   (paddr & 0xFFFF), hl


// Assembly macros for Separate I&D space
#define SEP_ID_ON 	ld a, MMIDR_VALUE $\
							ioi ld (MMIDR), a
#define SEP_ID_OFF 	ld a, CS1_ALWAYS_ON << 4 $\
							ioi ld (MMIDR), a
#endif

/************************** LOGICAL MEMORY MAP (Not sep I&D space) **********************

  FFFF                          ---------------
                                |             |
                                |             |
                                ~             ~
                                     Xmem
                                ~             ~
                                |             |
                                |             |
  E000                          ---------------
                                | Stack Top   |
                                |             |
                                ~             ~
                                     Stack
                                ~             ~
                                |             |
                                |             |
  STACKORG                      ---------------
                                | internal    |
                                | vector tabl.|
  STACKORG-80h                 ---------------
                                | external    |
                                | vector tabl.|
  STACKORG-100h                 ---------------
                                | internal    |
                                | vector tabl.|
  STACKORG-200h                 ---------------
                                |     |       |
                                |    /|\      |
                                |     |       |
                                ~             ~
                                  Watch Code
                                ~             ~
                                |     |       |
                                |    \|/      |
   WATCHCODEORG                 |     |       |
                                ---------------
   ROOTDATAORG                  |     |       |
                                |    /|\      |
                                |     |       |
                                ~             ~
                                   Root Data
                                ~             ~
                                |     |       |
                                |    \|/      |
                                |     |       |
                                ---------------
                                |     |       |
                                |    /|\      |
                                |     |       |
                                ~             ~
                                   Root Code
                                ~             ~
                                |     |       |
                                |    \|/      |
                                |     |       |
  ROOTCODEORG (0000)            ---------------

*******************************************************************/


//***** Libraries needed by the BIOS *********************************
//#use BIOSFSM.LIB 						// The communication Finite State Machine
#use SYSIODEFS.LIB
#use SYSIO.LIB 						// IO register assignemnts and functions
//#use DBUGKERN.LIB						// The debug kernel
#use CSUPPORT.lib						// Some C support functions
#use MemConfig.lib
#use FLASHWR.lib						// The flash writing functions
#use STACK.lib							// The stack initialization functions
#use IDBLOCK.LIB						// Flash ID block access fcns (and CRC).
#use ERRORS.LIB						// Runtime error handling
//#use TC.LIB                      // New-style target communications
//#use TC_SYSTEMAPP.LIB				// The System-type handler
//#use DKCORE.LIB						// Debug kernel
//#use DKAPP.LIB							// Debug kernel - target communications interface
#use cpuparam.lib						// CPU-specific settings
//#use MUTIL.LIB							// Math support for error logging and other math manipulations in the bios
#if __SEPARATE_INST_DATA__
//#use separateid.lib
#endif

//#use XMEM.LIB
/*
#if (_CPU_ID_ == R2000) && __SEPARATE_INST_DATA__
	#fatal "Separate instruction and data space is not supported by this processor."
#endif
*/
//#pragma DATAWAITSUSED on
/*
#if __SEPARATE_INST_DATA__
#rcodorg	periodic_intvec 	0x0   SID_INTVEC_BASE + PERIODIC_OFS 	0x10 ispace
#rcodorg rst10_intvec 		0x0 	SID_INTVEC_BASE + RST10_OFS 		0x10 ispace
#rcodorg rst18_intvec 		0x0 	SID_INTVEC_BASE + RST18_OFS 		0x10 ispace
#rcodorg rst20_intvec 		0x0 	SID_INTVEC_BASE + RST20_OFS 		0x10 ispace
#rcodorg rst28_intvec 		0x0 	SID_INTVEC_BASE + RST28_OFS 		0x10 ispace
#rcodorg rst38_intvec 		0x0 	SID_INTVEC_BASE + RST38_OFS 		0x10 ispace
#rcodorg slave_intvec		0x0	SID_INTVEC_BASE + SLAVE_OFS 		0x10 ispace
#rcodorg timera_intvec		0x0	SID_INTVEC_BASE + TIMERA_OFS 		0x10 ispace
#rcodorg timerb_intvec		0x0	SID_INTVEC_BASE + TIMERB_OFS 		0x10 ispace
#rcodorg sera_intvec 		0x0 	SID_INTVEC_BASE + SERA_OFS			0x10 ispace
#rcodorg serb_intvec			0x0	SID_INTVEC_BASE + SERB_OFS 		0x10 ispace
#rcodorg serc_intvec			0x0	SID_INTVEC_BASE + SERC_OFS 		0x10 ispace
#rcodorg serd_intvec 		0x0 	SID_INTVEC_BASE + SERD_OFS			0x10 ispace
#if _CPU_ID_ == R3000
#rcodorg sere_intvec			0x0	SID_INTVEC_BASE + SERE_OFS			0x10 ispace
#rcodorg serf_intvec			0x0	SID_INTVEC_BASE + SERF_OFS			0x10 ispace
#rcodorg inputcap_intvec	0x0	SID_INTVEC_BASE + INPUTCAP_OFS	0x10 ispace
#rcodorg	quad_intvec			0x0	SID_INTVEC_BASE + QUAD_OFS			0x10 ispace
#endif
#rcodorg ext0_intvec 		0x0 	SID_XINTVEC_BASE + EXT0_OFS		0x10 ispace
#rcodorg ext1_intvec 		0x0 	SID_XINTVEC_BASE + EXT1_OFS		0x10 ispace
#endif

#if (ENABLE_CLONING == 1)
#use CLONE.LIB							// Contains cloning support functions
#endif
*/
/********************************************************************
*********************************************************************
*
*	BIOS internals follow
*
*********************************************************************
*********************************************************************/
#ifndef FLASH_WSTATES
	#if (NUM_FLASH_WAITST==0)
	#define FLASH_WSTATES 0xc0
	#elif (NUM_FLASH_WAITST==1)
	#define FLASH_WSTATES 0x80
	#elif (NUM_FLASH_WAITST==2)
	#define FLASH_WSTATES 0x40
	#elif (NUM_FLASH_WAITST==4)
	#define FLASH_WSTATES 0x00
	#endif
#endif

// May be on CS1 or CS2
#ifndef RAM_WSTATES
	#if (NUM_RAM_WAITST==0)
	#define RAM_WSTATES 0xc0
	#elif (NUM_RAM_WAITST==1)
	#define RAM_WSTATES 0x80
	#elif (NUM_RAM_WAITST==2)
	#define RAM_WSTATES 0x40
	#elif (NUM_RAM_WAITST==4)
	#define RAM_WSTATES 0x00
	#endif
#endif

// Used for RAM on CS1 when primary ram is on CS2
#if (NUM_RAM2_WAITST==0)
#define RAM2_WSTATES 0xc0
#elif (NUM_RAM2_WAITST==1)
#define RAM2_WSTATES 0x80
#elif (NUM_RAM2_WAITST==2)
#define RAM2_WSTATES 0x40
#elif (NUM_RAM2_WAITST==4)
#define RAM2_WSTATES 0x00
#endif

#define SIZESTDIOBUFF 130

#define HW_MEMBREAK 0xe2

// disable clock spreader if not Rabbit 3000 CPU
#if (_CPU_ID_ != R3000)
	#undef ENABLE_SPREADER
	#define ENABLE_SPREADER	0
#endif

//*** end of macro definitions ***

//***** variable definitions ******************************************
#define BIOSSTACKSIZE 40
char dbXPC;

void *dkcState;							//	the address (state) of  debugger kernel comm machine
char BiosStack[BIOSSTACKSIZE]; 		// initial stack for bios in data the segment
//char commBuffer[256];					//	communication buffer
char stdioCommBuff[SIZESTDIOBUFF];	// STDIO buffer
char watchBuffer[8];						//	buffer for watch expression results

//struct _dcParam DCParam;

//struct _DCSrcDebug DCSrcDebug;
char  OPMODE, _dkfirst, cloningNow;

// *** do not change the order of definition ***
struct _regBuffer registers;			//	RAM location to store the registers
char _dkenable28,_dkdisable28; 		// byte to store at RST28 enable, disable
int _SectorSize;
char	bios_divider19200;				// raw time constant calculated by BIOS
char	freq_divider;						// time constant for 19200 baud

// store value of GCSR register to determine reset reason
char	reset_status;
/*
// the internal I/O shadow registers
char GCSRShadow, GOCRShadow, GCDRShadow;
char PADRShadow;
char PBDRShadow;
char PCDRShadow, PCFRShadow;
char PDDRShadow, PDCRShadow, PDFRShadow, PDDCRShadow, PDDDRShadow;
char PEDRShadow, PECRShadow, PEFRShadow, PEDDRShadow;

char MMIDRShadow, MECRShadow;
char MB3CRShadow, MB2CRShadow, MB1CRShadow, MB0CRShadow;
char IB0CRShadow, IB1CRShadow, IB2CRShadow, IB3CRShadow, IB4CRShadow,
     IB5CRShadow, IB6CRShadow, IB7CRShadow;
char I0CRShadow, I1CRShadow;
char TACSRShadow, TACRShadow, TAT1RShadow, TAT2RShadow, TAT3RShadow,
	  TAT4RShadow, TAT5RShadow, TAT6RShadow, TAT7RShadow;
char TBCSRShadow, TBCRShadow;
char SPCRShadow, SACRShadow, SBCRShadow, SCCRShadow, SDCRShadow,
	  SECRShadow, SFCRShadow;
char SAERShadow, SBERShadow, SCERShadow, SDERShadow;

#if (_CPU_ID_ == R3000)
// Rabbit 3000-specific registers
char GPSCRShadow;
char MTCRShadow;
char PBDDRShadow;
char PFDRShadow, PFCRShadow, PFFRShadow, PFDDRShadow, PFDCRShadow;
char PGDRShadow, PGCRShadow, PGFRShadow, PGDDRShadow, PGDCRShadow;
char GCM0RShadow, GCM1RShadow;
char BDCRShadow;
char ICCSRShadow, ICCRShadow;
char ICT1RShadow, ICS1RShadow, ICT2RShadow, ICS2RShadow;
char PWL0RShadow, PWM0RShadow, PWL1RShadow, PWM1RShadow,
	  PWL2RShadow, PWM2RShadow, PWL3RShadow, PWM3RShadow;
char QDCSRShadow, QDCRShadow;
char SEERShadow, SFERShadow;
char TAPRShadow;
char TAT8RShadow, TAT9RShadow, TAT10RShadow;
#endif // End of _CPU_ID_ == R3000
*/
char NewPrintConFlag;

// dc_timestamp and prog_param are defined by the compiler.
extern struct progStruct prog_param;

// date/time stamp at the end of the compile (seconds since Jan. 1 1980)
extern long dc_timestamp;

#if __SEPARATE_INST_DATA__
	// relays for interrupts
	char intvec_relay[0x10][0x10];
	char xintvec_relay[0x10][0x10];
#endif // End of __SEPARATE_INST_DATA__

#if (ERRLOG_USE_MESSAGE==1)
	char   errlogMessage[8];
#endif // End of ERRLOG_USE_MESSAGE

#if (ENABLE_ERROR_LOGGING==1)
	struct _errLogInfo  errLogInfo;
	struct _errLogEntry  errLogEntry;
#endif  // end if error logging enabled

//*** end of variable definitions ***
//***** Begin prototypes **********************************************
#if __SEPARATE_INST_DATA__
 root void sid_intvec_cleanup();
#endif
 root void biosmain();
 root void dkInit();
 xmem void WaitSettle();
 xmem void dkInitTC();
 root void dkcDoSerial();
 root void DevMateSerialISR();
 root void DevMateReadPort();
 root void DevMateWritePort();
 root void divider19200();
 root void dkDoMsg();
 root void __bp_init();
 xmem void dkBLDebugInit();
 root void _xexit();
 xmem void dkSetIntVecTabP();
 xmem void dkSetIntVecTabR();
 root void dkStartup();
 root void dkEnd();
 root void symTab();
 root void AUX_STACK_LIMIT();
 root void AUX_STACK_PTR();
 root void STACK_LIMIT();
 root void symTabEnd();

//*** End Prototypes ***

#ifndef _TARGETLESS_COMPILE_
#define _TARGETLESS_COMPILE_ 0
#endif
#if (NUM_RAM_WAITST!=0)
#pragma DATAWAITSUSED on
#endif

// Setup interrupt relays to xmem if compiling to separate I&D space and Flash

/*
#if __SEPARATE_INST_DATA__

#asm
#rcodorg	periodic_intvec apply
//		Note that the periodic isr is overridden is VDRIVER.LIB to be fast and non-modifiable.
		INTVEC_RELAY_SETUP(intvec_relay + PERIODIC_OFS)

#rcodorg rst10_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + RST10_OFS)

#rcodorg rst18_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + RST18_OFS)

#rcodorg rst20_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + RST20_OFS)

#rcodorg rst28_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + RST28_OFS)

#rcodorg rst38_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + RST38_OFS)

#rcodorg slave_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SLAVE_OFS)

#rcodorg timera_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + TIMERA_OFS)

#rcodorg timerb_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + TIMERB_OFS)

// Make serial A fast.  Use relay if you want interrupt to be modifiable.
#rcodorg sera_intvec apply
		jp DevMateSerialISR
//    Uncomment the following line and comment out the preceding line if relay is desired.
//		INTVEC_RELAY_SETUP(intvec_relay + SERA_OFS)

#rcodorg serb_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERB_OFS)

#rcodorg serc_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERC_OFS)

#rcodorg serd_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERD_OFS)

#if _CPU_ID_ == R3000
#rcodorg sere_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERE_OFS)

#rcodorg serf_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERF_OFS)

#rcodorg inputcap_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + INPUTCAP_OFS)

#rcodorg quad_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + QUAD_OFS)
#endif

#rcodorg rootcode resume

#endasm

#endif // End of __SEPARATE_INST_DATA__
*/
//***** BIOS main program *********************************************
#asm nodebug
biosmain::
// don't insert any code inbetween this label and the jp
// or the jp and the dw!!!
#if FLASH_COMPILE || FAST_RAM_COMPILE
	jp		InFlashNow
#else
	jp		InRAMNow
#endif
#endasm

// *** BIOS constants and special variables ***
#if FAST_RAM_COMPILE
// Flag for the FAST_RAM_COMPILE copy
#asm
FastRAM_InRAM::
	db 0
#endasm
#endif

// When I&D space is enabled, the constants in the following block are only visible via ldp.
#asm
loaderParameters:
	dw		BIOSCodeEnd	;	store BIOS size, RAM start location, and
#if FLASH_COMPILE || FAST_RAM_COMPILE
	dw 0
#else
	dw		InRAMNow
#endif
divider19200::
	db _FREQ_DIV_19200_
I_am_a_clone::
	dw    0
I_am_a_sterile_clone::
	dw    0
#endasm

#asm const
//*** These 4 labels and their data must be kept below address 0x80 !
ErrLogConfiguration::
	dw 	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff
ErrLogLastHeader::
	dw 	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff
#endasm


//***** Begin BIOS code ***********************************************
#asm
InFlashNow::
#if FLASH_COMPILE || FAST_RAM_COMPILE
dkInit::
ljp __dkinit__
#endasm

#asm xmem
__dkinit__::
	; set STATUS pin low and CLK pin off
	ld		a,0xA0
	ioi 	ld		(GOCR),a
	ld		a,00001000b	;	normal oscilator, processor and peri.
								;	from main clock, no periodic interrupt
	ioi   ld	(GCSR),a
#if __SEPARATE_INST_DATA__
	; error out if processor does not support I&D space
	ioi	ld		a, (GCPU)
	and 	0x1f
	ld 	h, a
	ioi   ld		a, (GREV)
	and  	0x1f
	bool 	hl		; Test for R2000 Rev 0
	jp    z, __xexit__
#endif
#endif

#if RAM_COMPILE
InRAMNow::
#if 0
	xor   a
	ld    (I_am_a_clone),a
#endif
	ipset 3

//****** a slight delay for things to settle
	ld    b,50
	ld    de,0e8h        ; the upper 3 bits turn over every 1/1024 s
	ld    h,0
oloop:
	ld    a, 0x5a
	ioi 	ld (WDTCR),a
	ld		a,0ffh			;write any value to 0r to capture count
	ioi	ld (RTC0R),a
	ld		iy,RTC0R
	ioi	ld L,(iy)
	and   hl,de
loop:
	ld		a,0ffh			;capture and compare
	ioi	ld (RTC0R),a
	ld		iy,RTC0R
	ioi	ld a,(iy)
	and   e
	cp    L
	jr    z,loop
	djnz  oloop         ; loop until the first value reached

	; set STATUS pin low and CLK pin off
	ld		a,0xA0
	ioi 	ld		(GOCR),a
	ld		a,00001000b		;	normal oscilator, processor and peri.
								;	from main clock, no periodic interrupt
	ioi   ld	(GCSR),a
#endif // end of if RAM_COMPILE

// *** Set the Segment size and data segment ***
dkSetMMU:
	ld 	a,MEMBREAK
	ioi 	ld (SEGSIZE), a
#if FAST_RAM_COMPILE
	ld 	a, BBDATASEGVAL
#else
	ld 	a, DATASEGVAL
#endif
	ioi 	ld (DATASEG), a

// *** Set the MMIDR, turn on Separate ID (if enabled) ***
#if (CS1_ALWAYS_ON==0)
	xor 	a
#else
	ld 	a, 0x10
#endif
#if __SEPARATE_INST_DATA__
	ld 	a, MMIDR_VALUE
#endif
	ioi 	ld (MMIDR),a

#if FAST_RAM_COMPILE
// **NOTE** This code needs to come right after the write to MMIDR
//  			so that, if enabled, Separate I&D is on before the copy
// For the FAST_RAM_COMPILE mode, we need to copy the entire flash
// to the fast RAM, and then restart the BIOS in RAM. First we copy
// each bank of flash to the RAM, mapping the RAM to physical memory
// bank 3 for the copy.
//
// After the copy, we remap the physical banks as follows
// (for boards with 512K Flash, 512K Fast RAM, and optionally,
//  a 128K battery-backed RAM):
//
//		+--------------------------------------+ 0xFFFFF
//		| Bank 3 (MB3CR)								|
//		| Reserved for Bank Switching/Flash		|
//		| File-system									|
//		| 													|
//		+--------------------------------------+ 0xC0000
//		| Bank 2 (MB2CR)								|
//		| Upper 256L of 512K Fast RAM on CS/2  |
//		| OR												|
//		| 128K Battery-Backed RAM on CS/1		|
//		+--------------------------------------+ 0x80000
//		| Bank 1 (MB1CR)								|
//		| Upper 256K of 512K Fast RAM on CS/2  |
//		| 													|
//		| 													|
//		+--------------------------------------+ 0x40000
//		| Bank 0 (MB0CR)								|
//		| Lower 256K of 512K fast RAM on CS/2  |
//		| 													|
//		| 													|
//		+--------------------------------------+ 0x00000
//

// NOTES:
//			1) We cannot assign the shadows for the Memory Bank Control Registers
// 			(MBxCRShadow), or any other variables before the copy, since
//				the RAM will be overwritten.
//			2) We do not set up any stack, since it would be destroyed in
//			   the copy. We use the HL' reigister to store the
//				return address for any function calls, and use jp (HL) to return.

// First we check that we are running in flash to do the copy
// otherwise, we are running in RAM and have already finished the copy
// FastRAM_InRAM: 1=running in RAM, 0=Running in FLASH
	xor	a							; Bottom quadrant
	ld		ix, FastRAM_InRAM		; Address of the flag
	ldp 	hl, (ix)					; Load the flag
	bit	0,l						; Set 0 flag if the bottom bit is 0 (In Flash)
	jp		nz, .fastRAMCopyDone ; skip copy if already done (FastRAM_InRAM == 1)

.swapProgToRAM::
;; Swap the compiled program over to RAM from flash. Maps flash to
;; the first 2 physical memory banks (MB0CR, MB1CR), and RAM to
;; the top banks (MB2CR, MB3CR)

	;; Map flash to bottom 2 memory banks
	ld		a, FLASH_WSTATES | CS_FLASH
	ioi	ld (MB0CR), a			; Map memory bank 0
	ioi	ld (MB1CR), a			; Map memory bank 1

	;; Map execution RAM to top 2 memory banks
	ld		a, RAM_WSTATES | CS_RAM
	ioi	ld (MB2CR), a
	ioi	ld (MB3CR), a


	;; Copy the first quadrant of flash to the first quadrant of RAM
	ld		a, 0x00					; copy first quadrant (0x00000-0x3FFFF)
	ld		c, 0x08					; to address 0x80000 higher (0x00000 of RAM)
	ld		de, .swapProgToRAM2	; Put return address in HL'
	ld		hl', de
	jp		.copyFlashToRAM
// DO NOT INSERT CODE HERE - .copyFlashToRam will return to the next line
.swapProgToRAM2::

	;; Copy flash to RAM
	ld		a, 0x04					; copy first quadrant (0x00000-0x3FFFF)
	ld		c, 0x08					; to address 0x80000 higher (0x40000 of RAM)
	ld		de, .framswitch		; Put return address in HL'
	ld		hl', de
	jp		.copyFlashToRAM
// DO NOT INSERT CODE HERE - .copyFlashToRam will return to the next line
.framswitch::

	;; Set the new flag value (in RAM, now that the copy is complete)
	ld		a, 0x8					; Bottom quadrant of RAM
	ld		ix, FastRAM_InRAM		; Address of the flag
	ld		hl, 0x1					; We will be running in RAM
	ldp	(ix), hl					; Store the flag

;; Set up the FAST_RAM_COMPILE physical memory mapping
;; This needs to be done before the BIOS is restarted
;; (the "jp 0" instruction below) since we now want
;; to run the program in RAM, not flash.

	;; map bank 3 to the Flash on /CS0
	ld		a, MB3CR_SETTING
	ioi	ld (MB3CR), a

	;; map bank 2 to the RAM on /CS1, or replicate Bank 1
	ld		a, MB2CR_SETTING
	ioi	ld (MB2CR), a

	;; map bank 1 to the upper portion of the RAM on /CS2
	ld		a, MB1CR_SETTING
	ioi	ld (MB1CR), a

	;; map bank 0 to bottom half of RAM on /CS2
	ld		a, MB0CR_SETTING
	ioi	ld (MB0CR), a

;; Jump to address 0 to restart BIOS (In RAM now)
	jp 0
// ****** End of FAST_RAM_COMPILE copy *******

// .copyFlashToRAM
// pseudo-function (does not use any stack) used to copy the contents of
// the flash to the fast RAM for the FAST_RAM_COMPILE mode
.copyFlashToRAM::
	;; starting 64kb block passed in A (bits 16-19 of physical address)
	;; RELATIVE offset (in 64kb blocks) to destination FROM SOURCE passed in C
	;; IX is the increment offset

	ld		ix, 0x0000		// starting address, bits 00-15
	ld		b, 4				// number of 64kb blocks to copy
.bigloop:
	ld		de, 0x8000		// number of WORDS in 64kb block

	ex		af, af'			; Save a
.loop:
	ex		af, af'			; Restore a
	ldp	hl, (ix)			; Load word from source into HL (A:IX is 20-bit address)
	add	a, c				; Add the address and offset to get destination for ldp
	ldp	(ix), hl			; Copy word to destination
	sub	a, c				; Subtract offset, to get back to source address
	ex		af, af'			; Save a

	inc	ix					; Increment IX to next word address
	inc	ix

	dec	de					; 1 less word to copy
	ld		a, d				; Load top half of words to copy into A
	or		e					; Logical OR with bottom half - A contains non-zero value if more to copy
	jr		nz, .loop		; More to copy? Loop

	ex		af, af'			; Restore a
	inc	a					; Address of next block to copy
	djnz	.bigloop			; decrement number of blocks to copy and jump if remaining

	exx						; Swap regs with alternates to retrieve return address
	jp (hl)					; We dont have a stack, so use hl to store return address

;;-----------------------------------------------
;; Continue running the BIOS after the copy
.fastRAMCopyDone::

;; Set the Memory Bank Control Register Shadows
;; now that we are running in RAM
#endif // ----- FAST_RAM_COMPILE -----

;  MB0CR is setup below after crystal frequency calculation
;  at this point execution actually continues at the
;  copy of the next instruction in flash
	ld 	a, MB1CR_SETTING
	ioi 	ld (MB1CR),a

#if RAM_COMPILE
	// RAM compile mode-specific initialization
#if (RAMONLYBIOS == 0)		// don't map in flash if it doesn't exist
	ld 	a, FLASH_WSTATES | CS_FLASH
	ioi 	ld (MB2CR),a

	ld 	a, FLASH_WSTATES | CS_FLASH
	ioi 	ld (MB3CR),a

;	ld 	a, FLASH_WSTATES | CS_FLASH
;	ld 	hl,MB2CRShadow
;	ld    (hl),a

;	ld 	a, FLASH_WSTATES | CS_FLASH
;	ld 	hl,MB3CRShadow
;	ld    (hl),a

//  Before we disable the flash, disable any existing
//  flash BIOS by writing a tight loop to the start
//  of flash. This ensures that runmode will not work
//  until the BIOS and user program have been recompiled
//  to flash. This prevents the user from deploying a board
//  when he just compiled and debugged to RAM, because when
//  power cycles and the programming cable is not connected,
//  the previously loaded flash BIOS, if it exists, will run
//  the previously loaded user program, if it exists.
/*
	ld		hl, 0x04 | 0x08
	call	_readIDBlock
	bool	hl
	jr		z, idBlockOk
*/
#endif
/*
	;; erase SysIDBlock if error retured (or RAM-only BIOS)
	ld		hl, SysIDBlock
	ld		b, SysIDBlock+marker+6-SysIDBlock
	xor	a
blockEraseLoop:
	ld		(hl), a
	inc	hl
	djnz	blockEraseLoop
idBlockOk:
*/
#endif // end of RAM section
/*
	ld 	a, MB2CR_SETTING
	ioi 	ld (MB2CR),a
//	ld 	hl,MB2CRShadow
	ld    (hl),a

	ld 	a, MB3CR_SETTING
	ioi 	ld (MB3CR),a

	; Set MMIDR Shadow
	ld		a, MMIDR_VALUE
//	ld 	hl,MMIDRShadow
	ld    (hl),a

	; Set MB0CR shadow
	ld		a, MB0CR_SETTING
	ld 	hl,MB0CRShadow
	ld    (hl),a

	; Set MB1CR shadow
	ld		a, MB1CR_SETTING
	ld 	hl,MB1CRShadow
	ld    (hl),a

	; Set MB2CR shadow
	ld		a, MB2CR_SETTING
	ld 	hl,MB2CRShadow
	ld    (hl),a

	; Set MB3CR shadow
	ld		a, MB3CR_SETTING
	ld 	hl,MB3CRShadow
	ld    (hl),a
*/
	;; set up temporary stack in RAM
dkSetSP:
	ld		hl,BiosStack			; Set up a temporary stack to perform call.
	ld    de,BIOSSTACKSIZE-1   ; Note that stack segment register is not setup here,
	add   hl,de						; but in _ProgStkSwitch.  _ProgStkSwitch is normally
	ld		sp,hl						; called in premain after stack alloc init.

	;; measure crystal frequency for proper baud rates
	xor	a
	ioi	ld (GCDR), a			; disable clock divider (if enabled by pilot)
	ld		a, MB0CR_SETTING
	and	0x3F
	or		0x40						; enable 2 wait states
	ioi	ld (MB0CR), a

	;; measure crystal frequency for baud rate calculations
	ld    bc,0000h       	; our counter
	ld    de,07FFh    		; mask for RTC bits

.wait_for_zero:
	ioi	ld (RTC0R),a    	; fill RTC registers
	ioi	ld hl,(RTC0R)		; get lowest two RTC regs
	and   hl,de					; mask off bits
	jr    nz,.wait_for_zero	; wait until bits 0-9 are zero
.timing_loop:
	inc   bc						; increment counter
	push  bc						; save counter
	ld    b,98h             ; empirical loop value
									; (timed for 2 wait states)
	ld    hl,WDTCR
.delay_loop:
	ioi	ld(hl),5Ah			; hit watchdog
	djnz  .delay_loop
	pop   bc						; restore counter
	ioi	ld (RTC0R),a		; fill RTC registers
	ioi	ld hl,(RTC0R)		; get lowest two RTC regs
	bit   2,h					; test bit 10
	jr    z, .timing_loop	; repeat until bit set
	ld    h,b
	ld    l,c
	ld    de, 0x0008
	add   hl,de					; add 8 (equiv. to rounding up later)
	rr    hl
	rr    hl
	rr    hl
	rr    hl						; divide by 16
	ld    a,l   				; this is our divider for 57600 baud
	sla	a
	add	a,l					; multiply by 3 to get 19200 baud
	ld		(bios_divider19200), a	; save for later use

	ld		a, MB0CR_SETTING	; restore MB0CR_SETTING
	ioi	ld (MB0CR), a

#if FLASH_COMPILE || FAST_RAM_COMPILE
#if  (ZERO_OUT_STATIC_DATA==1)
	//****************************************
	//		Initialize dataseg data area to zeros
	//    ~500K clocks for 0x7000
	//***************************************
;	ld    a,0x5a         ; hitwd
;	ioi   ld (WDTCR),a
;   ld    hl, ROOTDATAORG
;   ld    de, ROOTDATASIZE
;	xor   a
;	sbc   hl,de
;	inc   hl
;   ld    bc, ROOTDATASIZE
.zISDloop:
;	ld    (hl),a
;	inc   hl
;	dec   bc
;	cp    c
;	jr    nz,.zISDloop
;	cp    b
;	jr    nz,.zISDloop
;	ld    a,0x5a         ; hitwd
//	ioi   ld (WDTCR),a
#endif

	ioi	ld a, (GCSR)
//	ld		(reset_status), a		; save bits in GCSR to determine reason for reset
//   lcall _more_inits0			; Call code for flash initialization
#endif // end of FLASH || FAST_RAM_COMPILE section

#if RAM_COMPILE
;******************************************************************************
; Enter here for software reset requested by Dynamic C
; to turn off periodic interrupt
dkInit::
	ipset 3
	ld		a,00001000b		;	normal oscilator, processor and peri.
								;	from main clock, no periodic interrupt
	ioi   ld	(GCSR),a

;*******************************************************************************
#endif

	;	initialize comm ports now
	;	at port a, async, 8-bit, interrupt priority 1
	; make sure other serial ports are disabled
	ld		a, 0x00
	ioi	ld (SBCR), a
	ioi	ld (SCCR), a
	ioi	ld (SDCR), a
	ld		a, 0x01
	ioi 	ld (SACR),a
//	ld 	hl,SACRShadow
	ld    (hl),a

;	xor	a  						; zero the xpc
;	ld    xpc,a						// REMOVE remove the comments that is?

dkSetComm:

#if !__SEPARATE_INST_DATA__
dkSetProgPort:
	;	parallel port D, bit 6,0 as output lines
	ld		a,0xc3		;	jump instruction
	ld		(INTVEC_BASE+SERA_OFS),a
	ld		hl,DevMateSerialISR
	ld		(INTVEC_BASE+SERA_OFS+1),hl
#endif

#if (ENABLE_CLONING == 1)
	xor a
	ldp  hl,(I_am_a_sterile_clone)	; in code space, so must access via ldp
	bool hl
	jr    nz,_skip_clone_check

	;; see if cloning cable is attached (was PB1 low?)
	ld		a, (cloningNow)
	or		a
	jr		z, NotCloning
	ljp		CloneMode
NotCloning:
#endif
_skip_clone_check:


#ifndef COMPILE_SECONDARY_PROG
	// check for run mode
	ioi ld a,(SPCR)
	and 60h
	cp  60h
	jp  nz,RunMode
	; Reset
#else
	// can't debug secondary program, so run it
	jp  RunMode
#endif

#if (ENABLE_CLONING==1)
	xor a
	ldp hl, (I_am_a_clone)	; in code space, so must access via ldp
	bool hl
	jp nz,RunMode
#endif
#if (_CPU_ID_ == R3000)
	xor   a
   ld    (BDCRShadow),a  // RST 28Hs are active
#endif
 	jp __dktrap   // switch back to root

#endasm

#asm
__dktrap::
//	lcall _init_dkLoop
//	call	dkSaveContext
//	call	dkCore
	jp 	__dktrap

RunMode:
	ipset 3
//	lcall dkSetIntVecTabR       ;  disable debug RSTs
	ipset 0

//***** Jump to user code *********************************************
#if RAM_COMPILE || FAST_RAM_COMPILE
	ld hl,OPMODE
	ld (hl),0x80
	jp StartUserCode
#endif

#if FLASH_COMPILE
	ld 	a,80h
 	ld   	(OPMODE),a

//	lcall	WaitSettle

	jp StartUserCode

#endif

#endasm
// *** End of main BIOS code ***

/*
//***** BIOS functions ************************************************
#asm xmem
;; This code repeatedly measures the timing on the 32kHz oscillator until
;; it matches twice in a row.  The 32kHz oscillator circuit could take as
;; long as 1.5 seconds to rise to a full voltage and we want to wait until
;; that is true...

WaitSettle::
	ld		de, 0000h				// first measurement value
	push	de							// save it for later
startmeas:
	ld		bc, 0000h				// our counter
	ld		de, 07FFh				// mask for RTC bits
wait_for_zero:
	ioi	ld (RTC0R), a			// load RTC registers
	ioi	ld hl, (RTC0R)			// read 1st two RTC registers
	and	hl, de					// mask off bits
	jr		nz, wait_for_zero		// repeat until all zero
timing_loop:
	inc	bc							// increment counter
	push	bc							// save counter
	ld		b, 057h					// empirically-derived loop counter
	ld		hl, WDTCR
delay_loop:
	ioi	ld	(hl), 05Ah			// hit watchdog
	djnz	delay_loop
	pop	bc							// restore counter
	ioi	ld (RTC0R), a			// load RTC registers
	ioi	ld hl, (RTC0R)			// read 1st two RTC registers
	bit	2, h						// test bit for 0800h
	jr		z, timing_loop			// if nonzero, keep looping

	ld		h, b						// h = upper nibble of counter
	ld		a, c
	add	a, 08h					// (effectively rounding up)
	ld		l, a						// l = lower nibble of counter + 8
	rr		hl
	rr		hl
	rr		hl							// divide by 16
	rr		hl
	ld		a, l						// lower nibble is divisor!
	dec	a
	pop	de							// get last count
	cp		a, e						// compare last and current counts
	jr		z, donemeas				// if alike, done
	ld		e, a
	push	de							// save this count
	jr		startmeas
donemeas:
	lret

dkInitTC::
;	ld		(TCState+[TCState]+SysWriteHandler),hl	; special-case hander for sys-writes
;	xor	a
;	inc	a
;	ld		(TCState+[TCState]+TrapSysWrites),a		; flag that sys-writes should be handled as a special case

; 	ld		iy,TCState
;	lcall	dkcSystemBufINIT		;  initialize the system buffers
;	lcall	dkcInit					;	initialize comm module
;	call	dkcSystemINIT			;  initialize the system-type handler

;   ld 	hl, _xexit
;  ld    (DCParam+errorExit), hl  ; initialize error exit for exception handler

;	lcall	dkBLDebugInit
;	call	dkInitDebugKernelComs ; initial debug kernel - target communication interface
	lret
#endasm
//*** End Xmem BIOS code section ***
*/

#asm
dkcDoSerial::
;	ld		iy,TCState			; iy == pointer to our state structure
;	ioi 	ld a,(SASR)			;	check the status
;	bit	SS_RRDY_BIT,a			;  was a character received?
;	jr		z,_DevMatenoRxIntReq

//	call	dkcEntryRX			;  handle the RX interrupt
;	jr		_DevMatereadyToExit		;  all done for now

_DevMatenoRxIntReq:
	bit	3,a
;	jr		z,_DevMateSecondTXInt
;	call	dkcEntryTX			;	handle the TX interrupt
;	jr		_DevMatereadyToExit

_DevMateSecondTXInt:
//	ld		a,(TCState+[TCState]+TXBusy)
	or		a
	jr		z,_DevMateSkipInt

	; this really is a good int
	jr		_DevMatereadyToExit

_DevMateSkipInt:
	; just clear the int
	ioi 	ld (SASR), a

_DevMatereadyToExit:
	ret

DevMateSerialISR::
	;	serial interrupt handler
	push	ip
	push	af
	ex    af,af'
	push	af
	ld    a,xpc
	push	af
	push	bc
	push	de
	push	hl
	push  ix
	push	iy

	exx
	push	bc
	push	de
	push	hl

	call	dkcDoSerial

	pop	hl
	pop	de
	pop	bc
	exx

	pop	iy
	pop   ix
	pop	hl
	pop	de
	pop	bc
	pop	af
	ld    xpc,a
	pop	af
	ex    af,af'
	pop	af
	pop	ip
	ipres

	ret

DevMateReadPort::
	;	destroys A
	;	returns byte read (if any) in A
	;	returns with Z set if nothing is read

	;	check if there is anything available
	ioi 	ld a, (SASR)
	bit	SS_RRDY_BIT,a		;	if a received byte ready?
	ret	z						;	nope, return with z set
	;	otherwise, a byte *is* ready, read from data port
	ioi 	ld a, (SADR)
	ret							;	return with z *not* set

DevMateClearReadInt::
	ld		a,SS_RRDY_BIT
	ioi	ld (SASR),a
	ret

DevMateWritePort::	;	assumes byte to transmit is in C
	;	destroys A
	;	returns with Z reset if not transmitted

	;	check if the port is ready
	ioi 	ld a, (SASR)
	bit	SS_TFULL_BIT,a		;	can I transmit now?
	ret	nz						;	nope, return with nz set
	;	otherwise, the transmit buffer is ready, write to it!
	ld		a,c					;	move byte to transmit to a
	ioi 	ld (SADR), a
	ret							;	return with z *not* set
#endasm

#asm const
dkDCID::
	db "DynamiC"
dkProdName::
	db PRODUCT_NAME
#endasm

#define ERROR_EXIT DCParam+errorExit

#asm
sid_intvec_cleanup::

bioshitwd::
#endasm

#asm
dkDoMsg::
#endasm

#asm xmem
; initialize portions of debug kernel that need to be done at
; the bios level
dkBLDebugInit::
;	ld		(dkLastAsmStart),hl
;	ld		(dkLastAsmEnd),hl
;	ld		(dkExitHook),hl
;	ld		(dkSingleStepHook),hl
;	ld		(dkPrologHook),hl
;	ld		(dkEpilogHook),hl
;	ld		(dkSetStdioTimerHook),hl
;	ld		(dkCheckStdioTimeOutHook),hl
	xor	a
;	ld		(dkAckToSend),a
;	ld		(dkLocks),a
;	ld		(dkStatusFlags),a
;	ld		(dkPrologCount),a
;	ld		(dkCharData),a
;	ld		(dkCharReady),a
	lret
#endasm

#asm
_xexit::
	push	af
;	set	DKF_SEND_STATUS,a
;	set	DKF_SEND_ACK,a
;	ld		(dkSendFlags),a
;	ld		a,(dkStatusFlags)
;	res	DKF_STAT_RUNMODE,a
;	set	DKF_STAT_ATEXIT,a
;	set	DKF_STAT_AT_BP,a
; 	ld		(dkStatusFlags),a
;	ld		a,TC_DEBUG_ATBREAKPOINT
;	ld		(dkAckToSend),a
	ld		a,(OPMODE)							;if board is in runmode, don't reenable rst 20
	and	8
	jr		z, _xexit_skiprst20enable
	ld		a,0xc3
	ld		(INTVEC_BASE+RST20_OFS),a
_xexit_skiprst20enable:
	pop	af
	rst	0x20
__xexit__::
	ipset 3
	ld    a,0x00
	ioi   ld (WDTTR),a
_xexitLoop:
	jr		_xexitLoop
#endasm

/*
#asm xmem

dkSetIntVecTabP::	 ; Program mode
	;	set up interrupt vector table
#if __SEPARATE_INST_DATA__ //&& _FLASH_
	ld		a,0xff & (SID_INTVEC_BASE >> 8)	;
	ld		iir,a
	ld		a,0xff & (SID_XINTVEC_BASE >> 8)	;
	ld		eir,a
#else
	ld		a,0xff & (INTVEC_BASE >> 8)	;
	ld		iir,a
	ld		a,0xff & (XINTVEC_BASE >> 8)	;
	ld		eir,a
#endif

#if (_CPU_ID_ == R3000)
   ld    a,0x80
   ioi   ld (BDCR),a      ; make RST 28Hs NOPs
   ld    (BDCRShadow),a
#endif
	lret

dkSetIntVecTabR::   ; Runmode
	;	set up interrupt vector table
#if __SEPARATE_INST_DATA__ //&& _FLASH_
	ld		a,0xff & (SID_INTVEC_BASE >> 8)	;
	ld		iir,a
	ld		a,0xff & (SID_XINTVEC_BASE >> 8)	;
	ld		eir,a
#else
	ld		a,0xff & (INTVEC_BASE >> 8)	;
	ld		iir,a
	ld		a,0xff & (XINTVEC_BASE >> 8)	;
	ld		eir,a
#endif
	ld		a,0xc9		;	this is the ret instruction
	ld		(INTVEC_BASE+RST18_OFS),a		;	all are relays
	ld		(INTVEC_BASE+RST20_OFS),a
	ld		(INTVEC_BASE+RST28_OFS),a
#if (_CPU_ID_ == R3000)
   ld    a,0x80
   ioi   ld (BDCR),a      ; make RST 28Hs NOPs
   ld    (BDCRShadow),a
#endif
	lret
_more_inits0::

#if (ENABLE_ERROR_LOGGING==1)
	lcall  errlogRecordStartStatus
#endif

	//
	//  Probe for the System ID block.  Tell readIDBlock() where the flash
	//  chips are mapped.  It will pick the high address end to look for it.
	//  If not found, then destroy the ident marker; readIDBlock() will
	//  already set the tableVersion field to 0 when it can't find the thing.
	//  Also initialize Flash driver, could reserve space for File System.
	//
	//  Don't need to adjust User Block start address since it's relative
	//  offset to where SysID block is located (thanks Lynn!).
	//
#if FAST_RAM_COMPILE
	ld		hl, 0x08
#elif (_FLASH_SIZE_ == 0x80)
	ld		hl, 0x01 | 0x02
#else
	ld		hl, 0x01
#endif

#ifdef COMPILE_SECONDARY_PROG
  #ifndef INVERT_A18_ON_PRIMARY_FLASH
	// we are running the second flash
   ld    a,FLASH_WSTATES
	and   0xfc          // set control bits to 1rst flash
	ioi   ld (MB1CR),a
	ld		hl, 0x03
  #endif
#endif

	call	_readIDBlock

#ifdef COMPILE_SECONDARY_PROG
  #ifndef INVERT_A18_ON_PRIMARY_FLASH
   ld    a,MB1CR_SETTING
	ioi   ld (MB1CR),a
  #endif
#endif

	bool	hl
	jr		z, idBlockOk

	;; erase SysIDBlock if error retured
	ld		hl, SysIDBlock
	ld		b, SysIDBlock+marker+6-SysIDBlock
	xor	a
blockEraseLoop:
	ld		(hl), a
	inc	hl
	djnz	blockEraseLoop
idBlockOk:

#if FAST_RAM_COMPILE
	// The flash is mapped to bank 3 in FAST_RAM_COMPILE mode
	ld		hl, 0x08
#else
	#if (_FLASH_SIZE_ == 0x80)
		ld		hl, 0x01 | 0x02
	#else
		ld		hl, 0x01
	#endif
#endif
	call	_InitFlashDriver
#if (ENABLE_ERROR_LOGGING==1)
	call  InitializeErrorLog
#endif
	lret

//**************************************************************
_init_IO_regs::
	// The following registers have specific values already
	// set by the BIOS and loaded into their shadow registers:
	//		GCSR, WDTCR, GCMxR, GOCR, GCDR, MBxCR, MTCR,
	//		 SPCR, PCDR, PCFR, TACSR, TACR, SACR
	//
	// This next set of registers will be reset to these values
	// upon reset, startup, and when debugging is ended (i.e.
	// pressing F4).

	xor	a							// a = 0x00
	ioi	ld (PADR), a
	ld		(PADRShadow), a
	ioi	ld (PBDR), a
	ld		(PBDRShadow), a
	ioi	ld (PCDR), a
	ld		(PCDRShadow), a

	ioi	ld	(PDDDR),a
	ld		(PDDDRShadow),a
	ioi	ld (PDDR), a
	ld		(PDDRShadow), a
	ioi	ld (PDCR), a
	ld		(PDCRShadow), a
	ioi	ld (PDFR), a
	ld		(PDFRShadow), a
	ioi	ld (PDDCR), a
	ld		(PDDCRShadow), a
	ioi	ld	(PEDDR),a
	ld		(PEDDRShadow),a
	ioi	ld (PEDR), a
	ld		(PEDRShadow), a
	ioi	ld (PECR), a
	ld		(PECRShadow), a
	ioi	ld (PEFR), a
	ld		(PEFRShadow), a
	ioi	ld (TBCSR), a
	ld		(TBCSRShadow), a
	ioi	ld (TBCR), a
	ld		(TBCRShadow), a

	ioi	ld (IB0CR), a
	ld		(IB0CRShadow), a
	ioi	ld (IB1CR), a
	ld		(IB1CRShadow), a
	ioi	ld (IB2CR), a
	ld		(IB2CRShadow), a
	ioi	ld (IB3CR), a
	ld		(IB3CRShadow), a
	ioi	ld (IB4CR), a
	ld		(IB4CRShadow), a
	ioi	ld (IB5CR), a
	ld		(IB5CRShadow), a
	ioi	ld (IB6CR), a
	ld		(IB6CRShadow), a
	ioi	ld (IB7CR), a
	ld		(IB7CRShadow), a
	ioi	ld (I0CR), a
	ld		(I0CRShadow), a
	ioi	ld (I1CR), a
	ld		(I1CRShadow), a

	ioi	ld (TAT1R), a
	ld		(TAT1RShadow), a

	ioi	ld (SBCR), a
	ld		(SBCRShadow), a

	ioi	ld (SCCR), a
	ld		(SCCRShadow), a
	ioi	ld (SDCR), a
	ld		(SDCRShadow), a

	ld    a,0x80					// ingore SMODE pins --  Dynamic C does this,
	ioi	ld (SPCR), a			// but need to do it running stand alone too.
	ld		(SPCRShadow), a

	; enable watchdog
   ld    a,0x00
   ioi   ld (WDTTR),a

#if (_CPU_ID_ == R3000)
	xor	a							// a = 0x00
	ioi	ld (GPSCR), a
	ld		(GPSCRShadow), a
	ioi	ld (BDCR), a
	ld		(BDCRShadow), a
	ioi	ld (MECR), a
	ld		(MECRShadow), a

	ioi	ld (PFDR), a
	ld		(PFDRShadow), a
	ioi	ld (PFCR), a
	ld		(PFCRShadow), a
	ioi	ld (PFFR), a
	ld		(PFFRShadow), a
	ioi	ld (PFDCR), a
	ld		(PFDCRShadow), a
	ioi	ld (PFDDR), a
	ld		(PFDDRShadow), a

	ioi	ld (PGDR), a
	ld		(PGDRShadow), a
	ioi	ld (PGCR), a
	ld		(PGCRShadow), a
	ioi	ld (PGFR), a
	ld		(PGFRShadow), a
	ioi	ld (PGDCR), a
	ld		(PGDCRShadow), a
	ioi	ld (PGDDR), a
	ld		(PGDDRShadow), a

	ioi	ld (PWL0R), a
	ld		(PWL0RShadow), a
	ioi	ld (PWM0R), a
	ld		(PWM0RShadow), a
	ioi	ld (PWL1R), a
	ld		(PWL1RShadow), a
	ioi	ld (PWM1R), a
	ld		(PWM1RShadow), a
	ioi	ld (PWL2R), a
	ld		(PWL2RShadow), a
	ioi	ld (PWM2R), a
	ld		(PWM2RShadow), a
	ioi	ld (PWL3R), a
	ld		(PWL3RShadow), a
	ioi	ld (PWM3R), a
	ld		(PWM3RShadow), a

	ioi	ld (ICCSR), a
	ld		(ICCSRShadow), a
	ioi	ld (ICCR), a
	ld		(ICCRShadow), a
	ioi	ld (ICT1R), a
	ld		(ICT1RShadow), a
	ioi	ld (ICT2R), a
	ld		(ICT2RShadow), a
	ioi	ld (ICS1R), a
	ld		(ICS1RShadow), a
	ioi	ld (ICS2R), a
	ld		(ICS2RShadow), a

	ioi	ld	(PWL0R), a
	ld		(PWL0RShadow), a
	ioi	ld	(PWM0R), a
	ld		(PWM0RShadow), a
	ioi	ld	(PWL1R), a
	ld		(PWL1RShadow), a
	ioi	ld	(PWM1R), a
	ld		(PWM1RShadow), a
	ioi	ld	(PWL2R), a
	ld		(PWL2RShadow), a
	ioi	ld	(PWM2R), a
	ld		(PWM2RShadow), a
	ioi	ld	(PWL3R), a
	ld		(PWL3RShadow), a
	ioi	ld	(PWM3R), a
	ld		(PWM3RShadow), a

	ioi	ld (QDCSR), a
	ld		(QDCSRShadow), a
	ioi	ld (QDCR), a
	ld		(QDCRShadow), a

	ld		a, 0xC0
	ld		(PBDDRShadow), a

#ifndef	USE_TIMERA_PRESCALE
	ld		a, 0x01
#else
	xor	a
#endif
	ld		(TAPRShadow), a
#endif

   lret

//**************************************************************
// Init. the status pin, clocks, clock doubler, programming baud
// rate, stdio comm buffer

_more_inits02::


#if (ENABLE_SPREADER > 0)
	lcall	_enableClockModulation
#endif

	ld		a, 0c0h
	ioi	ld (PCFR), a	; as initialized in pilot
	ld 	hl, PCFRShadow
	ld    (hl), a

	ld    a,0xA0h
	ld		(statusPinState),a

	ld 	a,88h
	ld   	(OPMODE),a

	ld 	a,0
	ld   	(_dkfirst),a

#if (ENABLE_CLONING==1)
	;; check to see if cloning cable is attached
	ld 	a, 0x02
	ld		hl, PBDR
	ioi 	and a, (hl)
	ld 	hl, cloningNow
	jr 	nz, noCloneCable
	ld    (hl), 0x01
	jr    endCloneCheck
noCloneCable:
	ld    (hl), 0x00
endCloneCheck:
#else
	ld 	hl, cloningNow
	ld    (hl), 0x00
#endif

	ld b,SIZESTDIOBUFF
	ld hl,stdioCommBuff
clear_stdio:
	ld (hl),0
	inc hl
	djnz clear_stdio

dkSetTimer:

	;	initialize baud rate generator
	;	for main clock at 8MHz, the calculation follows:
	;	timer A4 to clock serial port A
	;	timer A5 to clock serial port B
	;	timer A6 to clock serial port C
	;	timer A7 to clock serial port D

	;	make timers tick
	ld 	a, 00000001b
	ioi 	ld (TACSR),a
	ld 	hl,TACSRShadow
	ld    (hl),a

	;	make timer A4 clocked by main clock
	;	disable interrupt
	ld 	a, 00000000b
	ioi 	ld (TACR),a
	ld 	hl,TACRShadow
	ld    (hl),a

	;	Read the time constant for 19200 baud calculated by the BIOS...
	ld		a, (bios_divider19200)
	ld		b, a
#ifdef	USE_TIMERA_PRESCALE
	; enable timer A prescale if desired
	xor	a
	ioi	ld (TAPR), a
	sla	b						; if prescale enabled, multiply constant by two
#endif
checkDoubled:
 ;; is clock doubled?
	ld		a, (GCDRShadow)	; get clock doubler register
	or		a
	jr		z, checkDiv			; if zero, clock not doubled
	sla	b						; if clock doubled, multiply constant by two
checkDiv:
 ;; is clock divided by eight?
	ld		a, (GCSRShadow)	; get global control/status register
	and	018h					; mask off unnecessary bits
	jr		nz, saveFreq		; if nonzero, periph clock not divided by 8
	srl	b
	srl	b						; divide by eight
	srl	b
saveFreq:
	ld		a, b
	ld		(freq_divider), a	; save for later use

#if (_BIOSBAUD_ == 2400)
	ld		c, a
	sla   c					 	; multiply by eight
	sla	c
	sla	c
#endif
#if (_BIOSBAUD_ == 4800)
	ld		c, a
	sla   c					 	; multiply by four
	sla	c
#endif
#if (_BIOSBAUD_ == 9600)
	ld		c, a
	sla   c					 	; multiply by two
#endif
#if (_BIOSBAUD_ == 19200)
	ld		c, a					; freq_divider = 19200 divisor
#endif
#if (_BIOSBAUD_ == 38400)
	ld		c, a
	srl   c					 	; divide by two
#endif
#if ( (_BIOSBAUD_ == 57600) || (_BIOSBAUD_ == 115200) )
setDCbaudrate:
  ;; set up proper baud rate for Dynamic C communication by doing some
  ;; 		fixed-point arithmetic to get 57600 rate (= 19200 rate / 3)
	ld		c, 0					; bc = FREQADDR (b = integer part, c = fraction)
	ld		de, 055h				; de = ~1/3
	mul							; multiply it out
	ld		c, l					; keep integer part
	inc	c						; round up since 055h is actually slightly less than 1/3

#if	(_BIOSBAUD_ == 115200)
	srl   c					 	; if 115200 baud wanted, divide by two
#endif
#endif

setBaudRate:
	dec	c						; put divider-1 into timer scaling registers
	ld		a, c
	ioi	ld (TAT4R),a		; save scaledown for 57600 baud
	ld 	hl,TAT4RShadow
	ld    (hl),a
   lret

//**************************************************************
// prepare to jump into the debugger

_init_dkLoop::

	call  bioshitwd
	xor	a
	ld		(dkRequest),a
	ld		(dkStatusFlags),a
	ld		(dkSendFlags),a
	ipset 0
   lret

#endasm
*/

#asm
; .startup/dkStartup
; code to run each time the program executes
; initializes the stack allocation
;
dkStartup::
	pop hl
   jp	(hl)					; jump to old return address
#endasm

//#flushlib

#asm

dkEnd::
__xexit::  jp _xexit
#endasm
#flushlib
#asm
_store_ID_Block_Size::
	dw 0xffff
symTab::
	dw		symsize
#endasm

//#flushlib

//	we need to end the symbol table now
#asm
	dw	0		;	indicates end of symbol table
	db	0

symTabEnd::
symsize: equ symTabEnd-symTab-2
#endasm


// include bios C support libraries
#use default.h

//!!!Do NOT insert code between here and #pragma CompileProgram!!!
//#flushlib
#asm
BIOSCodeEnd:
StartUserCode::
#endasm

#if __SEPARATE_INST_DATA__
#asm xmem
XBIOSCodeEnd:
XStartUserCode::
#endasm
#endif
#pragma CompileProgram

// NOTE: The #use precompile.lib MUST come after the #pragma CompileProgram
#use precompile.lib


