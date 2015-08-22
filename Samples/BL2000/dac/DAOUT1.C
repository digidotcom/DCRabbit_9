/***************************************************************************
	daout1.c
	
   Z-World, 2001
   This sample program is used with the BL20XX series controllers.

   This program outputs a voltage that can be read with a voltmeter. The 
   output voltage is computed with using the calibration constants that
   are read from EEPROM (simulated in Flash).
   	
	!!!This program must be compiled to Flash.
   
	Instructions:
	Connect a voltage meter to an output channel DAC0 or DAC1.
	Compile and run this program.
	Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int channel;
	auto float voltout;
	auto char tmpbuf[32];
	
	brdInit();
	printf("Please enter an output channel, 0 or 1....");
	do
	{
		channel = getchar();
	} while (!((channel >= '0') && (channel <= '1')));
	printf("channel %d chosen.\n", channel-=0x30);
		
	while (1)
	{
		printf("\nType a desired voltage (in Volts)  ");
		voltout = atof(gets(tmpbuf));
		printf("Observe voltage on meter.....\n");
		anaOutVolts(channel, voltout);
	}
}
