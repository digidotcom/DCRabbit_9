/*******************************************************************
	flashled2.c
  	Z-World, 2002

	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	The sample library, \Samples\RCM3000\rcm3000.lib, contains a
	brdInit() function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program uses cofunction and costatements to flash
	LED's, DS1 and DS2, on the prototyping board at different
	intervals.  The LEDs are access by the "master" module.
	
	Instructions
	============
	1.  Compile and run this program.
	2.  DS1 and DS2 LED's flash on/off at different times.
*******************************************************************/
#class auto


#use rcm3000.lib 	//sample library used for this demo

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
	ledon(led);
	waitfor(DelayMs(ontime));
	ledoff(led);
	waitfor(DelayMs(offtime));
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void main()
{
	brdInit();				//initialize board for this demo
	
	for(;;) {									// run forever
		// Two costatements, each with their own wfd containing a flashled[]
		//  indexed cofunction instance, keep the flash timing of each LED
		//  independent of the other.  If both cofunction calls were within
		//  either one or two wfds in a single costatement then the LEDs' flash
		//  timings would not be independent of each other.
		costate {								// start 1st costatement
			wfd {
				// 50 ms on/off
				flashled[0](DS1,50,50);		// flashes DS1 (PG6)
			}
		}											// end 1st costatement
		costate {								// start 2nd costatement
			wfd {
				// 200 ms on/100 ms off
				flashled[1](DS2,200,100);	// flashes DS2 (PG7)
			}
		}											// end 2nd costatement
	}												// end for loop
}
