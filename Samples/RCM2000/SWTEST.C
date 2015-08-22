/***************************************************************

   swtest.c
   Zworld, 2000

   Rabbit 2000 Core Module

   This program uses switches S2 and S3 to control of leds DS2 and DS3
   on the Rabbit 2000 Core Module Prototyping board.

   S2-S3   = PBDR 2,3	switches
   DS2-DS3 = PADR 0,1	leds

****************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

/***************************************************************
  predefines for the leds and switches
****************************************************************/
#define DS2 0
#define DS3 1

#define S2	2
#define S3	3

/***************************************************************
  initialize_ports();
  set up the parallel port PORTA as output
****************************************************************/
void initialize_ports()
{
#asm
	ld		a,0x84				; port a all outputs
	ioi ld	(SPCR),a
#endasm
}

/***************************************************************
  update_ds1to4()
  reads switches S2-S3 and sets leds DS2-DS3 to their values.
****************************************************************/
void update_ds1to4()
{
#asm
ioi ld	a,(PBDR)		; load port b
	srl	a				; shift port b right and mask off inputs 2-5
	srl	a
	and	0x0f

	ld		l,a
ioi ld 	a,(PADR)
	and	0xf0
	or		l

ioi ld	(PADR),a		; load 0-3 with port b 2-5
	ld		(PADRShadow), a	; load shadow register
#endasm
}


#if 0
/***************************************************************
  int sw(int which)
  return the value of the specified switch
****************************************************************/
int sw(int which)
{
	1<<which;
#asm
ioi ld	a,(PBDR)
	cpl
	and	l
	bool	hl
	ld		l,a
	bool	hl
#endasm
}
#endif

void main()
{
	initialize_ports();

	for(;;) {

		update_ds1to4();  // update leds based on switches

	}
}