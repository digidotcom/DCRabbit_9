/*****************************************************

	demobrd1.c
   Z-World, 2000

 	This sample program is used with products such as
	Intellicom Series and TCP/IP Development Board with
	Z-World Demo Board.

   This program will flash the LED's attached to outputs
   O0 and O1 with 180 degree phase.

   Connect board outputs O0 and O1 to Demo Board LED's.

******************************************************/
#class auto					/* Change local var storage default to "auto" */

// C programs require a main() function
void main()
{
	int j;	// define an integer j to serve as a loop counter


	// set lowest 2 bits of port D as outputs
  	WrPortI(PDDDR, &PDDDRShadow, 0x03);
	// set all port D outputs to not be open drain
	WrPortI(PDDCR, &PDDCRShadow, 0x00);


	while(1) {  // begin an endless loop

		BitWrPortI(PDDR, &PDDRShadow, 0xFF, 0);	// turn led on output 0 on
		BitWrPortI(PDDR, &PDDRShadow, 0x00, 1);	// turn led on output 1 off

		for(j=0; j<20000; j++)
			; // time delay loop

		BitWrPortI(PDDR, &PDDRShadow, 0x00, 0);	// turn led on output 0 off
		BitWrPortI(PDDR, &PDDRShadow, 0xFF, 1);	// turn led on output 1 on

		for(j=0; j<20000; j++)
			; // time delay loop

	}	// end while loop

} // end program