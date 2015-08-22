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
/********************************************************************
	lcd_demo.c			 

	The is a sample program for connecting to an LCD (which uses the
	HD44780 controller, or an equivalent), using the Rabbit 2000
	external I/O rabbit port.

	This file contains information for a 4 bit interface.  The 4 bit
	interface uses one of the parallel I/O.

This is the initialization sequence for a 2 by 20 character LCD
programmed in 4 bit mode (chosen in order to reduce the number of
I/O pins):
	delay for power up - allow LCD to reset
	command: 4 bit mode
	command: 2 lines, 5x10 character size, and 4 bit mode again
	command: turn on display and cursor - non blinking
	command: incr address and shift cursor with each character
Note: each command must be followed by a delay to allow the display
to respond.

Connection list: 
Jackrabbit Pin LCD Signal
	PA0 J3-3		DB4
	PA1 J3-4		DB5
	PA2 J3-5		DB6
	PA3 J3-6		DB7

	PA6 J3-9		RS		Register Select: 0=command, 1=data
	PA7 J3-10	E		normally low: latches on high to low transition

DB0 - DB3 of the LCD are grounded as is the R/W pin.  Since we are
using 4 bit mode DB0 - DB3 are not used by the controller.  We will
only be writing so we ground the R/W pin

Since Port A is used to control the LEDs on the expansion board, they
will be controlled by the data values which you program.

VEE is used to control brightness - the simplest thing to do is ground
it. If you need to control the brightness you can connect a
potentiometer between ground and -5V with the arm going to the VEE
pin. Check the specs on the LCD before doing this as some may require
a different connection.  VCC is +5 and VSS is ground. 
********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


/********************************************************************
msDelay - delay some number of milliseconds
	input parameter: int number of milliseconds to delay
	return value: none
	errors: none
********************************************************************/
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

/*******************************************************************
UsDelay - delay some number of micro seconds - very approximate!!
	about 11usec per iteration with the 7.3MHz crystal.  The formula
	used here was determined experimentally.
********************************************************************/
void UsDelay ( int iDelay )
{	auto int i;
	iDelay /= 11;
	for ( i=0; i<iDelay; i++ );
}

/*************************** 4 bit functions ***********************/
#define DATA		0x40
#define COMMAND	0x00
int iDataFlag;

/*******************************************************************
WriteNibble - send a single nibble to the LCD
********************************************************************/
void WriteNibble ( char cNib )
{	cNib &= 0x0F;								// remove upper 4 bits
	cNib |= iDataFlag;						// insert data flag for RS
	WrPortI ( PADR, NULL, cNib|0x80 );	// assert E
	WrPortI ( PADR, NULL, cNib );			// remove E
}

/*******************************************************************
				for those who prefer assembly code.........
#asm
WriteNibble::
; character is in HL
			ld		a, L			; get the character
			and	0x0F			; insure upper nibble is zero
			ld		hl, iDataFlag ; get address of data flag
			or		(hl)			; insert Data/Command bit
			or		0x80			; assert E
	ioi	ld		(PADR),a		; ship data value and E
			and	0x7F			; remove E
	ioi	ld		(PADR),a		; ship data value without E
			ret
#endasm
*******************************************************************/

/******************************************************************
WriteByte - send two nibbles to the LCD, upper nibble first
*******************************************************************/
void WriteByte ( char cNib )
{	auto int i;
	i = cNib;				// temp store
	cNib >>= 4;				// put upper nibble into lower 4 bits
	WriteNibble ( cNib );	// send upper nibble
	WriteNibble ( i );		// send lower nibble
	UsDelay (100);
}

/*********************************************************************
LCD_Init - Initialize the LCD for the following operating parameters:
	4 bit mode, 2 lines, 5x10
	turn on display and cursor: non-blinking
	incr address and shift cursor with each character
*******************************************************************/
void LCD_Init ()
{	WrPortI ( PADR, NULL, 0 );		// write 0 to port A output register
	WrPortI ( SPCR, NULL, 0x84 );	// make port A output
	iDataFlag = COMMAND;				// show command mode
	msDelay ( 1000 );				// wait for LCD to reset itself
	WriteNibble ( 2 );					// 4 bit mode
	UsDelay (100);
	WriteByte ( '\B00101100' );		// 4 bit mode, 2 lines, 5x10
	WriteByte ( '\B00001110' );		// turn on display and cursor: non-blinkin
	WriteByte ( '\B00000110' );		// incr address and shift cursor with each character
}

/*********************************************************************
Display - display a line of text on the LCD
	argument: address of null terminated text string
*******************************************************************/
void Display ( char *szp )
{	iDataFlag = DATA;					// next bytes are data
	while ( *szp ) WriteByte ( *szp++ );
}

void Clear ( void )
{	iDataFlag = COMMAND;				// set up for command
	WriteByte ( 0x01 );					// clear the display
	msDelay (3);						// insure at least 2 msec
}

void Line2 ( void )
{	iDataFlag = COMMAND;				// set up for command
	WriteByte ( 0xC0 );					// set RAM address to Line 2
					//note: this value may vary for different displays
	msDelay (3);						// insure at least 2 msec
}


void main ()
{

	LCD_Init ();
		// IMPORTANT - the initialization sequence should only
		// be executed once after powerup.  Multiple executions
		// will cause the display to malfunction.
	
	while (1) {
		Clear();
		Display ( "0123456789" );
		msDelay (500);
		Clear();
		Display ( "This is a test!" );
		msDelay (500);

		Line2 ();
		Display ( "This is line TWO!" );
		msDelay (500);
		}
}
