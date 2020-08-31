/*
   Copyright (c) 2015 Digi International Inc.

   This Source Code Form is subject to the terms of the Mozilla Public
   License, v. 2.0. If a copy of the MPL was not distributed with this
   file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/***********************************************************************
* ******************************************************************** *
* *                                                                  * *
* *   Universal BIOS for Rabbit Semiconductor, Inc. Rabbit 2000      * *
* *   and 3000 CPU based boards.                                     * *
* *                                                                  * *
* *   Assumes ATMEL compatible flash algorithms.                     * *
* *                                                                  * *
* ******************************************************************** *
***********************************************************************/
/*** BeginHeader */

#ifdef __RABBITSYS
	#define _SYSTEM 					__RABBITSYS == 2
   #define _USER						__RABBITSYS == 1
   #define _MIXED_SYSTEM_USER    __RABBITSYS > 0
#endif

#if __SEPARATE_INST_DATA__
	#define DSPACE dspace
   #define ISPACE ispace
   #define SID_64KADJUSTMENT	 0x10000L
#else
	#define DSPACE
	#define ISPACE
   #define SID_64KADJUSTMENT	 0x0L
#endif // End of __SEPARATE_INST_DATA__

#define PLD_ETH_DISABLE_DRIVER

#use "BOARDTYPES.LIB"	// board-specific initialization header

#define UNPROT_MODE 	0
#define SHADOWS_MODE UNPROT_MODE

#pragma origin_table_merge true
/////// BEGIN USER MODIFIABLE MACROS /////////////////////////////////

//***** Miscellaneous Information *******************************************
#define PRODUCT_NAME   "Universal Rabbit BIOS Version 9.50"

//***** Timer A prescale information (Rabbit 3000 only) ****************

//#define   USE_TIMERA_PRESCALE  // The Rabbit 3000 has the ability to
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
// Address line values for setting the origin directive MMIDR inversions
// Bitwise or with __SEPARATE_INST_DATA__ to automatically enable/disable
// respective settings in the MMIDR (from MMIDR_VALUE) and origin directive.
#define _SID_ADDRESS_16 (0x10 & (__SEPARATE_INST_DATA__ << 4))
#define _SID_ADDRESS_19 (0x80 & (__SEPARATE_INST_DATA__ << 7))

// Specify inversions for the MMIDR required for separate I&D space
#if FLASH_COMPILE || FAST_RAM_COMPILE
	#define DATASEG_INV _SID_ADDRESS_19
	#define CODESEG_INV _SID_ADDRESS_16
#endif
#if RAM_COMPILE
	#define DATASEG_INV _SID_ADDRESS_16
	#define CODESEG_INV _SID_ADDRESS_16
#endif


//***** DATAORG definition ********************************************
// Defines the beginning logical address for the data segment

#ifndef DATAORG                  // Allow user to override in the compiler options.
	#if __SEPARATE_INST_DATA__
		#define DATAORG      0x3000// Beginning logical address of variable data seg.
                                 // Root constant data space can be increased
                                 // at the expense of root variable data space
                                 // by raising this in increments of 0x1000 to
                                 // a maximum of 0xB000.
                                 // Root variable data space can be increased
                                 // at the expense of root constant data space
                                 // by lowering this in increments of 0x1000 to
                                 // a minimum of 0x1000.

	#else
		#define DATAORG      0x6000// Beginning logical address of variable data seg.
                                 // Root code space can be increased at the
                                 // expense of root variable data space by
                                 // raising this in increments of 0x1000 to a
                                 // maximum of 0xB000.
                                 // Root variable data space can be increased
                                 // at the expense of root code space by
                                 // lowering this in increments of 0x1000 to a
                                 // minimum of 0x3000.
	#endif //sep. i&d
#endif
#use "FATCONFIG.LIB"
#use "ERRLOGCONFIG.LIB"
#use "CLONECONFIG.LIB"
#use "MEMCONFIG.LIB"
#use "DKCONFIG.LIB"


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

#if (DK_ENABLE_WATCHEXPRESSIONS==1)
	#define WATCHCODESIZE    0x200   // Number of root RAM bytes for Watch Code
#else
	#define WATCHCODESIZE    0x00
#endif

#ifdef ZERO_OUT_STATIC_		      // Define macro to Zero out static data on startup/reset.
	#define ZERO_OUT_STATIC_DATA 1//  Not compatible with protected variables.
#else                            //  Does not conflict with GLOBAL_INIT.
	#define ZERO_OUT_STATIC_DATA 0
#endif

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

#use "TWOPROGRAMCONFIG.LIB"

#ifdef CC_VER
	#define __DC__		CC_VER
#endif
typedef unsigned char 		uint8;
typedef unsigned short int	uint16;
typedef short int				int16;
typedef unsigned long int	uint32;
typedef unsigned long int  faraddr_t;
typedef unsigned long int	longword;     // Unsigned 32 bit
#define FARADDR_NULL	0L

#ifdef PILOT_BIOS	// matching #endif at next "PB_Marker_A"
	typedef unsigned word;
#else	// ie:  ifndef PILOT_BIOS

///////////////////////////////////////////////////////////////////////
// ***** BIOS data structures *****************************************

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


typedef struct {
   unsigned short addr;  // address
   unsigned char base;   // base (BBR or CBR)
}   ADDR24_S;

typedef union {
	unsigned long l;    // long for increment/decrement
   struct {
		ADDR24_S a; // the address itself
      char flags; // flags associated
   } aaa;
} ADDR24;

typedef struct
{
   unsigned    short  Size;
   unsigned    short  Type;
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
               XDB,XDE,      // extended data (RAM) (Begin and End)
               RCDB,RCDE,  // root constant data (Begin and End)
               HPA;         // Highest address of program in flash (max of RCDE, RCE, XCE)
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
   int    errType;
#if ERRLOG_USE_MESSAGE
	char   message[8];
#endif
#if ERRLOG_USE_REG_DUMP
	struct _regBuffer regDump;
#endif
#if ERRLOG_STACKDUMP_SIZE
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

#endif	// matching #ifdef/#else at prior "PB_Marker_A"

//***** Error Logging Checks ******************************************
#if ENABLE_ERROR_LOGGING
	#if 0x10000ul <= ERRLOG_LOG_SIZE
		#error "Error log size too big"
	#endif
#endif

//***** Settings for bank control registers ***************************
//***  The first expression in each define must NOT have parenthesis.
//***  If both CS_FLASH and CS_FLASH2 are not defined, then can't write to Flash!

#if FLASH_COMPILE  // running in flash
	#define MB0CR_SETTING   FLASH_WSTATES | CS_FLASH  | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
	#if (_FLASH_SIZE_ == 0x80)
		#define MB1CR_SETTING   FLASH_WSTATES | CS_FLASH  | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
	#else
		#define MB1CR_SETTING   FLASH_WSTATES | CS_FLASH2 | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
	#endif
	#define MB2CR_SETTING   RAM_WSTATES | CS_RAM | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
	#define MB3CR_SETTING   RAM_WSTATES | CS_RAM | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)
#endif

#if FAST_RAM_COMPILE // compile to flash running in RAM
	#define MB0CR_SETTING   RAM_WSTATES | CS_RAM  | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
	#define MB1CR_SETTING   RAM_WSTATES | CS_RAM  | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
	#if (RUN_IN_RAM_CS == 0x2) // assumes second RAM on CS1
		#define MB2CR_SETTING   RAM2_WSTATES | CS_RAM2 | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
	#else
		#define MB2CR_SETTING   RAM_WSTATES | CS_RAM  | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
	#endif
	#define MB3CR_SETTING   FLASH_WSTATES | CS_FLASH | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)
#endif

#if RAM_COMPILE  // running in RAM
	#define MB0CR_SETTING   RAM_WSTATES | CS_RAM | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
	#define MB1CR_SETTING   RAM_WSTATES | CS_RAM | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
	#define MB2CR_SETTING   RAM_WSTATES | CS_RAM | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
	#define MB3CR_SETTING   RAM_WSTATES | CS_RAM | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)
#endif

// ***** System Org Definitions ***************************************
#define STACKORG        0xD000   // Beginning logical address for stack segment
#define ROOTCODEORG     0x0000   // Beginning logical address for root code
#define XMEM_WINDOW	  	0xE000UL // Beginning logical address for Xmem

// This should work for separate I&D!
//***** Compute the SEGSIZE value *************************************
#if (RAM_COMPILE)
	#if __SEPARATE_INST_DATA__
		#define MEMBREAK    0+(STACKORG/256) + (STACKORG/4096)  // SEGSIZE reg value
	#endif
#endif

#if FAST_RAM_COMPILE && (RUN_IN_RAM_CS == 0x2)
	#define MEMBREAK    0+(STACKORG/256) + ((STACKORG-BBROOTDATASIZE)/4096)  // SEGSIZE reg value, minus battery-backed size
	#define CLONE_MEMBREAK    0+(STACKORG/256) + (DATAORG/4096)  				// SEGSIZE reg value
	#if __SEPARATE_INST_DATA__
		#define CLONE_MMIDR_VALUE 0x4 | (MMIDR_VALUE) 					// Invert A16 for the data segment to allow cloning to run out of flash
	#else
		#define CLONE_DATASEGVAL 0x80 + (DATASEGVAL) 	// Adjust data segment to allow cloning to run out of flash (ram is moved up 512K)
	#endif
#endif

#ifndef MEMBREAK
	#define MEMBREAK 0+(STACKORG/256)+(DATAORG/4096)	// SEGSIZE reg value
#endif

//***** Compute Code and Data Xmem sizes ******************************
#ifndef FS_RESERVE_SIZE
	#define FS_RESERVE_SIZE 0	// default to no space reserved for the FS in the first flash
#endif

#if FLASH_COMPILE
 #ifdef USE_2NDFLASH_CODE
  #ifdef COMPILE_PRIMARY_PROG
   #error "USE_2NDFLASH_CODE is incompatible with COMPILE_PRIMARY_PROG."
   #fatal "Undefine one or both of USE_2NDFLASH_CODE or COMPILE_PRIMARY_PROG."
  #endif
  #ifdef COMPILE_SECONDARY_PROG
   #error "USE_2NDFLASH_CODE is incompatible with COMPILE_SECONDARY_PROG."
   #fatal "Undefine one or both of USE_2NDFLASH_CODE or COMPILE_SECONDARY_PROG."
  #endif
   #define XMEMORYSIZE2      FLASH_SIZE*4096L
 #endif

 #ifdef USER_BLOCK_NOT_IN_PROG_SPACE
   #define FLASH_USERBLOCK_SIZE 0
 #else
   #define FLASH_USERBLOCK_SIZE MAX_USERBLOCK_SIZE
 #endif

   #define XMEMORYSIZE FLASH_SIZE * 4096L - SID_64KADJUSTMENT - DATAORG - \
                        FLASH_USERBLOCK_SIZE - XMEM_RESERVE_SIZE - WATCHCODESIZE
#endif

#if RAM_COMPILE
	// Calculate reserved Xmem size for RAM compile mode
	#if __SEPARATE_INST_DATA__
		#if RAM_SIZE <= 0x20
			#if FAT_TOTAL
				#warnt "May be too little xmem RAM to support the FAT Data store."
			#endif
			#if FS2_RAM_RESERVE
				#warnt "May be too little xmem RAM to support the FS2 RAM store."
			#endif
			#if _SOS_USERDATA
				#warnt "May be too little xmem RAM to support the User Data store."
			#endif
		#endif
	#endif
	// 0xE000 = 0xD000 shared root space + 4kb flash transfer buffer
	//  (excludes stack space because that is xalloc'd later)
	#define XMEM_RAM_RESERVE  SID_64KADJUSTMENT+XMEM_WINDOW+TC_SYSBUF_BLOCK+FAT_TOTAL+(FS2_RAM_RESERVE+USERDATA_NBLOCKS)*4096L+WATCHCODESIZE+ROOTDATASIZE+VECTOR_TABLE_SIZE
#endif

#if FAST_RAM_COMPILE
	// 0xE000 = 0xD000 shared root space + 4kb flash transfer buffer
	//  (excludes stack space because that is xalloc'd later;
	//   minus bbram size because that is reserved elsewhere)
	#define XMEM_RAM_RESERVE SID_64KADJUSTMENT+0xE000L-BBROOTDATASIZE+TC_SYSBUF_BLOCK+WATCHCODESIZE+FLASH_BUF_SIZE
#endif

#define SID_XMEMORYSIZE SID_64KADJUSTMENT-SID_XCODE_START

#if RAM_COMPILE
	// Calculate available Xmem size for RAM compile mode
	#define XMEMORYSIZE RAM_SIZE*4096L-(XMEM_RAM_RESERVE)
#elif FAST_RAM_COMPILE
 #ifndef FLASH_USERBLOCK_SIZE
	#define FLASH_USERBLOCK_SIZE MAX_USERBLOCK_SIZE
 #endif
	// Calculate available Xmem size for fast RAM compile mode
	#define XMEMORYSIZE FLASH_SIZE * 4096L - \
	                     (SID_64KADJUSTMENT + XCODE_START + FLASH_USERBLOCK_SIZE)
#endif

#if FAST_RAM_COMPILE
	#define FLASHXMEMSIZE FLASH_SIZE*4096L-SID_64KADJUSTMENT-DATAORG-MAX_USERBLOCK_SIZE-XMEM_RESERVE_SIZE
#endif


//***** Compute Code and Data ORGs ************************************
#if FLASH_COMPILE
	#define WATCHCODEORG XMEM_WINDOW + 0x1000 - WATCHCODESIZE
#else
	#define WATCHCODEORG XMEM_WINDOW
#endif

#if 0
#if __SEPARATE_INST_DATA__
	#define WATCHCODEORG XMEM_WINDOW + (0xfff & ROOTCODESIZE)
#else // not separate I&D space for both RAM and FLASH
	#if FLASH_COMPILE
	#else
		#define WATCHCODEORG XMEMORYORG + WATCHCODESIZE
	#endif
	// allow 0x0200 for INTVECs and XINTVECs; if present, move down for battery backed area
#endif
#endif

//***** Compute the MMU segment registers *****************************

//
#define _RK_SEGSHIFT(X) X >> 4

// Address line values for setting the origin directive MMIDR inversions
// Bitwise or with __SEPARATE_INST_DATA__ to automatically enable/disable
// respective settings in the MMIDR (from MMIDR_VALUE) and origin directive.
#define _SID_ADDRESS_16 (0x10 & (__SEPARATE_INST_DATA__ << 4))
#define _SID_ADDRESS_19 (0x80 & (__SEPARATE_INST_DATA__ << 7))

// Specify inversions for the MMIDR required for separate I&D space
#if FLASH_COMPILE || FAST_RAM_COMPILE
	#define DATASEG_INV _SID_ADDRESS_19
	#define CODESEG_INV _SID_ADDRESS_16
#endif
#if RAM_COMPILE==1
	#define DATASEG_INV _SID_ADDRESS_16
	#define CODESEG_INV _SID_ADDRESS_16
#endif

// Setup the enable and inversion bits for the MMIDR.  The _SEPARATE_INST_DATA_
// macro value is used to set the I&D enable (5th) bit of the MMIDR.
#define MMIDR_ENABLE (__SEPARATE_INST_DATA__ << 5)

// Calculate bits 2 and 3 of the MMIDR (data seg inversions).  This calculation
// is a bit tricky here as the actual address line 19 and line 16 inversion values
// are used to set or unset the associated enable bits of the MMIDR.
#define MMIDR_DATA_INV (((DATASEG_INV & _SID_ADDRESS_19) >> 4) | \
                  		((DATASEG_INV & _SID_ADDRESS_16) >> 2))
// Calculate bits 0 and 1 of the MMIDR (code seg inversions).  This calculation
// is a bit tricky here as the actual address line 19 and line 16 inversion values
// are used to set or unset the associated enable bits of the MMIDR.
#define MMIDR_CODE_INV (((CODESEG_INV & _SID_ADDRESS_19) >> 6) | \
								((CODESEG_INV & _SID_ADDRESS_16) >> 4))

#if (_CPU_ID_) >= (R3000_R1)
	#define MMIDR_16BIT_IO_MODE 0x80
#else
	#define MMIDR_16BIT_IO_MODE 0x0
#endif

// The actual value to load into the MMIDR register.
#define MMIDR_VALUE  MMIDR_16BIT_IO_MODE | MMIDR_ENABLE | MMIDR_DATA_INV | MMIDR_CODE_INV |(CS1_ALWAYS_ON << 4)

#if __SEPARATE_INST_DATA__ || (FAST_RAM_COMPILE && RUN_IN_RAM_CS == 0x2)
	#define DATASEGVAL   0
#else
	//***** the 1+ represents space for a 4K flash transfer buffer
	#define DATASEGVAL RAM_START+RAM_SIZE-(1+TC_SYSBUF_BLOCK4K+FAT_NBLOCKS+FS2_RAM_RESERVE+ERRLOG_NBLOCKS+USERDATA_NBLOCKS+(STACKORG>>12))
#endif

#if _RK_FIXED_VECTORS
	#define SID_XCODE_START STACKORG
#else
	#define SID_XCODE_START ROOTCODESIZE
#endif


#if FLASH_COMPILE
  	#define XCODE_START DATAORG + WATCHCODESIZE
#else
  	#define XCODE_START STACKORG-BBROOTDATASIZE+WATCHCODESIZE
#endif

#define SID_XMEMSEGVAL  (0xff & ((SID_XCODE_START-XMEM_WINDOW)>>12))

#define SID_XMEMORYORG XMEM_WINDOW + ((SID_XCODE_START) & 0xFFF) // add 200 to allow space for watch code

#define XMEMORYORG 	  XMEM_WINDOW + (XCODE_START & 0xFFF)
#define XMEMSEGVAL      (0xff & ((SID_64KADJUSTMENT+XCODE_START-XMEM_WINDOW)>>12))

// We have a flash, which can be used for the file system
#if FAST_RAM_COMPILE
	#define FLASHXMEMSEGVAL  (0xff & ((SID_64KADJUSTMENT+DATAORG-XMEM_WINDOW)>>12))
#endif

#ifdef USE_2NDFLASH_CODE
	#if RAM_COMPILE==1 || FAST_RAM_COMPILE
		#undef USE_2NDFLASH_CODE
		#warnt "USE_2NDFLASH_CODE disabled when compiling to RAM."
	#endif
#endif

#ifdef USE_2NDFLASH_CODE
	#define XMEMSEGVAL2    (0xff & ((-XMEM_WINDOW)/4096 + FLASH_SIZE))
#endif

#if FLASH_COMPILE
	#define WATCHSEGVAL (((DATASEG_ORG_VAL*0x1000L)+ROOTDATAORG-(ROOTDATASIZE))>>12)-0xe
#else
	#define WATCHSEGVAL XMEMSEGVAL
#endif



/*
#if __SEPARATE_INST_DATA__
	#if FAST_RAM_COMPILE
		#define WATCHSEGVAL (ROOTCODESIZE>>12)-0xe
   #else
		#define WATCHSEGVAL RAM_START-1
   #endif
#elif FAST_RAM_COMPILE // not separate I&D space for both RAM and FLASH
	#define WATCHSEGVAL XMEMSEGVAL
#else
	// put watch code below data segment
#endif
*/

#if (FAST_RAM_COMPILE && (RUN_IN_RAM_CS == 0x2))	// Only if we have a battery-backed RAM on CS1
	#define BBROOTDATASIZE 0x1000			// Size of battery-backed data
	#define BBROOTDATAORG STACKORG-0x1	// battery-backed root data org
	#if __SEPARATE_INST_DATA__
		#define BBDATASEGVAL 0
	#else
		// If file system(s), a user data store and/or an error log exist on or
		//  use the /CS1 RAM, push the BB data segment down below them.
		#define BBDATASEGVAL RAM2_START+RAM2_SIZE-(FAT_NBLOCKS+FS2_RAM_RESERVE+ERRLOG_NBLOCKS+USERDATA_NBLOCKS+(STACKORG>>12))
	#endif
#else
	#define BBROOTDATASIZE 0				// Default to zero (does not exist)
#endif

#if __SEPARATE_INST_DATA__
	#define ROOTCONSTORG 0x0						 // Beginning of constants
	#define ROOTCONSTSIZE DATAORG					 // Constant space size
	#define ROOTCONSTSEGVAL 0x0					 // Constant segment offset
   #if _RK_FIXED_VECTORS
	#define ROOTCODESIZE (STACKORG-VECTOR_TABLE_SIZE) // Size of root code less vectors
   #else
	#define ROOTCODESIZE (STACKORG-BBROOTDATASIZE-RAMSR_SIZE) // Size of root code less vectors
   #endif
	#if RAM_COMPILE && (RAM_SIZE <= 0x20)
		#define ROOTDATAORG STACKORG-FLASHDRIVER_SIZE-BBROOTDATASIZE-0x1	// Beginning of root data (0x1000 for bbram)
	#else
		#define ROOTDATAORG STACKORG-FLASHDRIVER_SIZE-VECTOR_TABLE_SIZE-BBROOTDATASIZE-0x1	// Beginning of root data (0x1000 for bbram)
	#endif
#else // not Separate I&D
	#define ROOTCODESIZE DATAORG				// Size of root code
	#define ROOTDATAORG STACKORG-(FLASHDRIVER_SIZE+VECTOR_TABLE_SIZE)-BBROOTDATASIZE-0x1	// Beginning of root data
#endif
#define ROOTDATASIZE ROOTDATAORG-DATAORG+0x1	// Size of root data

/* marker for where the end of XMEM is */
#define END_OF_XMEMORY   (((XMEMSEGVAL<<12L)+(long)XMEMORYORG+(long)XMEMORYSIZE)&0xFFFFF)

#if FAST_RAM_COMPILE
	#define END_OF_FLASH_XMEM (((FLASHXMEMSEGVAL<<12L)+(long)XMEMORYORG+(long)FLASHXMEMSIZE)&0xFFFFF)
#endif

#define BBRAM_RESERVE_SIZE (USERDATA_NBLOCKS*4096L)+FAT_TOTAL+(FS2_RAM_RESERVE*4096L)+(ERRLOG_NBLOCKS*4096L)

#if FAST_RAM_COMPILE
	#define RAM_RESERVE_SIZE  TC_SYSBUF_BLOCK+FLASH_BUF_SIZE
#else
	#define RAM_RESERVE_SIZE  BBRAM_RESERVE_SIZE+TC_SYSBUF_BLOCK+FLASH_BUF_SIZE
#endif

//***** This is to let Dynamic C programs know we are running in Flash or RAM.
//***** _RAM_, and _FLASH_ are defined by the compiler.  RUN_IN_RAM
//***** predates these compiler generated macros.
//***** FAST_RAM_COMPILE mode compiles to flash and runs in RAM
#if FLASH_COMPILE
	#define RUN_IN_RAM 0
#else
	#define RUN_IN_RAM 1      // RAM_COMPILE || FAST_RAM_COMPILE
#endif

//***** Tell the compiler this is a BIOS ******************************
#pragma Rabbit CompileBIOS

#ifdef USE_2NDFLASH_CODE
	#if FS_RESERVE_SIZE > 0
		#error "Using second flash for both code and a filesystem is not currently supported."
	#endif
#endif

//**** internal and external interrupt vector table locations *****************
#define XINTVEC_BASE STACKORG-BBROOTDATASIZE-0x100
#define INTVEC_BASE  STACKORG-BBROOTDATASIZE-0x200

#if __SEPARATE_INST_DATA__
	#if FLASH_COMPILE
		#define SID_DATAORG RAM_START
	#endif
	#if RAM_COMPILE || FAST_RAM_COMPILE
		#define SID_DATAORG RAM_START+0x10
	#endif

	#if _RK_FIXED_VECTORS
		#undef XINTVEC_BASE
		#undef INTVEC_BASE

		#define XINTVEC_BASE xintvec_relay
		#define INTVEC_BASE intvec_relay

		#define SID_XINTVEC_BASE ROOTCODESIZE+0x100
		#define SID_INTVEC_BASE   ROOTCODESIZE

		#define INTVEC_RELAY_SETUP(x) $\
			lcall SID_DATAORG-(0xe-(0xf&((x)>>12))), XMEM_WINDOW | ((x) & 0x0FFF) $\
			jp sid_intvec_cleanup
 	#endif
	// Macro to load 16 bit values into a physical address
	#define PHY_LOAD_VALUE(paddr, value) $\
		ld a, paddr >> 16 $\
		ld hl, value $\
		ldp (paddr & 0xFFFF), hl


	// Assembly macros for Separate I&D space
	#define SEP_ID_ON ld a, MMIDR_VALUE $\
		ioi ld (MMIDR), a
	#define SEP_ID_OFF ld a, CS1_ALWAYS_ON << 4 $\
		ioi ld (MMIDR), a
#endif

#ifdef PILOT_BIOS	// matching #endif at next "PB_Marker_B"
// These directives are not actually used, but are present to silence
//  compiler complaints until the pilot BIOS's directives are applied.
#rcodorg rootcode 0x00 0x0000 0x0100 apply
#rvarorg rootdata 0x00 0x0200 0x0100 apply
#wcodorg watcode  0x00 0x0200 0x0100 apply
#xcodorg xmemcode 0xF2 0xE300 0x0100 apply
#else	// ie:  ifndef PILOT_BIOS

/*-------------------------------------------------------------------
**  CODE ORG statements to determine code locations in physical and
**  logical memory space.
**
**  Type      Name     Segment       Bottom       Size                 Use
--------------------------------------------------------------------*/

#rcodorg rootcode   0x00          _cexpr(ROOTCODEORG)  _cexpr(ROOTCODESIZE) ISPACE apply
#if __SEPARATE_INST_DATA__
	#xcodorg sid_xmemcode _cexpr(SID_XMEMSEGVAL)	_cexpr(SID_XMEMORYORG)	_cexpr(SID_XMEMORYSIZE)	apply
	#xcodorg xmemcode		 _cexpr(XMEMSEGVAL) 		_cexpr(XMEMORYORG)		_cexpr(XMEMORYSIZE) 		follows sid_xmemcode
#else
	#xcodorg xmemcode  	 _cexpr(XMEMSEGVAL) 		_cexpr(XMEMORYORG) 		_cexpr(XMEMORYSIZE) 		apply
#endif

#ifdef USE_2NDFLASH_CODE
	#xcodorg xmemcod2 (XMEMSEGVAL2) XMEMORYORG    XMEMORYSIZE2       follows xmemcode // this needs to change for sepid
#endif

#if FLASH_COMPILE
#pragma nowarn warns
#resvorg removeflash 0x0 0x0 0x80000 reserve
#elif FAST_RAM_COMPILE
// mark as bbram to ensure xalloc region is marked as battery backed.  See init_xalloc
#resvorg removeflash 0xc0 				0x0 			0x40000 batterybacked reserve
#endif

#define RAM_RESERVEORG ((DATAEND-(RAM_RESERVE_SIZE)+1) >> 12)
#define RAM_RESERVEBASE ((DATAEND-(RAM_RESERVE_SIZE)+1) & 0xfff)

#resvorg ramreserve   _cexpr(RAM_RESERVEORG)   _cexpr(RAM_RESERVEBASE)    _cexpr(RAM_RESERVE_SIZE)   reserve

#if RAM_COMPILE || FLASH_COMPILE && RAM_START + _RAM_SIZE_ < 0x100
   // removes and reserves repeated ram image
   #resvorg removeram RAM_START+_RAM_SIZE_ 0x0 0x100000-((RAM_START + _RAM_SIZE_)*0x1000L) reserve
#endif

#if FAST_RAM_COMPILE
	#define DATASEG_ORG_VAL  (DATASEGVAL | CODESEG_INV)
	#define BBDATASEG_ORG_VAL  (BBDATASEGVAL | DATASEG_INV)
	#if BBRAM_RESERVE_SIZE
   	#define BBRAMBEGIN_SEG 	_cexpr((DATAEND+1)>>12)

      #define BBRAMBEGIN_SIZE _cexpr((((long)BBDATASEG_ORG_VAL<<12)+BBROOTDATAORG \
      								-BBROOTDATASIZE)-(DATAEND+1))

		#if BBRAMBEGIN_SIZE != 0
		   #resvorg bbrambegin    BBRAMBEGIN_SEG  0   BBRAMBEGIN_SIZE batterybacked
		#endif
	#endif

#else
	#define DATASEG_ORG_VAL  (DATASEGVAL | DATASEG_INV)
#endif

#ifdef DATA2END
#define BBRAM_RESERVEORG  ((DATA2END-(BBRAM_RESERVE_SIZE)+1) >> 12)
#define BBRAM_RESERVEBASE ((DATA2END-(BBRAM_RESERVE_SIZE)+1) & 0xfff)

#if BBRAM_RESERVE_SIZE
#resvorg bbramreserve _cexpr(BBRAM_RESERVEORG) _cexpr(BBRAM_RESERVEBASE)  _cexpr(BBRAM_RESERVE_SIZE) reserve batterybacked
#endif

#endif

#define CONSTSEG_ORG_VAL (ROOTCONSTSEGVAL | CODESEG_INV)

/*------------------------------------------------------------------------------------------------
**  DATA ORG statements to determine data locations in physical and
**  logical memory space.
**
**  Type Name     	Segment        			Top          			Size               	Use
-------------------------------------------------------------------------------------------------*/
#rvarorg rootdata   _cexpr(DATASEG_ORG_VAL)	_cexpr(ROOTDATAORG)	_cexpr(ROOTDATASIZE)	DSPACE apply

#if !(_RK_FIXED_VECTORS)
	#resvorg vectors (DATASEG_ORG_VAL) INTVEC_BASE VECTOR_TABLE_SIZE reserve
#endif

#if BBROOTDATASIZE > 0
	#rvarorg bbrootdata (BBDATASEG_ORG_VAL) BBROOTDATAORG BBROOTDATASIZE batterybacked DSPACE apply
#endif

#if __SEPARATE_INST_DATA__
	#rconorg rootconst CONSTSEG_ORG_VAL ROOTCONSTORG ROOTCONSTSIZE apply
#endif

// ram area for flash driver run in ram buffer variables
#if FLASH_COMPILE
	#define FLASHDRIVER_SEG DATASEG_ORG_VAL
#else
	#define FLASHDRIVER_SEG CODESEG_INV
#endif

#if (DK_ENABLE_WATCHEXPRESSIONS==1)
	#wcodorg   watcode  (WATCHSEGVAL)  WATCHCODEORG WATCHCODESIZE     apply reserve
#endif

#if FLASHDRIVER_SIZE > 0
	#rvarorg flashdriver _cexpr(FLASHDRIVER_SEG) _cexpr(FLASHDRIVER_BASE)  _cexpr(FLASHDRIVER_SIZE) DSPACE
#endif

#endif	// matching #ifndef at prior "PB_Marker_B"

/************************** LOGICAL MEMORY MAP (Not sep I&D space) **********************

  0xFFFF                              ---------------
                                      |             |
                                      |             |
                                      ~             ~
                                           Xmem
                                      ~             ~
                                      |             |
                                      |             |
  0xE000                              ---------------
                                      | Stack Top   |
                                      |             |
                                      ~             ~
                                           Stack
                                      ~             ~
                                      |             |
                                      |             |
  STACKORG                            ---------------
  BBROOTDATAORG (STACKORG-1)          |             |
                                      |             |
                                      ~             ~
                                        possible
                                        bbram data
                                      ~             ~
                                      |             |
                                      |             |
  BBROOTDATAORG-BBROOTDATASIZE+1      ---------------
                                      | internal    |
                                      | vector table|
  ROOTDATAORG+WATCHCODESIZE+0x0180+1  ---------------
                                      | external    |
                                      | vector table|
  ROOTDATAORG+WATCHCODESIZE+0x0100+1  ---------------
                                      | internal    |
                                      | vector table|
  ROOTDATAORG+WATCHCODESIZE+1         ---------------
                                      |     |       |
                                      |    /|\      |
                                      |     |       |
                                      ~             ~
                                        Watch Code
                                      ~             ~
                                      |     |       |
                                      |    \|/      |
                                      |     |       |
   WATCHCODEORG (ROOTDATAORG+1)       ---------------
   ROOTDATAORG                        |     |       |
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
  ROOTCODEORG (0x0000)                ---------------

*******************************************************************/

#ifdef PILOT_BIOS	// matching #endif at next "PB_Marker_C"
	#use "SYSIO.LIB"        // IO register assignemnts and functions
	#use "FLASHWR.LIB"      // The flash writing functions
	#use "IDBLOCK.LIB"      // Flash ID block access fcns (and CRC).
	#use "CPUPARAM.LIB"     // CPU-specific settings
#else	// ie:  ifndef PILOT_BIOS

   #define _system
   #define _SYS_CALL_VARS
   #define _NET_SYSCALL(x)
   #define _stub
   #define SYS_LOCK_TCP()

   #define IOWRITE_A(VAL) ioi ld (VAL),a
   #define IOREAD_A(VAL)  ioi ld a,(VAL)

   #define defineErrorHandler _rs_defineErrorHandler

//***** Libraries needed by the BIOS *********************************
	#use "BIOSFSM.LIB"      // The communication Finite State Machine
	#use "SYSIO.LIB"        // IO register assignemnts and functions
	#use "DBUGKERN.LIB"     // The debug kernel
	#use "CSUPPORT.LIB"     // Some C support functions
	#use "FLASHWR.LIB"      // The flash writing functions
	#use "STACK.LIB"        // The stack initialization functions
	#use "IDBLOCK.LIB"      // Flash ID block access fcns (and CRC).
	#use "ERRORS.LIB"       // Runtime error handling
	#use "TC.LIB"           // New-style target communications
	#use "TC_SYSTEMAPP.LIB" // The System-type handler
	#use "DKTCBASE.LIB"		// debug communications interface
	#use "CPUPARAM.LIB"     // CPU-specific settings
	#use "MUTIL.LIB"        // Math support for error logging and other math manipulations in the bios

	#if __SEPARATE_INST_DATA__
	#use "SEPARATEID.LIB"
	#endif

	#use "XMEM.LIB"
	#use "RWEB_SUPPORT.LIB"

	#ifdef USE_TIMERA_PRESCALE
	   #if (CPU_ID_MASK(_CPU_ID_) < R3000)
	      #warns "Timer A prescale exists on Rabbit 3000 only; disabling USE_TIMERA_PRESCALE macro."
	      #undef USE_TIMERA_PRESCALE
	   #endif
	#endif

#if (_CPU_ID_ == R2000_R0) && __SEPARATE_INST_DATA__
   #fatal "Separate instruction and data space is not supported by this processor."
#endif

#pragma DATAWAITSUSED on

#if __SEPARATE_INST_DATA__
#if _RK_FIXED_VECTORS
#rcodorg   periodic_intvec 0x0   _cexpr(SID_INTVEC_BASE + PERIODIC_OFS)   0x10 ispace
#rcodorg rst10_intvec      0x0   _cexpr( SID_INTVEC_BASE + RST10_OFS)     0x10 ispace
#rcodorg rst18_intvec      0x0   _cexpr(SID_INTVEC_BASE + RST18_OFS)       0x10 ispace
#rcodorg rst20_intvec      0x0   _cexpr(SID_INTVEC_BASE + RST20_OFS)       0x10 ispace
#rcodorg rst28_intvec      0x0   _cexpr(SID_INTVEC_BASE + RST28_OFS)       0x10 ispace
#rcodorg rst38_intvec      0x0   _cexpr(SID_INTVEC_BASE + RST38_OFS)       0x10 ispace
#rcodorg slave_intvec      0x0   _cexpr(SID_INTVEC_BASE + SLAVE_OFS)       0x10 ispace
#rcodorg timera_intvec     0x0   _cexpr(SID_INTVEC_BASE + TIMERA_OFS)       0x10 ispace
#rcodorg timerb_intvec     0x0   _cexpr(SID_INTVEC_BASE + TIMERB_OFS)       0x10 ispace
#rcodorg sera_intvec       0x0   _cexpr(SID_INTVEC_BASE + SERA_OFS)         0x10 ispace
#rcodorg serb_intvec       0x0   _cexpr(SID_INTVEC_BASE + SERB_OFS)       0x10 ispace
#rcodorg serc_intvec       0x0   _cexpr(SID_INTVEC_BASE + SERC_OFS)       0x10 ispace
#rcodorg serd_intvec       0x0   _cexpr(SID_INTVEC_BASE + SERD_OFS)         0x10 ispace

#if CPU_ID_MASK(_CPU_ID_) >= R3000
#rcodorg sere_intvec       0x0   _cexpr(SID_INTVEC_BASE + SERE_OFS)         0x10 ispace
#rcodorg serf_intvec       0x0   _cexpr(SID_INTVEC_BASE + SERF_OFS)         0x10 ispace
#rcodorg inputcap_intvec   0x0   _cexpr(SID_INTVEC_BASE + INPUTCAP_OFS)   0x10 ispace
#endif

#if _CPU_ID_ >= R3000_R1
#rcodorg pwm_intvec        0x0	_cexpr(SID_INTVEC_BASE + PWM_OFS)			0x10 ispace
#endif

#rcodorg   quad_intvec     0x0   _cexpr(SID_INTVEC_BASE + QUAD_OFS)         0x10 ispace

#if _CPU_ID_ >= R3000_R1
#rcodorg secwd_intvec		0x0	_cexpr(SID_INTVEC_BASE + SECWD_OFS)		0x10	ispace
#endif

#rcodorg ext0_intvec       0x0    _cexpr(SID_XINTVEC_BASE + EXT0_OFS)     0x10 ispace
#rcodorg ext1_intvec       0x0    _cexpr(SID_XINTVEC_BASE + EXT1_OFS)     0x10 ispace
#endif //fixed vectors
#endif

	#if (ENABLE_CLONING == 1)
	#use "CLONE.LIB"        // Contains cloning support functions
	#endif

#endif	// matching #ifndef at prior "PB_Marker_C"

/********************************************************************
*********************************************************************
*
*   BIOS internals follow
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
#ifndef RAM2_WSTATES
#if (NUM_RAM2_WAITST==0)
#define RAM2_WSTATES 0xc0
#elif (NUM_RAM2_WAITST==1)
#define RAM2_WSTATES 0x80
#elif (NUM_RAM2_WAITST==2)
#define RAM2_WSTATES 0x40
#elif (NUM_RAM2_WAITST==4)
#define RAM2_WSTATES 0x00
#endif
#endif

#define SIZESTDIOBUFF 130

#define HW_MEMBREAK 0xe2

// disable clock spreader if CPU or board type doesn't support it
#if (_CPU_ID_ < R2000_R3) || (_BOARD_TYPE_ == BL1800) || (_BOARD_TYPE_ == BL1805)
   #undef ENABLE_SPREADER
   #define ENABLE_SPREADER   0
#endif

//*** end of macro definitions ***

#ifndef PILOT_BIOS	// matching #endif at next "PB_Marker_D"

//***** variable definitions ******************************************
#define BIOSSTACKSIZE 40
char dbXPC;

void *dkcState;                     //   the address (state) of  debugger kernel comm machine
char BiosStack[BIOSSTACKSIZE];       // initial stack for bios in data the segment
char commBuffer[256];               //   communication buffer
char stdioCommBuff[SIZESTDIOBUFF];   // STDIO buffer
char watchBuffer[8];                  //   buffer for watch expression results

struct _dcParam DCParam;

struct _DCSrcDebug DCSrcDebug;
char  OPMODE, _dkfirst;

// *** do not change the order of definition ***
struct _regBuffer registers;         //   RAM location to store the registers
char _dkenable28,_dkdisable28;       // byte to store at RST28 enable, disable
int _SectorSize;
int dkInitStkTop;
char   bios_divider19200;            // raw time constant calculated by BIOS
char   freq_divider;                  // time constant for 19200 baud

// store value of GCSR register to determine reset reason
char   reset_status;

char dkcstartuserprog;

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

// Rabbit 2000 rev 3 and Rabbit 3000-specific shadow registers
#if (_CPU_ID_ >= R2000_R3)
char GCM0RShadow, GCM1RShadow;
char MTCRShadow;
#endif

#if (CPU_ID_MASK(_CPU_ID_) >= R3000)
// Rabbit 3000-specific registers
char GPSCRShadow;
char PBDDRShadow;
char PFDRShadow, PFCRShadow, PFFRShadow, PFDDRShadow, PFDCRShadow;
char PGDRShadow, PGCRShadow, PGFRShadow, PGDDRShadow, PGDCRShadow;
char BDCRShadow;
char ICCSRShadow, ICCRShadow;
char ICT1RShadow, ICS1RShadow, ICT2RShadow, ICS2RShadow;
char PWL0RShadow, PWM0RShadow, PWL1RShadow, PWM1RShadow,
     PWL2RShadow, PWM2RShadow, PWL3RShadow, PWM3RShadow;
char QDCSRShadow, QDCRShadow;
char SEERShadow, SFERShadow;
char TAPRShadow;
char TAT8RShadow, TAT9RShadow, TAT10RShadow;
char STKCRShadow;
#endif // End of CPU_ID_MASK(_CPU_ID_) >= R3000

char NewPrintConFlag;

// dc_timestamp and prog_param are defined by the compiler.
extern struct progStruct prog_param;

// date/time stamp at the end of the compile (seconds since Jan. 1 1980)
extern long dc_timestamp;

#if __SEPARATE_INST_DATA__
   // relays for interrupts - do NOT change the order of declaration or declare
   // data between these arrays.
   char xintvec_relay[0x10][0x10];
   char intvec_relay[0x10][0x10];
#endif // End of __SEPARATE_INST_DATA__

#if ERRLOG_USE_MESSAGE
   char   errlogMessage[8];
#endif // End of ERRLOG_USE_MESSAGE

#if ENABLE_ERROR_LOGGING
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
 root void _xexit();
 root void __xexit__();
 xmem void dkSetIntVecTabP();
 xmem void dkSetIntVecTabR();
 root void dkStartup();
 root void dkEnd();
 root void symTab();
 root void symTabEnd();
 char      dkInBiosStack;
 unsigned int _xexithook;

//*** End Prototypes ***

#endif	// matching #ifndef at prior "PB_Marker_D"

#ifndef _TARGETLESS_COMPILE_
#define _TARGETLESS_COMPILE_ 0
#endif
#if (NUM_RAM_WAITST!=0)
#pragma DATAWAITSUSED on
#endif

#ifndef PILOT_BIOS	// matching #endif at next "PB_Marker_E"

// Setup interrupt relays to xmem if compiling to separate I&D space and Flash
#if __SEPARATE_INST_DATA__
#if _RK_FIXED_VECTORS
#asm
#rcodorg   periodic_intvec apply
		// Note that the periodic isr is overridden in VDRIVER.LIB,
		//  to be fast and non-modifiable.
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

#rcodorg sera_intvec apply
// Make serial A fast.  Use relay if you want interrupt to be modifiable.
		jp DevMateSerialISR
// To use relay uncomment the following line and comment out the preceding line.
//		INTVEC_RELAY_SETUP(intvec_relay + SERA_OFS)

#rcodorg serb_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERB_OFS)

#rcodorg serc_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERC_OFS)

#rcodorg serd_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERD_OFS)

#rcodorg ext0_intvec apply
		INTVEC_RELAY_SETUP(xintvec_relay + EXT0_OFS)

#rcodorg ext1_intvec apply
		INTVEC_RELAY_SETUP(xintvec_relay + EXT1_OFS)

#if CPU_ID_MASK(_CPU_ID_) >= R3000
#rcodorg sere_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERE_OFS)

#rcodorg serf_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + SERF_OFS)

#rcodorg inputcap_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + INPUTCAP_OFS)

#if _CPU_ID_ >= R3000_R1
#rcodorg pwm_intvec apply
      INTVEC_RELAY_SETUP(intvec_relay + PWM_OFS)
#endif

#rcodorg quad_intvec apply
		INTVEC_RELAY_SETUP(intvec_relay + QUAD_OFS)
#endif //>=R3000

#rcodorg rootcode resume

#endasm
#endif // fixed vectors
#endif // End of __SEPARATE_INST_DATA__

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
// When I&D space is enabled, the constants and variables in the
//  following block are only visible via ldp.
#asm
#if FAST_RAM_COMPILE
// Flag for the FAST_RAM_COMPILE copy
FastRAM_InRAM::
		dw		0
#endif
loaderParameters:
		dw		BIOSCodeEnd		; store BIOS size, RAM start location, and
#if FLASH_COMPILE || FAST_RAM_COMPILE
		dw		0
#else
		dw		InRAMNow
#endif
divider19200::
		db		_FREQ_DIV_19200_
#if ENABLE_CLONING == 1
I_am_a_clone::
		dw		0
I_am_a_sterile_clone::
		dw		0
#endif
#endasm

#if ENABLE_ERROR_LOGGING
#asm const
//*** These 4 labels and their data must be kept below address 0x80 !
ErrLogConfiguration::
		dw		0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff
ErrLogLastHeader::
		dw		0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff
#endasm
#endif
//***** Begin BIOS code ***********************************************
#asm
InFlashNow::
#if FLASH_COMPILE || FAST_RAM_COMPILE
dkInit::
		ljp	__dkinit__
#endasm

#asm xmem
__dkinit__::
		;; set 2 second WDT time out
		ld		a, 0x5A
ioi	ld		(WDTCR), a

		;; turn off secondary WDT
		;ld		a, 0x5A
ioi	ld		(SWDTR),a
		ld		a, 0x52
ioi	ld		(SWDTR),a
		ld		a, 0x44
ioi	ld		(SWDTR),a

#if _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
		;; BL2600Q's and RCM39xx's /BUFEN is SD card select, stop external I/O
		;; cycle toggling, set STATUS pin low, CLK pin off, /BUFEN pin high
		ld		a, 0xA3
#else	// _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
		;; set STATUS pin low and CLK pin off
		ld		a, 0xA0
#endif	// _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
ioi	ld		(GOCR), a
		ld		a, 00001000b	; normal oscillator, processor and peri.
									;  from main clock, no periodic interrupt
ioi	ld		(GCSR), a
	#if __SEPARATE_INST_DATA__
		;; error out if processor does not support I&D space
ioi	ld		a, (GCPU)
		and	0x1f
		ld		h, a
ioi	ld		a, (GREV)
		and	0x1f
		ld		l, a
		#if _RK_FIXED_VECTORS
      ; Test for R2000 Rev 0
		bool	hl
		jp		nz, dkSetMMU
      #else
      ; Test for Rabbit 3000A or later
      or    a
		ld		de, _cexpr(R3000_R1)
      sbc   hl, de
		jp		nc, dkSetMMU
      #endif

		ipset 3
		ld    a,0x00
		ioi   ld (WDTTR),a
__dki_resetloop:
	   jr		__dki_resetloop
	#endif
#endif

#if RAM_COMPILE
InRAMNow::
		ipset	3

		;; turn off secondary WDT
		ld		a, 0x5A
ioi	ld		(SWDTR),a
		ld		a, 0x52
ioi	ld		(SWDTR),a
		ld		a, 0x44
ioi	ld		(SWDTR),a

//****** a slight delay for things to settle
		ld		b, 50
		ld		de, 0e8h			; the upper 3 bits turn over every 1/1024 s
		ld		h, 0
oloop:
		ld		a, 0x5a
ioi	ld		(WDTCR), a
		ld		a, 0ffh			; write any value to 0r to capture count
ioi	ld		(RTC0R), a
		ld		iy, RTC0R
ioi	ld		L, (iy)
		and	hl, de
loop:
		ld		a, 0ffh			; capture and compare
ioi	ld		(RTC0R), a
		ld		iy, RTC0R
ioi	ld		a, (iy)
		and	e
		cp		L
		jr		z, loop
		djnz	oloop				; loop until the first value reached

#if _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
		;; BL2600Q's and RCM39xx's /BUFEN is SD card select, stop external I/O
		;; cycle toggling, set STATUS pin low, CLK pin off, /BUFEN pin high
		ld		a, 0xA3
#else	// _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910  || _BOARD_TYPE_ == BL2600Q
		;; set STATUS pin low and CLK pin off
		ld		a, 0xA0
#endif	// _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910  || _BOARD_TYPE_ == BL2600Q
ioi	ld		(GOCR), a
		ld		a, 00001000b	; normal oscillator, processor and peri.
									;  from main clock, no periodic interrupt
ioi	ld		(GCSR), a
#endif	// end of if RAM_COMPILE

// *** Set the Segment size and data segment ***
dkSetMMU:
		ld		a, MEMBREAK
ioi	ld		(SEGSIZE), a

#if FAST_RAM_COMPILE
		#pragma nowarn warnt	// suppress following line's trivial demotion warning
		ld		a, BBDATASEGVAL
#else
		#pragma nowarn warnt	// suppress following line's trivial demotion warning
		ld		a, DATASEGVAL
#endif
ioi	ld		(DATASEG), a

// *** Set the MMIDR, turn on Separate ID (if enabled) ***
		ld		a, MMIDR_VALUE
ioi	ld		(MMIDR), a

#if __SEPARATE_INST_DATA__ && !(_RK_FIXED_VECTORS)
		ld    a, RAMSR_VALUE
ioi   ld    (RAMSR), a
#endif

#if FAST_RAM_COMPILE
// **NOTE** This code needs to come right after the write to MMIDR
//           so that, if enabled, Separate I&D is on before the copy
// For the FAST_RAM_COMPILE mode, we need to copy the entire flash
// to the fast RAM, and then restart the BIOS in RAM. First we copy
// each bank of flash to the RAM, mapping the RAM to physical memory
// bank 3 for the copy.
//
// After the copy, we remap the physical banks as follows
// (for boards with 512K Flash, 512K Fast RAM, and optionally,
//  a 128K battery-backed RAM):
//
//      +--------------------------------------+ 0xFFFFF
//      | Bank 3 (MB3CR)                        |
//      | Reserved for Bank Switching/Flash      |
//      | File-system                           |
//      |                                        |
//      +--------------------------------------+ 0xC0000
//      | Bank 2 (MB2CR)                        |
//      | Upper 256L of 512K Fast RAM on CS/2  |
//      | OR                                    |
//      | 128K Battery-Backed RAM on CS/1      |
//      +--------------------------------------+ 0x80000
//      | Bank 1 (MB1CR)                        |
//      | Upper 256K of 512K Fast RAM on CS/2  |
//      |                                        |
//      |                                        |
//      +--------------------------------------+ 0x40000
//      | Bank 0 (MB0CR)                        |
//      | Lower 256K of 512K fast RAM on CS/2  |
//      |                                        |
//      |                                        |
//      +--------------------------------------+ 0x00000
//

// NOTES:
//         1) We cannot assign the shadows for the Memory Bank Control Registers
//          (MBxCRShadow), or any other variables before the copy, since
//            the RAM will be overwritten.
//         2) We do not set up any stack, since it would be destroyed in
//            the copy. We use the HL' reigister to store the
//            return address for any function calls, and use jp (HL) to return.

// First we check that we are running in flash to do the copy
// otherwise, we are running in RAM and have already finished the copy
// FastRAM_InRAM: 1=running in RAM, 0=Running in FLASH
		xor	a							; MB0CR quadrant
		ld		ix, FastRAM_InRAM		; Address of the flag
		ldp	hl, (ix)					; Load the flag
		bit	0, l						; Set 0 flag if the bottom bit is 0 (In Flash)
		jp		nz, .fastRAMCopyDone	; skip copy if already done (FastRAM_InRAM == 1)

.swapProgToRAM::
;; Swap the compiled program over to RAM from flash. Maps flash to
;; the first 2 physical memory banks (MB0CR, MB1CR), and RAM to
;; the top banks (MB2CR, MB3CR)

		;; Map flash to bottom 2 memory banks
		ld		a, FLASH_WSTATES | CS_FLASH
ioi	ld		(MB0CR), a				; Map memory bank 0
ioi	ld		(MB1CR), a				; Map memory bank 1

		;; Map execution RAM to top 2 memory banks
		ld		a, RAM_WSTATES | CS_RAM | BANK_INVERT_A19
ioi	ld		(MB2CR), a
ioi	ld		(MB3CR), a


		;; Copy the MB0CR quadrant of flash to the MB2CR quadrant of RAM
		ld		a, 0x00					; copy MB0CR quadrant (0x00000-0x3FFFF) to
		ld		c, 0x08					;  address 0x80000 higher (0x00000 of RAM)
		ld		de, .swapProgToRAM2	; Put return address in HL'
		ld		hl', de
		jp		.copyFlashToRAM
// DO NOT INSERT CODE HERE - .copyFlashToRam will return to the next line
.swapProgToRAM2::

		;; Copy flash to RAM
		ld		a, 0x04					; copy MB0CR quadrant (0x00000-0x3FFFF)
		ld		c, 0x08					; to address 0x80000 higher (0x40000 of RAM)
		ld		de, .framswitch		; Put return address in HL'
		ld		hl', de
		jp		.copyFlashToRAM
// DO NOT INSERT CODE HERE - .copyFlashToRam will return to the next line
.framswitch::

   ;; Set the new flag value (in RAM, now that the copy is complete)
		ld		a, 0x8					; bottom (MB2CR) quadrant of RAM
		ld		ix, FastRAM_InRAM		; Address of the flag
		ld		hl, 0x1					; We will be running in RAM
		ldp	(ix), hl					; Store the flag

;; Set up the FAST_RAM_COMPILE physical memory mapping
;; This needs to be done before the BIOS is restarted
;; (the "jp 0" instruction below) since we now want
;; to run the program in RAM, not flash.

		;; map bank 3 to the Flash on /CS0
		ld		a, MB3CR_SETTING
ioi	ld		(MB3CR), a

		;; map bank 2 to the RAM on /CS1, or replicate Bank 1
		ld		a, MB2CR_SETTING
ioi	ld		(MB2CR), a

		;; map bank 1 to the upper portion of the RAM on /CS2
		ld		a, MB1CR_SETTING
ioi	ld		(MB1CR), a

		;; map bank 0 to bottom half of RAM on /CS2
		ld		a, MB0CR_SETTING
ioi	ld		(MB0CR), a

		;; Jump to address 0 to restart BIOS (In RAM now)
		jp		0
// ****** End of FAST_RAM_COMPILE copy *******

// .copyFlashToRAM
// pseudo-function (does not use any stack) used to copy the contents of
// the flash to the fast RAM for the FAST_RAM_COMPILE mode
.copyFlashToRAM::
		;; starting 64kb block passed in A (bits 16-19 of physical address)
		;; RELATIVE offset (in 64kb blocks) to destination FROM SOURCE passed in C
		;; IX is the increment offset
		ld		ix, 0x0000				; starting address, bits 00-15
		ld		b, 4						; number of 64kb blocks to copy
.bigloop:
		ld		de, 0x8000				; number of WORDS in 64kb block

		ex		af, af'					; Save a
.loop:
		ex		af, af'					; Restore a
		ldp	hl, (ix)					; Load word from source into HL
											;  (A:IX is 20-bit address)
		add	a, c						; Add address and offset to get ldp destination
		ldp	(ix), hl					; Copy word to destination
		sub	a, c						; Subtract offset to get back to source address
		ex		af, af'					; Save a

		inc	ix							; Increment IX to next word address
		inc	ix

		dec	de							; 1 less word to copy
		ld		a, d						; Load top half of words to copy into A
		or		e							; Logical OR with bottom half, A contains
											;  non-zero value if more to copy
		jr		nz, .loop				; More to copy? Loop

		ex		af, af'					; Restore a
		inc	a							; Address of next block to copy
		djnz	.bigloop					; decrement number of blocks to copy and jump
											;  if remaining

		exx								; Swap regs with alternates to retrieve return
		jp		(hl)						;  address (no stack so we used hl to store it)

;;-----------------------------------------------
;; Continue running the BIOS after the copy
.fastRAMCopyDone::

;; Set the Memory Bank Control Register Shadows
;; now that we are running in RAM
#endif // ----- FAST_RAM_COMPILE -----

#if !FAST_RAM_COMPILE
;; If FLASH_COMPILE or RAM_COMPILE modes, we must map our memory devices now,
;;  before we attempt to write anything to any of them.  (If FAST_RAM_COMPILE
;;  mode, appropriate memory devices mapping has already been done; see above.)
;;
;; At this point execution actually continues at the copy of the next
;;  instruction in the MB0CR_SETTING memory device.
		ld		a, MB0CR_SETTING
ioi	ld		(MB0CR), a
		ld		a, MB1CR_SETTING
ioi	ld		(MB1CR), a
		ld		a, MB2CR_SETTING
ioi	ld		(MB2CR), a
		ld		a, MB3CR_SETTING
ioi	ld		(MB3CR), a
#endif	// !FAST_RAM_COMPILE

#if ZERO_OUT_STATIC_DATA
		//****************************************
		// Initialize dataseg data area to zeros
		//  ~500K clocks for 0x7000
		//***************************************
		ld		a, 0x5a					; hitwd
ioi	ld		(WDTCR), a
		ld		hl, ROOTDATAORG
		ld		de, ROOTDATASIZE
		xor	a
		sbc	hl, de
		inc	hl
		ld		bc, ROOTDATASIZE
.zISDloop:
		ld		(hl), a
		inc	hl
		dec	bc
		cp		c
		jr		nz, .zISDloop
		cp		b
		jr		nz, .zISDloop
		ld		a, 0x5a					; hitwd
ioi	ld		(WDTCR), a
#endif


;; Now that we have mapped our memory devices and possibly zeroed out our
;;  static data area, we can actually save things to memory!
		; Set MMIDR Shadow
		ld		a, MMIDR_VALUE
		ld		(MMIDRShadow), a

		; Set MB0CR shadow
		ld		a, MB0CR_SETTING
		ld		(MB0CRShadow), a

		; Set MB1CR shadow
		ld		a, MB1CR_SETTING
		ld		(MB1CRShadow), a

		; Set MB2CR shadow
		ld		a, MB2CR_SETTING
		ld		(MB2CRShadow), a

		; Set MB3CR shadow
		ld		a, MB3CR_SETTING
		ld		(MB3CRShadow), a

dkSetSP:
		;; set up temporary stack in RAM
		ld		hl, BiosStack			; Set up a temporary stack to perform call.
		ld		de, BIOSSTACKSIZE-1	; Note that stack segment register is not setup
		add	hl, de					; here, but in _ProgStkSwitch which is normally
		ld		sp, hl					; called in premain after stack alloc init.
		ld		a, 1						; Now that all the memory devices are mapped,
		ld		(dkInBiosStack), a	;  set our "in BIOS stack" flag.

#if RAM_COMPILE
;; RAM compile mode-specific initialization
	#if !RAMONLYBIOS
		; Map in the flash only if it exists!
		ld		a, FLASH_WSTATES | CS_FLASH
		ld		(MB3CRShadow), a
ioi	ld		(MB3CR), a

		ld		hl, 0x08					; look for an ID block at the top
		call	_readIDBlock			;  of the /CS0 flash
		ld		a, MB3CR_SETTING
		ld		(MB3CRShadow), a
ioi	ld		(MB3CR), a
		bool	hl
		jr		z, idBlockOk
	#endif	// !RAMONLYBIOS
		;; erase SysIDBlock if error returned or if RAM-only BIOS
		ld		hl, SysIDBlock
		ld		b, SysIDBlock+marker+6-SysIDBlock
		xor	a
blockEraseLoop:
		ld		(hl), a
		inc	hl
		djnz	blockEraseLoop
idBlockOk:

#endif	// end of RAM_COMPILE section
		;; measure crystal frequency for proper baud rates
		xor	a
ioi	ld		(GCDR), a				; disable clock doubler (if enabled by pilot)
		ld		a, MB0CR_SETTING
		and	0x3F
		or		0x40						; enable 2 wait states
ioi	ld		(MB0CR), a

#ifdef ROBUST_NON_BB_BAUD_RATE_CALC
		;; initialize baud rate calculation comparison value
		;; (this comparison makes non-battery-backed applications' baud rate
		;;  calculation much more robust, at the cost of adding at least 1/16
		;;  second to Rabbit's startup time)
		xor	a							; "impossible" initial comparison value
.outer_timing_loop:
		ld		(bios_divider19200), a	; save for next comparison (or later use)
#endif

		;; measure crystal frequency for baud rate calculations
		ld		bc, 0000h				; our counter
		ld		de, 07FFh				; mask for RTC bits

		;; ensure that no interrupts interfere with timing loop
		ipset	3

.wait_for_zero:
ioi	ld		(RTC0R), a				; fill RTC registers
ioi	ld		hl, (RTC0R)				; get lowest two RTC regs
		and	hl, de					; mask off bits
		jr		nz, .wait_for_zero	; wait until bits 0-10 are zero

.timing_loop:
		inc	bc							; increment counter
		push	bc							; save counter
		ld		b, 98h					; empirical loop value
											;  (timed for 2 wait states)
		ld		hl, WDTCR
.delay_loop:
ioi	ld		(hl), 5Ah				; hit watchdog
		djnz	.delay_loop

		pop	bc							; restore counter
ioi	ld		(RTC0R), a				; fill RTC registers
ioi	ld		hl, (RTC0R)				; get lowest two RTC regs
		bit	2, h						; test bit 10
		jr		z, .timing_loop		; repeat until bit set

		;; timing loop done, interrupts are OK now
		ipres

		ld		hl, 0x0008
		add	hl, bc					; add 8 (equiv. to rounding up later)
		rr		hl
		rr		hl
		rr		hl
		rr		hl							; divide by 16
		ld		a, l						; this is our divider for 57600 baud
		add	a, l
		add	a, l						; multiply by 3 to get 19200 baud

#ifdef ROBUST_NON_BB_BAUD_RATE_CALC
		ld		hl, bios_divider19200
		cp		(hl)						; set Z flag if consecutive matching values
		jr		nz, .outer_timing_loop	; if non-matching divider value then re-do
#else
		ld		(bios_divider19200), a	; save for later use
#endif

		ld		a, MB0CR_SETTING		; restore MB0CR_SETTING
ioi	ld		(MB0CR), a

ioi	ld		a, (GCSR)
		ld		(reset_status), a		; save bits in GCSR to determine reason for reset
#if FLASH_COMPILE || FAST_RAM_COMPILE
		lcall	_more_inits0			; Call code for flash initialization
#endif	// end of FLASH || FAST_RAM_COMPILE section

		lcall	_more_inits02

#if RAM_COMPILE
;******************************************************************************
; Enter here for software reset requested by Dynamic C
; to turn off periodic interrupt
dkInit::
		ipset	3
		ld		a, 00001000b			; normal oscillator, processor and peri.
											;  from main clock, no periodic interrupt
ioi	ld		(GCSR), a

;*******************************************************************************
#endif

		; initialize comm ports now
		;  make sure most of the serial ports are disabled
		xor	a							; get zero into register A
		ld		(SBCRShadow), a
ioi	ld		(SBCR), a
		ld		(SCCRShadow), a
ioi	ld		(SCCR), a
		ld		(SDCRShadow), a
ioi	ld		(SDCR), a
#if (CPU_ID_MASK(_CPU_ID_) >= R3000)
		ld		(SECRShadow), a
ioi	ld		(SECR), a
		ld		(SFCRShadow), a
ioi	ld		(SFCR), a
#endif
		;  set serial port a to async, 8-bit, interrupt priority 1
		ld		a, 0x01
		ld		(SACRShadow), a
ioi	ld		(SACR), a

dkSetComm:
		lcall	dkInitTC
		lcall	dkSetIntVecTabP
dkSetProgPort:
		; The following code only sets up the programming port in separate I&D
		;  space if the jump to DevMateSerialISR is not hardcoded.
		;  (See sera_intvec org and relay code above.)
		; Parallel port D bits 6,0 as output lines.

		ld		a, _OP_JP			; jump instruction
		ld		(INTVEC_BASE+SERA_OFS), a
		ld		hl, DevMateSerialISR
		ld		(INTVEC_BASE+SERA_OFS+1), hl
#if (ENABLE_CLONING == 1)
		xor	a
		ldp	hl, (I_am_a_sterile_clone)	; in code space, so must access via ldp
		bool	hl
		jr		nz, NotCloning

		;; check to see if cloning cable is attached
		ld		a, 0x02
		ld		hl, PBDR
ioi	and	a, (hl)

		;; see if cloning cable is attached (was PB1 low?)
		jr		nz,  NotCloning
		ljp	CloneMode
NotCloning:
#endif

		lcall	_init_IO_regs

#ifndef COMPILE_SECONDARY_PROG
		; check for run mode
ioi	ld		a, (SPCR)
		and	60h
		cp		60h
		jp		nz, RunMode
		; Reset
#else
		jp		RunMode					; can't debug secondary program, so run it
#endif

#if (ENABLE_CLONING==1)
		xor	a
		ldp	hl, (I_am_a_clone)   ; in code space, so must access via ldp
		bool	hl
		jp		nz, RunMode
#endif
#if (CPU_ID_MASK(_CPU_ID_) >= R3000)
		xor	a
		ld		(BDCRShadow), a		; RST 28Hs are active
#endif
		jp		__dktrap					; switch back to root

#endasm

#asm
__dktrap::

; When a program is compiled to run from RAM or to small sector flash, Dynamic C
; compiles and downloads the bios through the pilot, starts the bios, then
; compiles the program and downloads the program through the bios.  The bios
; will wait in this loop until Dynamic C informs the bios that the user program
; has been completely downloaded.

		ld		a,RAM_COMPILE
      or		a
      jr		nz,.need_user_prog
		ld		a,(_FlashInfo+writeMode)
      cp		2
      jp		nz,StartUserCode

.need_user_prog:
		call _init_dkLoop
.waitforuserprog:
		call	bioshitwd
		ld		a,(dkcstartuserprog)
      or		a
      jr		z,.waitforuserprog

		jp		StartUserCode
RunMode:
		ipset	3
		lcall	dkSetIntVecTabR		; disable debug RSTs
		ipset	0

//***** Jump to user code *********************************************
		ld		hl, OPMODE
		ld		(hl), 0x80
#if FLASH_COMPILE
		lcall	WaitSettle
#endif
		jp		StartUserCode


#endasm
// *** End of main BIOS code ***

//***** BIOS functions ************************************************
#asm xmem
;; This code repeatedly measures the timing on the 32kHz oscillator until
;; it matches twice in a row.  The 32kHz oscillator circuit could take as
;; long as 1.5 seconds to rise to a full voltage and we want to wait until
;; that is true...

WaitSettle::
   ld      de, 0000h            // first measurement value
   push   de                     // save it for later
startmeas:
   ld      bc, 0000h            // our counter
   ld      de, 07FFh            // mask for RTC bits
wait_for_zero:
   ioi   ld (RTC0R), a         // load RTC registers
   ioi   ld hl, (RTC0R)         // read 1st two RTC registers
   and   hl, de               // mask off bits
   jr      nz, wait_for_zero      // repeat until all zero
timing_loop:
   inc   bc                     // increment counter
   push   bc                     // save counter
   ld      b, 057h               // empirically-derived loop counter
   ld      hl, WDTCR
delay_loop:
   ioi   ld   (hl), 05Ah         // hit watchdog
   djnz   delay_loop
   pop   bc                     // restore counter
   ioi   ld (RTC0R), a         // load RTC registers
   ioi   ld hl, (RTC0R)         // read 1st two RTC registers
   bit   2, h                  // test bit for 0800h
   jr      z, timing_loop         // if nonzero, keep looping

   ld      h, b                  // h = upper nibble of counter
   ld      a, c
   add   a, 08h               // (effectively rounding up)
   ld      l, a                  // l = lower nibble of counter + 8
   rr      hl
   rr      hl
   rr      hl                     // divide by 16
   rr      hl
   ld      a, l                  // lower nibble is divisor!
   dec   a
   pop   de                     // get last count
   cp      a, e                  // compare last and current counts
   jr      z, donemeas            // if alike, done
   ld      e, a
   push   de                     // save this count
   jr      startmeas
donemeas:
   lret

dkInitTC::
   ld      hl,DevMateReadPort
   ld      (TCState+[TCState]+ReadPort),hl      ; the low-level read driver
   ld      hl,DevMateWritePort
   ld      (TCState+[TCState]+WritePort),hl      ; the low-level write driver
   ld      hl,DevMateClearReadInt
   ld      (TCState+[TCState]+ClearReadInt),hl   ; low-level driver to clear spurious ints
   ld      hl,dkcRXBufferListStore
   ld      (TCState+[TCState]+RXBufferList),hl   ; the list of receive buffers, sorted by 'type'
   ld      hl,dkcCallbackListStore
   ld      (TCState+[TCState]+CallbackList),hl   ; the list of callbacks, sorted by 'type'
   ld      hl,dkcSystemWriteHeader
   ld      (TCState+[TCState]+SysWriteHandler),hl   ; special-case hander for sys-writes
   xor   a
	ld		  (dkcstartuserprog),a
   inc   a
   ld      (TCState+[TCState]+TrapSysWrites),a      ; flag that sys-writes should be handled as a special case

    ld      iy,TCState
   lcall   dkcSystemBufINIT      ;  initialize the system buffers
   lcall   dkcInit               ;   initialize comm module
   call    dkcSystemINIT         ;  initialize the system-type handler

   ld     hl, _xexit
   ld     (DCParam+errorExit), hl  ; initialize error exit for exception handler

   call   dkInitDebugKernelComs ; initial debug kernel - target communication interface
   lret
#endasm

//*** End Xmem BIOS code section ***
///////////////////////////////////////////////////////////////////////

#asm root
dkcDoSerial::
   ld      iy,TCState         ; iy == pointer to our state structure
   ioi    ld a,(SASR)         ;   check the status
   bit   SS_RRDY_BIT,a         ;  was a character received?
   jr      z,_DevMatenoRxIntReq

   call   dkcEntryRX         ;  handle the RX interrupt
   jr      _DevMatereadyToExit      ;  all done for now

_DevMatenoRxIntReq:
   bit   3,a
   jr      z,_DevMateSecondTXInt
   call   dkcEntryTX         ;   handle the TX interrupt
   jr      _DevMatereadyToExit

_DevMateSecondTXInt:
   ld      a,(TCState+[TCState]+TXBusy)
   or      a
   jr      z,_DevMateSkipInt

   ; this really is a good int
   call   dkcEntryTX
   jr      _DevMatereadyToExit

_DevMateSkipInt:
   ; just clear the int
   ioi    ld (SASR), a

_DevMatereadyToExit:
   ret

DevMateSerialISR::
   ;   serial interrupt handler
   push   ip
   push   af
   ex    af,af'
   push   af
   ld    a,xpc
   push   af
   push   bc
   push   de
   push   hl
   push  ix
   push   iy

   exx
   push   bc
   push   de
   push   hl

   call   dkcDoSerial

   pop   hl
   pop   de
   pop   bc
   exx

   pop   iy
   pop   ix
   pop   hl
   pop   de
   pop   bc
   pop   af
   ld    xpc,a
   pop   af
   ex    af,af'
   pop   af
   pop   ip
   ipres

   ret

DevMateReadPort::
   ;   destroys A
   ;   returns byte read (if any) in A
   ;   returns with Z set if nothing is read

   ;   check if there is anything available
   ioi    ld a, (SASR)
   bit   SS_RRDY_BIT,a      ;   if a received byte ready?
   ret   z                  ;   nope, return with z set
   ;   otherwise, a byte *is* ready, read from data port
   ioi    ld a, (SADR)
   ret                     ;   return with z *not* set

DevMateClearReadInt::
   ld      a,SS_RRDY_BIT
   ioi   ld (SASR),a
   ret

DevMateWritePort::   ;   assumes byte to transmit is in C
   ;   destroys A
   ;   returns with Z reset if not transmitted

   ;   check if the port is ready
   ioi    ld a, (SASR)
   bit   SS_TFULL_BIT,a      ;   can I transmit now?
   ret   nz                  ;   nope, return with nz set
   ;   otherwise, the transmit buffer is ready, write to it!
   ld      a,c               ;   move byte to transmit to a
   ioi    ld (SADR), a
   ret                     ;   return with z *not* set
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
   dec sp          ; retrieve the xpc
   ex (sp), hl
   ld l, a
   ld a, h
   ld xpc, a
   ld a, l
   ex (sp), hl
   inc sp          ; adjust stack for a normal return
   inc sp
   ret

bioshitwd::
   push  af
   ld    a, 0x5a
   ioi    ld (WDTCR),a
   pop  af
   ret
#endasm


#asm
_xexit::
	ld		ix,(_xexithook)
	jp		(ix)

/*
__xexit__::
   ipset 3
   ld    a,0x00
   ioi   ld (WDTTR),a
_xexitLoop:
   jr		_xexitLoop
*/
#endasm

#asm xmem

dkSetIntVecTabP::    ; Program mode
   ;   set up interrupt vector table
#if __SEPARATE_INST_DATA__ && _RK_FIXED_VECTORS
   ld      a,0xff & (SID_INTVEC_BASE >> 8)   ;
   ld      iir,a
   ld      a,0xff & (SID_XINTVEC_BASE >> 8)   ;
   ld      eir,a
#else
   ld      a,0xff & (INTVEC_BASE >> 8)   ;
   ld      iir,a
   ld      a,0xff & (XINTVEC_BASE >> 8)   ;
   ld      eir,a
#endif

#if (CPU_ID_MASK(_CPU_ID_) >= R3000)
   ld    a,0x80
   ioi   ld (BDCR),a      ; make RST 28Hs NOPs
   ld    (BDCRShadow),a
#endif
   lret

dkSetIntVecTabR::   ; Runmode
   ;   set up interrupt vector table
#if __SEPARATE_INST_DATA__ && _RK_FIXED_VECTORS
   ld      a,0xff & (SID_INTVEC_BASE >> 8)   ;
   ld      iir,a
   ld      a,0xff & (SID_XINTVEC_BASE >> 8)   ;
   ld      eir,a
#else
   ld      a,0xff & (INTVEC_BASE >> 8)   ;
   ld      iir,a
   ld      a,0xff & (XINTVEC_BASE >> 8)   ;
   ld      eir,a
#endif
   ld      a,0xc9      ;   this is the ret instruction
   ld      (INTVEC_BASE+RST18_OFS),a      ;   all are relays
   ld      (INTVEC_BASE+RST20_OFS),a
   ld      (INTVEC_BASE+RST28_OFS),a

#if (CPU_ID_MASK(_CPU_ID_) >= R3000)
   ld    a,0x80
   ioi   ld (BDCR),a      ; make RST 28Hs NOPs
   ld    (BDCRShadow),a
#endif
   lret
_more_inits0::

#if ENABLE_ERROR_LOGGING
   lcall  errlogRecordStartStatus
#endif

   //
   //  Probe for the System ID block.  Tell _readIDBlock() where the flash
   //  chips are mapped.  It will pick the high address end to look for it.
   //  If not found, then destroy the ident marker; _readIDBlock() will
   //  already set the tableVersion field to 0 when it can't find the thing.
   //  Also initialize Flash driver, could reserve space for File System.
   //
   //  Don't need to adjust User Block start address since it's relative
   //  offset to where SysID block is located (thanks Lynn!).
   //
#ifndef COMPILE_PRIMARY_PROG
#ifndef COMPILE_SECONDARY_PROG
	; standard program, not a DLM or a DLP
	#if FAST_RAM_COMPILE
		; SysIDBlock area of flash mapped into MB3CR quadrant
		ld		hl, 0x08
	#elif (_FLASH_SIZE_ == 0x80)
		; primary 512K flash, SysIDBlock in MB1CR quadrant
		ld		hl, 0x01 | 0x02
	#else
		; primary 256K or 128K flash, SysIDBlock in MB0CR quadrant
		ld		hl, 0x01
	#endif
#endif
#endif
#ifdef COMPILE_PRIMARY_PROG
  #ifdef ROUTE_A18_TO_FLASH_A17
		; Primary program, one double-mapped 256K flash, ID Block at 512K.
		; We are running in first part of single flash.
		; Do NOT invert A18 to make last half of flash visible in MB1CR quadrant.
		; CS_FLASH2 is 0, so second half of flash in MB1CR quadrant maps (twice)
		;  to 256-384K, 384-512K.
		ld		hl, 0x03
  #else
    #if _FLASH_SIZE_ == 0x80
		; Primary program, one 512K flash, ID Block at 512K.
		; CS_FLASH is 0, we are running in the first flash.
		ld		hl, 0x03
    #else
		; Primary program, two 256K flashes, ID Block at 256K.
		; CS_FLASH is 0, we are running in the first flash.
		ld		hl, 0x01
    #endif
  #endif
#endif
#ifdef COMPILE_SECONDARY_PROG
  #ifdef ROUTE_A18_TO_FLASH_A17
		; Secondary program, one 256K flash, ID Block at 256K.
		; We are running in second part of single flash.
		; Inverted A18 to make last half of flash visible in MB0CR quadrant.
		; CS_FLASH is 0, so second half of flash in MB0CR quadrant maps (twice)
		;  to 0-128K, 128-256K.
		ld		hl, 0x01
  #else
    #if _FLASH_SIZE_ == 0x80
		; Secondary program, one 512K flash, ID Block at 256K.
		; CS_FLASH is 0, we are running in second part of single flash
		;  with A18 inverted in MB0CR quadrant.
		ld		hl, 0x01
    #else
		; Secondary program, two 256K flashes, ID Block at 512K.
		; CS_FLASH is 2, we are running in the second flash.
		; CS_FLASH2 is 0, so the primary's ID Block is in the MB1CR quadrant.
		ld		hl, 0x03
    #endif
  #endif
#endif

   call   _readIDBlock         //  HL <- _readIDBlock(HL:quad_bitmask)

   bool   hl
   jr      z, .idBlockOk

   ;; erase SysIDBlock if error retured
   ld      hl, SysIDBlock
   ld      b, SysIDBlock+marker+6-SysIDBlock   ; B <- sizeof(SysIDBlock)
   xor   a
.blockEraseLoop:
   ld      (hl), a
   inc   hl
   djnz   .blockEraseLoop
.idBlockOk:
#if FAST_RAM_COMPILE
   // The flash is mapped to bank 3 in FAST_RAM_COMPILE mode
   ld      hl, 0x08
#else
   #if (_FLASH_SIZE_ == 0x80)
      ld      hl, 0x01 | 0x02
   #else
      ld      hl, 0x01
   #endif
#endif
	call	_InitFlashDriver

#if ENABLE_ERROR_LOGGING
   call  InitializeErrorLog
#endif
   lret

//**************************************************************
_init_IO_regs::
   // The following registers have specific values already
   // set by the BIOS and loaded into their shadow registers:
   //      GCSR, WDTCR, GCMxR, GOCR, GCDR, MBxCR, MTCR,
   //       SPCR, PCDR, PCFR, TACSR, TACR, SACR
   //
   // This next set of registers will be reset to these values
   // upon reset, startup, and when debugging is ended (i.e.
   // pressing F4).

   xor   a
   ioi   ld (PADR), a			; Parallel Port A Data Register
   ld      (PADRShadow), a
   ioi   ld (PBDR), a         ; Parallel Port B Data Register
   ld      (PBDRShadow), a

#ifdef _ZW_RESET_PCDR_ALL_ZEROS
   ioi   ld (PCDR), a         ; set TxA,TxB,TxC,TxD bits all low
   ld      (PCDRShadow), a    ;  (restore previous BIOS behavior)
#else
   #if (CPU_ID_MASK(_CPU_ID_) == R2000)
   ioi   ld (PCDR), a         ; set TxA,TxB,TxC,TxD bits all low
   ld      (PCDRShadow), a    ;  (Rabbit 2000 default after reset)
   #endif
#endif

   ioi   ld   (PDDDR),a			; Parallel port D
   ld      (PDDDRShadow),a
   ioi   ld (PDDR), a
   ld      (PDDRShadow), a
   ioi   ld (PDCR), a
   ld      (PDCRShadow), a
   ioi   ld (PDFR), a
   ld      (PDFRShadow), a
   ioi   ld (PDDCR), a
   ld      (PDDCRShadow), a

   ioi   ld   (PEDDR),a			; Parallel port E
   ld      (PEDDRShadow),a
   ioi   ld (PEDR), a
   ld      (PEDRShadow), a
   ioi   ld (PECR), a
   ld      (PECRShadow), a
   ioi   ld (PEFR), a
   ld      (PEFRShadow), a

   ioi   ld (TBCSR), a			; Timer B
   ld      (TBCSRShadow), a
   ioi   ld (TBCR), a
   ld      (TBCRShadow), a

   ioi   ld (IB0CR), a			; External I/O Control Registers
   ld      (IB0CRShadow), a
   ioi   ld (IB1CR), a
   ld      (IB1CRShadow), a
   ioi   ld (IB2CR), a
   ld      (IB2CRShadow), a
   ioi   ld (IB3CR), a
   ld      (IB3CRShadow), a
   ioi   ld (IB4CR), a
   ld      (IB4CRShadow), a
   ioi   ld (IB5CR), a
   ld      (IB5CRShadow), a
   ioi   ld (IB6CR), a
   ld      (IB6CRShadow), a
   ioi   ld (IB7CR), a
   ld      (IB7CRShadow), a

   ioi   ld (I0CR), a			; External Interrupt Control Registers
   ld      (I0CRShadow), a
   ioi   ld (I1CR), a
   ld      (I1CRShadow), a

   ioi   ld (TAT1R), a			; Timer A Time Constant Register 1
   ld      (TAT1RShadow), a

   ioi   ld (SBCR), a         ; Serial Port B Control Register
   ld      (SBCRShadow), a
   ioi   ld (SCCR), a         ; Serial Port C Control Register
   ld      (SCCRShadow), a
   ioi   ld (SDCR), a         ; Serial Port D Control Register
   ld      (SDCRShadow), a

	ld		a, 0x80				// Ignore SMODE pins.  Dynamic C does this,
	ioi	ld (SPCR), a		//  but must do it running stand alone too.
	ld		(SPCRShadow), a

	; enable watchdog
	xor	a						// a = 0x00
	ioi	ld (WDTTR), a

#if (CPU_ID_MASK(_CPU_ID_) >= R3000)
   #ifndef _ZW_RESET_PCDR_ALL_ZEROS
   ld      a, 0x15             ; set TxA bit low but TxB,TxC,TxD bits high
   ioi   ld (PCDR), a          ;  (Rabbit 3000 default after reset)
   ld      (PCDRShadow), a
   #endif

   xor   a                     // a = 0x00
   ioi   ld (GPSCR), a
   ld      (GPSCRShadow), a
   ioi   ld (BDCR), a
   ld      (BDCRShadow), a
   ioi   ld (MECR), a
   ld      (MECRShadow), a

   ioi   ld (PFDR), a			; Parallel Port F Registers
   ld      (PFDRShadow), a
   ioi   ld (PFCR), a
   ld      (PFCRShadow), a
   ioi   ld (PFFR), a
   ld      (PFFRShadow), a
   ioi   ld (PFDCR), a
   ld      (PFDCRShadow), a
   ioi   ld (PFDDR), a
   ld      (PFDDRShadow), a

   ioi   ld (PGDR), a         ; Parallel Port G Registers
   ld      (PGDRShadow), a
   ioi   ld (PGCR), a
   ld      (PGCRShadow), a
   ioi   ld (PGFR), a
   ld      (PGFRShadow), a
   ioi   ld (PGDCR), a
   ld      (PGDCRShadow), a
   ioi   ld (PGDDR), a
   ld      (PGDDRShadow), a

   ioi   ld (PWL0R), a  		; Pulse Width Modulation Registers
   ld      (PWL0RShadow), a
   ioi   ld (PWM0R), a
   ld      (PWM0RShadow), a
   ioi   ld (PWL1R), a
   ld      (PWL1RShadow), a
   ioi   ld (PWM1R), a
   ld      (PWM1RShadow), a
   ioi   ld (PWL2R), a
   ld      (PWL2RShadow), a
   ioi   ld (PWM2R), a
   ld      (PWM2RShadow), a
   ioi   ld (PWL3R), a
   ld      (PWL3RShadow), a
   ioi   ld (PWM3R), a
   ld      (PWM3RShadow), a

   ioi   ld (ICCSR), a			; Input Capture Registers
   ld      (ICCSRShadow), a
   ioi   ld (ICCR), a
   ld      (ICCRShadow), a
   ioi   ld (ICT1R), a
   ld      (ICT1RShadow), a
   ioi   ld (ICT2R), a
   ld      (ICT2RShadow), a
   ioi   ld (ICS1R), a
   ld      (ICS1RShadow), a
   ioi   ld (ICS2R), a
   ld      (ICS2RShadow), a

   ioi   ld (QDCSR), a			; Quadrature Decoder Registers
   ld      (QDCSRShadow), a
   ioi   ld (QDCR), a
   ld      (QDCRShadow), a

   ld      a, 0xC0				; Parallel Port B Data Direction Register
   ld      (PBDDRShadow), a

#ifndef   USE_TIMERA_PRESCALE
   ld      a, 0x01
#else
   xor   a
#endif
   ld      (TAPRShadow), a		; Timer A Prescale Register
#endif

   lret

//**************************************************************
// Init. the status pin, clocks, clock doubler, programming baud
// rate, stdio comm buffer

_more_inits02::
#if _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
		;; BL2600Q's and RCM39xx's /BUFEN is SD card select, stop external I/O
		;; cycle toggling, set STATUS pin low, CLK pin off, /BUFEN pin high
		ld		a, 0xA3
#else	// _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
		;; set STATUS pin low and CLK pin off
		ld		a, 0xA0
#endif	// _BOARD_TYPE_ == RCM3900 || _BOARD_TYPE_ == RCM3910 || _BOARD_TYPE_ == BL2600Q
   ld      hl, GOCRShadow
   ld      (hl), a

   ld      a,00001000b         ;   normal oscilator, processor and peri.
   ld    hl, GCSRShadow
   ld    (hl), a

   lcall   _getDoublerSetting

   ld      a, l
   ld      (GCDRShadow), a
   ioi   ld (GCDR), a      ; set clock doubler now


#if _CPU_ID_ >= R2000_R3
   lcall   _enableClockModulation	; enables spreader
#endif

   ld      a, 0c0h
   ioi   ld (PCFR), a   ; as initialized in pilot
   ld    hl, PCFRShadow
   ld    (hl), a

   ld    a,0xA0h
   ld      (statusPinState),a

   ld    a,88h
   ld      (OPMODE),a

   ld    a,0
   ld      (_dkfirst),a

   ld b,SIZESTDIOBUFF
   ld hl,stdioCommBuff
clear_stdio:
   ld (hl),0
   inc hl
   djnz clear_stdio

dkSetTimer:

   ;   initialize baud rate generator
   ;   for main clock at 8MHz, the calculation follows:
   ;   timer A4 to clock serial port A
   ;   timer A5 to clock serial port B
   ;   timer A6 to clock serial port C
   ;   timer A7 to clock serial port D

   ;   make timers tick
   ld    a, 00000001b
   ioi    ld (TACSR),a
   ld    hl,TACSRShadow
   ld    (hl),a

   ;   make timer A4 clocked by main clock
   ;   disable interrupt
   ld    a, 00000000b
   ioi    ld (TACR),a
   ld    hl,TACRShadow
   ld    (hl),a

   ;   Read the time constant for 19200 baud calculated by the BIOS...
   ld      a, (bios_divider19200)
   ld      b, a
#ifdef   USE_TIMERA_PRESCALE
   ; enable timer A prescale if desired
   xor   a
   ioi   ld (TAPR), a
   sla   b                  ; if prescale enabled, multiply constant by two
#endif
checkDoubled:
 ;; is clock doubled?
   ld      a, (GCDRShadow)   ; get clock doubler register
   or      a
   jr      z, checkDiv         ; if zero, clock not doubled
   sla   b                  ; if clock doubled, multiply constant by two
checkDiv:
 ;; is clock divided by eight?
   ld      a, (GCSRShadow)   ; get global control/status register
   and   018h               ; mask off unnecessary bits
   jr      nz, saveFreq      ; if nonzero, periph clock not divided by 8
   srl   b
   srl   b                  ; divide by eight
   srl   b
saveFreq:
   ld      a, b
   ld      (freq_divider), a   ; save for later use

#if (_BIOSBAUD_ == 2400)
	sla	b							; multiply by eight
	sla	b
	sla	b
#elif (_BIOSBAUD_ == 4800)
	sla	b							; multiply by four
	sla	b
#elif (_BIOSBAUD_ == 9600)
	sla	b							; multiply by two
#elif (_BIOSBAUD_ == 19200)
   ; b already contains freq_divider (= 19200 rate)
#elif (_BIOSBAUD_ == 38400)
	srl	b							; divide by two
#elif ((_BIOSBAUD_ == 57600) || (_BIOSBAUD_ == 115200))
setDCbaudrate:
	;; set up proper baud rate for Dynamic C communication by doing some
	;;  fixed-point arithmetic to get 57600 rate (= 19200 rate / 3)
	ld		c, b
	ld		b, 0						; bc = freq_divider
	ld		de, 055h					; d = 0, e = approximately 1/3 (ie: 0.33203125)
	mul								; hl:bc = bc * de
	inc	b							; round up since 055h is slightly less than 1/3

#if (_BIOSBAUD_ == 115200)
	srl	b							; if 115200 baud wanted, divide by two
#endif
#endif

setBaudRate:
	dec	b							; put divider-1 into timer scaling registers
	ld		a, b
	ioi	ld	(TAT4R), a			; save scaledown BIOS baud rate
	ld		hl, TAT4RShadow
	ld		(hl), a
	lret

//**************************************************************
// prepare to jump into the debugger

_init_dkLoop::

   call  bioshitwd
#if 0
   xor   a
   ld      (dkRequest),a
   ld      (dkStatusFlags),a
   ld      (dkSendFlags),a
#endif
   ipset 0
   lret

#endasm
/*
#asm
; .startup/dkStartup
; code to run each time the program executes
; initializes the stack allocation
;
dkStartup::
   pop hl
   jp   (hl)               ; jump to old return address
#endasm
*/
//#flushlib

#asm

dkEnd::
__xexit::
	jp	_xexit
#endasm
#flushlib
#asm
_store_ID_Block_Size::
	dw	0xffff
symTab::
	dw	symsize
#endasm

//#flushlib

//   we need to end the symbol table now
#asm
	dw	0							; indicates end of symbol table
	db	0

symTabEnd::
symsize: equ symTabEnd-symTab-2
#endasm


#use "DEFAULT.H"			// pull in BIOS C support libraries

#asm
BIOSCodeEnd:
#endasm

#if __SEPARATE_INST_DATA__
#asm xmem
XBIOSCodeEnd:
#endasm
#endif
//!!!Do NOT insert code between here and #pragma CompileProgram!!!
#flushlib

// NOTE:  The #use "PRECOMPILE.LIB" MUST come after the #pragma CompileProgram.
#endif	// matching #ifndef at prior "PB_Marker_E"

/*** EndHeader */