/***************************************************************

   Samples\RCM2200\FlashLEDS.c
   Zworld, 2001

   Rabbit 2200 Micro Core Module Demo board example program

   This program flashes the DS2 and DS3 LED's on the Rabbit 2200
   Micro Ethernet Core Module Prototyping board (PE1 and PE7 ports
   on the Rabbit Core Module).  The MASTER core controls the LEDs.

   It also demonstrates coding with assembly instructions,
   indexed cofunctions, and costatements.

****************************************************************/
#class auto

/***************************************************************
  index predefines for the leds and switches
****************************************************************/
#define DS2 0
#define DS3 1

/***************************************************************
  initialize_ports();
  set up the parallel ports so PORTA are outputs, and PORTE bits
  1,7 are I/O (ie, not control signals) others unchanged.  It
  updates SPCRShadow, PEFRShadow, PEDDRShadow and PECRShadow .
****************************************************************/
void initialize_ports()
{
/*
 *  Use leading zero to force constant value, not memory address.
 *  The 1 shifted over 1 place and 7 places refer to bits 1 and 7
 *  in Port E.  This creates a mask of bits to affect.  The second
 *  line creates a mask of bits NOT to affect; good for turning off
 *  PE1 and PE7.
 */
#define  PE_IO_SETTINGS		0+(1<<1)|(1<<7)			;; Bits 7 and 1 turned on.
#define  PE_NOT_IO_SETTINGS   0+~PE_IO_SETTINGS    ;; 0x7D	; ~82

#asm

	ld		a,0x84				; port A all outputs
ioi ld	(SPCR),a
	ld		(SPCRShadow), a	; store value into shadow register

	ld		a, (PEFRShadow)	; get current settings
	and	a, PE_NOT_IO_SETTINGS	; port E bit 1,7 normal I/O
ioi ld	(PEFR),a
	ld		(PEFRShadow), a	; store shadow register

	ld		a,PE_IO_SETTINGS	; Bits 1,7 as output.
ioi ld	(PEDDR),a
	ld		(PEDDRShadow), a	; store shadow register

	xor	a  					; default transfer clocking
ioi ld	(PECR),a
	ld		(PECRShadow), a
#endasm
}

/***************************************************************
  ledon(int led)
  turn the specified led on.
****************************************************************/
void ledon(int led)
{
	/*  Map from index into bit map. ON requires logic 0, so invert.
	 *  The value will appear in reg-pair HL.
	 */
	(led == DS2) ? ~(1 << 1) : ~(1 << 7);

#asm
	ld		a,(PEDRShadow)
	and	l      					; reg L has bit value (computed above)
ioi ld	(PEDR),a
	ld		(PEDRShadow), a
#endasm
}

/***************************************************************
  ledoff(int led)
  turn the specified led off.
****************************************************************/
void ledoff(int led)
{
	/*  Map from index into bit map. OFF requires logic 1.
	 *  The value will appear in reg-pair HL.
	 */
	(led == DS2) ? (1 << 1) : (1 << 7);

#asm
	ld		a,(PEDRShadow)
	or		l      					; reg L has bit value (computed above)
ioi ld	(PEDR),a
	ld		(PEDRShadow), a
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
