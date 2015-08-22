/*******************************************************************************

	nflash_dump.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers equipped with an on-
	board supported NAND flash device.


	Description
	===========
	This program is a handy utility for dumping the non-erased content of a NAND
	flash device to STDIO, which may be redirected to a serial port.  When the
	program starts it attempts to initialize the on-board NAND flash device.  If
	initialization is successful and the main page size is acceptable, non-erased
	(non-0xFF) NAND flash page content is dumped to STDIO.


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
//#define NFLASH_CANERASEBADBLOCKS	// to also allow reading initial bad blocks

// if NFLASH_USEERASEBLOCKSIZE is not defined, its value defaults to 1
//  0 == use 512 byte main data page size; 1 == use 16 KB main data page size
#define NFLASH_USEERASEBLOCKSIZE 0	// must use 512 byte main data page size

//#define NFLASH_VERBOSE
//#define NFLASH_DEBUG
#use "nflash.lib"	// base nand flash driver library

// local function prototypes
int non_erased(long dataBuffer, long dataSize);

// global variables
// protected in separate battery backed /CS2 SRAM, when one is available
protected nf_device nandFlash;

int main(void)
{
	static char myMainBuffer[512];
	auto int status;
	auto long bufSize, pagenum;

	_sysIsSoftReset();	// restore any protected variables

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	if (nf_initDevice(&nandFlash, 0)) {
		printf("\nOn-board NAND flash init failed!\n");
		exit(-1);
	} else {
		printf("\nOn-board NAND flash init OK.\n");
		printf("# of blocks: 0x%08lx\n", nf_getPageCount(&nandFlash));
		printf("size of block: 0x%08lx\n", nf_getPageSize(&nandFlash));
	}

	bufSize = nf_getPageSize(&nandFlash);
	if ((512L < bufSize) || (0L > bufSize)) {
		printf("\nNAND flash page size is out of range (%ld)!\n", bufSize);
		exit(-2);
	}

	for (pagenum = 0; pagenum < nf_getPageCount(&nandFlash); ++pagenum) {
		status = nf_readPage(&nandFlash, paddr(myMainBuffer), pagenum);
		if (status) {
			printf("\nError, nf_readPage(%08lx) result is %d!\n", pagenum, status);
		}
		// only dump page's non-erased main data
		if (non_erased(paddr(myMainBuffer), nf_getPageSize(&nandFlash))) {
			_nf_print_data("\nApplication's main", pagenum, paddr(myMainBuffer),
			              nf_getPageSize(&nandFlash));
		}
		// only dump page's non-erased spare data
		if (non_erased(nandFlash.sparebuffer, nandFlash.sparesize)) {
			_nf_print_data("\nApplication's spare", pagenum,
			                nandFlash.sparebuffer, nandFlash.sparesize);
		}
	}
}

// Checks non-erased (non 0xFF) condition of xmem buffer.
// Returns 1 if any byte in the buffer is not an 0xFF, otherwise returns 0.
int non_erased(long dataBuffer, long dataSize)
{
	auto char temp;

	while (dataSize % (long) sizeof(long)) {
		xmem2root(&temp, dataBuffer, sizeof(char));
		if ('\xFF' != temp) {
			return 1;
		}
		++dataBuffer;
		--dataSize;
	}
	while (dataSize) {
		if (0xFFFFFFFFL != xgetlong(dataBuffer)) {
			return 1;
		}
		dataBuffer += (long) sizeof(long);
		dataSize -= (long) sizeof(long);
	}
	return 0;
}

