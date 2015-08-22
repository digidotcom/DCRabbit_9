/*****************************************************

	test_osc3000.c
	Z-World, 2002

	This program demonstrate the various CPU oscillator
	settings available with the Rabbit 3000 processor.

	This program will blink LEDs at a fixed rate for
	different oscillator settings.  The following
	settings are demonstrated:

			- full processor clock
			- 1/2 processor clock
			- 1/4 processor clock
			- 1/6 processor clock
			- 1/8 processor clock
			- run processor clock off 32kHz oscillator
			- full processor clock

	Dynamic C will lose communication with the target
	in debug mode when the program switches from full
	processor clock mode.  To avoid this, run with no
	polling (alt-F9).

	Running with the 32kHz oscillator also disables
	the periodic interrupt.  If your program depends
	on that interrupt (i.e. uses TICK_TIMER, MS_TIMER,
	SEC_TIMER, of waitfor statements in costates),
	then periodically update those counters by calling
	updateTimers() within your code.

	This program will successfully blink LEDs on the
	prototyping board for the RCM3000 and RCM3100
	core modules.

 *****************************************************/
#if _USER
	#error "RabbitSys does not allow oscillator changes"
#endif

#class auto 			// Change default storage class for local variables: on the stack

// We need a delay independent of the real-time clock,
// so this function will delay for several hundred
// thousand CPU clock cycles.

nodebug void delayloop()
{
	auto unsigned int	i;

	for (i=0; i<5000; i++)
		;
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

#define	STEPDELAY	5		// delay in secs for each mode

void main()
{
	auto int	done;

	WrPortI(PGCR, &PGCRShadow, 0x00);
	WrPortI(PGFR, &PGFRShadow, 0x00);
	WrPortI(PGDCR, &PGDCRShadow, 0x00);
	WrPortI(PGDDR, &PGDDRShadow, 0xC0);

	done = FALSE;
	while (!done) {

		costate {
			// full processor clock
			useMainOsc();
			waitfor(DelaySec(STEPDELAY));

			// 1/2 processor clock
			useClockDivider3000(CLKDIV_2);
			waitfor(DelaySec(STEPDELAY));

			// 1/4 processor clock
			useClockDivider3000(CLKDIV_4);
			waitfor(DelaySec(STEPDELAY));

			// 1/6 processor clock
			useClockDivider3000(CLKDIV_6);
			waitfor(DelaySec(STEPDELAY));

			// 1/8 processor clock
			useClockDivider3000(CLKDIV_8);
			waitfor(DelaySec(STEPDELAY));

         // run processor clock off 32kHz oscillator
         use32kHzOsc();
            // waitfor() will not work when running off the
            // 32kHz oscillator (the periodic interrupt is
            // disabled).  Instead we'll just call the
            // LED-blinking code once, which should be enough
            // (note that it will take one the order of
            // half a minute to finish).
         blink_leds();

			// full processor clock
			useMainOsc();
			waitfor(DelaySec(STEPDELAY));
			done = TRUE;
		}

		costate {
			blink_leds();
		}
	}

}