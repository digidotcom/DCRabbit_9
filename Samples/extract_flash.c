/*
   Copyright (c) 2017, Digi International Inc.

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
/*
	extract_flash.c
	
	A program you can compile to RAM on Rabbit hardware in order to dump a copy
	of the firmware stored on the flash.  You can use the Rabbit Field Utility
	(RFU) to install the firmware on a new board after converting the Base64
	dump to a binary file (using the built-in Windows command-line CERTUTIL.EXE).
	
	You may need to temporarily comment out compiler warnings in BOARDTYPES.LIB
	around line 336:
		#error "Compile to RAM option programs the battery-backed RAM on CS1 and destroys any persistent data stored there."
		#fatal "Comment out this error if you still want to run off the ram on CS1."

	Limitations
	===========
	Limited testing included devices with 256KB and 512KB flash configurations,
	but not 128KB (BL1810 and BL1820).  Detecting prog_param structure (used
	to calculate image size) worked on firmware built with Dynamic C versions
	from 8.61 to 9.62.
	
	Appears to work with 512KB flash, but not dual-256K flash boards (RCM2260).
	
	Not designed/tested for systems using the DLM/DLP or "COMPILE_SECONDARY_PROG"
	compiler option.
	
	If you need to comment out the compiler warnings in BOARDTYPES.LIB (see
	above), this process will erase the board's non-volatile, battery-backed RAM.
	
	If extracting the entire flash, look for a large run of 0xFF bytes to find
	the end of the program.
	
	How to use it
	=============
	Run from Dynamic C and confirm that the program finds the prog_param
	structure at the start of the flash.  It uses that structure to
	determine the firmware image size.  Without it, it will just dump the
	entire flash and you'll have to manually truncate it.
	
	Then run from the command line and confirm that it still works.  If you leave
	the IDE running, be sure to choose "Close Connection" from the "Run" menu
	to free up the serial port.
	
		C:\DCRABBIT_9.62>dccl_cmp Samples\extract_flash.c -mr -d VERBOSE

	If extract_flash can't determine the size of the firmware image, you can try
	defining the following macros (by adding "-d MACRONAME" to the command line):
	
		ALTERNATE_PATTERN: Try another pattern to find prog_param on firmware
		                   built with "Separate I&D" disabled.

		SCAN_FOR_END: Skip over the System ID Block and User Blocks at the upper
		              end of the flash, then skip over 0xFF bytes to find the
		              end of the actual firmware image.

	Then run again with DUMP_FIRMWARE defined to create a base64-encoded file.
	NOTE: This will take minutes to complete -- be patient!  You can monitor
	progress by checking the file size in another window.
	
		C:\DCRABBIT_9.62>dccl_cmp Samples\extract_flash.c -mr -d DUMP_FIRMWARE > firmware.b64

	Then use the Windows program CERTUTIL.EXE to convert it to a binary file:
	
		C:\DCRABBIT_9.62>certutil -decode firmware.b64 firmware.bin

	Note that you may need to edit the .b64 file to remove any compiler warnings
	written before the base64-encoded data.
	
	You should now be able to program a new board with firmware.bin using RFU.
	
	If you want an image of the entire flash, define the macro DUMP_FLASH
	instead of DUMP_FIRMWARE.  In this case, you'll need to locate the end
	of the actual firmware and truncate the file.  Expect to see a System ID
	Block at the top of the flash, User Data Blocks below that, and then a
	run of 0xFF bytes before the actual firmware image.

	Configuration Options
	=====================
	By default, this program just tries to determine the size of the firmware
	stored in the flash by looking for a structure called "prog_param" stored
	in the BIOS of the firmware.
	
	Define the macro DUMP_FIRMWARE to actually dump the firmware image (if it
	can determine the size) as Base64-encoded data.
	
	Or, define the macro DUMP_FLASH to skip image size detection and dump the
	entire flash (including UserBlock(s) and SystemIdBlock).
/*
	
/*	(Optional) Define the macro ALTERNATE_PATTERN to search for the prog_param
	structure using the RCDB/RCDE fields instead of the XDB/XDE fields.  Note
	that this pattern is not present if compiled with Separate I&D enabled. */
//#define ALTERNATE_PATTERN

/* (Optional) Define the macro VERBOSE to print contents of prog_param. */
//#define VERBOSE

#ifndef RAM_COMPILE
#error "Must compile to RAM so you don't overwrite the flash!"
#fatal "Choose Options/Project Options/Compiler/BIOS Memory Setting/Code and BIOS in RAM"
#endif

#use "base64.lib"

#if FLASH_SIZE == 512>>2
	#define BASE_ADDR   0x80000ul
	#define FLASH_BYTES 0x80000ul
#elif FLASH_SIZE == 256>>2
	#define BASE_ADDR   0xC0000ul
	#define FLASH_BYTES 0x40000ul
#elif FLASH_SIZE == 128>>2
	#warns "128KB flash untested and based on guesses about the memory map!"
	#define BASE_ADDR   0xC0000ul
	#define FLASH_BYTES 0x20000ul
#else
	#fatal "Unexpected flash size."
#endif

// Dynamic C 9 does not support defined() in preprocessor
#ifndef DUMP_FLASH
	#ifndef SCAN_FOR_END
		#define SEARCH_PROG_PARAM
	#endif
#endif
#ifdef DUMP_FIRMWARE
	#define DUMPING_BASE64
#else
	#ifdef DUMP_FLASH
		#define DUMPING_BASE64
	#endif
#endif

#ifdef ALTERNATE_PATTERN
// Search for this signature from the prog_param.RCDB/RCDE (will not work for
// images with Separate I&D enabled).
const char search_pattern[] = { 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00 };
#define PATTERN_OFFSET 32
#else
// Alternate search pattern that might work if the first pattern doesn't.
// Searches for this signature from the prog_param.XDB/XDE
const char search_pattern[] = { 0x00, 0xE0, 0x7E, 0x00, 0x00, 0xE0, 0x7E, 0x00 };
#define PATTERN_OFFSET 24
#endif

// keep a buffer of the start of flash so we can hopefully find the prog_param
char flash_start[12*1024];


unsigned long try_sysid_block(void)
{
	struct userBlockInfo uBI;
	unsigned long search_addr, value;

	GetUserBlockInfo(&uBI);
	if (!uBI.addrA) {
		#ifdef VERBOSE
			printf("\nNo valid User block found on this board!\n");
		#endif
		return 0;
	}
	#ifdef VERBOSE
		printf("Searching for end of firmware before UserBlock @ 0x%08LX\n", uBI.addrA);
	#endif
	search_addr = (uBI.addrB ? uBI.addrB : uBI.addrA) - 4;
	for (; search_addr > 0; search_addr -= 4) {
		value = xgetlong(BASE_ADDR + search_addr);
		if (value != 0xFFFFFFFF) {
			#ifdef VERBOSE
				printf("found last portion @0x%06lX = 0x%08lX\n", search_addr, value);
			#endif
			return search_addr + 4;
		}
	}
	
   #ifdef VERBOSE
      printf("Flash mapping error?  All bytes are 0xFF\n");
   #endif
	return 0;
}


#define PRINT_ADDR24(x, desc) \
	printf(desc " %02X:%04X to %02X:%04X\n", \
	flash_prog_param->x##B.aaa.a.base, flash_prog_param->x##B.aaa.a.addr, \
	flash_prog_param->x##E.aaa.a.base, flash_prog_param->x##E.aaa.a.addr)
#define CHUNK_SIZE 57   // 57 bytes allows for 76-byte lines of base64 output
int main()
{
	char base64_buffer[((CHUNK_SIZE+2)/3*4)+1];
	char flash_buffer[CHUNK_SIZE];
	char *p, *end;
	struct progStruct *flash_prog_param;
	int i;
	unsigned int copy;
	unsigned long addr;
	unsigned long dump_bytes, firmware_size;
	
	dump_bytes = 0;
	flash_prog_param = NULL;
	
#if FLASH_SIZE == 512>>2
	// 512KB of flash (single or 2x256KB?) mapped to MB2 and MB3
	WrPortI(MB2CR, &MB2CRShadow, FLASH_WSTATES | 0x00);
	WrPortI(MB3CR, &MB3CRShadow, FLASH_WSTATES | 0x00);
	_InitFlashDriver(0x0C);
#elif FLASH_SIZE == 256>>2 || FLASH_SIZE == 128>>2
	// single flash (256KB or 128KB?) mapped to MB3
	WrPortI(MB3CR, &MB3CRShadow, FLASH_WSTATES | 0x00);
	_InitFlashDriver(0x08);
#endif

#ifdef SCAN_FOR_END
	dump_bytes = try_sysid_block();
#endif

	if (dump_bytes == 0) {
		dump_bytes = FLASH_BYTES;
	}
	
#ifdef SEARCH_PROG_PARAM
	// Check for struct progStruct from 0x0000 to 0x3000 or 0x10000 to 0x13000
	// (which is the location if Separate Instruction & Data enabled).
	flash_prog_param = NULL;
	for (i = 0; flash_prog_param == NULL && i < 2; ++i) {
		xmem2root(flash_start, BASE_ADDR + (i * 0x10000ul), sizeof flash_start);
		end = &flash_start[sizeof flash_start - 8];
		for (p = flash_start; p < end; ++p) {
			p = memchr(p, search_pattern[0], end - p);
			if (p == NULL) {
				break;
			}
			if (memcmp(p, search_pattern, 8)) {
				continue;
			}
			flash_prog_param = (void *)(p - PATTERN_OFFSET);
			dump_bytes = flash_prog_param->HPA.aaa.a.addr +
				(flash_prog_param->HPA.aaa.a.base << 12lu);
			dump_bytes &= 0xFFFFF;	// mask to 20 bits
			break;
		}
	}

#ifndef DUMPING_BASE64
	if (flash_prog_param == NULL) {
		printf("Did not locate prog_param in first %uKB; unknown image size.\n",
			sizeof(flash_start)/1024);
#ifndef ALTERNATE_PATTERN
		printf("You could try again with ALTERNATE_PATTERN defined.\n");
#endif
	} else {
		printf("Found prog_param at 0x%04X (using %s pattern).\n",
			(char *)flash_prog_param - flash_start,
			#ifdef ALTERNATE_PATTERN
				"alternate"
			#else
				"primary"
			#endif
			);
		#ifdef VERBOSE
			PRINT_ADDR24(RC, "root code");
			PRINT_ADDR24(XC, "extended code");
			PRINT_ADDR24(RD, "root data");
			PRINT_ADDR24(XD, "extended data");
			PRINT_ADDR24(RCD, "root constant data");
			printf("HPA 0x%05lX (%lu bytes)\n", dump_bytes, dump_bytes);
			printf("auxStk 0x%04X to 0x%04X\n",
				flash_prog_param->auxStkB, flash_prog_param->auxStkE);
			printf("stk 0x%04X to 0x%04X\n",
				flash_prog_param->stkB, flash_prog_param->stkE);
			printf("free 0x%04X to 0x%04X\n",
				flash_prog_param->freeB, flash_prog_param->freeE);
			printf("heap 0x%04X to 0x%04X\n",
				flash_prog_param->heapB, flash_prog_param->heapE);
			printf("\n");
		#endif // VERBOSE
	}
#endif // ! DUMPING_BASE64
#endif // SEARCH_PROG_PARAM

#ifndef DUMPING_BASE64
	if (dump_bytes > FLASH_BYTES) {
		printf("Calculated %lu-byte firmware larger than %lu-byte flash\n",
			dump_bytes, FLASH_BYTES);
	} else if (dump_bytes != FLASH_BYTES) {
		printf("Define DUMP_FIRMWARE to extract %lu-byte firmware image.\n",
			dump_bytes);
	}
	printf("Define DUMP_FLASH to extract entire %uKB flash.\n",
		(unsigned)FLASH_SIZE << 2);
	return 0;
#endif // ! DUMPING_BASE64
	
	addr = BASE_ADDR;
	while (dump_bytes) {
		copy = CHUNK_SIZE;
		if (copy > dump_bytes) {
			copy = (unsigned int) dump_bytes;
		}
		
		xmem2root(flash_buffer, addr, copy);
		base64_encode(base64_buffer, flash_buffer, copy);
		printf("%s\n", base64_buffer);
		
		dump_bytes -= copy;
		addr += copy;
	}
	
	return 0;
}


