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

	sAMPLES\bl2000\IO\pwm.c

	This sample program is for the BL2000 series controllers.

	This program demonstrates the use of the timer B to generate a PWM
	signal on digital output OUT8. The program generates a 42Hz PWM
	signal with the duty cycle adjustable from 1 to 99%.

	This program is based on the "pwm.c" sample program located on the
	rabbitsemiconductor WEB page at:
	http://www.rabbitsemiconductor.com/support_center/rab20_support.html

	Test Instructions:
	------------------
	1. Compile and run this program.

	2. Monitor Digitial output OUT8 with a voltmeter.
	  (located on connector J8).

	3. You should see the following DC voltages when you change to the
	   the duty cycle listed.

	   Duty cycle	Voltage @Out8
	   ----------	-------------
	   10% appox  	.44  volts
	   50% appox  	2.48 volts
	   90% appox  	4.43 volts


**************************************************************************/
#class auto


#define PWM_PORT         PEDR			// using port E, bit PE0 (OUT8)
#define PWM_SHADOW       PEDRShadow
#define PWM_BIT_LOW		~0x01
#define PWM_BIT_HIGH     0x01

const float maxcount = 1023;	// Timer B is a 10 bit counter
float dutycycle;					// dutycycle is 1% to 99% of maxcount
unsigned int count;				// dutycycle converted to int
int flag;							// used for ISR program control
int increment, decrement;     // used to control the dutycycle of the PWM signal

char pwm_lsb;
char pwm_msb;

int key;
void timerb_isr();

void update_duty_cycle( void )
{
	count = (int)((dutycycle * maxcount)/100);

	// change to interrupt level 3, don't allow interrupts while executing this code
#asm
	push	ip		;save-off IP state
	ipset	3		;set interrupt priority to level 3

	; Breakdown the 10 bit counter interger into two 8 bit values
	; so that it can be loaded into Timer B when the ISR executes.
	c pwm_msb = (char) (count >> 2);
	c pwm_lsb = (char) count;
	pop 	ip
#endasm
}


void main()
{
	auto int i;

	// initialize the I/O ports
	brdInit();

	// initialize program variables
	dutycycle = 10;
	count = 0;
	increment  = FALSE;
	decrement  = FALSE;

#if __SEPARATE_INST_DATA__ && (_RK_FIXED_VECTORS)
		interrupt_vector timerb_intvec timerb_isr;
#else
		SetVectIntern(0x0B, timerb_isr);	   // initialize Timer B interrupt vector
#endif

	// calculate the default duty cycle setting
	count = (int)((dutycycle * maxcount)/100);
	flag = 0;

	// initialize Timer B
	WrPortI(TBCR, &TBCRShadow, 0x05);	// clock timer B with (Timer A1) and set interrupt level to 1
	WrPortI(TAT1R, &TAT1RShadow, 0xff); // Load Timer A1 with max time constant

	WrPortI(TBM1R, NULL, 0 );
	WrPortI(TBL1R, NULL, 0 );				// set initial match

	WrPortI(PEB7R, &PEDRShadow, (PEDRShadow | PWM_BIT_HIGH));

	// sync-up to Timer B
	for(i = 0; i < 3; i++)
	{
		while((RdPortI(TBCMR) & 0xC0) != 0x80);
		WrPortI(TBM1R, NULL, 0 );
		WrPortI(TBL1R, NULL, 0 );
		while((RdPortI(TBCMR) & 0xC0) != 0xC0);
	}


	WrPortI(TBCSR, &TBCSRShadow, 0x03);	// enable timer B and B1 match interrupts
	update_duty_cycle();

	// display usage message
	printf("PWM control\n\r");
	printf("-----------\n\r");
	printf("- Press I to Increment PWM duty cycle\n\r");
	printf("- Press D to Decrement PWM duty cycle\n\n\r");
	printf ( "Duty Cycle = %d%c \r", (int)dutycycle, '%');

	// generate a PWM signal where the user can change the dutycycle
	while (1)
	{
		costate
		{
			// increase dutycycle
			if(increment)
			{
				waitfor(DelayMs(100));
				if (dutycycle < 99) dutycycle += 1.0;
				update_duty_cycle();
				printf ( "Duty Cycle = %d%c \r", (int)dutycycle, '%');
				increment = FALSE;

			}
		}
		costate
		{
			// decrease dutycycle
			if(decrement)
			{
				waitfor(DelayMs(100));
				if (dutycycle > 1 ) dutycycle -= 1.0;
				update_duty_cycle();
				printf ( "Duty Cycle = %d%c \r", (int)dutycycle, '%');
				decrement = FALSE;
			}
		}
		costate
		{
			if(kbhit())
			{
				key = getchar();
				if (key == 'I' || key == 'i')
				{
					while(kbhit()) getchar();
					increment = TRUE;
     			}
     			if (key == 'D' || key == 'd')
				{
					while(kbhit()) getchar();
					decrement = TRUE;
     			}
			}
		}
	}
}

#asm root nodebug
timerb_isr::
; For this ISR to function properly you MUST update (TBL1R) after (TBM1R)!!!

			push	af						; save registers
			push	hl

	ioi	ld		a, (TBCSR)			; clear the interrupt

			ld		hl, (flag)			; get flag value
			inc	hl						; increment
			ld		(flag), hl			; save for next time

			ld		a, 01h				; mask off all but bit 0
			and 	l
			jr		z, pwm_low	 		; jump to set the PWM output low

pwm_high:
			ld    a,(PWM_SHADOW)		; get copy of the shadow register
		  	or    PWM_BIT_HIGH		; intialize register to set PWM output high
			ioi	ld (PWM_PORT), a	; set PWM output high
			ld    (PWM_SHADOW),a		; update the shadow register

			ld		a, (pwm_msb) 		; put high byte value into bits 6 & 7
   		ioi	ld	(TBM1R), a

			ld		a, (pwm_lsb)		; get low byte value
	ioi	ld		(TBL1R), a	   	; writing to low byte enables next interrupt
			jr		done

pwm_low:
			ld    a, (PWM_SHADOW)	; get copy of the shadow register
			and   PWM_BIT_LOW			; initialize register to set PWM output low
			ioi	ld (PWM_PORT), a	; set PWM output low
			ld    (PWM_SHADOW),a    ; update the shadow register

			xor	a						; setup to load counter B with zero's
   		ioi	ld	(TBM1R), a
   		ioi	ld	(TBL1R), a		; writing to low byte enables next interrupt
done:
			pop 	hl						; restore registers
			pop	af

			ipres							; restore interrupts
			ret							; return
#endasm