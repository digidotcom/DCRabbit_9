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
	Samples\Rabbit3000\pwm_test.c

	Description
	===========
	This program demonstrates the PWM functions of the Rabbit 3000
	processor.
	
	It will set the four PWM channels, port F bits 4 - 7, to the
	following values:
	
	(PF4)	Channel 0:	10% duty cycle
	(PF5)	Channel 1:	25% duty cycle
	(PF6)	Channel 2:	50% duty cycle
	(PF7)	Channel 3:	99% duty cycle

	Pulse spreading can be turned on or off by 
	TEST_PWM_OPTIONS below. See documentation for
	pwm_set() for details.

	Instructions
	============
	1. Compile and run this program.
	2. Best way to view duty cycles is to observe each channel
	   with an oscilloscope. 

*********************************************************************/
#class auto

void main()
{
	unsigned long	freq;
	int	pwm_options;
	
	// request 10kHz PWM cycle (will select closest possible value)
	freq = pwm_init(10000ul);
	printf("Actual PWM frequency = %lu Hz\n", freq);

	pwm_options = 0;

	/*
 	Uncomment the following line to spread the PWM output
   throughout the cycle.
   */
//	pwm_options = PWM_SPREAD;

	pwm_set(0, 0.10 * 1024, pwm_options);
	pwm_set(1, 0.25 * 1024, pwm_options);
	pwm_set(2, 0.50 * 1024, pwm_options);
	pwm_set(3, 0.99 * 1024, pwm_options);
	while(1); //keep running the program		
}
