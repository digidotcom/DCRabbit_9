/*******************************************************************
	flashled1.c
  	Z-World, 2002

	This program is used with RCM3100 series controllers
	with prototyping boards.
	
	The sample library, \Samples\RCM3100\rcm3100.lib, contains a
	brdInit() function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program uses cofunction and costatements to flash
	LED's, DS1 and DS2, on the prototyping board at different
	intervals.
	
	Instructions
	============
	1.  Compile and run this program.
	2.  DS1 and DS2 LED's flash on/off at different times.
*******************************************************************/

#use rcm3100.lib 	//sample library used for this demo

#define DS1 6		//port G bit 6
#define DS2 7		//port G bit 7

///////////////////////////////////////////////////////////
// DS1 led on protoboard is controlled by port G bit 6
// DS2 led on protoboard is controlled by port G bit 7
// turns oon when port bit is set to 0
///////////////////////////////////////////////////////////
void ledon(int led)
{
	BitWrPortI(PGDR, &PGDRShadow, 0, led);
}

///////////////////////////////////////////////////////////
// DS1 led on protoboard is controlled by port G bit 6
// DS2 led on protoboard is controlled by port G bit 7
// turns off when port bit is set to 1
///////////////////////////////////////////////////////////
void ledoff(int led)
{
	BitWrPortI(PGDR, &PGDRShadow, 1, led);	
}

///////////////////////////////////////////////////////////
// set DS1 and DS2 leds to stay on and off at intervals
///////////////////////////////////////////////////////////
cofunc flashled[2](int led, int ontime, int offtime)
{
	for(;;)
	{
		ledon(led);
		waitfor(DelayMs(ontime));
		ledoff(led);
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
				flashled[0](DS1, 50, 50);    	// Flashes DS1 on/off 50 ms
				flashled[1](DS2, 200, 100);   // Flashes DS2 on 200 ms, off 100 ms
			}
		}
		
	}
}
