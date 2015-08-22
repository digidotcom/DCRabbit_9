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
	pwmout.c

	This sample program is intended for the LP3500 series controllers.

	Description
	===========
	This program demonstrates the PWM functions.

	It will set the PWM channels, PWM0-PWM2, to the	following
	duty cycles:

	PWM CHAN 0 to 10%
	PWM CHAN 1 to 25%
	PWM CHAN 2 to 50%

	Instructions
	============
	1. Compile and run this program.
	2. Best way to view duty cycles is to observe each channel
	   with an oscilloscope.

*********************************************************************/
#class auto				// Change default storage for local variables to "auto"


#define PWMCHAN0	0
#define PWMCHAN1	1
#define PWMCHAN2	2


void main()
{
	unsigned long	freq;

	// request 3kHz PWM cycle (routine will select closest possible value)
	freq = pwmOutConfig(3000ul);
	printf("Actual PWM frequency = %lu Hz\n", freq);

	pwmOut(PWMCHAN0, 0.10);
	pwmOut(PWMCHAN1, 0.25);
	pwmOut(PWMCHAN2, 0.50);

	while (1); //keep running the program
}