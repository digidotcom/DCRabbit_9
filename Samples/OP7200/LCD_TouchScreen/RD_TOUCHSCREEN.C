/***************************************************************************
	rd_touchscreen.c
	
	Z-World, 2002
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
