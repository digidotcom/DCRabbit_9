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

	Samples\reset_demo.c

	Demonstration of using Rabbit reset functions and
	protected variables.

	This program will print out to serial port B
	why the last reset occurred, then cause either a
	watchdog timeout or a soft reset according to a
	protected counter.

	To properly run this demo, compile it to flash,
	disconnect the programming cable, connect serial
	port B to your computer at 19200 baud, and hit the
	reset button (or cycle the board's power).  The
	controller will then start running the program.
	A hard (power fail) reset can be forced by pressing
	the reset button or cycling the power.

	NOTE 1:  It is necessary to disconnect the target
	controller's programming cable in order for the
	target to run the loaded program after reset.  If
	not disconnected from the programming cable, after
	reset the target controller executes its bootstrap
	loader loop instead of the previously loaded code.

	NOTE 2:  Loss of target communication will result
	when a target controller is power-cycled or reset
	while it is communicating with Dynamic C when
	running in debug mode.

******************************************************/
#class auto

#define  BAUD_RATE	19200L
#define	BINBUFSIZE	31
#define	BOUTBUFSIZE	31
int delayMs(int ms);

void main()
{
	static long	counter;
	char	s[256];

	// This is necessary for initializing RS232 functionality of LP35XX boards.
#if _BOARD_TYPE_ == 0x1200 || _BOARD_TYPE_ == 0x1201
	brdInit();
#endif

	_sysIsSoftReset();	// recover any protected variables

	serBopen(BAUD_RATE);		// open serial port B at 19200 baud

	// check why the controller was last reset
	if (chkHardReset()) {
		sprintf(s, "Hard reset (power fail) occurred.\r\n");
		serBputs(s);
	}

	if (chkWDTO()) {
		sprintf(s, "Watchdog timeout occurred.\r\n");
		serBputs(s);
	}

	if (chkSoftReset()) {
		sprintf(s, "'Soft' reset (by software) occurred.\r\n");
		serBputs(s);
	}

	counter++;				// increment counter

	if (counter & 0x01) {		// force a watchdog timeout
		// allocate one virtual watchdog, timing = 1/16 sec
		VdGetFreeWd(1);
		delayMs(1000);

	} else {

		delayMs(1000);			 // force a software reset
		forceSoftReset();
	}
}



int delayMs(int ms)
{
	auto unsigned long	t0;

	t0 = MS_TIMER;
	while (MS_TIMER < (t0 + ms))
		/* do nothing */;
}