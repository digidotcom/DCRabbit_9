/*****************************************************

	test_osc2000.c
	Z-World, 2000

	This program demonstrate the various CPU oscillator
	settings available with the Rabbit 2000 processor.

	This program will blink LEDs at a fixed rate for
	different oscillator settings.  The following
	settings are demonstrated:

			- normal oscillator
			- normal oscillator divided by 8
			- 32kHz oscillator
			- back to normal oscillator

	Dynamic C will lose communication with the target
	in polling mode when the program switches to the
	32kHz oscallator.  To avoid this, compile and download
	with [F5] then run with no polling (alt-F9).	

	Running with the 32kHz oscillator also disables
	the periodic interrupt.  If your program depends
	on that interrupt (i.e. uses TICK_TIMER, MS_TIMER,
	SEC_TIMER, of waitfor statements in costates),
	then periodically update those counters by calling
	updateTimers() within your code.

	This program will successfully blink LEDs on the
	prototyping board for the BL18xx, RCM20xx, RCM21xx,
	RCM22xx, and RCM23xx families.  In addition, the
	LEDs of the Z-World Demo Board will blink when
	connected to the digital outputs of the TCP/IP DevKit,
	OP6xxx/Intellicom, and BL20xx families of boards.

*****************************************************/
#class auto


int blink(int max);

void main()
{
	// show lights at normal speed
	blink(4096);

	// turn on clock divider (div by 8)	
	useClockDivider();	
	blink(512);

	// switch to 32kHz oscillator (this is REAL slow).
	use32kHzOsc();
	blink(32);

	// switch back to main oscillator
	useMainOsc();
	blink(4096);
	printf( " -- done --\n" );

	exit(0);
}

/************************************************
	The following function will blink the
	LEDs on a variety of boards.
 ************************************************/

#define BOARD_FAMILY		(_BOARD_TYPE_ >> 8)
  
int blink(int max)
{
	int	i, j;

	printf( " blick = %d ", max ); 		/* DEBUG */

#if ((BOARD_FAMILY == 0x01) || (BOARD_FAMILY == 0x02) || (BOARD_FAMILY == 0x07))
	// support for BL18xx, RCM20xx, RCM21xx
	WrPortI(PADR, &PADRShadow, 0xFF);
	WrPortI(SPCR, &SPCRShadow, 0x84);
#endif

#if ((BOARD_FAMILY == 0x03) || (BOARD_FAMILY == 0x08))
	// support for TCP/IP Dev Kit, OP6xxx/Intellicom, BL20xx
	brdInit();
#endif

#if ((BOARD_FAMILY == 0x09) || (BOARD_FAMILY == 0x0A))
	// support for RCM22xx, RCM23xx
	BitWrPortI(PEFR, &PEFRShadow, 0x00, 1);
	BitWrPortI(PEFR, &PEFRShadow, 0x00, 7);
	BitWrPortI(PEDDR, &PEDDRShadow, 0x02, 1);
	BitWrPortI(PEDDR, &PEDDRShadow, 0x80, 7);
	BitWrPortI(PEDR, &PEDRShadow, 0x02, 1);
	BitWrPortI(PEDR, &PEDRShadow, 0x80, 7);
#endif
	
	for (i=0; i<max; i++) {

#if ((BOARD_FAMILY == 0x01) || (BOARD_FAMILY == 0x07))
		WrPortI(PADR, &PADRShadow, ~i);
#endif

#if (BOARD_FAMILY == 0x02)
		WrPortI(PADR, &PADRShadow, ~(i & 0x0C)>>2);
#endif

#if ((BOARD_FAMILY == 0x03) || (BOARD_FAMILY == 0x08))
		digOut(0, (i & 0x01)>>0);
		digOut(1, (i & 0x02)>>1);
		digOut(2, (i & 0x04)>>2);
#endif

#if ((BOARD_FAMILY == 0x09) || (BOARD_FAMILY == 0x0A))
		BitWrPortI(PEDR, &PEDRShadow, (i & 0x04)>>2, 1);
		BitWrPortI(PEDR, &PEDRShadow, (i & 0x08)<<4, 7);
#endif
		
		for (j=0; j<10; j++) {
			// update TICK_TIMER, MS_TIMER, and SEC_TIMER since
			//		the periodic interrupt is not running!
			updateTimers();

			// when running with the 32kHz oscillator, this needs
			// to be called manually (periodic interrupt is stopped)
			hitwd();
		}
	}
	printf( "  done\n" );
}
