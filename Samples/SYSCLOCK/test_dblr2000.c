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
/*****************************************************

	test_dblr2000.c

	This program demonstrate the Rabbit 2000
	processor's clock doubler.

	This program will blink LEDs at a fixed rate for
	different clock doubler settings.
	
	Dynamic C may lose communication with the target
	in polling mode depending on the crystal frequency
	and communications baud rate.  To avoid this, compile
	and download with [F5] then run with no polling (alt-F9).
	
	This program will successfully blink LEDs on the
	prototyping board for the BL18xx, RCM20xx, RCM21xx,
	RCM22xx, and RCM23xx families.  In addition, the
	LEDs of the Z-World Demo Board will blink when
	connected to the digital outputs of the TCP/IP DevKit,
	OP6xxx/Intellicom, and BL20xx families of boards.
			 
******************************************************/
#class auto

void	blink(int max);

void main()
{
	if (GCDRShadow == 0) {
		printf("Clock doubler is currently off.  This board\n");
		printf("is most likely not designed to be run with the\n");
		printf("clock doubler enabled.\n");
		exit(1);
	}
	
	// show lights at default speed (clock doubler on)
	blink(40);

	// turn off doubler and blink lights
	clockDoublerOff();
	blink(20);

	// back to default speed
	clockDoublerOn();
	blink(40);

	exit(0);
}

/************************************************
	The following function will blink the
	LEDs on a variety of boards.
 ************************************************/

#define BOARD_FAMILY		(_BOARD_TYPE_ >> 8)
  
void blink(int max)
{
	int	i, j;

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
		/* This shows every 4 counts on DS2, ad every 8 on DS3: */
		BitWrPortI(PEDR, &PEDRShadow, (i & 0x04)>>2, 1);
		BitWrPortI(PEDR, &PEDRShadow, (i & 0x08)<<4, 7);
#endif

		for (j=0; j<5000; j++) {
			hitwd();					// delay for a time
		}

	}   /* end while.. */
}
