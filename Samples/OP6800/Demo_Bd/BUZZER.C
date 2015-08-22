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
/**************************************************************************

	buzzer.c

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
