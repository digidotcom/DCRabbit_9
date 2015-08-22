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
/**************************************************************************
	cal_touchscreen.c

	This program demonstrates how to recalibrate the touchscreen by
	using minimum and maximum points on the touchscreen to generate
	x,y calibration coefficients, offset and gain.

	This program also display's the x,y coordinate's in realtime or
	debounced mode which is selectable by the user.

	Note: When using button functions from the glTouchscreen library,
	      the btnGet() function automatically handles the debouncing
	      and realtime operation of the touchscreen.

	!!! Caution !!!
	1. This program will overwrite the calibration constants set at
	   the factory.
	2. This program must be compiled to Flash.

	Instructions:
	1. Compile and run this program.
	2. Follow the instruction displayed in the STDIO window.

***************************************************************************/
#class auto


#define REALTIME '1'

test_touchscreen( int mode )
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
	int x1, x2, y1, y2;
	int mode, key, valid;

	brdInit();
	glInit();
	glBackLight(1);
	glSetContrast(22);

	glPlotCircle(317, 237, 3);
	glFillCircle(317, 237, 3);
	while(1)
	{
		printf("!!!Caution this will overwrite the calibration constants set at the factory.\n");
		printf("Do you want to continue(Y/N)? ");

		while(!kbhit());
		key = getchar();
		if(key == 'Y' || key == 'y')
		{
			printf("%c\n\n", key);
			break;
		}
		else if(key == 'N' || key == 'n')
		{
			printf("%c\n\n", key);
			exit(0);
		}

	}

	valid = FALSE;
	glPlotCircle(3, 3, 3);
	glFillCircle(3, 3, 3);
	while(!valid)
	{
		printf("Locate the small dot displayed on the LCD in the upper left-hand corner of\n");
		printf("the touchscreen. While pressing the touchscreen where the dot is located,\n");
		printf("press any key on the PC keyboard to read in the minimum x,y coordinate's.\n");
		printf("(Both coordinate's must be less than 500)\n\n");

		while(!kbhit());
		while(kbhit()) key = getchar();

		TsXYvector(&x1, &y1, RAW_MODE);
		printf("x1 = %d, y1 = %d\n\n", x1, y1);
		if(x1 < TSCAL_MINIMUM && y1 < TSCAL_MINIMUM)
			valid = TRUE;
	}

	valid = FALSE;
	glPlotCircle(234, 314, 3);
	glFillCircle(234, 314, 3);
	while(!valid)
	{
		printf("Locate the small dot displayed on the LCD in the lower right-hand corner\n");
		printf("of the touchscreen. While pressing the touchscreen where the dot is located,\n");
		printf("press any key on the PC keyboard to read in the maximum x,y coordinate's.\n");
		printf("(Both coordinate's must be greater than 3200)\n\n");
		while(!kbhit());
		while(kbhit()) key = getchar();
		TsXYvector(&x2, &y2, RAW_MODE);
		printf("x2 = %d, y2 = %d\n\n", x2, y2);
		if(x2 > TSCAL_MAXIMUM && y2 > TSCAL_MAXIMUM)
			valid = TRUE;
	}

	if(TsCalib(x1, y1, x2, y2) || TsCalibEEWr() || TsCalibEERd())
	{
   	printf("Calibration error occurred!  Exiting...\n\n");
   	exit(1);
	}
   printf("Calibration constants successfully written to Flash\n\n");
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