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

	This program is used with BL2500 series controllers
	with demo boards.
	
	Description
	===========
	This program uses costatements to detect switches with
	press and release method debouncing. Corresponding LED's,
	DS1, DS2, DS3, DS4, will turn on or off.

		
	Connections
	===========
	Make the following connections:
	
	Digital input		Demo-board
	--------------		----------------------
  							Jumper H2 pins 3-5						
  							Jumper H2 pins 4-6						
	IN00		<----->	SW1
	IN01		<----->	SW2
	IN02		<----->	SW3
	IN03		<----->	SW4
 J11 pin 9  <----->  GND
  J7 pin 3  <----->  +K (not to exceed 18V)

  								
	Instructions
	============
	1. Compile and run this program.
	2. Press and release SW1 switch to toggle DS1 LED on/off.
	3. Press and release SW2 switch to toggle DS2 LED on/off.
	4. Press and release SW3 switch to toggle DS3 LED on/off.
	5. Press and release SW4 switch to toggle DS4 LED on/off.
	
*******************************************************************/
#class auto

#define IN00 0
#define IN01 1
#define IN02 2
#define IN03 3
#define DS1 0
#define DS2 1
#define DS3 2
#define DS4 3

main()
{
	auto int sw1, sw2, sw3, sw4, led1, led2, led3, led4;
	
	brdInit();				//initialize board for this demo

	led1=led2=0;			//initialize leds to off value 
	led3=led4=0;			//initialize leds to off value 
	sw1=sw2=0;				//initialize switches to false value 
	sw3=sw4=0;				//initialize switches to false value 

	while (1)
	{
		costate
		{	
			if (digIn(IN00))					//wait for switch S1 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (digIn(IN00))					//wait for switch release
			{
				sw1=!sw1;						//set valid switch
				abort;
			}
		}
		
		costate
		{	
			if (digIn(IN01))					//wait for switch S2 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (digIn(IN01))					//wait for switch release
			{
				sw2=!sw2;						//set valid switch
				abort;
			}
		}
		
		costate
		{	
			if (digIn(IN02))					//wait for switch S3 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (digIn(IN02))					//wait for switch release
			{
				sw3=!sw3;						//set valid switch
				abort;
			}
		}

		costate
		{	
			if (digIn(IN03))					//wait for switch S4 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (digIn(IN03))					//wait for switch release
			{
				sw4=!sw4;						//set valid switch
				abort;
			}
		}

		costate
		{	// toggle DS1 led upon valid S1 press/release and clear switch
			if (sw1)
			{
				ledOut(DS1, led1=led1?0:1);
				sw1=!sw1;
			}
		}
		
		costate
		{	// toggle DS2 led upon valid S2 press/release and clear switch
			if (sw2)
			{
				ledOut(DS2, led2=led2?0:1);
				sw2=!sw2;
			}
		}
		
		costate
		{	// toggle DS3 led upon valid S3 press/release and clear switch
			if (sw3)
			{
				ledOut(DS3, led3=led3?0:1);
				sw3=!sw3;
			}
		}
		
		costate
		{	// toggle DS4 led upon valid S4 press/release and clear switch
			if (sw4)
			{
				ledOut(DS4, led4=led4?0:1);
				sw4=!sw4;
			}
		}
		

	}
}
