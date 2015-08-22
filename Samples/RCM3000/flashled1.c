/*******************************************************************
	flashled1.c
  	Z-World, 2002

	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	Description
	===========
	This assembly program uses costatements to flash LED's,
	DS1 and DS2, on the prototyping board at different intervals.
	The LEDs are access by the "master" module.	

	Instructions
	============
	1.  Compile and run this program.
	2.  DS1 and DS2 LED's flash on/off at different times.
*******************************************************************/
#class auto

#define DS1 6		//port G bit 6
#define DS2 7		//port G bit 7
#define ON  0		//state to turn on led
#define OFF 1		//state to turn off led

///////////////////////////////////////////////////////////
// Initializes port G only
// See brdInit() in sample library rcm3000.lib for
// other port initializing
///////////////////////////////////////////////////////////
void initPort()
{
#asm
	ld		a,0x00					;clear all bits for pclk/2
	ioi	ld (PGCR),a
	ld		(PGCRShadow),a
	
	ld		a,0x00					;clear all bits for normal function
	ioi	ld (PGFR),a
	ld		(PGFRShadow),a
	
	ld		(PGDCRShadow),a
	or		0xC0						;set bits 7,6 drive open drain
	res	2,a						;clear bit 2 drive output
	ioi	ld (PGDCR),a
	ld		(PGDCRShadow),a
	
	ld		(PGDRShadow),a
	or		0xC0						;set bits 7,6 output high
	res	2,a						;clear bit 2 output low
	ioi	ld (PGDR),a
	ld		(PGDRShadow),a
	
	ld		a,0xC4					;set bits 7,6,2 to output, clear bits 5,4,3,1,0 to input
	ioi	ld (PGDDR),a
	ld		(PGDDRShadow),a
#endasm
}

///////////////////////////////////////////////////////////
// DS1 led on protoboard is controlled by port G bit 6
// turns on if state = 0
// turns off if state = 1
///////////////////////////////////////////////////////////
void DS1led(int state)
{
	if (state == ON)
	{
#asm
	ld		a,(PGDRShadow)			;use shadow register to keep other bit values
	res	DS1,a						;clear bit 6 only
	ioi	ld (PGDR),a				;write data to port g
	ld		(PGDRShadow),a			;update shadow register
#endasm
	}
	else
	{
#asm
	ld		a,(PGDRShadow)			;use shadow register to keep other bit values
	set	DS1,a						;set bit 6 only
	ioi	ld (PGDR),a				;write data to port g
	ld		(PGDRShadow),a			;update shadow register
#endasm
	}
}
	
///////////////////////////////////////////////////////////
// DS2 led on protoboard is controlled by port G bit 7
// turns on if state = 0
// turns off if state = 1
///////////////////////////////////////////////////////////
void DS2led(int state)
{
	if (state == ON)
	{
#asm
	ld		a,(PGDRShadow)			;use shadow register to keep other bit values
	res	DS2,a						;clear bit 7 only
	ioi	ld (PGDR),a				;write data to port g
	ld		(PGDRShadow),a			;update shadow register
#endasm
	}
	else
	{
#asm
	ld		a,(PGDRShadow)			;use shadow register to keep other bit values
	set	DS2,a						;set bit 7 only
	ioi	ld (PGDR),a				;write data to port g
	ld		(PGDRShadow),a			;update shadow register
#endasm
	}
}
	
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void main()
{

	initPort();				// initializes port G only
		
	while (1)
	{
		costate
		{	// DS1 LED
			DS1led(ON);							//on for 50 ms
			waitfor(DelayMs(50));							
			DS1led(OFF);						//off for 100 ms
			waitfor(DelayMs(100));
		}
		
		costate
		{	// DS2 LED
			DS2led(ON);							//on for 200 ms
			waitfor(DelayMs(200));								
			DS2led(OFF);						//off for 50 ms
			waitfor(DelayMs(50));
		}
	}
}
