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
	disable_ethernet.c

	This sample program includes a method to disable the Ethernet controller
	on the RCM3900 Series boards.  In programs that don't include a call to
	sock_init(), the ASIX Ethernet controller consumes power and generates
	heat.

*******************************************************************************/

#class auto
#use "RCM39XX.LIB"	// sample library for RCM3900 Series boards

// ------------- code to initialize ASIX and put it to sleep -----------
#define USE_ETHERNET 1
#define USE_PPPOE 0
#use "BOARD_DEPS.LIB"

// Initialize the ASIX Ethernet controller and put it to sleep so it doesn't
// generate heat trying to establish a link.  Tested on RCM3910, but should
// work on any Rabbit 3000 board with an ASIX controller.
void asix_init_and_sleep(void)
{
	BitWrPortI(PEFR, &PEFRShadow, 1, 2);
	WrPortI(IB2CR, &IB2CRShadow, 0x89);		// use the 0x4000-0x6000 ioe range
	
	while (TICK_TIMER < 2000);					// wait until 2 seconds after startup
	
#ifdef _AX88796B
	WrPortE(ASIX_WRIO+0x00, NULL, 0xE0);	// page 3, stop
	WrPortE(ASIX_WRIO+0x0B, NULL, 0x12);	// D2 mode, regulator in standby
#else
	WrPortE(ASIX_WRIO+0x00, NULL, 0x21);	// page 0, stop, no DMA
	WrPortE(ASIX_WRIO+0x17, NULL, 0x50);	// power down PHY
#endif
}
// -------------------------------------------------------------------------

void main(void)
{
	brdInit();							// initialize board-specific I/O
	asix_init_and_sleep();			// initialize ASIX controller and power down
	
	printf("ASIX Ethernet controller initialized and sleeping.\n");
	
	while(1);	// do nothing
}


