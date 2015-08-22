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
	rd_touchscreen.c
	
	Sample program to demonstrate reading the touchscreen in debounced
	or realtime modes using low level functions.

	Note: When using button functions from the glTouchscreen library,  
	      the btnGet() function automatically handles the debouncing
	      and realtime operation of the touchscreen.
	           
	Instructions:
	1. Compile and run this program.
	2. Follow the instruction displayed in the STDIO window.

***************************************************************************/
#class auto


#define REALTIME '1'

void test_touchscreen( int mode )
{
	auto int x,y;
	auto long TouchscreenXY;

	if(mode == REALTIME)
	{
		// Check if the touchscreen is currently being pressed, if so
		// continuously read the touchscreen until the touchscreen is
		// no longer being activated. 
		while(TsActive())
		{
			while(1)
			{
				TsXYvector(&x, &y, CAL_MODE);
				printf("X = %d, Y = %d\n", x, y);
				if(!TsActive())
				{
					printf("Press 'Q' to select another menu option\n\n");
					break;
				}
			}
		}
	}
	else
	{
		// Take a snapshot of the current state of the touchscreen. 
		TsScanState();
		
		// Read the x,y coordinate buffer which was loaded by the
		// TsScanState() function.  
		TouchscreenXY = TsXYBuffer();
		if(TouchscreenXY != -1 && !(TouchscreenXY & BTNRELEASE))
		{
			x = (int) ((TouchscreenXY >> 16) & 0x00000FFF);
			y = (int) (TouchscreenXY & 0x00000FFF);		
			printf("X = %d, Y = %d\n", x, y);
			printf("Press 'Q' to select another menu option\n\n");
		}
	}
}

void main()
{
	int mode, key;

	brdInit();
	glInit();

	for(;;)
	{
   	printf("Touchscreen mode menu\n");
   	printf("1.Read touchscreen continuously when pressed(Realtime mode)\n");
   	printf("2.Read touchscreen single time when pressed(Debounced mode)\n\n");
   	printf("Select mode of operation (1 or 2) = ");   
		do
		{
			mode = getchar();
		} while (!( (mode >= '1') && (mode <= '2')) );
		printf("%c\n\n", mode);
		printf("Press touchscreen to display coordinate's\n\n");
   	do
   	{         
			if(mode == REALTIME)
				test_touchscreen(REALTIME);
			else
				test_touchscreen(!REALTIME);
			key = 0;
			if(kbhit())
			{
				key = getchar();
				while(kbhit()) getchar();
			}
		}while(key != 'q' && key != 'Q');
	}
}
