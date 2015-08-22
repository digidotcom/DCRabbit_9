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
/********************************************************************
	pwm.c

	This sample program is intended for the BL2500 series controllers.

	Description
	===========
	This program demonstrates pulse-width modulation as an analog
	output.  PWM cycles will output at rate of main clock/256.
	Timer A Prescale Register will be set so that the main clock
	for Timer A is the peripheral clock.

	Target communication	will be lost when this program executes.
	To keep target communication active, uncomment the line

		#define USE_TIMERA_PRESCALE

	in .\lib\bioslib\sysconfig.lib.  Doing this will properly set all
	configurations related to Timer A.


	Instructions
	============
	1. Press F9 to compile and run this program.
	2. Connect a voltmeter to DA0 or DA1 to measure the voltage
		output.

*********************************************************************/
#class auto

#define DA0	0
#define DA1	1


main()
{
	WrPortI(TAPR, &TAPRShadow, 0);	//must be done before calling brdInit()

	brdInit();

	pwmOut(DA0, 310);		//approximately 1.0V
	pwmOut(DA1, 930);		//approximately 3.0V

	while (1)
	{
	}

}