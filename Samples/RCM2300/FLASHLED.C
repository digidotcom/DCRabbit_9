/////////////////////////////////////////////////////////////////////////////
//
//  Samples\RCM2300\FlashLED.c
//  Z-World, 2001
//
//  RCM2300 and prototyping board example program
//
//  This program will repeatedly flash LED DS3 (PE7) on the RCM2300
//  prototyping board. The MASTER core controls the LEDs.  Timing is done
//  with a delay loop.  DS2 (PE1) will be solid on.
//  
//
/////////////////////////////////////////////////////////////////////////////

// Dynamic C by default makes local variables "static", change to expected
// C "auto" class.
#class auto

// C programs require a main() function
void main()
{
	int j;	// define an integer j to serve as a loop counter

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

	/*  4.  Loop turning LED off, delay, back on, and another delay.
	 */
	while(1) {  // begin an endless loop

		WrPortI(PEB7R, NULL, 0xff);  // turn LED DS3 off

		for(j=0; j<32000; j++)
			; // time delay loop
			
		WrPortI(PEB7R, NULL, 0);     // turn LED DS3 on

		for(j=0; j<25000; j++)
			; // time delay loop

	}	// end while loop
  
} // end program
