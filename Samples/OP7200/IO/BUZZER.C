/**************************************************************************

	buzzer.c

   Z-World, 2001
	This sample program is for the OP7200 series controllers.
	
	This program demonstrates the control of the buzzer on the
	controller.

	Note: The Buzzer is mono-tone, however by toggling the control bit
	      faster or slower, some pitch and motor boat affects can be
	      obtained.
	         
**************************************************************************/
#class auto
#memmap xmem  // Required to reduce root memory usage


nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 


void main()
{
	auto int i, delay, loop;
	
	brdInit();
	printf("\n\n");

	// Main loop to control the buzzer
	for(loop = 0; loop < 2; loop++)
	{
		// Generate 3 long beeps	
		printf("Generate 3 beeps\n\n");
		for(i=0; i < 3; i++)
		{
			for(delay = 0; delay < 8000; delay++);
			buzzer(1);
			for(delay = 0; delay < 10000; delay++);
			buzzer(0);
		
		}
		msDelay(3000);

		// Make the buzzer go up in pitch
		printf("Make the buzzer goes up in pitch\n\n");
		for(i=0; i < 800; i++)
		{
			for(delay = 800; delay > i; delay--);
			buzzer(1);
			for(delay = 800; delay > i; delay--);
			buzzer(0);
		}
		msDelay(3000);
		
		// Make the buzzer go down in pitch
		printf("Make the buzzer goes down in pitch\n\n");
		for(i=0; i < 1000; i++)
		{
			for(delay = 0; delay < i; delay++);
			buzzer(1);
			for(delay = 0; delay < i; delay++);
			buzzer(0);
		}
		msDelay(3000);
	}
}