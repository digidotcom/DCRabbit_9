/*****************************************************

   demobrd2.c
   Z-World, 2000

 	This sample program is used with products such as
	Intellicom Series and TCP/IP Development Board with
	Z-World Demo Board.

   This program will repeatedly flash LED attached to
   output O0.  This program also shows the use
   of the runwatch() function to allow Dynamic C
   to update watch expressions while running.

   To test this:

   1. Connect board outputs O0 to Demo Board LED.
 	2. add a watch expression for "k" under
		"Inspect:Add/Del Watch Expression"
	3.	click "Add to top" so that it will be
		permanently in the watch list
	4.	while the program is running, type Ctrl+U
		to update the watch window

******************************************************/
#class auto					/* Change local var storage default to "auto" */

// C programs require a main() function
void main()
{
	int j;	// define an integer j to serve as a loop counter
	int k;	// define a counter k to test the watch expression with


	// set lowest 2 bits of port D as outputs
  	WrPortI(PDDDR, &PDDDRShadow, 0x03);
	// set all ports D outputs to not be open drain
	WrPortI(PDDCR, &PDDCRShadow, 0x00);

	k=0;		// initialize k to zero


	while(1) {  // begin an endless loop

		BitWrPortI(PDDR, &PDDRShadow, 0x00, 0);   // turn led on output 1 off

		for(j=0; j<25000; j++)
			; // time delay loop

		BitWrPortI(PDDR, &PDDRShadow, 0xFF, 0);   // turn led on output 0 on

		for(j=0; j<25000; j++)
			; // time delay loop

		k++;					// increment k

		// update watch expressions for Dynamic C to read
		runwatch();

	}	// end while loop

} // end program

