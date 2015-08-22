/***************************************************************************
	daout2.c
	
   Z-World, 2001
   This sample program is used with the BL2100 series controllers.
   
   This program demonstrates the use of both the DAC and the A/D converters.
   The user selects both the DAC and A/D channel to be used, then sets the
   DAC output voltage to be read by the A/D channel. All activity will be
   displayed in the STDIO window.

	!!!This program must be compiled to Flash.
	
   Instructions:
	1. Connect DAC0 or DAC1 to a A/D input channel 0 - 10.
	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.
	
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif


nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
} 


///////////////////////////////////////////////////////////////////////////

void main()
{
	auto int dac_channel, ad_channel;
	auto char tmpbuf[32];
	auto float voltage;
	
	brdInit();		// Required for BL2100 series boards
	printf("Please enter an input channel, 0 thru A (10)....");
	do
	{
		ad_channel = getchar();
	}while(!isxdigit(ad_channel));

	// convert the ascii hex value to a integer
	if( ad_channel >= '0' && ad_channel <='9')
	{
		ad_channel = ad_channel - 0x30;
	}
	else
	{
		ad_channel = tolower(ad_channel);
		ad_channel = (ad_channel - 'a') + 10;
	}
	     
	printf("channel %d chosen.\n", ad_channel);


	printf("Please enter an DAC output channel, 0 - 3...");
	do
	{
		dac_channel = getchar();
	} while (!((dac_channel >= '0') && (dac_channel <= '3')));
	printf("channel %d chosen.\n", dac_channel-=0x30);

	
	while (1)
	{
		printf("\n\n\rEnter DAC voltage up to 10 volts... ");
		gets(tmpbuf);
	   voltage = atof(tmpbuf);
		anaOutVolts(dac_channel, voltage);
		msDelay(2); // need some voltage settling time	 
		voltage = anaInVolts(ad_channel);
		printf("Voltage at CH%d is %.3fv \n", ad_channel, voltage);
	}
}
