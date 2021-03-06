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
/*******************************************************************
	flashled2.c

	This program is used with RCM3400 series controllers
	with prototyping boards.
	
	Description
	===========
	This program uses cofunction and costatements to flash
	LED's, DS1 and DS2, on the prototyping board at different
	intervals.
	
	Output control		LED on the proto-board
	--------------		----------------------
	Port D bit 6		DS1
	Port D bit 7		DS2
	
	Instructions
	============
	1.  Compile and run this program.
	2.  DS1 and DS2 LED's flash on/off at different times.
*******************************************************************/
#class auto

#define DS1 6		//port G bit 6
#define DS2 7		//port G bit 7

///////////////////////////////////////////////////////////
// DS1 led on protoboard is controlled by port D bit 6
// DS2 led on protoboard is controlled by port D bit 7
// turns oon when port bit is set to 0
///////////////////////////////////////////////////////////
void pbledon(int led)
{
	BitWrPortI(PDDR, &PDDRShadow, 0, led);
}

///////////////////////////////////////////////////////////
// DS1 led on protoboard is controlled by port D bit 6
// DS2 led on protoboard is controlled by port D bit 7
// turns off when port bit is set to 1
///////////////////////////////////////////////////////////
void pbledoff(int led)
{
	BitWrPortI(PDDR, &PDDRShadow, 1, led);	
}

///////////////////////////////////////////////////////////
// set DS1 and DS2 leds to stay on and off at intervals
///////////////////////////////////////////////////////////
cofunc flashled[2](int led, int ontime, int offtime)
{
	for(;;)
	{
		pbledon(led);
		waitfor(DelayMs(ontime));
		pbledoff(led);
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
