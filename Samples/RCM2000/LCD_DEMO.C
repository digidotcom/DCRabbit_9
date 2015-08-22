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

This is the initialization sequence for a 2 by 20 character LCD
programmed in 8 bit mode:
	delay for power up - allow LCD to reset
	command: 8 bit mode
	command: 2 lines, 5x10 character size, and 8 bit mode again
	command: turn on display and cursor - non blinking
	command: incr address and shift cursor with each character

Note: each command must be followed by a delay to allow the display
to respond.

Connection list:
Core Module	   LCD
 Signal		  Signal
	D0				DB0
	D1				DB1
	D2				DB2
	D3				DB3
	D4				DB4
	D5				DB5
	D6				DB6
	D7				DB7
	A0				RS		Register Select: 0=command, 1=data
	A1				R/W	0=write, 1=read
	*				E		normally low: latches on high to low transition

* = circuit which uses a 74HC02, or equivalent, as follows

			|----\
/PE0-----O		|	|-----|	|----\
			|		|--|		|	|		|
/IOR-----O		|	|		|	|		|
			|----/	|		|	|		|
						|		O--O		|---- E
			|----\	|		|	|		|
/PE0-----O		|	|		|	|		|
			|		|--|		|	|		|
/IOW-----O		|	|-----|	|----/
			|----/

	Bus address		Function
		0				write command byte
		1				write data byte
		2				read busy flag and address
		3				read data from RAM

	See notes above for power and brightness.

********************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

/********************************************************************
MsDelay - delay some number of milliseconds
	input parameter: int number of milliseconds to delay
	return value: none
	errors: none
********************************************************************/
void MsDelay ( int iDelay )
{
	auto unsigned long done_time
	done_time = iDelay + MS_TIMER;				// when it is over.
	while( (int)(MS_TIMER - done_time) < 0 );
}

/*******************************************************************
UsDelay - delay some number of micro seconds - very approximate!!
	about 11usec per iteration with the 7.3MHz crystal.  The formula
	used here was determined experimentally.
********************************************************************/
void UsDelay ( int iDelay )
{
	auto int i;
	iDelay /= 11;
	for ( i=0; i<iDelay; i++ );
}

#define	WCOMMAND	0
#define	WDATA		1
#define	RBUSY		2	// not currrently used
#define	RRAM		3	// not currrently used

/*********************************************************************
WriteByte - send a byte to the LCD
*******************************************************************/
#asm
;function requires two parameters:
;@sp+2= 1st parameter value = external I/O address
;@sp+4= 2nd parameter value = data
;
;Need to clock doubler in order to meet the set up and hold
;time requirements of the LCD.  For a faster CPU time you may
;need to set the CPU clock to "divide by 8" mode

WriteByte::
			xor	a				;a = 0
	ioi	ld		(GCDR), a	;turn off clock doubler

			ld		hl,(sp+4)	; get data value
			ld		a, l			; save it for ioe
			ld		hl,(sp+2)	; get data address
	ioe	ld		(hl),a		; ship data value

			ld		a, (GCDRShadow)
	ioi	ld		(GCDR), a	;restore clock doubler

			ld		hl, 100		;set up for
			push	hl
			call	UsDelay	;delay
			pop	hl
			ret
#endasm


/*********************************************************************
LCD_Init - Initialize the LCD for the following operating parameters:
	8 bit mode, 2 lines, 5x10
	turn on display and cursor: non-blinking
	incr address and shift cursor with each character
*******************************************************************/
void LCD_Init ()
{
	// set up PE0 as I/O strobe
	WrPortI ( PEDDR, &PEDDRShadow, 1 );	// PE0 = output
	WrPortI ( PEFR, &PEFRShadow, 1 );	// PE0 = I/O strobe
	WrPortI ( IB0CR, NULL, 0x08 );		// permit write, chip select, 15 wait states
	MsDelay ( 1000 );							// wait for LCD to reset itself

	WriteByte ( WCOMMAND, '\B00111100' );	// 8 bit mode
	UsDelay (100);
	WriteByte ( WCOMMAND, '\B00111100' ); 	// 8 bit mode,  2 lines, 5x10
	WriteByte ( WCOMMAND, '\B00001110' );	// turn on display and cursor: non-blinkin
	WriteByte ( WCOMMAND, '\B00000110' );	// incr address and shift cursor with each character
}


/*********************************************************************
Display - display a line of text on the LCD
	argument: address of null terminated text string
*******************************************************************/
void Display ( char *szp )
{
	while ( *szp ) WriteByte ( WDATA, *szp++ );
}

void Clear ( void )
{
	WriteByte ( WCOMMAND, 0x01 );	// clear the display
	MsDelay (3);						// insure at least 2 msec
}

void Line2 ( void )
{
	WriteByte ( WCOMMAND, 0xC0 );	// set RAM address to Line 2
					//note: this value may vary for different displays
	MsDelay (3);						// insure at least 2 msec
}

/********************************************************************/
/********************************************************************/

void main ()
{

	LCD_Init ();
		// IMPORTANT - the initialization sequence should only
		// be executed once after powerup.  Multiple executions
		// will cause the display to malfunction.

	while (1) {
		Clear();
		Display ( "0123456789" );
		MsDelay (500);
		Clear();
		Display ( "This is a test!" );
		MsDelay (500);

		Line2 ();
		Display ( "This is line TWO!" );
		MsDelay (500);
		}
}