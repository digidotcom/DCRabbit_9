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
/**********************************************************
	Samples\Rabbit3000\qd_test.c

	This program is used with Rabbit 3000 based controllers.

	Description
	===========
	This program demonstrates the input capture system to
	measure relative phase of detectors in a quadrature
	decoding arrangement.

	Connect PF0 to PF7 to get input capture on PF0 QD input
	phase of quadrature signals will be printed to STDOUT
	along with count.

	Connections
	===========
	Connect a quadrature encoder or a pair of switches
	to the QD input pair.

								  PF7 <--+
								  			|
		Channel 1: 	A <---> PF0 <--+
						B <---> PF1

	If you are using an RCM3000 prototyping board, a simple
	circuit would be to connect on the proto-board:

					  	+-> PF7
					  	|
		Channel 1: 	+-> PF0 <---> PG1 (operates switch S2)
							 PF1 <---> PG0 (operates switch S3)

	Instructions
	============
	1. Make circuit connections as above.
	2.	Compile and run this program.
	3. If using switches to test, press and release the
		first switch and then the second switch directly
		after the first.

***********************************************************/
#class auto


#define IC_DIVISOR 10

char ic_state; //ISR is done when state == 2
char ic_overflow; //set in ISR if either count rolls over

//ISR for interrupt capture
#asm root
ic_isr::
	 push	 af
	 push	 bc

ioi ld	 a, (ICCSR)
	 ld	 c, a				;hold ICCSR result in reg C

	 ld	 a, (ic_state)
	 or	 a
	 jr	 z, start_check
	 dec	 a
	 jr	 z, stop_check
	 jr 	 finish		;state==2, don't do anything, shouldn't have gotten here

start_check:
	 ld	 a, c
	 and	 0x60
	 jr	 z, finish	;not the right event
	 ld	 a, 0x5d
ioi ld	 (ICT2R), a		;enable counter 2 (catch any transition to start)
	 ld	 a, 1
	 ld	 (ic_state), a		;now looking for stop
	 jr	 finish

stop_check:
	 ld	 a, c
	 and	 0x40
	 jr	 z, finish	;not the right event
	 xor	 a
ioi ld	 (ICT1R), a	;disable counter 1
ioi ld	 (ICT2R), a	;disable counter 2
	 ld	 a, 2
	 ld	 (ic_state), a		;counts are ready
	 ld	 a, c
	 and	 0x0c
	 jr	 z, finish
	 ld	 a, 1
	 ld	 (ic_overflow), a
finish:
	 ipres
	 pop	 bc
	 pop	 af
	 ret


#endasm


main()
{
	long qd_reading;
	float first_count, second_count;


	qd_init(1);

	WrPortI(TAT8R, NULL, IC_DIVISOR);
	WrPortI(ICCSR, NULL, 0x0c); //zero out counters
	//IC1 times a low pulse on the QD1,2 input
	// for IC1, start pin is PF1(conected to QD1,2) and stop is
	// PF1(QD1,2)
	WrPortI(ICS1R, NULL, 0x88);
	//IC2 times from a transition on QD1,1 to the end of the QD1,2 pulse
	//for IC2, start pin is PF7, stop pin is PF1
	WrPortI(ICS2R, NULL, 0xb8);

	ic_state = 0;
   SetVectIntern(0x1A, ic_isr); //setup ISR
	WrPortI(ICCR, NULL, 0x01); //enable interrupt

	while(1)
	{
		//setup for captures
		ic_state = 0;
		ic_overflow = 0;
		WrPortI(ICCSR, NULL, 0x6c); //zero counters, IC1 interrupts on
		//set counter 1 triggers, start on low, stop on high
		WrPortI(ICT1R, NULL, 0x59);
		while(ic_state != 2); //wait
		if(!ic_overflow)
		{
			qd_reading = qd_read(1);
			first_count = RdPortI(ICL1R);
			first_count += RdPortI(ICM1R)*256;
			second_count = RdPortI(ICL2R);
			second_count += RdPortI(ICM2R)*256;
			printf("QD count: %10ld phase: %.1f deg    \r", qd_reading,
				(second_count / first_count)*180);
		}
	}

}