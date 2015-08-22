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

	toggleswitch.c

	This program is used with RCM3300 series controllers and
	prototyping boards.

	Description
	===========
	This program uses costatements to detect switches with
	press and release method debouncing. Corresponding LED's,
	DS2 and DS3, will turn on or off.

	Instructions
	============
	1. Compile and run this program.
	2. Press and release S2 switch to toggle DS3 LED on/off.
	3. Press and release S3 switch to toggle DS4 LED on/off.

*******************************************************************/
#class auto

#use rcm33xx.lib		//sample library to use with this application

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6

#define S2  2
#define S3  3
#define ON 1
#define OFF 0

main()
{
	auto int sw1, sw2, led1, led2;

	brdInit();				//initialize board for this demo

	led1=led2=OFF;			//initialize leds to off value
	sw1=sw2=0;				//initialize switches to false value

	while (1)
	{
		costate
		{	//wait for switch S2 press
			if (switchIn(S2))
				abort;
			waitfor(DelayMs(25));	//switch press detected if got to here
			if (switchIn(S2))			//wait for switch release
			{
				sw1=!sw1;				//set valid switch
				abort;
			}
		}

		costate
		{	//	wait for switch S3 press
			if (switchIn(S3))
				abort;
			waitfor(DelayMs(25));	//switch press detected if got to here
			if (switchIn(S3))			//wait for switch release
			{
				sw2=!sw2;				//set valid switch
				abort;
			}
		}

		costate
		{	// toggle DS3 led upon valid S2 press/release and clear switch
			if (sw1)
			{
         	ledOut(DS3, led1=led1?0:1);
				sw1=!sw1;
			}
		}

		costate
		{	// toggle DS4 upon valid S3 press/release and clear switch
			if (sw2)
			{
         	ledOut(DS4, led2=led2?0:1);
				sw2=!sw2;
			}
		}

	}
}