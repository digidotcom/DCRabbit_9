/*****************************************************

	test_32khz_3000.c
	Z-World, 2002

	This program demonstrate the Rabbit 3000 processor's
	ability to run off the 32kHz oscillator divided by
	various values.

	This program will successfully blink LEDs on the prototyping
   board for the RCM3000, RCM3100 and RCM3200 core modules
   at a fixed rate for different oscillator settings.
   The following processor clock rates are demonstrated:

			- full processor clock
			- 32kHz oscillator
			- 32kHz oscillator divided by two
			- 32kHz oscillator divided by four
			- 32kHz oscillator divided by eight
			- 32kHz oscillator divided by sixteen
			- back to full processor clock

	Dynamic C will lose communication with the target
	in debug mode when the program switches from full
	processor clock mode -- to avoid this, run with
	no polling (Alt-F9).

	Running with the 32kHz oscillator also disables
	the periodic interrupt.  If your program depends
	on that interrupt (i.e. uses TICK_TIMER, MS_TIMER,
	SEC_TIMER, of waitfor statements in costates),
	then periodically update those counters by calling
	updateTimers() within your code.

	Note also that since the periodic interrupt is
	disabled, you will need to call hitwd()
	periodically to ensure that the Rabbit 3000 hardware
	watchdog timer does not expire.

 *****************************************************/
#if _USER
	#error "RabbitSys does not allow oscillator changes"
#endif

#class auto 			// Change default storage class for local variables: on the stack

// We need a delay independent of the real-time clock,
// so this function will delay for a few hundred
// CPU clock cycles.

void delayloop()
{
	auto unsigned	i;

	for (i=0; i<10; i++)
		hitwd();		// necessary when running off 32kHz oscillator
}

////////////////////////////////////////////////////////////

void blink_leds()
{
	WrPortI(PGDR, &PGDRShadow, 0x80);
	delayloop();
	WrPortI(PGDR, &PGDRShadow, 0x40);
	delayloop();
}

////////////////////////////////////////////////////////////

void main()
{
	auto int	i;

	// set up port G to blink LEDs
	WrPortI(PGCR, &PGCRShadow, 0x00);
	WrPortI(PGFR, &PGFRShadow, 0x00);
	WrPortI(PGDCR, &PGDCRShadow, 0x00);
	WrPortI(PGDDR, &PGDDRShadow, 0xC0);

	// full processor clock
	useMainOsc();
	for (i=0; i<10000; i++)
		blink_leds();

	// switch to 32kHz oscillator
	use32kHzOsc();

	// run processor clock off 32kHz oscillator
	set32kHzDivider(OSC32DIV_1);
	for (i=0; i<16; i++)		blink_leds();

	// 32kHz oscillator divided by two
	set32kHzDivider(OSC32DIV_2);
	for (i=0; i<8; i++)		blink_leds();

	// 32kHz oscillator divided by four
	set32kHzDivider(OSC32DIV_4);
	for (i=0; i<4; i++)		blink_leds();

	// 32kHz oscillator divided by eight
	set32kHzDivider(OSC32DIV_8);
	for (i=0; i<2; i++)		blink_leds();

	// 32kHz oscillator divided by sixteen
	set32kHzDivider(OSC32DIV_16);
	for (i=0; i<1; i++)		blink_leds();

	// full processor clock
	useMainOsc();
	for (i=0; i<10000; i++)
		blink_leds();

   /*
    *  Stay here waiting for power-cycle.  Otherwise, the application
    *  will start over again.
    */
	while (1) { }
}