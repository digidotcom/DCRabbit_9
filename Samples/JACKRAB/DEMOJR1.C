/*****************************************************

     Samples\JackRab\demojr1.c
     Z-World, 1999

     Demonstration of using the Jackrabbit Development Board

     This program will repeatedly flash LED DS3 on the Jackrabbit Development Board
     (bit 2 on port A).

******************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


// C programs require a main() function
void main()
{
	static int j;	// define an integer j to serve as a loop counter

	// Write 84 hex to slave port control register which initializes parallel
	// port A as an output port (port A drives the LED's on the development board).

	WrPortI(SPCR, &SPCRShadow, 0x84); 

	// now write all ones to port A which sets outputs high and LED's off
  
	WrPortI(PADR, &PADRShadow, 0xff);
  
	while(1) {  // begin an endless loop

		BitWrPortI(PADR, &PADRShadow, 1, 2);  // turn LED DS3 off

		for(j=0; j<25000; j++)
			; // time delay loop
			
		BitWrPortI(PADR, &PADRShadow, 0, 2);  // turn LED DS3 on

		for(j=0; j<1000; j++)
			; // time delay loop

	}	// end while loop
  
} // end program
