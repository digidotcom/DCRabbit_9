/*******************************************************************
	flashleds.c
  	Z-World, 2002

	This program is used with BL2500 series controllers.
	
	Description
	===========
	This program uses cofunction and costatements to flash
	the controllers's LED's, DS1, DS2, DS3, and DS4, at
	different intervals.
	
	Instructions
	============
	1.  Compile and run this program.
	2.  LED's will flash on/off at different times.
*******************************************************************/

#class auto

#define ON 1
#define OFF 0
#define DS1 0
#define DS2 1
#define DS3 2
#define DS4 3

///////////////////////////////////////////////////////////
// set DS1, DS2, DS3, DS4 leds to stay on and off at intervals
///////////////////////////////////////////////////////////
cofunc flashled[4](int led, int ontime, int offtime)
{
	for(;;)
	{
		ledOut(led, ON);
		waitfor(DelayMs(ontime));
		ledOut(led, OFF);
		waitfor(DelayMs(offtime));
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
main()
{
	brdInit();				//initialize board for this demo
	
	for(;;)
	{
		costate
		{
			wfd
			{
				flashled[0](DS1, 50, 100);   	// Flashes DS1 on 50 ms, off 100 ms
				flashled[1](DS2, 50, 200);   	// Flashes DS2 on 50 ms, off 200 ms
				flashled[2](DS3, 50, 400);   	// Flashes DS3 on 50 ms, off 400 ms
				flashled[3](DS4, 500, 200);   // Flashes DS4 on 500 ms, off 200 ms
			}
		}
		
	}
}
