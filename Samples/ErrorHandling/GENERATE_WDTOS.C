/*****************************************************

GENERATE_WDTOs.C

Z-World, 2001

This program generates watchdog timeouts that
can be displayed with DISPLAY_ERRORLOG.C.

1) Make sure ENABLE_ERROR_LOGGING at the top of the
   ERRLOGCONFIG.LIB is #defined to 1.

2) Compile this program to flash.

3) Disconnect the programming cable from the target

4) Cycle power on the board or hit the reset button if
   it has one.

5) Wait a moment, then reconnect the programming cable.

6) Compile DISPLAY_ERRORLOG.C and run it to see the
   count of watchdog timeouts
******************************************************/
#class auto

void main() {
	;

#asm
	ipset 3          ; disable interrupts so that the
	                 ; periodic ISR doesn't hit the
	                 ; watchdog.

	ld a,0x53		  ; set the WD timeout period to 250 ms
	ioi ld (WDTCR),a
#endasm

   while(1);        // now loop.
}