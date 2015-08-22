/*******************************************************************

	toggleswitch.c
 	Z-World, 2003

	This program is used with RCM3700 series controllers
	with prototyping boards.

	Description
	===========
	This program uses costatements to detect switches with
	press and release method debouncing. Corresponding LED's,
	DS1 and DS2, will turn on or off.

	I/O control			On proto-board
	--------------		----------------------
	Port F bit 4		S1, switch
	Port B bit 7		S2, switch
	Port F bit 6		DS1, LED
	Port F bit 7		DS2, LED

	Instructions
	============
	1. Compile and run this program.
	2. Press and release S1 switch to toggle DS1 LED on/off.
	3. Press and release S2 switch to toggle DS2 LED on/off.
*******************************************************************/
#class auto

#define DS1 6		//led, port F bit 6
#define DS2 7		//led, port F bit 7
#define S1  4		//switch, port F bit 4
#define S2  7		//switch, port B bit 7

main()
{
	auto int sw1, sw2, led1, led2;

	brdInit();				//initialize board for this demo

	led1=led2=1;			//initialize leds to off value
	sw1=sw2=0;				//initialize switches to false value

	while (1)
	{
		costate
		{
			if (BitRdPortI(PFDR, S1))		//wait for switch S1 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (BitRdPortI(PFDR, S1))		//wait for switch release
			{
				sw1=!sw1;						//set valid switch
				abort;
			}
		}

		costate
		{
			if (BitRdPortI(PBDR, S2))		//wait for switch S2 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (BitRdPortI(PBDR, S2))		//wait for switch release
			{
				sw2=!sw2;						//set valid switch
				abort;
			}
		}

		costate
		{	// toggle DS1 led upon valid S1 press/release and clear switch
			if (sw1)
			{
				BitWrPortI(PFDR, &PFDRShadow, led1=led1?0:1, DS1);
				sw1=!sw1;
			}
		}

		costate
		{	// toggle DS2 upon valid S2 press/release and clear switch
			if (sw2)
			{
				BitWrPortI(PFDR, &PFDRShadow, led2=led2?0:1, DS2);
				sw2=!sw2;
			}
		}

	}
}