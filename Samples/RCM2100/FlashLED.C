////////////////////////////////////////////////////////////////////////////////
//
//  Samples\RCM2100\FlashLED.c
//  Z-World, 2000
//
//  Demonstration of using the RCM 2100 Development Board
//
//  This program will repeatedly flash LED DS3 (PA1) on the RCM 2100
//  Development Board.
//  
//
////////////////////////////////////////////////////////////////////////////////

//  Normally local vars are "static", but ask Dynamic C to make them "auto"
#class auto


// C programs require a main() function. This one returns no value.
void main()
{
	int j;	// define an integer j to serve as a loop counter

	// Write 84 hex to slave port control register which initializes parallel
	// port A as an output port (port A drives the LED's on the development board).

	WrPortI(SPCR, &SPCRShadow, 0x84); 

	// now write all ones to port A which sets outputs high and LED's off
  
	WrPortI(PADR, &PADRShadow, 0xff);
  
	while(1) {  // begin an endless loop

		BitWrPortI(PADR, &PADRShadow, 1, 1);  // turn LED DS3 off

		for(j=0; j<32000; j++)
			; // time delay loop
			
		BitWrPortI(PADR, &PADRShadow, 0, 1);  // turn LED DS3 on

		for(j=0; j<25000; j++)
			; // time delay loop

	}	// end while loop
  
} // end program
