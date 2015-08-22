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
#ifndef __BIOS_ALREADY_COMPILED
	#define __BIOS_ALREADY_COMPILED
#else
	#fatal "Cannot compile the BIOS as a user program!"
#endif

#ifndef _FLASH_
   #fatal "RAU loader.c requires 'Code and BIOS in Flash' compile mode."
#endif

// function prototypes required by supporting library code
root void _xexit(void);
root void __xexit__(void);

#define FLASH_COMPILE 1
#define FAST_RAM_COMPILE 0
#define RAM_COMPILE 0

#use "BOARDTYPES.LIB"	// board-specific initialization header
#use "SYSCONFIG.LIB"		// All user-defined macros for BIOS configuration
#use "SYSIODEFS.LIB"

#pragma Rabbit CompileBIOS

#define STACKORG    0xD000   // Beginning logical address for stack segment
#define ROOTCODEORG 0x0000   // Beginning logical address for root code
#define XMEM_WINDOW 0xE000UL // Beginning logical address for Xmem

#define MEMBREAK 0+(STACKORG/256)+(DATAORG/4096)	// SEGSIZE reg value

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

#define MB0CR_SETTING   FLASH_WSTATES | CS_FLASH  | (MB0CR_INVRT_A18<<4) | (MB0CR_INVRT_A19<<5)
#if (_FLASH_SIZE_ == 0x80)
	#define MB1CR_SETTING   FLASH_WSTATES | CS_FLASH  | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
#else
	#define MB1CR_SETTING   FLASH_WSTATES | CS_FLASH2 | (MB1CR_INVRT_A18<<4) | (MB1CR_INVRT_A19<<5)
#endif
#define MB2CR_SETTING   RAM_WSTATES | CS_RAM | (MB2CR_INVRT_A18<<4) | (MB2CR_INVRT_A19<<5)
#define MB3CR_SETTING   RAM_WSTATES | CS_RAM | (MB3CR_INVRT_A18<<4) | (MB3CR_INVRT_A19<<5)

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

// macros required by loader.c and / or library code
#define _MIXED_SYSTEM_USER   __RABBITSYS > 0
#define _SYSTEM              __RABBITSYS == 2
#define _stub
#define _system
#define _USER                __RABBITSYS == 1
#define ENABLE_ERROR_LOGGING 0
#define NULL                 (void *) 0
#define SHADOWS_MODE         UNPROT_MODE
#define UNPROT_MODE          0

// typedefs required by loader.c and / or supporting library code
typedef unsigned char      uint8;
typedef unsigned short     size_t;
typedef unsigned short int uint16;
typedef short int          int16;
typedef unsigned long int  uint32;
typedef unsigned long int  faraddr_t;
typedef unsigned long int  longword;	// unsigned 32 bit
typedef unsigned           word;
typedef struct {
   char *STACKP;
   char *IX;
   char *RETADR;
   char RETXPC;
   char RETSTACKSEG;
} jmp_buf[1];

// Turning this pragma off causes the compiler to not substitute replacements
// for ldir and lddr which are compiled with the bios.  The compilation of the
// the loader compiles the bios, but the bios is not included in the final image
// of the loader.
#pragma DATAWAITSUSED off

// Allocate 8k for loader code space
#define LOADER_MAX_CODE_SIZE		0x2000

// Do not allow loader to compile if dlm and dlp source paths and lengths are
// not specified
#ifndef DLM_IMP_PATH
#error DLM_IMP_PATH must be defined
#endif

#ifndef DLM_IMP_LENGTH
#error DLM_IMP_LENGTH must be defined
#endif

#ifndef DLP_IMP_PATH
#error DLP_IMP_PATH must be defined
#endif

#ifndef DLP_IMP_LENGTH
#error DLP_IMP_LENGTH must be defined
#endif

// Allocate 0x200 bytes of xmem code space and leave remainder for root code.
#define LDR_XCODSIZE			0x0200
#define LDR_ROOTCODESIZE   LOADER_MAX_CODE_SIZE - LDR_XCODSIZE

// Setup xmem base (xpc) for DLM and DLP.
#define DLM_BASE (0xff & ((LOADER_MAX_CODE_SIZE - 0xe000)/4096))
#define DLP_BASE (0xff & ((LOADER_MAX_CODE_SIZE + DLM_IMP_LENGTH-0xe000)/4096))

// Setup xmem code base (xpc) and offset for loader
#define LDR_XCODBASE		 (0xff & (((LDR_ROOTCODESIZE) - 0xe000)/4096))
#define LDR_XCODOFFS		 0xE000 + (0xfff & (LDR_ROOTCODESIZE))

#ifndef __DLM_BBROOTDATASIZE__
   #define __DLM_BBROOTDATASIZE__   0x1000
#endif

#ifndef DATAORG
   #define DATAORG   0x6000
#endif

// Move interrupt vector table base to the start of DATAORG, and the external
// interrupt vector table 256 bytes above.  The size of these vectors will
// be subtracted from the root data size to keep root data from growing into
// the interrupt vectors.
#undef  INTVEC_BASE
#define INTVEC_BASE		DATAORG
#undef  XINTVEC_BASE
#define XINTVEC_BASE		DATAORG + 0x0100

#define LDR_STACKSIZE		0x1000
#define LDR_ROOTDATAORG		0xE000 - LDR_STACKSIZE - __DLM_BBROOTDATASIZE__
#define LDR_ROOTDATASIZE   LDR_ROOTDATAORG - XINTVEC_BASE - 0x0100 - \
                           LDR_STACKSIZE + 0x1

/*----------------------------------------------------------------------------
**  DATA ORG - determines data locations in physical and
**  logical memory space.  This puts the loader's root data in the same physical
**  location as the DLM or DLP's root data.  The loader will *NOT* write to this
**  block of RAM.
**
**Type   Name     		Segment     	Top          	  Size               Use
------------------------------------------------------------------------------*/
#rcodorg ldrrootcode		0x00        	0x0000           LDR_ROOTCODESIZE   apply
#rvarorg ldrrootdata 	RAM_START		LDR_ROOTDATAORG  LDR_ROOTDATASIZE   apply
#xcodorg ldrxcode   		LDR_XCODBASE   LDR_XCODOFFS     LDR_XCODSIZE       apply
#xcodorg DLMArea 			DLM_BASE   		0xE000 			  0+DLM_IMP_LENGTH
#xcodorg DLPArea 			DLP_BASE   		0xE000			  0+DLP_IMP_LENGTH

#define _cexpr(X) 0+(X)

#asm root nodebug
	nop
#endasm

#flushlib
#flushlib

#flushcompile

#flushlib
#flushlib
#flushlib

//#pragma CompileProgram

#asm root nodebug
StartUserCode::
_xexit::
__xexit__::
;	nop
#endasm

