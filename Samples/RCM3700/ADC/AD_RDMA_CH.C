/***************************************************************************
	ad_rdma_ch.c
   Z-World, 2003

   This sample program is for the RCM3700 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates how to read an ADC milli-Amp channel using
	previously defined coefficients. It will also continuously display
	the current that is being monitored.

	Instructions
	============
	1.	Make sure pins 3-5 are connected on JP5, JP6, and JP7.  Connect pins 1-2,
		3-4, 5-6, 7-8 on JP8.

	2. Connect a power supply of 0-5 volts to one of the AIN channel 3-6
		on the controller.
	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Vary voltage (0-5v) on power supply to see the CURRENT meter track
	what is displayed by Dynamic C (4-20ma).

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/
#class auto

// So brdInit() can distinguish from RCM3720 proto-board
#define RCM3700_PROTOBOARD

#define STARTCHAN 3
#define ENDCHAN 6


main ()
{
	auto int inputnum;
	auto char buffer[64];
	auto float currentequ;

	brdInit();

	while (1)
	{
		printf("\nChoose the AD mAmp channel %d to %d .... ", STARTCHAN, ENDCHAN);
		gets(buffer);
		inputnum = atoi(buffer);

		printf("Vary current on channel %d\n", inputnum);
		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			currentequ = anaInmAmps(inputnum);
			printf("Current at CH%d is %.2fma\n", inputnum, currentequ);
			printf("\npress enter key to read value again or 'Q' to view another channel\n\n");
			gets(buffer);
		}
	}


}	//end main


