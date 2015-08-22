/***************************************************************************\
	keylcd.c

	This program demonstrates a simple setup for a 2x6 keypad and a 2x20 LCD.

	Use with Rabbit Core Modules.  Connect circuitry as described
	in your User Manual Sample Circuit Section.

	Upon exection of this program, the LCD will display two rows of 6 dots
	each representing a corresponding key.  When a key is pressed the
	corresponding dot will display an asterisk, "*".

	For this example to work properly, the LCD should be connected to
	parallel	port A as described here and in the example circuitry:

		PA.0	Out	LED Backlight (if connected)
		PA.1	Out	LCD CS (Active High)
		PA.2	Out	LCD RS (High = Control, Low = Data) / LCD Contrast 0
		PA.3	Out	LCD WR (Active Low) / LCD Contrast 1
		PA.4	Out	LCD D4 / LCD Contrast 2
		PA.5	Out	LCD D5 / LCD Contrast 3
		PA.6	Out	LCD D6 / LCD Contrast 4
		PA.7	Out	LCD D7 / LCD Contrast 5

	The keypad should be connected to parallel ports B, C and D
	as described here and in the example circuitry:

		PB.0	In		Keypad Col 0
		PC.1	In		Keypad Col 1
		PB.2	In		Keypad Col 2
		PB.3	In		Keypad Col 3
		PB.4	In		Keypad Col 4
		PB.5	In		Keypad Col 5
		PD.6	O.C.	Keypad Row 0
		PD.7	O.C.	Keypad Row 1

\***************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

///// delay function
void delay(unsigned int	wDelay)
{
	for (;wDelay>0;--wDelay);
}


///// 4-bit lcd command
void lcdCmd4 (char cmd4)
{
	WrPortI ( PADR,&PADRShadow,(cmd4&0xF1)|(PADRShadow&1) );		//	Ready Nibble
	WrPortI ( PADR,&PADRShadow,PADRShadow|0x02 );					//	Write Nibble
	WrPortI ( PADR,&PADRShadow,PADRShadow&0xFD );
}


///// 8-bit lcd command
void lcdCmd (int cmd)
{
	lcdCmd4 ( cmd );					//	Write Upper Nibble
	lcdCmd4 ( cmd<<4 );				//	Write Lower Nibble
	delay ( 4 );						//	Wait 40 uSec
}

///// place cursor at column and row
void lcdGoto (unsigned int col, unsigned int row)
{
	const static char acPos[4] = { 0x80,0xC0,0x94,0xD4 };

	if ((col < 20) && (row < 4))
		lcdCmd ( acPos[row]+col );
}

///// write character to lcd
void lcdPutc (char cData)
{
	WrPortI ( PADR,&PADRShadow,(cData&0xF0)|0x04|(PADRShadow&0x01) );
	WrPortI ( PADR,&PADRShadow,PADRShadow|0x02 );					//	Strobe Write
	WrPortI ( PADR,&PADRShadow,PADRShadow&0xFD );
																					//	Ready Lower Nibble
	WrPortI ( PADR,&PADRShadow,((cData<<4)&0xF0)|0x04|(PADRShadow&0x01) );
	WrPortI ( PADR,&PADRShadow,PADRShadow|0x02 );					//	Strobe Write
	WrPortI ( PADR,&PADRShadow,PADRShadow&0xFD );
	WrPortI ( PADR,&PADRShadow,(PADRShadow&0x01) );	//	Restore Contrast
	delay ( 4 );												//	Wait 40 uSec
}

///// write formatted data to lcd, similar to printf statement
void lcdPrintf	(char	*pcFormat,...)
{
	doprnt ( lcdPutc,pcFormat,&pcFormat+1,NULL, NULL, NULL );
}


///// scan keypad for key press
void keyCol(void)
{
	delay ( 1000 );
	lcdPrintf ( "%c%c%c%c%c%c",
			((RdPortI(PBDR)&0x20)?'.':'*'),
			((RdPortI(PBDR)&0x10)?'.':'*'),
			((RdPortI(PBDR)&0x08)?'.':'*'),
			((RdPortI(PBDR)&0x04)?'.':'*'),
			((RdPortI(PCDR)&0x02)?'.':'*'),
			((RdPortI(PBDR)&0x01)?'.':'*')
		);
}



void main (	void	)
{

	///// initialize keypad
	WrPortI ( PDFR,NULL,0x00 );			//	parallel Port D All I/O
	WrPortI ( PDDCR,NULL,0xC0 );			//	PD7..PD6 Open Collector
	WrPortI ( PDDDR,NULL,0xC0 );			//	PD7..PD6 Output

	///// initialize lcd
	WrPortI ( PADR,&PADRShadow,0x00 );	//	Assure Outputs are Low
	WrPortI ( SPCR,&SPCRShadow,0x84 );	//	parallel Port A = Outputs

	WrPortI ( PADR,&PADRShadow,PADRShadow&0x01 ); // Set Contrast at GND
	delay ( 1500 );						//	Wait 15 mSec (LCD to Stabilize)
	lcdCmd4 ( 0x30 );						//	Set to 8-Bit Interface
	delay ( 410 );							//	Wait 4.1 mSec
	lcdCmd4 ( 0x30 );						//	Set 8-Bit Interface
	delay ( 10 );							//	Wait 100 uSec
	lcdCmd4 ( 0x30 );						//	Set 8-Bit Interface
	lcdCmd4 ( 0x20 );						//	Set 8-Bit Interface
	lcdCmd ( 0x28 );						//	Set Dual Line Display
	lcdCmd ( 0x06 );						//	Disable Display Shift
	lcdCmd ( 0x0C );						//	Display On, Cursor Off
	lcdCmd ( 0x01 );						//	Clear Screen, Home Cursor, Backlight On
	delay ( 164 );							//	Wait 1.64 mSec

	for (;;) {
		WrPortI ( PDDR,NULL,0x40 );	//	Scan Row 0
		lcdGoto ( 0,0 );					//	Cursor on column 0, row 0
		keyCol ();							// Scan for keypress

		WrPortI ( PDDR,NULL,0x80 );	//	Scan Row 1
		lcdGoto ( 0,1 );					//	Cursor on column 0, row 1
		keyCol ();							// Scan for keypress
		}
}