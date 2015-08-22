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

	controlled.c

	This program is used with BL2500 series controllers
	with demo boards.
	
	Description
	===========
	This program uses costatements to vary the voltage output of
	DA0 and DA1 using pwmOut() and pwmOutVolts() functions.
		
	Connections
	===========
	Controller			Demo-board
	-----------			----------
	DA0  (J1-4)	<--->	LED1
	DA1  (J1-5)	<--->	LED2
	GND  (J1-9)	<--->	GND
	+3.3 (J1-1)	<--->	+K
	
	Instructions
	============
	1. Compile and run this program.
	2. LED1 and LED2 will vary in intensity as the outputs of
		DA0 and DA1 vary.
	
*******************************************************************/
#class auto

#define DA0 0
#define DA1 1


main()
{
	auto int count;
	auto float volt;

	
	brdInit();		//initializes PWM

	while (1)
	{
		costate		//DA0 varies by changing the count cycle
		{
			for (count=512; count>=0; count-=100)
			{
				pwmOut(DA0, count);					//led intensity increases
				waitfor(DelayMs(100));	
			}
			
			for (count=0; count<=512; count+=100)
			{
				pwmOut(DA0, count);					//led intensity decreases
				waitfor(DelayMs(100));	
			}
		}
		
		costate		//DA1 varies by changing the voltage
		{
			for (volt=0.0; volt<=1.5; volt+=0.1)
			{
				pwmOutVolts(DA1, volt);					//led intensity decreases
				waitfor(DelayMs(100));	
			}
			
			for (volt=1.5; volt>=0.0; volt-=0.1)
			{
				pwmOutVolts(DA1, volt);					//led intensity increases
				waitfor(DelayMs(100));	
			}
		}
	}
}
