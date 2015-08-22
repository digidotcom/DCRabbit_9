/**********************************************************
DLP_256kFlash.C

Z-World, 2001

	This program is provided as a Sample ONLY. It is intended for
	use on Rabbit based boards with 1 256K flash chip in conjunction
	with the sample program DLM_256kFlash.C.

   It is just a small test program to use as a downloaded program.
   It sends a character continuously over serial port B or C.

	Directions:

	1) In the Compile | Options dialog box in Dynamic C 7.21,
	   click on the "Defines" button, add COMPILE_SECONDARY_PROG
	   and INVERT_A18_ON_PRIMARY_FLASH to the list of macros, and make
	   sure COMPILE_PRIMARY_PROG is NOT on the list. Compile this
	   program to a BIN file, (include the BIOS). Select the option
	   "Compile with defined target configuration." A HEX file will be
	   automatically generated.

   2) Follow the instructions in DLM_256kFlash.C to compile the Download
      Manager to the target and to run the dumb terminal program.

   3) Using the DownLoad Manager Menu via the dumb terminal, download
      the generated HEX file (DLP_256kFlash.hex)to the target.

	As a load test ont he RCM2200, there is code to flash DS3.  This can
	test if the download process works.

**********************************************************/
#if (_USER)
	#error This sample is not compatible with RabbitSys - RabbitSys includes
   #error native support for remote firmware update.
#endif

#class static

//*** uncomment one only! ***
#define USE_SERIAL_PORT_B
//#define USE_SERIAL_PORT_C

void  _SwitchFlashChip(), _EndSwitchFlash();   // assem labels
#define  SWITCH_FUNC_SIZE	((int)((unsigned)_EndSwitchFlash - (unsigned)_SwitchFlashChip)+1)

#ifdef  COMPILE_PRIMARY_PROG
#error "Cannot compile as primary program."
#error "Remove COMPILE_PRIMARY_PROG macro in Compiler | #defines options and retry"
#endif

#ifndef  COMPILE_SECONDARY_PROG
#error "Must compile as secondary program."
#error "Put COMPILE_SECONDARY_PROG macro in Compiler | #defines options and retry"
#endif

#ifndef  INVERT_A18_ON_PRIMARY_FLASH
#error "Must invert A18 on secondary program."
#error "Put INVERT_A18_ON_PRIMARY_FLASH macro in Compiler | #defines options and retry"
#endif

#ifndef  ROUTE_A18_TO_FLASH_A17
#error "Must invert A18 on secondary program."
#error "Put ROUTE_A18_TO_FLASH_A17 macro in Compiler | #defines options and retry"
#endif

#if  __SEPARATE_INST_DATA__
#ifndef  DONT_SPLIT_RAM
#error "Separate I&D space is incompatible with split RAM."
#error "Put DONT_SPLIT_RAM macro in Compiler | #defines options."
#error "Must Recompile BIOS also. Hit Ctrl-Y then recompile."
#endif
#endif

#if !FLASH_COMPILE
#error "This sample, as written, must be compiled to and run from flash!"
#endif

#if _BOARD_TYPE_ == RCM2200
/**
 *  Define for debug test just to flash LEDs on RCM2200 proto board.  System
 *  will not respond to "restart.bin" indication in this mode.
 */
// #define RCM2200_FLASH_LED_ONLY
#endif

/*  Define to print some segment values. */
#define DLP_VERBOSE


/* -------------------------------------------------------------------- */

#ifdef USE_SERIAL_PORT_B
#define BINBUFSIZE  15
#define BOUTBUFSIZE 15
#else
#ifdef USE_SERIAL_PORT_C
#define CINBUFSIZE  15
#define COUTBUFSIZE 15
#endif
#endif

#define SERIAL_BAUDRATE 57600ul

/**
 * 	Where to find command line or parameter list from DLP.
 * 	This magic constant must match the DLM's value.
 */
#define  USER_ID_BLOCK_CMDLINE_OFFSET  60


//**** This must match the RESTARTSIGNAL defined in
//**** DLM_256KFLASH.C
#define RESTARTSIGNAL "\xaa\xbb\xcc\xbb\xaa"

#define RESTARTSIGNAL_TIMEOUT 10000


#if !(_RK_FIXED_VECTORS)
#rvarorg flashdriver resume
#endif

char  Chip_SW_Func [32];

#if !(_RK_FIXED_VECTORS)
#rvarorg rootdata resume
#endif

void  CopyChipSWtoRAM(void);
void  SwitchChip(void);
void ProcessRestartCommand(char ch);

void RestartDLM(void);

/* -------------------------------------------------------------------- */

#ifdef DLP_VERBOSE
/**
 * 	Print system constants and details.  Maybe helpful when debugging.
 * 	Looks into runtime private data structures, so
 */
nodebug void headings()
{
	auto char	userblock_data[80];
	auto char	buffer[98];
	auto int 	j;

	sprintf( buffer, "\r\n(DC %x.%02x) DATASEGVAL $%lx, _RAM_SIZE_ $%x, xavail() --> %ld \r\n",
					CC_VER >> 8, CC_VER & 0x0FF,
					(long)DATASEGVAL, _RAM_SIZE_, xavail(NULL) );
	serBputs( buffer );

	sprintf( buffer, "\tRAMEND $%lx , DATAEND $%lx \r\n",
				RAMEND, DATAEND );
	serBputs( buffer );

	sprintf( buffer, "\tMB0CR=%02X (CS_FLASH %02X), MB1CR=%02X (CS_FLASH2 %02X)\r\n",
					MB0CRShadow, CS_FLASH, MB1CRShadow, CS_FLASH2 );
	serBputs( buffer );

	sprintf( buffer, "\tflash size x%lx (_FLASH_SIZE x%lx)\r\n",
					(unsigned long)_FlashInfo.flashSize * 4096ul,
					(unsigned long)_FLASH_SIZE_ * 4096 );
	serBputs( buffer );

	sprintf( buffer, "\tID block version #%d \r\n", SysIDBlock.tableVersion );
	serBputs( buffer );

	/*  Extract the cmdline and print if first char is printable. */
	j = readUserBlock( userblock_data, USER_ID_BLOCK_CMDLINE_OFFSET, sizeof(userblock_data) );
	if( j == 0 && (' ' <= userblock_data[0] && userblock_data[0] <= '~') ) {
		userblock_data[sizeof(userblock_data)-1] = '\0';
		serBputs("*** Found a cmdline: ");
		serBputs(userblock_data);
		serBputs("***" );
	} else {
		sprintf( buffer, "*** Dump of command line area:\r\n%04x: ", USER_ID_BLOCK_CMDLINE_OFFSET );
		serBputs( buffer );
		for( j = 0 ; j < sizeof(buffer) ; ++j ) {
			sprintf( buffer, "%02x ", (unsigned char) userblock_data[j] );
			if( 0 == ((j+1) & 0x07) ) {
				buffer[2] = '.';
			}
			serBputs( buffer );
		}
	}
	serBputs( "\r\n****\r\n" );

}   /* end headings() */
#endif	// ifdef DLP_VERBOSE


int vwd;

root main()
{
	int cIn;

#ifdef RCM2200_FLASH_LED_ONLY
	/*  1. Convert the I/O ports.  Disable slave port which makes
	 *     Port A an output, and PORT E not have SCS signal.
	 */
	WrPortI(SPCR, & SPCRShadow, 0x84);

	/*  2. Read function shadow and set PE1 and PE7 as normal I/O.
	 *     LED's are conencted to PE1 and PE7, make them outputs.
	 *     Using shadow register preserves function of other Port E bits.
	 */
	WrPortI(PEFR,  & PEFRShadow,  ~((1<<7)|(1<<1)) & PEFRShadow);
	WrPortI(PEDDR, & PEDDRShadow, (1<<7)|(1<<1));

	/*  3. Turn on DS2 (0 -> PE1) and turn off DS3 (1 -> PE7).
	 */
	WrPortI(PEDR, & PEDRShadow, (1<<7));

	while(1) {  // begin an endless loop

		WrPortI(PEB7R, NULL, 0xff);  // turn LED DS3 off

		for(cIn=0; cIn<32000; cIn++)
			; // time delay loop

		WrPortI(PEB7R, NULL, 0);     // turn LED DS3 on

		for(cIn=0; cIn<25000; cIn++)
			; // time delay loop

	}	// end while loop

#endif  // ifdef RCM2200_FLASH_LED_ONLY

#if ((_BOARD_TYPE_ >= BL2000) && (_BOARD_TYPE_ <= BL2040))
	brdInit();		//required for BL2000 series boards
#endif
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();		// Required for RS232 functionality of LP35XX boards.
#endif

#ifdef USE_SERIAL_PORT_B
	serBopen(SERIAL_BAUDRATE);
   serBwrFlush();
   serBrdFlush();
 #ifdef DLP_VERBOSE
	headings();  			/* DEBUG */
 #endif
#else
#ifdef USE_SERIAL_PORT_C
	serCopen(SERIAL_BAUDRATE);
   serCwrFlush();
   serCrdFlush();
#else
#error "Must #define a serial port"
#endif
#endif

#if ((_BOARD_TYPE_ >= BL2000) && (_BOARD_TYPE_ <= BL2040))
   serMode(0);
#endif

	vwd  = VdGetFreeWd(10); // get a virtual watchdog

	while (1)
	{
      // Listen for restart command
#ifdef USE_SERIAL_PORT_B
		cIn = serBgetc();
#else
#ifdef USE_SERIAL_PORT_C
		cIn = serCgetc();
#endif
#endif

		VdHitWd(vwd);    // hit vwd

		if((cIn&0xff80)==0x0080)
		{
		    ProcessRestartCommand(cIn);
		}

		// Send a char
		costate
		{
			waitfor(DelayMs(50));
#ifdef USE_SERIAL_PORT_B
			waitfor(serBwrFree());
			serBputc('X');
#else
#ifdef USE_SERIAL_PORT_C
			waitfor(serCwrFree());
			serCputc('X');
#endif
#endif
		}
	}
}

nodebug
void ProcessRestartCommand(char ch)
{
	static unsigned long timelimit;
	static char restart[sizeof(RESTARTSIGNAL)];
	static int indx;
#GLOBAL_INIT{ indx = 0; memset(restart,0,sizeof(RESTARTSIGNAL)-1);}

	if(!indx)
	{
		timelimit = SEC_TIMER+RESTARTSIGNAL_TIMEOUT;
	}
	else
	{
		if((long)(SEC_TIMER - timelimit)>=0)
		{
			indx = 0;
			memset(restart,0,sizeof(RESTARTSIGNAL)-1);
			return;
		}
	}
	restart[indx] = ch;
	indx++;
	if(indx==sizeof(RESTARTSIGNAL))
	{
		memset(restart,0,sizeof(RESTARTSIGNAL)-1);
		indx = 0;
	}
   else if(!memcmp(restart,RESTARTSIGNAL,sizeof(RESTARTSIGNAL)-1))
	{
		RestartDLM();
	}
}

/* -------------------------------------------------------------------- */

/**
 *  This code is copied into "root data" area and then executed there
 *  (which is quadrant 3 physically).
 *
 * 	To restart, we need a little assembly routine that changes the
 * 	MB0CR value.  Don't use CS_FLASH (from bios) cuz that's been
 * 	redefined to indicate the DLP Flash!
 *
 *  If separate I&D space and not fast RAM compile,
 *	   ENTRY: 	nothing.
 *  Otherwise (combined I&D space and/or fast RAM compile),
 *    ENTRY:	nothing.
 */
#asm nodebug root
_SwitchFlashChip::
	ipset 3							  // turn off interrupts
	ld   a,0x51
	ioi  ld (WDTTR),a
	ld   a,0x54
	ioi  ld (WDTTR),a

	//  Single Flash, un-invert A18
	ld   a,0x48
   ioi  ld (MB0CR),a   // un-invert A18, write protect MB0, 2 wait states.

	ld   hl,0
	jp   (hl)
_EndSwitchFlash::
#endasm

nodebug
void SwitchChip(void)
{
#asm
#if _RK_FIXED_VECTORS
		ld		hl, Chip_SW_Func
		lcall	_CallRAMFunction
#else
		call	Chip_SW_Func
#endif
#endasm
}


/**
 * 	Do a restart into the DLM in quadrant 0 Flash.  If TCP used, then
 * 	should shutdown all those services and call tcp_tick() to let FIN
 * 	packets be ACK'ed, then shutdown the ethernet chip.  Since we're
 * 	fiddling with Flash, where code normally lies, we need the switch
 * 	code in RAM.  Do this by copying code into a RAM char array, then
 * 	jump to it.'  The RAM array will appear in root data section.
 */
nodebug void
RestartDLM()
{
#ifdef USE_SERIAL_PORT_B
	serBclose();
#else
#ifdef USE_SERIAL_PORT_C
	serCclose();
#endif
#endif

#if  _RK_FIXED_VECTORS
	//*** put flash switching function in data RAM
	xmem2xmem(paddrDS(Chip_SW_Func), (unsigned long) _SwitchFlashChip,
	          SWITCH_FUNC_SIZE);
#else
	// Put flash switching function in RAM
	memcpy( Chip_SW_Func, _SwitchFlashChip, SWITCH_FUNC_SIZE );
#endif

	/*	  ...and call it!  */
	SwitchChip();
}