/************************************************************************

	  Samples\Jackrab\jriotest.c
	  Z-World, 1999

     Program to demonstrate JackRabbit I/O drivers

     This program exercises the JackRabbit's 4 digital output channels,
     the 2 analog output channels, and the one analog input channel.For
     more details, read the following library functions decriptions in
     the user manual:

					jrioInit
					digOut
					anaOut
					anaIn

*************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


// General use milli-sec delay function
nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 

void main()
{
	auto char i;
	auto int	j;
	auto int	value;
		
	
	/////////////////////////////////////////////////////////////////////
	// initialize drivers - necessary!

	jrioInit();

	
	/////////////////////////////////////////////////////////////////////
	// test digital outputs
	// 	all four digital outputs should pulse up and down 250 times
	
	printf("Testing digital outputs\n");

	for (j=1; j<=500; j++) {
		digOut(0, j%2);
		digOut(1, j%2);
		digOut(2, j%2);
		digOut(3, j%2);
		msDelay(10);
	}


	/////////////////////////////////////////////////////////////////////
	// test D/A channel 0
	//		channel DA0 should gradually increase voltage, from about 130mV
	//		up to 3.0 volts.

	printf("Testing analog output 0\n");		// note this printf is NOT
															// used during analog output!
	
	for (j=PWM_MIN; j<PWM_MAX0; j++) {
		anaOut(0, j);
		msDelay(10);
	}	
	// turn channel back off to minimize CPU use
	anaOut(0, 0);


	/////////////////////////////////////////////////////////////////////
	// test D/A channel 1
	//		channel DA1 should gradually increase voltage, from about 90mV
	//		up to 3.7 volts.

	printf("Testing analog output 1\n");		// note this printf is NOT
															// used during analog output!
	
	for (j=PWM_MIN; j<PWM_MAX1; j++) {
		anaOut(1, j);
		msDelay(10);
	}	
	// turn channel back off to minimize CPU use
	anaOut(1, 0);



	/////////////////////////////////////////////////////////////////////
	// test D/A channel 1
	//		if you connect DA1 to AD0, the resulting value should be
	//		around 790...

	printf("Testing analog input AD0\n");		// note this printf is NOT
															// used during analog output!

	anaOut(1, 512);									// DA1 = 512 is about 2.5 volts
	do {
		anaIn(0, &value);

		if (value == -1023) {		// -1023 implies that AD0 is not connected to anything
			printf("value = -1023, AD0 not connected?\n");
			exit(1);
		}
		
	} while (value < 0);					// if negative then result suspect, try again

	printf("value = %d\n", value);				// should be around 768
															//   (~2.5 V on DA0)

	// turn DA1 back off to minimize CPU use
	anaOut(1, 0);
}
