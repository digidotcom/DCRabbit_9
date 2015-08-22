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
	scrolling.c
	
	This sample program is for the OP7200 series controllers.

  	This program demonstrates the scrolling features of the graphic
  	library.

  	Instructions:
  	-------------
  	1. Run and compile this program.
  	2. Watch the LCD display as it goes through the various scrolling
  	   demo's.

**************************************************************************/
#memmap xmem  // Required to reduce root memory usage 

// Create structure for 10x16 font character set
fontInfo fi10x16, fi12x16, fi17x35;

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

main()
{
	auto int i,x,y,z;
	auto char s[256];
	auto int NumPixel;
	auto int FontWidth, FontHeight;
	
	//------------------------------------------------------------------------
	// Initialize controller 
	//------------------------------------------------------------------------
	brdInit();			// Initialize Controller...Required for controllers!!! 

	glInit();			// Initialize the graphic driver
	glBackLight(1);
	glSetContrast(24);

	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);		//	Initialize basic font
	glXFontInit(&fi12x16, 12, 16, 32, 127, Font12x16);		//	Initialize basic font
	glXFontInit(&fi17x35, 17, 35, 32, 127, Font17x35);		//	Initialize basic font

	while(1)
	{	
		//------------------------------------------------------------------------
		// Text scroll-up example 
		//------------------------------------------------------------------------
		glPrintf(0, 0,&fi10x16,"Scroll-up Demo");
		msDelay(800);

		FontWidth  = 10;
		FontHeight = 16;
		glBuffLock();
		glBlankScreen();
		for (y = 0, z = 32; y < 240; y += FontHeight)
		{	
			for (x = 0; x < LCD_XS; x+=FontWidth)
			{
				glPrintf(x, y,&fi10x16,"%c",z++);
				if (z > 64) z = 32;
			}
		}
		glBuffUnlock();
		msDelay(750);

		for(y=0; y<3; y++)
		{
			glVScroll(0, 0, LCD_XS, LCD_YS, -FontHeight);
			for (x = 0; x < LCD_XS; x+=FontWidth)
			{
				glPrintf(x, LCD_YS-FontHeight, &fi10x16, "%c",z++);
				if (z > 126) z = 32;
					msDelay(5);
			}
			msDelay(600);
		} 
	
		//------------------------------------------------------------------------
		// Text scroll-down example 
		//------------------------------------------------------------------------
		glBlankScreen();
		glPrintf(0, 0,&fi10x16,"Scroll-Down Demo");
		msDelay(800);

		FontWidth  = 10;
		FontHeight = 16;
		glBuffLock();
		glBlankScreen();
		for (y = 0, z=32; y < LCD_XS; y += FontHeight)
		{
			for (x = 0; x < LCD_XS; x+=FontWidth)
			{
				glPrintf(x, y,&fi10x16,"%c",z++);
				if (z > 126) z = 32;
					msDelay(5);
			}
		}

		glBuffUnlock();
		msDelay(750);
		for(y=0; y<4; y++)
		{
			glVScroll(0, 0, LCD_XS, LCD_YS, FontHeight);
			for (x = 0; x < LCD_XS; x+=FontWidth)
			{
				glPrintf(x, 0,&fi10x16,"%c",z++);
				if (z > 64) z = 32;
					msDelay(5);
			}
			msDelay(600);
		}

		//------------------------------------------------------------------------
		// Text Scrolling left example 
		//------------------------------------------------------------------------
		glBlankScreen();
		glPrintf(0, 0,&fi10x16,"Scroll-Left Demo");
		msDelay(1500);

		NumPixel = 17;
		for(y = 0; y < 1; y++)
		{
			sprintf(s, "Hello from Z-World.....");
			i =0;
			while(s[i] != '\0')
			{
				glHScroll(0, 60, LCD_XS, 34, -NumPixel);
				glPrintf (LCD_XS-NumPixel, 60, &fi17x35, "%c", s[i++]);
				msDelay(300);
			}
		}
		glBlankScreen();
		
		//------------------------------------------------------------------------
		// Text Scrolling right and left example 
		//------------------------------------------------------------------------
		glPrintf(0, 0,&fi10x16,"Scroll Right&Left Demo");
		msDelay(1200);

		FontWidth  = 10;
		FontHeight = 16;
		sprintf(s, "Text to Scroll Right/Left");
		glPrintf (0, 40, &fi10x16, "%s", s);
		msDelay(1000);
		NumPixel = 4;
		for(y = 0; y < 3; y++)
		{		
			for(i=0; i<(LCD_XS -(strlen(s)*FontWidth)); i+=FontWidth)
			{
				glHScroll(0, 40, LCD_XS, FontHeight, FontWidth);	
			}
			msDelay(500);
			for(i=0; i<(LCD_XS-(strlen(s)*FontWidth)); i+=FontWidth)
			{
				glHScroll(0, 40, LCD_XS, FontHeight, -FontWidth);
			}
			msDelay(500);
		}
		glBlankScreen();
	}
}