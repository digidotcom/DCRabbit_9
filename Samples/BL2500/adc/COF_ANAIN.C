/************************************************************************
	cof_anain.c
  	Z-World 2002

   Program to demonstrate analog input cofunction.

   This program demonstrates the use of the analog input 
 	driver as a cofunction.  Connect DA1 to AD0 to provide an input
	voltage.  When the program runs, it will read the input voltage
   ten times while another costate is executed concurrently.  The
   values will be printed out at the end of the program.

*************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


void main()
{
	int	values[10], value, i;
	int	count1, count2;

	count1 = count2 = 0;

	brdInit();								// initialize I/O drivers
	pwmOut(1, 512);						// provide input voltage by
												//   connecting DA1 to AD0)

	while (count1<10)
	{					// read voltage 10 times

		costate
		{
			waitfordone
			{
				value=cof_anaIn(0);		// execute one step
			}
			
			if (value == -1) {			// this typically means that AD0 is disconnected
				printf("value out of range?\n");
				exit(1);
			}

			if (value > 0) {					// is value "good"?
				values[count1] = value;		//   yes - store it
				count1++;						//   otherwise repeat
			}
		}
		
		costate
		{
			count2++;							// 2nd costate (does
													//		nothing here)
		}

	}

	pwmOut(1, 0);								// turn off output

	for (i=0; i<10; i++)						// print values
		printf("values[%4d] = %6d\n", i, values[i]);
	
	printf("1st costate (cos_anaIn) executed %d times\n", count1);
	printf("2nd costate (nothing)   executed %d times\n", count2);
}

////////////////////////////////////////////////////////////////////////////////
