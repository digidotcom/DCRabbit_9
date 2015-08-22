////////////////////////////////////////////////////////////////////////////////
//
//  Samples\RCM2100\FlashLED2.c
//  Z-World, 2000
//
//  Demonstration of using the RCM 2100 Prototyping Board
//
//  This program will repeatedly flash LED DS3 (PA1) on the RCM 2100 Prototyping
//  Board.  This program also shows the use of the runwatch() function
//  to allow Dynamic C to update watch expressions while running.  To test this:
//
//    1.  Add a watch expression for "k" under "Inspect:Add/Del Watch Expression"
//    2.  Click "add to top" so that it will be in the watch list permanently
//    3.  While the program is running, type Ctrl+U to update the watch window
//
////////////////////////////////////////////////////////////////////////////////

//  Normally local vars are "static", but ask Dynamic C to make them "auto"
#class auto


// C programs require a main() function. This one returns no value.
void main()
{
	int j;	// define an integer j to serve as a loop counter
	int k;   // define a counter k with which to test the watch expression

	// Write 84 hex to slave port control register that initializes parallel
	// port A as an output port (port A drives the LEDs on the Prototyping Board).

	WrPortI(SPCR, &SPCRShadow, 0x84);

	k=0;     // initialize k to 0

	// now write all ones to port A which sets outputs high and LEDs off
  
	WrPortI(PADR, &PADRShadow, 0xff);
  
	while(1) {  // begin an endless loop

		BitWrPortI(PADR, &PADRShadow, 1, 1);  // turn LED DS3 off

		for(j=0; j<32000; j++)
			; // time delay loop
			
		BitWrPortI(PADR, &PADRShadow, 0, 1);  // turn LED DS3 on

		for(j=0; j<1000; j++)
			; // time delay loop

		k++;         // increment k
		runwatch();  // update watch expressions for Dynamic C to read

	}	// end while loop
  
} // end program
