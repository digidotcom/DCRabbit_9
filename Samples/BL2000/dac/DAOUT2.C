/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/***************************************************************************
	daout2.c
	
   This sample program is used with the BL2000 or BL2020 controller.

   This program demonstrates the use of both the DAC and the A/D converters.
   The user selects both the DAC and A/D channel to be used, then sets the
   DAC output voltage to be read by the A/D channel. All activity will be
   displayed in the STDIO window.

	!!!This program must be compiled to Flash.
	
   Instructions:
	1. Connect DAC0 or DAC1 to a A/D input channel 0 - 8.
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
	
	brdInit();
	printf("Please enter an input channel, 0 thru 8....");
	do
	{
		ad_channel = getchar();
	} while (!((ad_channel >='0') && (ad_channel <='8')));          
	printf("channel %d chosen.\n", ad_channel-=0x30);

	
	printf("Please enter an DAC output channel, 0 or 1......");
	do
	{
		dac_channel = getchar();
	} while (!((dac_channel >= '0') && (dac_channel <= '1')));
	printf("channel %d chosen.\n", dac_channel-=0x30);

	
	while (1)
	{
		printf("\n\n\rEnter DAC voltage up to 4 volts... ");
		gets(tmpbuf);
	   voltage = atof(tmpbuf);
		anaOutVolts(dac_channel, voltage);
		msDelay(2); // need some voltage settling time	 
		voltage = anaInVolts(ad_channel);
		printf("Voltage at CH%d is %.3fv \n", ad_channel, voltage);
	}
}
