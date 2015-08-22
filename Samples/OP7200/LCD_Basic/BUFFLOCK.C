/***********************************************************
     
	bufflock.c

   Z-World, 2001
	This sample program is for the OP7200 series controllers.
	
   Demonstration of increased LCD performance with using
   glBuffLock and glBuffUnlock functions.


   Instructions
   ------------
	1. Compile and run program.
	2. View the LCD screen as it executes the demo.
	     		
************************************************************/
#class auto
#memmap xmem  // Required to reduce root memory usage 

//----------------------------------------------------------
// Structures, arrays, variables
//----------------------------------------------------------
fontInfo fi8x12, fi10x16, fi12x16;


#define BOXTOPLEFTX 	20   	//box upper left x-coordinate
#define BOXTOPLEFTY 	50		//box upper left y-coordinate
#define BOXWIDTH 		300	//box width
#define BOXHEIGHT 	180  	//box height

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 


//////////////////////////////////////////////////////////
// draws box outline
//////////////////////////////////////////////////////////
void drawBox(int x, int y, int w, int h)
{
 	glPlotLine(x, y, w, y);		//north line
 	glPlotLine(w, y, w, h);		//east line
 	glPlotLine(w, h, x, h);		//south line
 	glPlotLine(x, h, x, y);		//west line
}

//////////////////////////////////////////////////////////
// Pixel bitmap 
//////////////////////////////////////////////////////////

xdata demo_figure {
'\x0F','\xFF','\xF0',
'\x1F','\xFF','\xF8',
'\x3F','\xFF','\xFC',
'\x7F','\xFF','\xFE',
'\xEF','\xFF','\xFF',
'\xEF','\xFF','\xFF',
'\xEF','\xFF','\xFF',
'\xEF','\xFF','\xFF',
'\x7F','\xFF','\xFE',
'\x3F','\xFF','\xFC',
'\x1F','\xFF','\xF8',
'\x0F','\xFF','\xF0'
};



//////////////////////////////////////////////////////////
// Function to position and draw image for demo program   
//////////////////////////////////////////////////////////
nodebug
void buffLock_cntrl(int* dx, int* dy, int* px, int* py, int glbuff_cnt  )
{
	auto int nx,ny;		// New Position         
	auto int lock_cnt;
	
	for(lock_cnt = 0; lock_cnt < glbuff_cnt; lock_cnt++)
   {	
		// Try New Position
		nx = *px + *dx;               	     
		ny = *py + *dy;

		// Avoid Collision
		if (nx <= BOXTOPLEFTX || ((nx+24) >= BOXWIDTH))     
		{
			glXPutBitmap (*px,*py,24,12,demo_figure);
			*dx = -*dx;
		}
		if (ny <= BOXTOPLEFTY || ((ny+12) >= BOXHEIGHT))
		{
			glXPutBitmap (*px,*py,24,12,demo_figure);
			*dy = -*dy;
		}

		// Next Position 
		nx = *px + *dx;               	
		ny = *py + *dy;
      
		glXPutBitmap (*px,*py,24,12, demo_figure);
	
		// Move Ball
		*px = nx; *py = ny;
	}
}

//////////////////////////////////////////////////////////
// Display sign-on message on the LCD and run Demo.
//////////////////////////////////////////////////////////
nodebug
void demo( void )
{
	auto int	px,py;      // Current Position     
	auto int dx,dy;      // Current Direction    
	auto int lock_cnt;
	auto int viewloop;
	auto int iterations;

	glBuffLock();
	glBlankScreen();
	drawBox(BOXTOPLEFTX, BOXTOPLEFTY,
			  BOXWIDTH, BOXHEIGHT);
	glPrintf(20,10, &fi10x16,  "glBuffer lock/unlocking Demo");
	glPrintf(20,200, &fi10x16, "Increase lock/unlocking usage");
	glPrintf(20,216, &fi10x16, "to 0.");	  
	glBuffUnlock();
	
   px = BOXTOPLEFTX + 1;   py = BOXTOPLEFTY + 1;    
   // Give Direction
   dx = 2;   dy = 2;                 			 

   
  	for(iterations = 1; iterations < 30; iterations++)
  	{
  		for(viewloop=0; viewloop < 10; viewloop++)
		{
			glBuffLock();
			glPrintf(20,216, &fi10x16, "to %d.", iterations);	  
			// By increasing the number of iterations between
			// locking and unlocking the graphic buffer you can
			// get increased performance.
         //
         // Note: Functions glBuffLock() and glBuffUnlock() can be
         // nested up to a level of 255, but be sure to balance the
         // calls.
         //
			
  			buffLock_cntrl(&dx, &dy, &px, &py, iterations);
     		glBuffUnlock();
     	}
     	
   }
}

//////////////////////////////////////////////////////////

void main ()
{
	brdInit();

	glInit();			// Initialize the graphic driver
	glBackLight(1);
	glSetContrast(24);
	
	glXFontInit(		//	initialize the font info structure needed later
		&fi10x16,		//	pointer to the font info structure to fill
		10,				//	horizontal size (in pixels) of each character
		16,				//	vertical size (in pixels) of each character
		32,				//	beginning code of printable characters
		127,				//	ending code of printable characters
		Font10x16		//	address of font bitmap in xmem
	);

	for(;;)
	{
		demo();
	
	}
}
