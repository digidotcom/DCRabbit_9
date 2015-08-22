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

   rabdb02.c

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
  defines for the leds and switches
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
  is an output. 
****************************************************************/
void initialize_ports()
{
#asm
		ld		a,0x84				; set port a all outputs
ioi	ld		(SPCR),a
		ld		(SPCRShadow),a		; copy to shadow register
		xor	a						; set port e normal I/O
ioi	ld		(PEFR),a
		ld		(PEFRShadow),a		; copy to shadow register
		ld		a,0x01				; port e bits 1..7 inputs, 0 output
ioi	ld		(PEDDR),a
		ld		(PEDDRShadow),a	; copy to shadow register
		xor	a						; set transfer clock as pclk/2
ioi	ld		(PECR),a
		ld		(PECRShadow),a		; copy to shadow register
#endasm
}
	
/***************************************************************
  update_ds1to4()
  reads switches S1-S4 and sets leds DS1-DS4 to their values.
****************************************************************/
void update_ds1to4()
{
#asm
ioi	ld		a,(PBDR)				; load current port b inputs
		srl	a						; shift port b data right 2 bits
		srl	a
		and	0x0F					; keep only port b inputs 2-5 (S1-S4)
		ld		l,a					; save shifted S1-S4 data
ioi	ld		a,(PADR)				; load current port a outputs
		and	0xF0					; mask off outputs 0-3 (DS1-DS4)
		or		l						; combine S1-S4 (DS1-DS4) data with DS5-DS8
ioi	ld		(PADR),a				; set combined DS1-DS8 data
		ld		(PADRShadow),a		; copy to shadow register
#endasm
}

/***************************************************************
  ledon(int led)
  turn the specified led on.
****************************************************************/
void ledon(int led)
{
	(1<<led)^0xFF;					// load int expression value into HL
#asm
ioi	ld		a,(PADR)				; get current port value
		and	l						; LED output is active low
ioi	ld		(PADR),a				; update port value
		ld		(PADRShadow),a		; copy to shadow register
#endasm
}

/***************************************************************
  ledoff(int led)
  turn the specified led off.
****************************************************************/
void ledoff(int led)
{
	1<<led;							// load int expression value into HL
#asm
ioi	ld		a,(PADR)				; get current port value
		or		l						; LED output is inactive high
ioi	ld		(PADR),a				; update port value
		ld		(PADRShadow),a		; copy to shadow register
#endasm
}

/***************************************************************
  int sw(int which)
  return the value of the specified switch
****************************************************************/
int sw(int which)
{
	1<<which;						// load int expression value into HL
#asm
ioi	ld		a,(PBDR)				; get current port b inputs
		cpl							; switches are active (pressed) low
		and	l						; mask off non-"which" switch bits
		bool	hl						; clear MSB of HL (short and fast!)
		ld		l,a					; load non-zero if "which" switch pressed
		bool	hl						; return only either 0 or 1 in HL
#endasm
}

/***************************************************************
  void buzzon()
  turn the buzzer on
****************************************************************/
void buzzon()
{
#asm
		ld		a,0x01				; buzzer output bit active high
ioi	ld		(PEDR),a				; activate buzzer output
		ld		(PEDRShadow),a		; copy to shadow register
#endasm
}

/***************************************************************
  void buzzoff()
  turn the buzzer off
****************************************************************/
void buzzoff()
{
#asm
		xor	a						; clear a (short and fast!)
ioi	ld		(PEDR),a				; deactivate buzzer output
		ld		(PEDRShadow),a		; copy to shadow register
#endasm
}

/***************************************************************
  flashled[4](int,int,int)
  cofunction which flashes the "which" led on for ontime then
  off for offtime
****************************************************************/

cofunc flashled[4](int which, int ontime, int offtime)
{
	ledon(which);
	waitfor(DelayMs(ontime));	// LED on delay
	ledoff(which);
	waitfor(DelayMs(offtime));	// LED off delay
}

void main()
{
	initialize_ports();						// initializes ports a and e
	
	for(;;) {									// run forever
			
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
		}
		else {
			buzzoff();
		}
		
		//
		// flash leds
		//
		
		// Four costatements, each with their own wfd containing a flashled[]
		//  indexed cofunction instance, keep the flash timing of each LED
		//  independent of the other.  If these cofunction calls were within
		//  wfds in a single costatement then the LEDs' flash timings would not
		//  be independent of each other.
		
		costate {
			wfd {
				flashled[0](DS5,500,1000);	// flashes DS5 (PA4)
			}
		}
		costate {
			wfd {
				flashled[1](DS6,1000,500);	// flashes DS6 (PA5)
			}
		}
		costate {
			wfd {
				flashled[2](DS7,250,1250);	// flashes DS7 (PA6)
			}
		}
		costate {
			wfd {
				flashled[3](DS8,750,900);	// flashes DS8 (PA7)
			}
		}
	}												// end for loop

}
