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
/*******************************************************************************

	nflash_erase.c

	This program is used with RCM3900 series controllers equipped with an on-
	board supported NAND flash device.


	Description
	===========
	This program is a handy utility for erasing all of a nand flash device's good
	blocks.  When the program starts it attempts to initialize the on-board NAND
	flash device.  If initialization is successful, block erase progress
	information is written to STDIO.


	Instructions
	============
	1. Compile and run this program.
	2. Follow the prompts.

*******************************************************************************/

// These defines redirect run mode STDIO to serial port A at 57600 baud.
#define STDIO_DEBUG_SERIAL SADR
#define STDIO_DEBUG_BAUD 57600

// This define adds carriage returns ('\r') to each newline char ('\n') when
//  sending STDIO output to a serial port.
#define STDIO_DEBUG_ADDCR

// Uncomment this define to force both run mode and debug mode STDIO to the
//  serial port specified above.
//#define STDIO_DEBUG_FORCEDSERIAL

#class auto
#use "rcm39xx.lib"	// sample library to use with this sample program

// Caution:  If NFLASH_CANERASEBADBLOCKS is defined before NFLASH.LIB is used,
//           then the nand flash driver will allow bad blocks to be erased,
//           including the bad block marker itself.  Thereafter, data that is
//           stored in the unmarked bad block may not be recoverable.
//#define NFLASH_CANERASEBADBLOCKS

#ifdef NFLASH_CANERASEBADBLOCKS
#error "Caution, defining NFLASH_CANERASEBADBLOCKS destroys bad block markers!"
#error "If you are sure you want to do this, comment out these error messages."
#endif

// if NFLASH_USEERASEBLOCKSIZE is not defined, its value defaults to 1
//  0 == use 512 byte main data page size; 1 == use 16 KB main data page size
#define NFLASH_USEERASEBLOCKSIZE 1	// always use 16 KB main data page size

#if NFLASH_USEERASEBLOCKSIZE != 1
#error "For efficiency, this sample requires NFLASH_USEERASEBLOCKSIZE == 1."
#endif

//#define NFLASH_VERBOSE
//#define NFLASH_DEBUG
#use "nflash.lib"	// base nand flash driver library

// global variables
// protected in separate battery backed /CS2 SRAM, when one is available
protected nf_device nandFlash;

int main(void)
{
	auto int status;
	auto long blocknum, erasedblocks, totalblocks;

	_sysIsSoftReset();	// restore any protected variables

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	if (nf_initDevice(&nandFlash, 0)) {
		printf("\nOn-board NAND flash init failed!\n");
		exit(-1);
	} else {
		printf("\nOn-board NAND flash init OK.\n");
		printf("# of blocks: 0x%08lx\n",
		       totalblocks = nf_getPageCount(&nandFlash));
		printf("size of block: 0x%08lx\n", nf_getPageSize(&nandFlash));
	}

	printf("\n");
	erasedblocks = 0L;
	for (blocknum = 0L; blocknum < totalblocks; ++blocknum) {
		while ((status = nf_eraseBlock(&nandFlash, blocknum)) == -1);
		if (status) {
			// indicate every error encountered on separate STDIO line
			printf("\nError, nf_eraseBlock(%08lx) result is %d!\n", blocknum,
			       status);
		}
		else {
			++erasedblocks;
			// keep erase speed up by only periodically reporting erased blocks
			if (!(blocknum % 16L) || ((totalblocks - 1L) == blocknum)) {
				printf("Erased block #0x%08lx.\r", blocknum);
			}
		}
	}
	printf("\n\nErased 0x%08LX blocks of 0x%08LX.\n", erasedblocks);
}

