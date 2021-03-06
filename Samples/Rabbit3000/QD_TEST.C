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
	This program demonstrates the quadature decoder peripheral
	on the Rabbit 3000 processor.
	
	Both channels are enabled and the current count of each
	is printed to STDOUT.  Additionally, channel 1 is zeroed
	out every five seconds to demonstrate the use of the
	qd_zero() function.

	Connections
	===========
	Connect a quadrature encoder or a pair of switches
	to each of the two QD input pairs:

		Channel 1: 	A <---> PF0
						B <---> PF1

		Channel 2:	A <---> PF2
						B <---> PF3

	If you are using an RCM3000 prototyping board, a simple
	circuit would be to connect on the proto-board:
	
		Channel 1:	PF0 <---> PG1 (operates switch S2)
						PF1 <---> PG0 (operates switch S3)
	or
							
		Channel 2:	PF2 <---> PG1 (operates switch S2)
						PF3 <---> PG0 (operates switch S3)
		
	Instructions
	============
	1. Make circuit connections as above.
	2.	Compile and run this program.
	3. If using switches to test, press and release the
		first switch and then the second switch directly
		after the first.
			
***********************************************************/
#class auto


void main()
{
	long reading1, reading2;
				
	qd_init(1);
	qd_zero(1);
	qd_zero(2);

	while(1)
	{

		// this costate prints out the decoder values twice per second
		costate {
			reading1 = qd_read(1);
			reading2 = qd_read(2);
			printf("qd1: %10ld      qd2: %10ld\n", reading1, reading2);
			waitfor(DelayMs(500));
		}

		// this costate zeroes QD channer 1 every 5 seconds
		costate {
			waitfor(DelaySec(5));
			qd_zero(1);
		}
	}
}
