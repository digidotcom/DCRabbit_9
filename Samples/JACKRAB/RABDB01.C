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
/***************************************************************

   rabdb01.c

   Jackrabbit demo board example program

   This program flashes the ds5-ds8 lights on the Jackrabbit
   demo board while allowing the user to control leds ds1-ds4
   with switches s1-s4.  The buzzer will sound whenever the 
   s1 switch is pressed.

	NOTE:  LEDs DS5-8 may not be installed on your development board.

   On the Jackrabbit board:

   s1-s4   = PBDR 2-5	switches
   ds1-ds8 = PADR 0-7	leds
   ls1	  = PEDR 0		buzzer

****************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


/***************************************************************
  predefines for the leds and switches
****************************************************************/
#define DS1 0
#define DS2 1
#define DS3 2
#define DS4 3
#define DS5 4
#define DS6 5
#define DS7 6
#define DS8 7

#define S1	2
#define S2	3
#define S3	4
#define S4	5

/***************************************************************
  initialize_ports();
  set up the parallel ports so PORTA are outputs and PORTE bit 0
  is and output. 
****************************************************************/
void initialize_ports()
{
#asm
	ld		a,0x84				; port a all outputs
ioi ld	(SPCR),a
	ld		(SPCRShadow), a	; load value into shadow register
	xor	a						; port e bit 1..7 inputs, 0 output
ioi ld	(PEFR),a
	ld		(PEFRShadow), a	; load shadow register
	ld		a,0x01
ioi ld	(PEDDR),a
	ld		(PEDDRShadow), a	; load shadow register
	xor	a
ioi ld	(PECR),a
	ld		(PECRShadow), a
#endasm
}
	
/***************************************************************
  update_ds1to4()
  reads switches S1-S4 and sets leds DS1-DS4 to their values.
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

/***************************************************************
  ledon(int led)
  turn the specified led on.
****************************************************************/
void ledon(int led)
{
	(1<<led)^0xff;   		// Leaves result in HL.
#asm
ioi ld	a,(PADR)
	and	l
ioi ld	(PADR),a
	ld		(PADRShadow), a
#endasm
}

/***************************************************************
  ledoff(int led)
  turn the specified led off.
****************************************************************/
void ledoff(int led)
{
	1<<led;    			// Leaves result in HL.
#asm
ioi ld	a,(PADR)
	or		l
ioi ld	(PADR),a
	ld		(PADRShadow), a
#endasm
}

/***************************************************************
  int sw(int which)
  return the value of the specified switch
****************************************************************/
int sw(int which)
{
	1<<which;	   		// Leaves result in HL.
#asm
ioi ld	a,(PBDR)
	cpl
	and	l
	bool	hl
	ld		l,a
	bool	hl
#endasm
}


/***************************************************************
  void buzzon()
  turn the buzzer on
****************************************************************/
void buzzon()
{
#asm
	ld		a,0x01
ioi ld	(PEDR),a
	ld		(PEDRShadow), a
#endasm
}


/***************************************************************
  void buzzoff()
  turn the buzzer off
****************************************************************/
void buzzoff()
{
#asm
	xor	a
ioi ld	(PEDR),a
	ld		(PEDRShadow), a
#endasm
}


void main()
{
	initialize_ports();
	
	for(;;) {

		//
		// update leds based on switches
		//
		
		update_ds1to4();
		
		//
		// buzz buzzer
		//
		
		if(sw(S1)) {
			costate {
				buzzon();
				waitfor(DelayMs(10));
				buzzoff();
				waitfor(DelayMs(200));
			}
		} else {
			buzzoff();
		}
		
		//
		// flash leds
		//
		
		costate {
			waitfor(DelayMs(500));
			ledoff(DS5);
			waitfor(DelayMs(1000));
			ledon(DS5);
		}

		costate {
			waitfor(DelayMs(1000));
			ledoff(DS6);
			waitfor(DelayMs(500));
			ledon(DS6);
		}
		
		costate {
			waitfor(DelayMs(250));
			ledoff(DS7);
			waitfor(DelayMs(1250));
			ledon(DS7);
		}
		
		costate {
			waitfor(DelayMs(750));
			ledoff(DS8);
			waitfor(DelayMs(900));
			ledon(DS8);
		}
	}

}
