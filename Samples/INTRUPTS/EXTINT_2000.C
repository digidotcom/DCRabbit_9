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
/**************************************************************************

	EXTINT_200O.C

	This program demonstrates how to use external interrupt lines to
   trigger an interrupt	service routine (ISR) in your code for the
   Rabbit 2000 CPU.  Due to a bug in the Rabbit 2000 interrupt
   handling (see technical note TN301 on the	Rabbit Semiconductor
   web page for more information), both external interrupts are
   used on the Rabbit 2000 CPU to ensure that no interrupts are
   missed.


   Instructions:
   -------------
   1. Verify there is a 1k resistor between interrupt lines PE4 and
   PE5 per technical note TN301. Also verify that the interrupt line
   isn't floating if they are then you will need to terminate the
   interrupt line with a pull-up or pull-down resistor ( >10k ).

   2. To demonstrate the interrupt, drive the interrupt line with
   a signal source to generate pulses to be counted by the interrupt
   routine for display. The program is setup to count 20 pulses and
   then will exit.

	Notes:
   -----
   1. Signal source can be as simple as a jumper wire connected to GND.
   2. The "interrupt" keyword is used to create an ISR in C code.


**************************************************************************/
#class auto

char count;

void my_isr();

void main()
{
	count = 0;

	WrPortI(PEDDR, &PEDDRShadow, 0x00);	// set port E as all inputs

	// Note that this function call has changed with version 6.19
	// of Dynamic C.  The first parameter is now the priority level
	// of the external interrupt, and the second is the ISR itself.
	SetVectExtern2000(1, my_isr);
	// re-setup ISR to show example of retrieving ISR address
	SetVectExtern2000(1, GetVectExtern2000());

	WrPortI(I0CR, &I0CRShadow, 0x2B);		// enable external INT0 on PE4, rising edge
	WrPortI(I1CR, &I1CRShadow, 0x2B);		// enable external INT1 on PE5, rising edge


	while (count < 20) {

		// output the interrupt count every second
		costate {
			printf("count = %3d\n", count);
			waitfor(DelaySec(1));
		}
	}

	WrPortI(I0CR, &I0CRShadow, 0x00);				// disable external interrupt 0
	printf("count = %3d\n", count);					// final count
}


/****** interrupt routine for external interrupt 0 ******/

nodebug root interrupt void my_isr()
{
	count++;
}