/*****************************************************

     Samples/RCM2200/toggleled.c
     Z-World, 2001

     Demonstration of using the Rabbit 2200 Micro Core Module Prototyping board
     and costatements

     This program will flash DS2 on the RCM 2200 Prototyping Board once per
     second (bit 1 on port E).  This program will also watch button S3 (port B
     bit 3) and toggle LED DS3 (port E bit 7) on/off when pressed.  Note that
     the button is debounced by the software.

     Port A can be set for all outputs or all inputs via the slave port control
     register (SPCTRL).  If the slave port is being used then port A should not
     be used.

     Port B bits 0-5 are always inputs, and bits 6-7 are always outputs.  If the
     slave port is being used then port B should not be used.

     Port E bits 1,7 are always I/O, and bits 0,2-6 are left as unchanged,
     available for external I/O control.

******************************************************/
#class auto

void main()
{
	int  vswitch;		// state of virtual switch controlled by button S1

	/*  1. Convert the I/O ports.  Disable slave port which makes
	 *     Port A an output, and PORT E not have SCS signal.
	 */
	WrPortI(SPCR, & SPCRShadow, 0x84);

	/*  2. Read function shadow and set PE1 and PE7 as normal I/O.
	 *     LED's are conencted to PE1 and PE7, make them outputs.
	 *     Using shadow register preserves function of other Port E bits.
	 */
	WrPortI(PEFR,  & PEFRShadow,  ~((1<<7)|(1<<1)) & PEFRShadow);
	WrPortI(PEDDR, & PEDDRShadow, (1<<7)|(1<<1));

	/*  3. Turn on DS2 (0 -> PE1) and turn off DS3 (1 -> PE7).
	 */
	WrPortI(PEDR, & PEDRShadow, (1<<7));

	/*  4. Initialize virtual switch as off (since DS3 is initially off).
	 */
   vswitch=0;

   for(;;) {					// endless loop
	
     // First task will flash LED DS2 for 200ms once per second.
     costate {    
       
		 WrPortI(PEB1R, NULL, 0);      			// Control DS3. Output 0 to turn LED on.
       waitfor(DelayMs(200));						// wait 200 milliseconds
       WrPortI(PEB1R, NULL, 1<<1);				// turn LED off
       waitfor(DelayMs(800)); 					// wait 800 milliseconds
     }

          
     // Second task will debounce switch S3 and toggle virtual switch "vswitch."
     costate {
         if (BitRdPortI(PBDR, 3))
         	abort;				// if button not down skip out of costatement

         waitfor(DelayMs(50));		// wait 50 ms

         if(BitRdPortI(PBDR,3))
         	abort;				// if button not still down skip out
         
         vswitch = !vswitch;	// toggle virtual switch since button was down 50 ms
  
         // now wait for the button to be up for at least 200 ms before considering another toggle
         while (1) {
           waitfor(BitRdPortI(PBDR, 3));		// wait for button to go up
           waitfor(DelayMs(200));				// wait additional 200 milliseconds
           if (BitRdPortI(PBDR,3))
           		break;				// if button still up break out of while loop
			}
      } // end of costate
               
		//  Make LED agree with vswitch if vswitch has changed.
		//  Normally we'd check a shadow register.  But since PEDR
		//  allows for bitwise programming (via PEB7R), we just
		//  read the PEDR to find current state and mask out the
		//  bit we want.  In fact, there is no pre-defined shadow
		//  variable for PEB7R.
		if( ((RdPortI(PEDR) & (1<<7)) >> 7) == vswitch) {
//			printf("change..\n");
			WrPortI(PEB7R, NULL, (vswitch) ? 0 : (1<<7));
		}
     
	}  // end of while loop

}  // end of main
