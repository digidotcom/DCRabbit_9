/*******************************************************************************

	nflash_inspect.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers equipped with an on-
	board supported NAND flash device.


	Description
	===========
	This program is a handy utility for inspecting the contents of a NAND flash
	device.  When it starts it attempts to initialize the on-board NAND flash
	device.  If initialization is successful, the user is presented with the
	following menu of options:
		'C' clears (to zero) all bytes in specified page;
		'E' erases (to 0xFF) all bytes in specified erase block;
		'P' prints out contents of specified page;
		'Q' quits program (automatic restart in run mode);
		'W' writes specified value to all bytes in specified page;
		'X' writes count pattern into specified page.

	The user should be aware of the trade-offs made by this sample in defining
	the NFLASH_USEERASEBLOCKSIZE macro value to be 0 (zero); this mode makes the
	nand flash driver use smaller (512 byte) chunks of data, which are less
	tedious to manage in this program than using the alternative larger (16 KB)
	chunks of data.  However, using smaller chunks of data means more nand flash
	block erases are required to update all of the program pages in an erase
	block, one per program page written.  In contrast, updating all of the
	program pages in an erase block can require only a single block erase when
	all of the program pages within the erase block are treated as a single large
	page.  See the nf_initDevice() function's help for more information.


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

// if NFLASH_USEERASEBLOCKSIZE is not defined, its value defaults to 1
//  0 == use 512 byte main data page size; 1 == use 16 KB main data page size
#define NFLASH_USEERASEBLOCKSIZE 0

//#define NFLASH_VERBOSE
//#define NFLASH_DEBUG
#use "nflash.lib"	// base nand flash driver library

// local function prototypes
long input_number(void);

// global variables

// protected in separate battery backed /CS2 SRAM, when one is available
protected nf_device nandFlash;
char hexstr[8];

int main(void)
{
	auto char inchar, pageval;
	auto int done, status;
	auto long bufAddr, bufSize, myMainBuffer, pagenum, endnum;

	_sysIsSoftReset();	// restore any protected variables

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	*hexstr = 0;         // default to decimal entry

	if (nf_initDevice(&nandFlash, 0)) {
		printf("\nOn-board NAND flash init failed!\n");
		exit(-1);
	} else {
		printf("\nOn-board NAND flash init OK.\n");
		printf("# of blocks: 0x%08LX\n", nf_getPageCount(&nandFlash));
		printf("size of block: 0x%08LX\n", nf_getPageSize(&nandFlash));
	}

	// Use a temp variable here, to avoid the possibility of _xalloc attempting
	//  to change nf_getPageSize(&nandFlash)'s value!
	bufSize = nf_getPageSize(&nandFlash);
	// Get application's main data buffer.  (Note that _xalloc will cause a run
	//  time error if there is not sufficient xmem data RAM available.)
#if BBROOTDATASIZE
	// If available, use the separate battery backed SRAM for the main data
	//  buffer.  This allows the nand flash driver to possibly recover a write
	//  interrupted by a power cycle.
	myMainBuffer = _xalloc(&bufSize, 0, XALLOC_BB);
#else
	myMainBuffer = _xalloc(&bufSize, 0, XALLOC_ANY);
#endif

	done = 0;	// not done yet!
	while (!done) {
		printf("\nChoose one of the following options:\n");
		printf("   'C' clears (to zero) all bytes in specified page;\n");
		printf("   'E' erases (to 0xFF) all bytes in specified erase block;\n");
		printf("   'H' toggles hex input mode for numerical entry;\n");
		printf("   'P' prints out contents of specified page;\n");
		printf("   'R' prints out contents of a range of pages;\n");
		printf("   'Q' quits program (automatic restart in run mode);\n");
		printf("   'W' writes specified value to all bytes in specified page;\n");
		printf("   'X' writes count pattern into specified page.\n");
		inchar = getchar();
		printf("\n");

		switch (tolower(inchar)) {
		case 'c':
			printf("Page number %sto clear (all bytes to zeros)?  ", hexstr);
			pagenum = input_number();
			printf("\n");
			if ((pagenum >= 0L) && (pagenum < nf_getPageCount(&nandFlash))) {
				pageval = 0;
				bufAddr = myMainBuffer + nf_getPageSize(&nandFlash);
				while (bufAddr > myMainBuffer) {
					--bufAddr;
					root2xmem(bufAddr, &pageval, sizeof(char));
				}
				status = nf_writePage(&nandFlash, myMainBuffer, pagenum);
				if (status) {
					printf("\nError, nf_writePage() result was %d!\n", status);
				}
			} else {
				printf("\nError, invalid page number.\n");
			}
			break;
		case 'e':
			printf("Page number %sto erase (entire erase block to 0xFFs)?  ",
			       hexstr);
			pagenum = input_number();
			printf("\n");
			if ((pagenum >= 0L) && (pagenum < nf_getPageCount(&nandFlash))) {
				status = nf_eraseBlock(&nandFlash, pagenum);
				if (status) {
					printf("\nError, nf_erasePage() result was %d!\n", status);
				}
			} else {
				printf("\nError, invalid page number.\n");
			}
			break;
		case 'h':
			if (*hexstr) {
				printf("\nDecimal input mode set.\n");
				*hexstr = 0;
			} else {
				printf("\nHexadecimal input mode set.\n");
				strcpy(hexstr, "in hex ");
			}
			break;
		case 'p':
			printf("Page number %sto print out?  ", hexstr);
			pagenum = input_number();
			printf("\n");
			if ((pagenum >= 0L) && (pagenum < nf_getPageCount(&nandFlash))) {
				status = nf_readPage(&nandFlash, myMainBuffer, pagenum);
				if (status) {
					printf("\nError, nf_readPage() result was %d!\n", status);
				}
				_nf_print_data("\nApplication's main", pagenum, myMainBuffer,
				               nf_getPageSize(&nandFlash));
				printf("\n");
			} else {
				printf("\nError, invalid page number.\n");
			}
			break;
		case 'r':
			printf("Beginning page number %sto print out?  ", hexstr);
			pagenum = input_number();
			printf("\n");
			if ((pagenum < 0L) || (pagenum > nf_getPageCount(&nandFlash))) {
				printf("\nError, invalid page number.\n");
				break;
			}
			printf("Ending page number %sto print out?  ", hexstr);
			endnum = input_number();
			printf("\n");
			if ((endnum < 0L) || (endnum > nf_getPageCount(&nandFlash))) {
				printf("\nError, invalid page number.\n");
				break;
			}
			while (pagenum <= endnum) {
				status = nf_readPage(&nandFlash, myMainBuffer, pagenum);
				if (status) {
					printf("\nError, nf_readPage() result was %d!\n", status);
				}
				_nf_print_data("\nApplication's main", pagenum++, myMainBuffer,
				               nf_getPageSize(&nandFlash));
				printf("\n");
			}
			break;
		case 'q':
			done = 1;
			break;
		case 'w':
			printf("Page number %sto write (all bytes to specified value)?  ",
			       hexstr);
			pagenum = input_number();
			printf("\n");
			if ((pagenum >= 0L) && (pagenum < nf_getPageCount(&nandFlash))) {
				printf("%secimal byte value to write? ", *hexstr ? "Hexad" : "D");
				pageval = (char) input_number();
				printf("\n");
				bufAddr = myMainBuffer + nf_getPageSize(&nandFlash);
				while (bufAddr > myMainBuffer) {
					--bufAddr;
					root2xmem(bufAddr, &pageval, sizeof(char));
				}
				status = nf_writePage(&nandFlash, myMainBuffer, pagenum);
				if (status) {
					printf("\nError, nf_writePage() result was %d!\n", status);
				}
			} else {
				printf("\nError, invalid page number.\n");
			}
			break;
		case 'x':
			printf("Page number %sto write (count pattern into)?  ", hexstr);
			pagenum = input_number();
			printf("\n");
			if ((pagenum >= 0L) && (pagenum < nf_getPageCount(&nandFlash))) {
				// into each main data buffer byte, put its physical address LSB
				bufAddr = myMainBuffer + nf_getPageSize(&nandFlash);
				while (bufAddr > myMainBuffer) {
					--bufAddr;
					root2xmem(bufAddr, &bufAddr, sizeof(char));
				}
				// put page number marker into each end of the main data buffer
				xsetlong(myMainBuffer, pagenum);
				xsetlong(myMainBuffer + nf_getPageSize(&nandFlash) -
				         (long) sizeof(long), pagenum);
				status = nf_writePage(&nandFlash, myMainBuffer, pagenum);
				if (status) {
					printf("\nError, nf_writePage() result was %d!\n", status);
				}
			} else {
				printf("\nError, invalid page number.\n");
			}
			break;
		default:
			printf("\nUnknown option, try again.\n");
		}	// end of switch
	}	// end of while

// If program execution gets here the program has been ended.
// In run mode the program will be restarted automatically.
}

// Gets positive numeric input from keyboard and returns long value when enter
// key is pressed.  Returns -1L if non-numeric keys are pressed.  (Allows
// backspace.)
long input_number(void)
{
	auto long number;
	auto char inchar;

	number = 0L;
	while (1) {
		inchar = toupper(getchar());
		putchar(inchar); //echo input
		if (inchar == '\n' || inchar == '\r') {
			return number;
		}
		else if (inchar == '\b') {
			// backspace
			number = *hexstr ? number >> 4 : number / 10L;
		} else {
			number = *hexstr ? number << 4 : number * 10L;
			if (inchar >= '0' && inchar <= '9') {
				number += inchar - '0';
			}
			else if (*hexstr && inchar >= 'A' && inchar <= 'F') {
				number += inchar - 55;
			} else {
				// bad input
				return -1L;
			}
		}
	}	// end of while
}

