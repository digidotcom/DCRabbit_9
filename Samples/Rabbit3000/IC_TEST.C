/**********************************************************
	ic_test.c
 	Z-World, 2002

	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	Description
	===========
	This program demonstrates a simple application of input
	capture peripheral.  It measures the duration of a low pulse
	on switch S2 (PG1) and displays to STDIO.  Port G bit 1 is
	channel 1 pulse capture pin.
	
	Note:  Interrupts are not used in the demo.

	Instructions
	============
	1. Compile and run this program.
	2. Press and release switch S2 on the proto-board.
	3. Observe the amount of time you hold S2 down in STDIO.

*************************************************************************/
#class auto

void main()
{
	char capture_status;
	unsigned long pulse_width;
	
	WrPortI(ICS1R, NULL, 0xCC); 	//PG1 is pulse capture pin for channel 1
	
	//use freq_divider for input capture prescaler
	//(freq_divider-1) runs at 19200*16Hz, multiply by 4 for better range
	WrPortI(TAT8R, NULL, freq_divider*4 - 1); //(TA8 prescaler)
	WrPortI(ICCSR, NULL, 0x0c); 					//zero out counters
	WrPortI(ICCR, NULL, 0x00); 					//no interrupts

	//run counter start to stop
	//start is falling edge, stop is rising edge
	//latch counter on stop
	WrPortI(ICT1R, NULL, 0x59);

	pulse_width = 0;
	
	while(1)
	{
		//listen for capture states
		capture_status = RdPortI(ICCSR);
		if(capture_status & 0x10)
		{
			//channel 1 stop occured
			//read capture value, LSB first
			pulse_width += RdPortI(ICL1R);
			pulse_width += RdPortI(ICM1R)*256;
			
			//76.8 = 19200*16*1000/4 (see TA8 setting above)
			printf("channel 1 pulse: %.1fms\n", (float)pulse_width/38.4);
			pulse_width = 0;
			WrPortI(ICCSR, NULL, 0x04); //zero out counter
		}
		else if(capture_status & 0x04)
		{
			printf("Input capture counter rolled over.\n");
			pulse_width += 0x10000;
		}
	}

}
