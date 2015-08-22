/***************************************************************************
	adccalib.c
	Z-World, 2002

	This sample program is for the BL2500 series controllers.
	
	Description
	===========
	This program demonstrates how to recalibrate one single-ended ADC channel
	using two known voltages to generate constants for that channel and
	rewritten into user block data area. This will overwrited previous
	factory calibrations.

	This program must be compiled to Flash.

	Connections
	===========
	Connect the power supply positive output to the AD0 and
	the negative output to J1 pin 3 on the controller.

	Connect a volt meter to monitor the voltage input.

	
	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
#class auto

#define LOVOLT 1.00		//calibration voltage
#define HIVOLT 3.00		//calibration voltage
#define CHANNEL 0			//analog input AD0

main ()
{
	auto int channel, value1, value2;
	auto float voltage, volts1, volts2;
	auto unsigned int rawdata, gaincode;
	auto char buffer[64];
	
	brdInit();	

	printf("\nAdjust voltage to approximately %.2f and enter actual =  ", LOVOLT);
	gets(buffer);
	volts1 = atof(buffer);
	value1 = anaIn(CHANNEL);
	if (value1 == ADOVERFLOW)
		printf("lo:  channel=%d overflow\n", CHANNEL);
	else		
		printf("lo:  channel=%d raw=%d\n", CHANNEL, value1);
	
	printf("\nAdjust voltage to approximately %.2f and enter actual =  ", HIVOLT);
	gets(buffer);
	volts2 = atof(buffer);
	value2 = anaIn(CHANNEL);
	if (value2 == ADOVERFLOW)
		printf("hi:  channel=%d overflow\n", CHANNEL);
	else		
		printf("hi:  channel=%d raw=%d\n", CHANNEL, value2);
	
	anaInCalib(CHANNEL, value1, volts1, value2, volts2);
	
	printf("\nstore constants to flash\n");
	anaInEEWr(CHANNEL);		
	
	printf("\nread back constants\n");
	anaInEERd(CHANNEL);		

	printf("\nVary voltage from 0 - 3.1  .... \n");
	while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
	{
		voltage = anaInVolts(CHANNEL);
		printf("Ch %2d Volt=%.5f \n", CHANNEL, voltage);
		printf("press enter key to read values again or 'Q' to exit\n\n");
		gets(buffer);
	}
}
///////////////////////////////////////////////////////////////////////////

