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

	Samples\Intrupts\EXTINT_300O.C

	This program demonstrates how to use the external
	interrupt lines to trigger an interrupt
	service routine (ISR) in your code for the
	Rabbit 3000 CPU.  Unlike the Rabbit 2000 CPU, both
	external interrupts are available for use on the
	Rabbit 3000.

	Two input lines should be attached to parallel port
	E pins 1 and 4 for this example.  The program will
	separately count pulses on both input line until
	20 pulses are counted on one of them.

   The "interrupt" keyword is used to create an ISR in C code.

******************************************************/


char count0, count1;

void my_isr0();
void my_isr1();

void main()
{
	handle peddr_hdl;
   handle i0cr_hdl, i1cr_hdl;
   char c;
	count0 = 0;
	count1 = 0;

   peddr_hdl = _sys_open(_SYS_OPEN_INTERFACE, PEDDR);
	_sys_write(peddr_hdl, 0x00);					// set port E as all inputs
   _sys_read(peddr_hdl, &c);
   if(c != 0) {
   	printf("ERROR: PEDDR did not read as written\n");
   }
   _sys_close(peddr_hdl);

	SetVectExtern3000(0, my_isr0);
	SetVectExtern3000(1, my_isr1);
   // re-setup ISR's to show example of retrieving ISR address using GetVectExtern3000
	SetVectExtern3000(0, GetVectExtern3000(0));
	SetVectExtern3000(1, GetVectExtern3000(1));

	i0cr_hdl = _sys_open(_SYS_OPEN_INTERFACE, I0CR);
   i1cr_hdl = _sys_open(_SYS_OPEN_INTERFACE, I1CR);


	_sys_write(i0cr_hdl, 0x21);		// enable external INT0 on PE4, rising edge, priority 1
	_sys_write(i1cr_hdl, 0x9);			// enable external INT1 on PE1, rising edge, priority 1

   _sys_read(i0cr_hdl, &c);
   if(c != 0x21) {
   	printf("ERROR: I0CR did not read as written\n");
   }
   _sys_read(i1cr_hdl, &c);
   if(c != 0x9) {
   	printf("ERROR: I1CR did not read as written\n");
   }


	while ((count0 < 20) && (count1 < 20)) {

		// output the interrupt count every second
		costate {
			printf("counts = %3d  %3d\n", count0, count1);
			waitfor(DelaySec(1));
		}
	}

	_sys_write(i0cr_hdl, 0x0);			// disable external interrupt 0
	_sys_write(i1cr_hdl, 0x0);	 		// disable external interrupt 1

   _sys_read(i0cr_hdl, &c);
   if(c != 0x0) {
   	printf("ERROR: I0CR did not read as written\n");
   }
   _sys_read(i1cr_hdl, &c);
   if(c != 0x0) {
   	printf("ERROR: I1CR did not read as written\n");
   }

   _sys_close(i0cr_hdl);
   _sys_close(i1cr_hdl);
	// final counts
	printf("counts = %3d  %3d\n", count0, count1);
}


/****** interrupt routines  ******/

nodebug root interrupt void my_isr0()
{
	count0++;
}

nodebug root interrupt void my_isr1()
{
	count1++;
}