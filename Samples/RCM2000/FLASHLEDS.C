/***************************************************************

   FlashLEDS.c
   Zworld, 2000

   RCM 2000 Demo board example program

   This program flashes the DS2 and DS3 LEDs on the RCM 2000
   Prototyping board (PA0 and PA1 ports on the RCM 2000).
   It also demonstrates coding with assembly instructions,
   cofunctions and costatements.

****************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

/***************************************************************
  defines for the leds
****************************************************************/
#define DS2 0
#define DS3 1

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
  flashled[2](int,int,int)
  cofunction which flashes the "which" led on for ontime then
  off for offtime
****************************************************************/

cofunc flashled[2](int which, int ontime, int offtime)
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
		// Two costatements, each with their own wfd containing a flashled[]
		//  indexed cofunction instance, keep the flash timing of each LED
		//  independent of the other.  If both cofunction calls were within
		//  either one or two wfds in a single costatement then the LEDs' flash
		//  timings would not be independent of each other.
		costate {								// start 1st costatement
			wfd {
				flashled[0](DS2,500,900);	// flashes DS2 (PA0)
			}
		}											// end 1st costatement
		costate {								// start 2nd costatement
			wfd {
				flashled[1](DS3,800,400);	// flashes DS3 (PA1)
			}
		}											// end 2nd costatement
	}												// end for loop
}
