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
/***************************************************************

   Samples\RCM2100\FlashLEDS2.c

   RCM 2100 Demo board example program

   This program flashes the DS2 and DS3 LED's on the RCM
   2100 Prototyping board (PA0 and PA1 ports on the RCM 2100).
   It also demonstrates coding with cofunctions and costatements.

****************************************************************/

//  Normally local vars are "static", but ask Dynamic C to make them "auto"
#class auto

//defines for the leds
#define DS2 0
#define DS3 1

// This cofunction flashes the specified led on for ontime, then off for
//  offtime, a total of flashcount times.  The additional delay at the end
//  provides a distinguishable gap in the flash pattern.
cofunc flashled[2](int led, int ontime, int offtime, int flashcount)
{
	int i;
	
	for(i = flashcount; i; i--) {
		WrPortI(PADR, &PADRShadow, ((1<<led)^0xFF)&RdPortI(PADR));	//led on
		waitfor(DelayMs(ontime));												//on delay
		WrPortI(PADR, &PADRShadow, (1<<led)|PADRShadow);				//led off
		waitfor(DelayMs(offtime));												//off delay
	}
	waitfor(DelayMs(offtime*2));												//gap delay
}

void main()
{
	//initialize ports
	WrPortI(SPCR, &SPCRShadow, 0x84);		//set port a all outputs, leds on
	WrPortI(PEFR, &PEFRShadow, 0x00);		//set port e normal i/o
	WrPortI(PEDDR, &PEDDRShadow, 0x01);		//set port e bits 7..1 inputs, 0 output
	WrPortI(PECR, &PECRShadow, 0x00);		//set transfer clock as pclk/2
	
	for(;;) {										//run forever
		costate {									//start costatement
			// Note that even though the wfd starts both cofunctions running
			//  nearly simultaneously (cooperatively interleaved), it doesn't exit
			//  until both are finished.  Thus, the shorter duration cofunction
			//  finishes first and is then bypassed until the wfd is restarted.
			wfd {										//use wfd (waitfordone) with cofunctions
				//shorter duration cofunction
				flashled[0](DS2,200,200,5);	//flash DS2 on 200ms, off 200ms (5 times)
				//longer duration cofunction
				flashled[1](DS3,400,400,5);	//flash DS3 on 400ms, off 400ms (5 times)
			}
		}												//end costatment
	}													//end for loop
}
