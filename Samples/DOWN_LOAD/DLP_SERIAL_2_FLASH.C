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
DLP_SERIAL_2_FLASH.C

	THIS PROGRAM IS PROVIDED AS A SAMPLE ONLY!

	It is intended for use ONLY with Rabbit based boards with 2 flash
	chips in conjunction with the DLM_SERIAL_2_FLASH.C. sample program.


	CONFIGURATION:
	==============

   It is just a small test program to use as a downloaded program, and
   is set up to continuously send a character over serial port B at
   57600 baud.  (The serial communication baud rate set by the
   "_232BAUD" macro below should match the DLM's baud rate.  If the
   target is a slow speed controller, the 57600 rate may not be
   practical when debugging the DLM with the programming cable attached.
   See the notes in the DLM for details.)

   To use serial port C instead of B, uncomment the USE_SERIAL_PORT_C
   macro definition and comment out the USE_SERIAL_PORT_B macro
   definition.

	Directions:

	1) In Targetless compile Options, select the target's specfic board
      type in the Board ID pull-down.  Then hit OK to register it.

	2) In the Compiler Options' Defines box, add COMPILE_SECONDARY_PROG
	   to the list of macros, and make sure COMPILE_PRIMARY_PROG is NOT
	   on the list.  If using separate I&D space, also add DONT_SPLIT_RAM
      to the list of macros.  Press Ctrl-Y to reset the target and
      recompile the BIOS.

	3) Compile this program to a BIN file, using the defined Target
      configuration and including the BIOS.  A DLP_SERIAL_2_FLASH.HEX
      file will be automatically generated in addition to the BIN file.

	4)	Following the directions in the DLM_SERIAL_2_FLASH.C sample
      program, compile it to the Rabbit target, disconnect the
      programming cable, and reset the Rabbit target to run the DLM.

	5) Follow the instructions in DLM_SERIAL_2_FLASH.C to download
	   the file DLP_SERIAL_2_FLASH.HEX file and run the DLP.

	Note:  The macro USE_2NDFLASH_CODE is not related to putting a
	secondary program in the second flash, and must be commented out when
	compiling this program.


****************************************************************/
#if (_USER)
	#error This sample is not compatible with RabbitSys - RabbitSys includes
   #error native support for remote firmware update.
#endif

#if __SEPARATE_INST_DATA__
#define MY_CHARACTER 'Z'
#else
#define MY_CHARACTER 'z'
#endif

//*** uncomment one only! ***
#define USE_SERIAL_PORT_B
//#define USE_SERIAL_PORT_C
//*** uncomment one only! ***

#ifdef  COMPILE_PRIMARY_PROG
#error "Cannot compile as primary program."
#error "Remove COMPILE_PRIMARY_PROG macro from Compiler Options' Defines box."
#error "Must recompile BIOS also.  Hit Ctrl-Y then recompile."
#endif

#ifndef  COMPILE_SECONDARY_PROG
#error "Must compile as secondary program."
#error "Put COMPILE_SECONDARY_PROG macro in Compiler Options' Defines box."
#error "Must recompile BIOS also.  Hit Ctrl-Y then recompile."
#endif

#ifdef  ROUTE_A18_TO_FLASH_A17
#error "Remove ROUTE_A18_TO_FLASH_A17 macro from Compiler Options' Defines box."
#error "Must recompile BIOS also.  Hit Ctrl-Y then recompile."
#endif

#ifdef  INVERT_A18_ON_PRIMARY_FLASH
#error "Remove INVERT_A18_ON_PRIMARY_FLASH macro from Compiler Options' Defines box."
#error "Must recompile BIOS also.  Hit Ctrl-Y then recompile."
#endif

#if  __SEPARATE_INST_DATA__
#ifndef  DONT_SPLIT_RAM
#error "Separate I&D space is incompatible with split RAM."
#error "Put DONT_SPLIT_RAM macro in Compiler Options' Defines box."
#error "Must recompile BIOS also.  Hit Ctrl-Y then recompile."
#endif
#endif

#if !FLASH_COMPILE
#error "This sample, as written, must be compiled to and run from flash!"
#endif

#ifdef USE_2NDFLASH_CODE
#error "Cannot compile secondary program with USE_2NDFLASH_CODE defined."
#error "Comment out the USE_2NDFLASH_CODE macro in .\lib\bioslib\StdBIOS.lib".
#error "Must recompile BIOS also.  Hit Ctrl-Y then recompile."
#endif


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

#define _232BAUD ((long)57600)

#define RESTARTSIGNAL "\xaa\xbb\xcc\xbb\xaa"
#define RESTARTSIGNAL_TIMEOUT 10000


/* -------------------------------------------------------------------- */


#if !(_RK_FIXED_VECTORS)
#rvarorg flashdriver resume
#endif

char  Chip_SW_Func [40]; 		// Place in root memory to hold switch code.

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


root void main()
{
	int cIn;
	int csize;

#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	// This is necessary for initializing RS232 functionality of LP35XX boards.
	brdInit();
#endif

	/* ASSERT: Make sure buffer is big enough. (remove once working) vvv */
	csize = (int) ((unsigned)_EndSwitchFlash - (unsigned)_SwitchFlashChip)+1;
	if( sizeof(Chip_SW_Func) < csize ) {
		exit(99);
	}
	/* Make sure buffer is big enough. (remove once working) ^^^ */

#ifdef USE_SERIAL_PORT_B
	serBopen(_232BAUD);
#else
#ifdef USE_SERIAL_PORT_C
	serCopen(_232BAUD);
#else
#error "Must #define a port"
#endif
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
			serBputc(MY_CHARACTER);
#else
#ifdef USE_SERIAL_PORT_C
			waitfor(serCwrFree());
			serCputc(MY_CHARACTER);
#endif
#endif
		}

	}   /* end forever loop.. */
}   /* end serial main() */


nodebug
void ProcessRestartCommand(char ch)
{
	static unsigned long timelimit;
	static char restart[sizeof(RESTARTSIGNAL)];
	static int indx;
#GLOBAL_INIT{ indx = 0;memset(restart,0,sizeof(RESTARTSIGNAL)-1);}

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
 *  If separate I&D space and not fast RAM compile,
 *	   ENTRY: 	DE = Chip Selects for MB0.
 *  Otherwise (combined I&D space and/or fast RAM compile),
 *    ENTRY:	HL = Chip Selects for MB0.
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

	ld   a,(MB0CRShadow)         // get shadow reg
   and  0xf8                    // mask out CS/WE/OE bits
   or 	L							  // set to new CS,WE,OE (HL = parameter #1)
   ioi  ld (MB0CR),a            // load MMU reg

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
 * 	Do a restart into the DLM in quadrant 0 Flash.  If TCP used, then
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

	/*
	 *  ...and call it.  Do not use CS_FLASH here, since that will select
	 * 	the flash where the DLP lives.  It doesn't select Flash #1.
	 * 	Therefore, explicitly pass in the chip selects _we_ want.
	 */
	SwitchChip(0|0);  // CS0/OE0/WE0 (appears as HL in assem code)
}

