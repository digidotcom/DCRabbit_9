/**************************************************************************
	led.c

   Z-World, 2001
	This sample program toggles the LED on the OP7200. 
	
**************************************************************************/
#class auto


#memmap xmem  // Required to reduce root memory usage 

#define LEDOFF 0  //Value used to turn the LED OFF
#define LEDON	1	//Value used to turn the LED ON

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto unsigned int i;	 	// variable for the loop counter
	auto unsigned int led; 	// variable used to cycle thru all the LED's
	
	brdInit();	// Initialize the controller

	for(;;)	// begin an endless loop
	{  		
		//Turn-on the LED indicated by the led variable
		ledOut(0, LEDON);
		for(i=0; i<20000; i++); // time delay loop

		//Turn-off the LED indicated by the led variable
		ledOut(0, LEDOFF);
		for(i=0; i<20000; i++); // time delay loop
	}	
} 
