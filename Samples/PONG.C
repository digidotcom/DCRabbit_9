/***********************************************************

      Samples\PONG.C
      Z-World, 1999

      Demonstration of output to STDIO. Uses ANSI escape sequences.
      Appearance will vary with different character sets.
	   For best results the font should be set to "Terminal" for
	   the Stdio window

************************************************************/
#class auto

int	px,py;                        // Current Position
int   dx,dy;                        // Current Direction
int   nx,ny;                        // New Position

int	xl, xh,	yl, yh;

/********* Position Cursor **********/
nodebug
void gotoxy (int x, int y)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c",x,y );
}

/********* Clear Screen ******/
void cls ()
{
   printf ( " \x1Bt" );            	// Space Opens Window
}

void box(int x,int y, int w, int h)
{
	int i;
	char hor_line[100];

	//define horizontal border
	for( i = 0 ; i < w-1 ; i++)
	{
		hor_line[i] = 0xC4;
	}
	hor_line[i] = 0;

	//upper left corner
	gotoxy(x,y);

   printf ( "\xda" );

	//print top
	gotoxy(x + 1, y);
 	printf ( "%s", hor_line);

	//upper right corner
  	gotoxy(x + w, y);
   printf ( "\xbf" );

	//print sides
	for( i = 1 ; i < h ; i++)
	{
		gotoxy(x, y + i);
	   printf ( "\xB3" );
		gotoxy(x + w, y + i);
	   printf ( "\xB3" );
	}

	//lower left corner
	gotoxy(x,y + h);
	   printf ( "\xC0" );

	//bottom
	gotoxy(x + 1, y + h);
 	printf ( "%s", hor_line );

	//lower right corner
	gotoxy(x + w, y+h);
   printf ( "\xd9" );
}

void pong(){

   while (1)
   {
 		costate
      {
	      gotoxy ( px,py );
   	   printf ( "\x01" );
			waitfor(DelayMs(10));

	      nx = px + dx;               	// Try New Position
   	   ny = py + dy;

      	if (nx <= xl || nx >= xh)     // Avoid Collision
         	dx = -dx;
	      if (ny <= yl || ny >= yh)
   	      dy = -dy;

	      nx = px + dx;               	// Next Position
   	   ny = py + dy;

      	gotoxy ( px,py );
	      printf ( " " );
			waitfor(DelayMs(10));

   	   px = nx; py = ny;           	// Move Ball
      }
   }
}

///////////////////////////////////////////////////////////////////////

void main ()
{
   cls ();                         	// Clear Screen
	xl =  1;									// box coordinates
	xh = 25;
	yl =  4;
	yh = 12;

   px = xl + 1; py = yl +1;        	// Position Ball
   dx = 1; dy = 1;                 	// Give Direction
   gotoxy ( (xh-xl)/2+xl, yl-1 );  	// Position Cursor
   printf ( "Pong" );              	// Title
	box(xl, yl, xh - xl, yh - yl);
	pong();
}