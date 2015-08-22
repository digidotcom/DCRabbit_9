/***********************************************************

      uCOSPong.c
      Z-World, 2000

      Demonstration of uC/OS-II.  This application has
      seven tasks.  The highest priority task reads the
      real time clock and updates the STDIO window every
      second.  The other tasks run instances of pong.
      Appearance will vary with different character sets.
	   For best results the font should be set to "Terminal" for
	   the STDIO window

See Newv251.pdf and Relv251.pdf in the samples/ucos-II directory for
information from Jean J. Labrosse pertaining to version 2.51 of uCOS-II.

************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

#define OS_MAX_TASKS				7
#use "ucos2.lib"

int pong(int px, int py, int dx, int dy, int xl, int xh, int yl, int yh);
int box(int x,int y, int w, int h);
void TimeTask (void* pdata);
void PongTask1(void* pdata);
void PongTask2(void* pdata);

void DispStr(int x, int y, char *s);

void main()
{
	auto INT8U Error;
	auto INT16U Ticks;

	// Initialize uC/OS-II internal data structures
	OSInit();

   // Create system tasks. If an error occurred during
   // task creation, exit with the exit code returned
   // from OSTaskCreate
   Error = OSTaskCreate(TimeTask,  NULL, 512, 0);
   if (Error != OS_NO_ERR)
   	exit(Error);
   Error = OSTaskCreate(PongTask1, NULL, 512, 1);
   if (Error != OS_NO_ERR)
   	exit(Error);
   Error = OSTaskCreate(PongTask1, NULL, 512, 2);
   if (Error != OS_NO_ERR)
   	exit(Error);
   Error = OSTaskCreate(PongTask1, NULL, 512, 3);
   if (Error != OS_NO_ERR)
   	exit(Error);
   Error = OSTaskCreate(PongTask2, NULL, 512, 4);
   if (Error != OS_NO_ERR)
   	exit(Error);
   Error = OSTaskCreate(PongTask2, NULL, 512, 5);
   if (Error != OS_NO_ERR)
   	exit(Error);
   Error = OSTaskCreate(PongTask2, NULL, 512, 6);
   if (Error != OS_NO_ERR)
   	exit(Error);

   // Begin muli-tasking by entering the first ready task
	OSStart();
}

/*****************************************************

		Adapted from rtc_test.c
	 	Z-World, 1999

		Read the real time clock and constantly update
		the STDIO window.  If the real time clock has
		not been set, it can be set using the sample
		program setrtckb.c in the samples\rtclock
		folder.

******************************************************/

// names of days of week
const char dayname[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

// function prototype for print_time function
void print_timexy(int x, int y, unsigned long thetime);

void TimeTask(void *pdata)
{
	auto unsigned int	i;
	auto struct tm		rtc;					// time struct
	auto unsigned long	t0, t1, t2;		// used for date->seconds conversion
	auto INT8U Error;

	while(1)
	{
		// read current date/time via tm_rd
		tm_rd(&rtc);							// get time in struct tm
		t0 = mktime(&rtc);					// convert struct tm into seconds since 1980

		print_timexy(27, 14, t0);

		// Update display every 1/4 second.
		OSTimeDly(OS_TICKS_PER_SEC / 4);
	}
}

// prints out date and time handed to it in tm struct
void print_timexy(int x, int y, unsigned long thetime)
{
	auto struct tm	thetm;
   static char s[25];
	mktm(&thetm, thetime);

	sprintf(s, "%s %02d/%02d/%04d %02d:%02d:%02d",
			dayname[thetm.tm_wday],
			thetm.tm_mon, thetm.tm_mday, 1900+thetm.tm_year,
			thetm.tm_hour, thetm.tm_min, thetm.tm_sec);
	DispStr(x, y, s);
}


/***********************************************************

      Adapted from PONG.C
      Z-World, 1999

		Demonstration of output to STDIO.
      Appearance will vary with different character sets.
	   For best results the font should be set to "Terminal" for
	   the Stdio window

************************************************************/

void PongTask1(void* pdata)
{
	auto int	xl, xh, yl, yh;
 	auto int	px,py;										// Current Position
	auto int dx,dy;										// Current Direction
   auto INT8U Error;

	xl =  1;													// box coordinates
	xh = 24;
	yl =  4;
	yh = 11;
	px = (int)(rand() * 24);
	if(px <= xl)
		px = xl + 1;

	py = (int)(rand() * 11);
	if(py <= yl)
		py = yl + 1;

	dx = ((int)(rand() * 2) & 0x01) ? -1 : 1;
	dy = ((int)(rand() * 2) & 0x01) ? -1 : 1;		// Give Direction

  	DispStr (((xh-xl)/2+xl) - (strlen("Pong 1")/2), yl-1, "Pong 1" ); // Position Cursor

  	box(xl, yl, xh - xl, yh - yl);
	pong(px, py, dx, dy, xl, xh, yl, yh);
}

void PongTask2(void* pdata)
{
   auto INT8U Error;
	auto int	xl, xh,	yl, yh;
	auto int	px,py;										// Current Position
	auto int   dx,dy;										// Current Direction

	xl = 26;													// box coordinates
	xh = 49;
	yl =  4;
	yh = 11;
	px = (int)(rand() * 49);
	if(px <= xl)
		px = xl + 1;

	py = (int)(rand() * 11);
	if(py <= yl)
		py = yl + 1;

	dx = ((int)(rand() * 2) & 0x01) ? -1 : 1;
	dy = ((int)(rand() * 2) & 0x01) ? -1 : 1;		// Give Direction

  	DispStr (((xh-xl)/2+xl) - (strlen("Pong 2")/2), yl-1, "Pong 2" ); // Position Cursor

	box(xl, yl, xh - xl, yh - yl);
	pong(px, py, dx, dy, xl, xh, yl, yh);
}

box(int x,int y, int w, int h)
{
	auto int i;
	auto char hor_line[100];
	auto INT8U Error;

	//define horizontal border
	for( i = 0 ; i < w-1 ; i++)
	{
		hor_line[i] = 0xC4;
	}
	hor_line[i] = 0;

	//upper left corner
	DispStr(x, y, "\xda");

	//print top
	DispStr(x + 1, y, hor_line);

	//upper right corner
  	DispStr(x + w, y, "\xbf");

	//print sides
	for( i = 1 ; i < h ; i++)
	{
		DispStr(x, y + i, "\xB3");
		DispStr(x + w, y + i, "\xB3");
	}

	//lower left corner
	DispStr(x,y + h, "\xC0");

	//bottom
	DispStr(x + 1, y + h, hor_line);

	//lower right corner
	DispStr(x + w, y+h, "\xd9");
}

pong(int px, int py, int dx, int dy, int xl, int xh, int yl, int yh)
{
	auto int nx,ny;            		// New Position
	auto INT8U Error;

   while (1)
   {
		hitwd();
      DispStr ( px, py, "\x01" );
      OSTimeDly(1);

      nx = px + dx;               	// Try New Position
      ny = py + dy;

      if (nx <= xl || nx >= xh)     // Avoid Collision
         dx = -dx;
      if (ny <= yl || ny >= yh)
         dy = -dy;

      OSTimeDly(1);
      nx = px + dx;               	// Next Position
      ny = py + dy;

      DispStr ( px, py, " ");
      OSTimeDly(1);

      px = nx; py = ny;           	// Move Ball
   }
}


/********* Position Cursor **********/
nodebug void DispStr (int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}