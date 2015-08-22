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
/*****************************************************

     Samples\RCM2100\toggleled.c

     Demonstration of using the RCM 2100 Prototyping board and costatements

     This program will flash DS3 on the RCM 2100 Prototyping Board once per
     second (bit 2 on port A).  This program will also watch button S1 (port B
     bit 2) and toggle LED DS2 (port A bit 1) on/off when pressed.  Note that
     the button is debounced by the software.

     Port A can be set for all outputs or all inputs via the slave port control
     register (SPCTRL).  If the slave port is being used then port A should not
     be used.

     Port B bits 0-5 are always inputs, and bits 6-7 are always outputs.  If the
     slave port is being used then port B should not be used.

******************************************************/
#class auto


void main()
{
	int vswitch;		// state of virtual switch controlled by button S1

   WrPortI(SPCR, &SPCRShadow, 0x84);	// setup parallel port A as output
   WrPortI(PADR, &PADRShadow, 0xff);		// turn off all LED's
   

   vswitch=0;				   // initialize virtual switch as off
       
	while (1) {					// endless loop
	
     // First task will flash LED DS4 for 200ms once per second.
     costate {    
       BitWrPortI(PADR, &PADRShadow, 0, 1);		// turn LED on
       waitfor(DelayMs(200));							// wait 200 milliseconds
       BitWrPortI(PADR, &PADRShadow, 1, 1);		// turn LED off
       waitfor(DelayMs(800)); 						// wait 800 milliseconds
     }

          
     // Second task will debounce switch S1 and toggle virtual switch vswitch.
     // also check button 1 and toggle vswitch on or off
     costate {
         if (BitRdPortI(PBDR, 2))
         	abort;				// if button not down skip out of costatement

         waitfor(DelayMs(50));		// wait 50 ms

         if(BitRdPortI(PBDR,2))
         	abort;				// if button not still down skip out
         
         vswitch = !vswitch;	// toggle virtual switch since button was down 50 ms
  
         // now wait for the button to be up for at least 200 ms before considering another toggle
         while (1) {
           waitfor(BitRdPortI(PBDR, 2));		// wait for button to go up
           waitfor(DelayMs(200));				// wait additional 200 milliseconds
           if (BitRdPortI(PBDR,2))
           		break;				// if button still up break out of while loop
			}
      } // end of costate
               
		// make led agree with vswitch if vswitch has changed
		if( (PADRShadow & 1) == vswitch)
			BitWrPortI(PADR, &PADRShadow, !vswitch, 0);
     
	}  // end of while loop

}  // end of main
