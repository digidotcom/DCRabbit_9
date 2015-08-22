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

   cofterma.c

	This sample program is used with products such as
	Intellicom Series.

   Example program demonstrating cofunctions, the cofunction
   serial library, and using a serial ANSI terminal, such as
   TeraTerm (or HyperTerm) from an available com port connection.

   This program is set up to run from a serial terminal on
   port C.  Unless otherwise defined, baud rate is set to 115200
   bps.   The serial port will default to port C.  Display (output)
   is the LCD.  Input is from the serial line.


   To PC
   Serial Port			To Controller, J7
	===================================

   DCD	1-----*
   			   |
   DSR	2-----*
   			   |
   RX		3-----|------TX
   			   |
   RTS	4--*  |
   			|  |
   TX		5--|--|------RX
   			|  |
   CTS	6--*  |
   			   |
   DTR	7-----*

   RI		8---NC

   GND	9------------GND


   Run this program and type in an 8-character name and
   password from remote terminal.  Hit "Enter" or "Tab"
   from keyboard after each entry. Characters will appear
   on controller display.

****************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

#define BCONTRAST 20  //Adjust for best contrast a value from 4 to 252

#define BAUD		115200

//
// To change the serial port change the B to A, C, or D:
//
// i.e. to use port A:
//
//   CINBUFSIZE	-> AINBUFSIZE
//   COUTBUFSIZE	-> AOUTBUFSIZE
//   cof_serCgetc -> cof_serAgetc
//   cof_serCputc -> cof_serAputc
//   cof_serCputs -> cof_serAputs
//   serCopen	   -> cof_serAputs
//

#define CINBUFSIZE 	31
#define COUTBUFSIZE	31

#define scof_getch cof_serCgetc
#define scof_gets	 cof_serCgets
#define scof_putch cof_serCputc
#define scof_puts  cof_serCputs
#define port_open	 serCopen


/***************************************************************
   basic single user cofunction input/output routines
***************************************************************/

#define ESCSEQSIZE 16

/***************************************************************

   __echoon==1 - echo character
   __echoon==0 - echo '*'

   echoon() is a firsttime function which turns the echo on.
   echooff() is a firsttime function which turns the echo off.

   These were made firsttime functions so they could be called
   inside the waitfordone/wfd statement.

***************************************************************/

int __echoon;
firsttime void echoon()  { __echoon=1; }
firsttime void echooff() { __echoon=0; }

/***************************************************************
	gotoxy(int xpos, int ypos) - locate the cursor at screen
	coordinates (xpos,ypos).
***************************************************************/

scofunc gotoxy(int x, int y)
{

	dispGoto(x,y);

}

/***************************************************************
	putat(int x, int y, char* str) - print the str at location
	(x,y).
***************************************************************/


scofunc putat(int x, int y, char* str)
{

	dispGoto(x,y);
	dispPrintf("%s", str);
}

/***************************************************************
	putatc(int x, int y, char* str) - print a char at location
	(x,y).
***************************************************************/

scofunc putatc(int x, int y, char ch)
{

	dispGoto(x,y);
	dispPutc(ch);
}

/***************************************************************

	getat(int x, int y, char* input, length) - get a string with
	the cursor starting at (x,y) into the variable input for a
	maximum number of characters length.  This function will
	print the string starting at the location (x,y) and allow
	editing at that point.

	As the user types characters they are echoed to the screen
	if the __echoon variable is set.  Otherwise, a "*" is echoed
	in the characters place.

	If there are any character in the string BS will back over
	the last character (removing it from the string).

	RET, TAB, or ESC terminate input and return to the caller.
	The ESC causes the input to terminate with an aborted code
	which stops the enclosing waitfordone statement.

	x=wfd {
		getat(...);
		getat(...);
		getat(...);
	}
	if(x<0) abort;

	The above waitfordone statement of treating the gets as one
	form where the escape will terminate the form.  By detecting
	the abort resonable action could be taken to abort or to back
	up a sequence of input screens.

***************************************************************/

scofunc getat(int xpos, int ypos, char* input, int length)
{
	static int x,offset;
	static char buffer[ESCSEQSIZE];

	offset=strlen(input);

	wfd gotoxy(xpos,ypos);			// locate cursor at (xpos,ypos)

	if(__echoon)
		wfd putat(xpos+offset, ypos, input);  // print the input string
	else {
		for(x=0;x<offset;x++)
			// if echo is off print '*' instead of input
			wfd putatc(xpos+offset, ypos, '*');
	}

	for(x=0;x<length;x++)
		wfd putatc(xpos+offset, ypos, ' ');  // blank out rest of string

	wfd gotoxy(xpos+offset,ypos);	// locate cursor at end of input

	offset=0;

	for(;;) {
		wfd x=scof_getch();			// get a character

		switch(x) {
			case '\b':
				if(offset>0) {
					offset--;
					wfd putatc(xpos+offset, ypos, ' '); // if backspace remove previous character
				} else
					wfd putatc(xpos+offset, ypos,'\x07');

				wfd gotoxy(xpos+offset,ypos);	// locate cursor at end of input
				break;

			case '\t':					// user done if they TAB or RET
			case '\r':
				input[offset]=0;
				return 0;

			case '\x1B':				// user canceling form with ESC
				abort;

			default:
				if(offset<length && isprint(x)) {
					if(__echoon)		// print the character or '*'
						wfd putatc(xpos+offset, ypos, x);
					else
						wfd putatc(xpos+offset, ypos, '*');

					input[offset++]=x; // place character in input
				}
				break;
		}
	}
}


/***************************************************************

	login()
	This function is an example of using putat and getat to
	build a form and demonstrates how echoon and echooff can
	be used.  The first waitfordone statement initialises the
	labels on the screen and the second gets input from the
	user.

***************************************************************/

scofunc login()
{
	static int x;
	static char name[16],pass[16],buffer[80];

	name[0]=pass[0]=0;

	dispClear();

	wfd {
		//clrscr();
		putat(0,0,"name:");
		putat(0,1,"password:");
		echoon();
	}

	x=wfd {
		getat(11,0,name,8);
		echooff();
		getat(11,1,pass,8);
		echoon();
	}

	if(x>0) {
		dispClear();
		sprintf(buffer,"name     %s\npassword %s\n\n     ACCEPTED", name,pass);
		wfd putat(0,0,buffer);
		waitfor(DelaySec(3));
	} else
		abort;
}


/***************************************************************

	main()
	Open the serial port window.
	Continuously call the login forms.

***************************************************************/

void main()
{
	int x;

	brdInit();						// initialize controller
	keypadDef();					// configure keypad to default
	dispContrast(BCONTRAST);	// adjust contrast
	dispCursor(DISP_CURON);		// turn cursor on
	dispBacklight(1);  			// turn on backlight

	port_open(BAUD);

	for(;;) {
		costate {
			x=wfd login();
			if(x<0) abort;
		}
	}
}