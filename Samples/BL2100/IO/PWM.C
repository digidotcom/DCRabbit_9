/**************************************************************************

	pwm.c

   Z-World, 2001
	This sample program is for the BL2100 series controllers.

	This program demonstrates the use of the timer B to generate a PWM
	signal on I/O bit PE5_INT (located on J11). This program generates a
	42Hz PWM signal with a duty cycle adjustable from 1 to 99%.

   Note: Signal PE5_INT is user configurable that can be one of the
         following:
         a) Interrupt input.
         b) A CMOS digital Input or Output.
         c) A PWM signal.

	Test Instructions:
	------------------
	1. Compile and run this program.

	2. Monitor I/O signal PE5_INT (located on connector J10) with a
	   voltmeter.

	3. You should see the following DC voltages when you change to the
	   the duty cycle listed.

	   Duty cycle	Voltage @PE5_INT
	   ----------	----------------
	   10% appox  	.5  volts
	   50% appox  	2.5 volts
	   90% appox  	4.9 volts


**************************************************************************/
#class auto


#define PWM_PORT         PEDR			// using port E, bit PE5 (PE5_INT)
#define PWM_SHADOW       PEDRShadow
#define PWM_BIT_LOW		~0x20
#define PWM_BIT_HIGH     0x20

const float maxcount = 1023;	// Timer B is a 10 bit counter
float dutycycle;					// dutycycle is 1% to 99% of maxcount
unsigned int count;				// dutycycle converted to int
char flag;							// used for ISR program control
int increment, decrement;     // used to control the dutycycle of the PWM signal

char pwm_lsb;
char pwm_msb;

int key;
void timerb_isr();

///////////////////////////////////////////////////////////////////////

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

	// Required for BL2100 series boards
	brdInit();

	// initialize PE5 and PE4
	WrPortI(PEFR, &PEFRShadow,   (PEFRShadow & ~0x30));

	// set PE5 to be an output
	WrPortI(PEDDR, &PEDDRShadow, (PEDDRShadow | 0x20));

	// Set PE4 to be an input
	WrPortI(PEDDR, &PEDDRShadow, (PEDDRShadow & ~0x10));

	// Set PE5 low
	WrPortI(PEDR, &PEDRShadow, (PEDRShadow & PWM_BIT_LOW));


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
	flag = 0x00;

	// initialize Timer B
	WrPortI(TBCR, &TBCRShadow, 0x05);	// clock timer B with (Timer A1) and set interrupt level to 1
	WrPortI(TAT1R, &TAT1RShadow, 0xff); // Load Timer A1 with max time constant

	WrPortI(TBM1R, NULL, 0 );
	WrPortI(TBL1R, NULL, 0 );				// set initial match

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
				if (dutycycle < 99) dutycycle++;
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
				if (dutycycle > 1 ) dutycycle--;
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

			ioi	ld	a, (TBCSR)		; clear the interrupt
			ld		a,(flag)				; get flag value
			inc	a						; increment
			ld		(flag), a			; save for next time
			and 	0x01					; mask off all but bit 0
			jr		nz, .pwm_high 		; jump to set the PWM output low

.pwm_low:
			ld    a, (PWM_SHADOW)	; get copy of the shadow register
			and   PWM_BIT_LOW			; initialize register to set PWM output low
			ioi	ld (PWM_PORT), a	; set PWM output low
			ld    (PWM_SHADOW),a    ; update the shadow register 
			
			ld		a, (pwm_msb) 		; put high byte value into bits 6 & 7
   		ioi	ld	(TBM1R), a

			ld		a, (pwm_lsb)		; get low byte value
			ioi	ld	(TBL1R), a	  	; writing to low byte enables next interrupt
			jr		.done

.pwm_high:
			ld    a,(PWM_SHADOW)		; get copy of the shadow register
		  	or    PWM_BIT_HIGH		; intialize register to set PWM output high
			ioi	ld (PWM_PORT), a	; set PWM output high	  
			ld    (PWM_SHADOW),a		; update the shadow register

			xor	a						; setup to load counter B with zero's
   		ioi	ld	(TBM1R), a
   		ioi	ld	(TBL1R), a		; writing to low byte enables next interrupt
.done:
			pop 	hl						; restore registers
			pop	af

			ipres							; restore interrupts
			ret							; return
#endasm
