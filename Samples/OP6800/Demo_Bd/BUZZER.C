/**************************************************************************

	buzzer.c

   Z-World, 2001
	This sample program is for the OP6800 series controllers.
	
	This program demonstrates the control of the buzzer on the OP6800
	Demo Board.

	OP6800 Demo Board Setup
	-----------------------
	1. Move the jumper on JP1 from 2-3 to 1-2 to enable the buzzer. This
	   jumper should be moved back to pins 2-3 when running all other
	   sample programs.

	2. Connect a +12v power source to the OP6800 Demo Board connector
	   J3/J5 as follows:

	   PS     	OP6800 Demo Board
	   --			-----------------
	   +12v		+RAW Terminal  
	   GND      GND Terminal

	Note: The Buzzer is mono-tone, however by toggling the control bit
	      faster or slower, some pitch and motor boat affects can be
	      obtained.
	         
**************************************************************************/
#class auto		// Change compiler default to local vars allocated on stack.
#memmap xmem  // Required to reduce root memory usage

main()
{
	auto int i, delay, loop;
	
	brdInit();

	// Main loop to control the speaker
	for(loop = 0; loop < 2; loop++)
	{
		// Generate 3 long beeps
		for(i=0; i < 3; i++)
		{
			for(delay = 0; delay < 8000; delay++);
			digOut(0, 0);
			for(delay = 0; delay < 8000; delay++);
			digOut(0, 1);
		
		}

		// Make the speaker go up in pitch
		for(i=0; i < 800; i++)
		{
			for(delay = 800; delay > i; delay--);
			digOut(0, 0);
			for(delay = 800; delay > i; delay--);
			digOut(0, 1);
		}

		// Make the speaker go down in pitch
		for(i=0; i < 1000; i++)
		{
			for(delay = 0; delay < i; delay++);
			digOut(0, 0);
			for(delay = 0; delay < i; delay++);
			digOut(0, 1);
		}
	}
}
