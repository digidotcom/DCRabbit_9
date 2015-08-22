/***************************************************************************
	Dacal.c
	
   Z-World, 2001
	This sample program is used with the BL20XX series controllers.

	!!!Caution this will overwrite the calibration constants set at
	   the factory.  This program must be compiled to Flash.
	
	This program demonstrates how to recalibrate an DAC channel using
	two known voltages and defines the two coefficients, gain and offset,
	which will be rewritten into the DAC's EEPROM (simulated in Flash). 

	Instructions:
	1.Connect a voltage meter to an output channel.
	2.Compile and run this program.
	3.Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif

#define LOCOUNT 400		//sets up a low voltage calibration point 
#define HICOUNT 3695    //sets up a high voltage calibration point

///////////////////////////////////////////////////////////////////////////
void main()
{
	auto int channel;
	auto float voltout, volt1, volt2;
	auto char tmpbuf[24];
	
	brdInit();
	
	printf("Please enter an output channel, 0 or 1....");
	do
	{
		channel = getchar();
	} while (!((channel >= '0') && (channel <= '1')));
	printf("channel %d chosen.\n", channel-=0x30);

	// set two known voltage points using rawdata values, the
	// user will then type in the actual voltage for each point
	anaOut(channel, HICOUNT);
	printf("Type first voltage reading from meter(appox 3.7 volts), then press Enter   ");
	volt1 = atof(gets(tmpbuf));
	
	anaOut(channel, LOCOUNT);
	printf("Type second voltage reading from meter(appox .4 volts), then press Enter  ");
	volt2 = atof(gets(tmpbuf));
	
	if (anaOutCalib(channel, HICOUNT, volt1, LOCOUNT, volt2))
		printf("Cannot make coefficients\n");
	else
	{
		//store coefficients into eeprom
		while (anaOutEEWr(channel));
		printf("Wrote coefficients to eeprom\n");
		
		//read back coefficients from eeprom
		while (anaOutEERd(channel));
		printf("Read coefficients from eeprom\n");
		
		while (1)
		{
			printf("\nType a desired voltage (in Volts)  ");
			voltout = atof(gets(tmpbuf));
			printf("Observe voltage on meter.....\n");
			anaOutVolts(channel, voltout);
		}
	}
}
///////////////////////////////////////////////////////////////////////////
