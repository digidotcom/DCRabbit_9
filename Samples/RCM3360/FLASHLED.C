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
	flashled.c

	This program is used with RCM3300 series controllers
	and prototyping boards.

	Description
	===========
	This simple program demonstrates flashing	LED's, on and off,
   User LED on the RCM3300, DS3, DS4, DS5, DS6 on the prototyping
   board.


	Instructions
	============
	1.  Compile and run this program.
	2.  DS3, DS4, DS5, DS6 and User LED's will flash on/off.


*******************************************************************/
#class auto

#use rcm33xx.lib		//sample library to use with this application

#define DS3 3
#define DS4 4
#define DS5 5
#define DS6 6
#define USERLED 0
#define ON 1
#define OFF 0


///////////////////////////////////////////////////////////
/// provides milli-second delay
///////////////////////////////////////////////////////////
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
main()
{

	brdInit();  //must do to initialize board

	for(;;)
	{
		ledOut(DS3, ON);
      msDelay(100);
		ledOut(DS3, OFF);
      msDelay(50);

		ledOut(DS4, ON);
      msDelay(100);
		ledOut(DS4, OFF);
      msDelay(50);

		ledOut(DS5, ON);
      msDelay(100);
		ledOut(DS5, OFF);
      msDelay(50);

		ledOut(DS6, ON);
      msDelay(100);
		ledOut(DS6, OFF);
      msDelay(50);

		ledOut(USERLED, ON);
      msDelay(100);
		ledOut(USERLED, OFF);
      msDelay(50);
	}
}