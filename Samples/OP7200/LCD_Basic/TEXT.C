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
/***************************************************************************
	text.c

	This sample program is for the OP7200 LCD Display.

  	This program demonstrates the Text functions from the graphic
  	library, which will show the following:

  	1. Font initialization.
  	2. Text window initialization.
  	3. Text window, end of line wrap-around demo.
	4. Creating different TEXT windows for display.

	Instructions:
	1. Compile and run this program.
	2. View the display for each of the above features.
	3. Following the instructions when prompted.

**************************************************************************/
#memmap xmem  // Required to reduce root memory usage

fontInfo fi6x8, fi8x10, fi10x16, fi12x16;
windowFrame textWindow1, textWindow2, textWindow3;

main()
{

	auto int row1, row2, row3;
	auto int col1, col2, col3;
	auto int counter;
	auto int wKey;
	auto int maxCharDisp;
	auto unsigned long xmemPtr;
	auto int exitloop;

	// Allocate 1200 bytes needed to display a full screen of 6x8 size
	// characters. This buffer will be used to do the auto wrapping of
	// strings that exceed the window width boundry.
	char buffer[1200];


	//------------------------------------------------------------------------
	// Board and drivers initialization
	//------------------------------------------------------------------------
	brdInit();		// Required for all controllers

	glInit();		// Initialize graphic driver
	glBackLight(1);
	glSetContrast(24);

	keyInit();		// Start-up the keypad driver
	keypadDef();	// Use the default keypad ASCII return values

	//------------------------------------------------------------------------
	// Font initialization
	//------------------------------------------------------------------------
	// Initialize structures with FONT bitmap information
	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);		//	initialize basic font
	glXFontInit(&fi12x16, 12, 16, 32, 127, Font12x16);		//	initialize basic font

	//------------------------------------------------------------------------
	// Text window initialization
	//------------------------------------------------------------------------

	// Setup the widow frame2 to be the upper half of the LCD to display information
	TextWindowFrame(&textWindow1, &fi10x16, 0, 0, LCD_XS, 150);
	TextBorderInit(&textWindow1, DOUBLE_LINE, " Text Window1 ");

	// Setup the widow frame3 to be the lower half of the LCD to display information
	TextWindowFrame(&textWindow2, &fi10x16, 70, 190, 170, 50);
	TextBorderInit(&textWindow2, DOUBLE_LINE, "Text Window2");

	//------------------------------------------------------------------------
	// Initialize buffer with text for display
	//------------------------------------------------------------------------
	sprintf(buffer, "The OP7200 intelligent operator interface is a small, high-performance, ");
	strcat(buffer, "C-programmable data acquisition and display unit that offers built-in I/O, ");
	strcat(buffer, "Ethernet connectivity, and an optional touchscreen.");
	strcat(buffer, "The OP7200 is designed for panel mounting and is NEMA-4 compatible. ");
	strcat(buffer, "The OP7200 incorporates the powerful Rabbit 2000 microprocessor, flash ");
	strcat(buffer, "memory, static RAM, industrialized digital I/O ports, RS-232/RS-485 serial ");
	strcat(buffer, "ports, a 10Base-T Ethernet port, and optional A/D converter ports and touchscreen.");

	//------------------------------------------------------------------------
	// Text Window Demo
	//------------------------------------------------------------------------
	TextBorder(&textWindow1);
	TextBorder(&textWindow2);

	maxCharDisp = 0;
	TextPrintf(&textWindow1, "%s", buffer);
	glPrintf(0, 155, &fi12x16, "Up/Down keys to scroll text");
	glPrintf(0, 171, &fi12x16, "and press ENTER to exit.");

	counter = 0;
	exitloop = FALSE;
	while(!exitloop)
	{
		keyProcess();
		wKey = keyGet();

		switch(wKey)
		{
			case 'D':
				if((maxCharDisp + TextMaxChars(&textWindow1)) < strlen(buffer))
				{
					maxCharDisp += TextMaxChars(&textWindow1);
					TextGotoXY(&textWindow1, 0, 0);
					TextWinClear(&textWindow1);
					TextPrintf(&textWindow1, "%s", &buffer[maxCharDisp]);
				}
				break;

			case 'U':
				if((maxCharDisp - TextMaxChars(&textWindow1)) >= 0)
				{
					maxCharDisp -= TextMaxChars(&textWindow1);
					TextGotoXY(&textWindow1, 0, 0);
					TextWinClear(&textWindow1);
					TextPrintf(&textWindow1, "%s", &buffer[maxCharDisp]);
				}
				break;

			case 'E':
				exitloop = TRUE;
				break;
		}

		// The string length must not exceed the maximum columns on a given
		// line, otherwise end-of-line auto-wrap will occur and "\r" will
		// take effect on the newline.

		TextPrintf(&textWindow2, " Counter = %03d\r",  counter++);
		// When using the number of digit specifier (ie "%3d") you'll need to keep
		// the value within the range that you specify otherwise you'll get asterisks
		// indicating an out of range error.
		if(counter == 999)
		{
			counter = 0;
		}
	}
}