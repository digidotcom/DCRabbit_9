/**********************************************************************

   demobrd3.c
   Z-World, 2000

 	This sample program is used with products such as
	Intellicom Series and TCP/IP Development Board with Z-World
	Demo Board.

   This program demonstrates use of costatements.

   Connect board outputs O0 and O1 to Demo Board LED's.  Connect
   input I0 to Demo Board switch.

  	This program will repeatedly flash an LED attached to output O1.
   This program will also toggle an LED attached to output O0 when
   a switch attached to input I0 is pressed.  Note that the button
   is debounced by the software.

 **********************************************************************/
#class auto					/* Change local var storage default to "auto" */


void main()
{
	int vswitch;		// state of virtual switch controlled by button S1


	// set lowest 2 bits of port D as outputs, the rest as inputs
  	WrPortI(PDDDR, &PDDDRShadow, 0x03);
	// set all port D outputs to not be open drain
	WrPortI(PDDCR, &PDDCRShadow, 0x00);

   vswitch=0;				   // initialize virtual switch as off

	while (1) {					// endless loop

     // First task will flash LED #4 for 200ms once per second.
     costate {

		 BitWrPortI(PDDR, &PDDRShadow, 0xFF, 1);	// turn LED on
       waitfor(DelayMs(200));		// wait 200 milliseconds
		 BitWrPortI(PDDR, &PDDRShadow, 0x00, 1);	// turn LED off
       waitfor(DelayMs(800));		// wait 800 milliseconds
     }


     // Second task will debounce switch #1 and toggle virtual switch vswitch.
     // also check button 1 and toggle vswitch on or off
     costate {
         if (!BitRdPortI(PDDR, 2))
         	abort;				// if button not down skip out of costatement

         waitfor(DelayMs(50));		// wait 50 ms

         if(!BitRdPortI(PDDR, 2))
         	abort;				// if button not still down skip out

         vswitch = !vswitch;	// toggle virtual switch since button was down 50 ms

         // now wait for the button to be up for at least 200 ms before considering another toggle
         while (1) {
           waitfor(!BitRdPortI(PDDR, 2));		// wait for button to go up
           waitfor(DelayMs(200));				// wait additional 200 milliseconds
           if (!BitRdPortI(PDDR, 2))
           		break;				// if button still up break out of while loop
			}
      } // end of costate

		// make LED agree with vswitch
			BitWrPortI(PDDR, &PDDRShadow, vswitch, 0);

	}  // end of while loop

}  // end of main

