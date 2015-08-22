/**************************************************************************

	power.c
   Z-World, 2002

	This sample program is intended for LP3500 series controllers.

	Description
	===========
	This program demonstrates switching normal power source and external
	battery using the provided DEMO board in your kit.  Pressing a switch
	will change from from most power consumption to the least and is displayed
	by flashing LED's.

	Connections
	===========

	When the controller is plugged into to the demo board the
	following connections are readily available.

	Controller			Demo Board
	----------			----------
			IN00 	<-->	S1
			OUT0 	<-->	DS1
			OUT1 	<-->	DS2
			OUT2 	<-->	DS3
				+K	<-->  Vcc

	Connect a battery or +3V source to VBAT EXT and GND.


	Instructions
	============
	1. Press F5 to compile this program.
	2. Press ALT-F9 to to run without polling. Remove the programming cable.
		(It consumes approximately 10 mAmps).
	3. DS1 should blink.
	4.	Press the demo board switch S1 and DS2 should light. The	board is in
		the lowest power mode, 10, and using the external battery.
	5. Press and hold the demo board switch S1 once again and DS3 should
		blink to	indicate that the board is running from normal power source
		and at maximum power mode.  A 30 second timeout will blink DS3 if S1
		is not pressed.
	6. To start program again, press reset button S2 on the controller.
	7. Try NORMALPOWER instead of BATTERY in digInAlert(). Leave the
		programming	cable on and ignore the	target communication message
		pop-up. Messages will still be displayed.

**************************************************************************/
#class auto				// Change default storage for local variables to "auto"

#define DS1 0
#define DS2 1
#define DS3 2
#define S1	0
#define LOPOWER 10
#define MAXPOWER 1
#define BATTERY 1
#define NORMALPOWER 0

void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

void main()
{
	auto int i;

	brdInit();

	printf ("Watch LED DS1 flash then press S1\n");
	while (digIn(S1)==1)
	{
		digOut(DS1,0);			//on
		msDelay(200);
		digOut(DS1,1);			//off
		msDelay(200);
	}
	digOut(DS1,0);				//on

	printf ("When LED DS2 turns on, press S1 again\n");
	digOut(DS2,0);							//on
	devPowerSet(ALLDEVICES, 0);		//deactivate all devices

	// detect with S1 on zero input, lowest to max power mode,
	// timeout 30 sec if no input detected
	//digInAlert(S1, 0, LOPOWER, MAXPOWER, NORMALPOWER, 30);
	digInAlert(S1, 0, LOPOWER, MAXPOWER, BATTERY, 30);

	if (rdPowerState() == NORMALPOWER)
		printf ("We're back!  DS3 flashes. ");
	devPowerSet(ALLDEVICES, 1);		//activate all devices

	while (1)
	{
		digOut(DS3,0);
		msDelay(200);
		digOut(DS3,1);
		msDelay(200);
	}
}