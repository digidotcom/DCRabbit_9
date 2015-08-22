/*******************************************************************
	flashled1.c
  	Z-World, 2002

	This program is used with RCM3400 series controllers
	with prototyping boards.
	
	Description
	===========
	This assembly program uses costatements to flash LED's,
	DS1 and DS2, on the prototyping board at different intervals.
	brdInit() is not called in this demonstration.
	
	Output control		LED on the proto-board
	--------------		----------------------
	Port D bit 6		DS1
	Port D bit 7		DS2
	
	Instructions
	============
	1.  Compile and run this program.
	2.  DS1 and DS2 LED's flash on/off at different times.
*******************************************************************/
#class auto

#define DS1 6		//port D bit 6
#define DS2 7		//port D bit 7
#define ON  0		//state to turn on led
#define OFF 1		//state to turn off led

///////////////////////////////////////////////////////////
// Initializes port D bits 6 and 7 only
// See brdInit() for other port initializing
///////////////////////////////////////////////////////////
void initPort()
{
#asm
	ld		a,0x00					;clear all bits for pclk/2
	ioi	ld (PDCR),a
	ld		(PDCRShadow),a
	
	ld		a,0x00					;clear all bits for normal function
	ioi	ld (PDFR),a
	ld		(PDFRShadow),a
	
	ld		(PDDCRShadow),a
	or		0xC0						;set bits 7,6 drive open drain
	ioi	ld (PDDCR),a
	ld		(PDDCRShadow),a
	
	ld		(PDDRShadow),a
	or		0xC0						;set bits 7,6 output high
	ioi	ld (PDDR),a
	ld		(PDDRShadow),a
	
	ld		a,(PDDDRShadow)
	or		0xC0						;set bits 7,6,2 to output
	ioi	ld (PDDDR),a
	ld		(PDDDRShadow),a
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
	ld		a,(PDDRShadow)			;use shadow register to keep other bit values
	res	DS1,a						;clear bit 6 only
	ioi	ld (PDDR),a				;write data to port 
	ld		(PDDRShadow),a			;update shadow register
#endasm
	}
	else
	{
#asm
	ld		a,(PDDRShadow)			;use shadow register to keep other bit values
	set	DS1,a						;set bit 6 only
	ioi	ld (PDDR),a				;write data to port 
	ld		(PDDRShadow),a			;update shadow register
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
	ld		a,(PDDRShadow)			;use shadow register to keep other bit values
	res	DS2,a						;clear bit 7 only
	ioi	ld (PDDR),a				;write data to port 
	ld		(PDDRShadow),a			;update shadow register
#endasm
	}
	else
	{
#asm
	ld		a,(PDDRShadow)			;use shadow register to keep other bit values
	set	DS2,a						;set bit 7 only
	ioi	ld (PDDR),a				;write data to port 
	ld		(PDDRShadow),a			;update shadow register
#endasm
	}
}
	
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
main()
{

	initPort();	
		
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
