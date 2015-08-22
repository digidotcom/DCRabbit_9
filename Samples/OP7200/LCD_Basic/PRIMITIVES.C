/***************************************************************************
	primitive.c

	Z-World, 2001
	Sample program to demonstrate the primitive graphic functions
	lines, circles, polygons and bitmaps.
   	
	
	Instructions:
	1. Compile and run program.
	2. View LCD display to see the various graphic possibilities. 

***************************************************************************/
#class auto
#memmap xmem  // Required to reduce root memory usage 

#define VSPACE (LCD_YS/8)
#define HSPACE (LCD_XS/8)
#define BITMAP_XS (LCD_XS / 5)
#define BITMAP_YS (LCD_YS / 4)
#define BITMAP_SIZE (BITMAP_XS * ((BITMAP_YS + 7) / 8))
#define NUM_VERTICES 4


nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

nodebug
void BitMapDemo( void )
{
	int i, j;

	glBlankScreen();
	glXPutBitmap(10, 8, 308,144, Zwbw_308);
	
	for(j=0; j < 3; j++)
	{
		for(i=0; i < 120; i+=6)
		{
			glBuffLock();
			_glBlankRegion(0, 156, 320, 101);
			if(i<50)
				glXPutBitmap(320-i, 156, 106,101,rabbit_0_bmp);
			else if (i<80)
				glXPutBitmap(320-i, 156, 106,101,rabbit_1_bmp);
			else if(i<100)
				glXPutBitmap(320-i, 156, 106,101,rabbit_2_bmp);
			else
				glXPutBitmap(320-i, 156, 106,101,rabbit_1_bmp);
			glBuffUnlock();
			msDelay(20);
		}
	
		glBuffLock();
		_glBlankRegion(0, 156, 320, 101);
		glXPutBitmap(139, 156, 106,101,rabbit_3_bmp);
		glBuffUnlock();	
		msDelay(80);

		glBuffLock();
		_glBlankRegion(0, 156, 320, 101);
		glXPutBitmap(114, 156, 106,101,rabbit_4_bmp);
		glBuffUnlock();	
		msDelay(70);

		glBuffLock();
		_glBlankRegion(0, 156, 320, 101);
		glXPutBitmap(89, 156, 106,101,rabbit_5_bmp);
		glBuffUnlock();
		msDelay(150);
	
		glBuffLock();
		_glBlankRegion(0, 156, 320, 101);
		glXPutBitmap(64, 156, 106,101,rabbit_6_bmp);
		glBuffUnlock();	
		msDelay(125);
			
		glBuffLock();
		_glBlankRegion(0, 156, 320, 101);
		glXPutBitmap(40, 156, 106,101,rabbit_6_bmp);
		glBuffUnlock();
		msDelay(100);
	
		glBuffLock();
		_glBlankRegion(0, 156, 320, 101);
		glXPutBitmap(30, 156, 106,101,rabbit_7_bmp);
		glBuffUnlock();
		msDelay(80);

		for(i=0; i < 180; i+=12)
		{			
			glBuffLock();
			_glBlankRegion(0, 156, 320, 101);
			glXPutBitmap(20-i,  156, 106,101,rabbit_0_bmp);
			glBuffUnlock();		
		}
	}
}


void main()
{
	auto unsigned long clippedBitMap, myBitMap ;
	auto int bitMapXS, bitMapYS, clippedMapXS;
	auto int i, j, k, y, loopcount;
	auto int maxXYS, minXYS, onOff;
	auto int polyV[NUM_VERTICES * 2];	// array of vertex coordinates	

	brdInit();
	glInit();
	glBackLight(1);
	glSetContrast(22);

	clippedBitMap = xalloc(BITMAP_SIZE);
	myBitMap		  = xalloc(BITMAP_SIZE);
	clippedMapXS = LCD_XS % BITMAP_XS;

	maxXYS = (LCD_XS > LCD_YS) ? LCD_XS : LCD_YS;	// maximum X or Y size
	minXYS = (LCD_XS < LCD_YS) ? LCD_XS : LCD_YS;	// minimum X or Y size

	loopcount = 0;
	while(1)
	{
		if(++loopcount%2)
		{
			BitMapDemo();
			msDelay(500);
			glBlankScreen();
		}
		// Set pixel brush type to XOR
		glSetBrushType(PIXXOR);

		// Display graphic "V" pattern
		for(k = 0; k < 2; k++)
		{
			polyV[0] = LCD_XS / 4;
			polyV[1] = LCD_YS / 4;
			polyV[2] = 0;
			polyV[3] = LCD_YS / 3;
			polyV[4] = LCD_XS / 3;
			polyV[5] = LCD_YS / 3;
			polyV[6] = LCD_XS / 3;
			polyV[7] = 0;
			
			glBuffLock();
			for(j = 0; j < minXYS - (minXYS / 3); j++)
			{
				glPlotVPolygon(NUM_VERTICES, polyV);
				for(i = 0; i < (sizeof(polyV) / sizeof(int)); i++)
				{
					(polyV[i])++;
				}
				if((j % 8) == 0)
				{
					glBuffUnlock();
					glBuffLock();
				}
			}
			glBuffUnlock();
		}
		
		// Display circle patterns
		for(k = 0; k < 2; k++)
		{
			glBuffLock();
			for(j = minXYS / 2 - 1; j >= 0; --j)
			{
				glPlotCircle(LCD_XS / 2, LCD_YS / 2, j);
				if((j % 4) == 0)
				{
					glBuffUnlock();
					glBuffLock();
				}
			}
			glBuffUnlock();
		}

		glBlankScreen();
		// Display multilevel graphic pattern built by primitive functions
		for(k = 0; k < 2; k++)
		{
			glBuffLock();
			for(j = 0; j < LCD_YS; j++)
			{
				glPlotLine(0, j, LCD_XS - 1, LCD_YS - 1 - j);
				if((j % 10) == 0)
				{
					glBuffUnlock();
					glBuffLock();
				}
			}
			glBuffUnlock();
			glBuffLock();
			for(i = LCD_XS - 1; i >= 0; --i)
			{
				glPlotLine(i, 0, LCD_XS - 1 - i, LCD_YS - 1);
				if((i % 10) == 0)
				{
					glBuffUnlock();
					glBuffLock();
				}
			}
			glBuffUnlock();
			if(k == 0)
			{
				glXGetBitmap((LCD_XS - BITMAP_XS) / 2, (LCD_YS - BITMAP_YS) / 2,
						        BITMAP_XS, BITMAP_YS, myBitMap);
				if(clippedMapXS > 0)
				{
					glXGetBitmap((LCD_XS - BITMAP_XS) / 2, (LCD_YS - BITMAP_YS) / 2,
							        clippedMapXS, BITMAP_YS, clippedBitMap);
				}
				glXPutBitmap((LCD_XS - BITMAP_XS) / 2, (LCD_YS - BITMAP_YS) / 2,
						        BITMAP_XS, BITMAP_YS, myBitMap);

				for(i = 15000; i; --i);
			
				glXPutBitmap((LCD_XS - BITMAP_XS) / 2, (LCD_YS - BITMAP_YS) / 2,
				              BITMAP_XS, BITMAP_YS, myBitMap);
			}
		}

		// Display bitmap that was read from the LCD display which was built
		// by the primitive graphic functions.
		for(k = 0; k < 2; k++)
		{
			for(j = 0; j < LCD_YS; j += BITMAP_YS)
			{
				for(i = 0; i < LCD_XS; i += BITMAP_XS)
				{
					bitMapXS = (BITMAP_XS < LCD_XS - i) ? BITMAP_XS : LCD_XS - i;
					bitMapYS = (BITMAP_YS < LCD_YS - j) ? BITMAP_YS : LCD_YS - j;
					if(bitMapXS < BITMAP_XS)
					{
						glXPutBitmap(i, j, bitMapXS, bitMapYS, clippedBitMap);
					}
					else
					{
						glXPutBitmap(i, j, bitMapXS, bitMapYS, myBitMap);
					}
				}
			}
		}

		glSetBrushType(PIXBLACK);	
		//	Draw Polygon pattern
		for (y = 1; y < 120-2; y+=2)
		{
			glPlotPolygon(4, 320-y, y,  y,y,   y, 240-y,   320-y, 240-y);
		}
		glSetBrushType(PIXWHITE);		
		for (y=117; y >80; y-=2)
		{
			glPlotPolygon(4, 320-y, y,  y,y,   y, 240-y,   320-y, 240-y);
		}
		glSetBrushType(PIXBLACK);
		for ( ; y < 120-2; y+=2)
		{
			glPlotPolygon(4, 320-y, y,  y,y,   y, 240-y,   320-y, 240-y);
		}
		glBlankScreen();
	
		
		//	Draw line pattern
		glSetBrushType(PIXBLACK);		
		for (i = 0; i < 40; ++i)
		{

			glBuffLock();
			glPlotLine(i*8,VSPACE,319,VSPACE+i*5);
			glPlotLine(319,VSPACE+i*5,319-i*8,VSPACE+199);
			glPlotLine(319-i*8,VSPACE+199,0,VSPACE+199-i*5);
			glPlotLine(0,VSPACE+199-i*5,i*8,VSPACE);
			glBuffUnlock();		
		}
	}
}
