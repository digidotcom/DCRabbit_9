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
	adc_cal_ma.c

	This sample program is for the BL2600 series controller.

	Description
	===========
	This program demonstrates how to recalibrate an ADC milli-amp channel
	using two known currents to generate two coefficients, gain and offset,
	which will be rewritten into reserved eeprom. It will also continuously
	displays the current that is being monitored.

	Instructions
	============
	1. Connect a current meter in series as shown below with it set to
   read 4 to 20 milli-amps of current.

	-----------------|									 |---------------------------
						  |									 | 4-20ma mode of operation
	Power supply	  |          + |-------| -		 |
	0 - 2.5v		 POS |------------|current|-------| AIN channels 0 - 3
	                 |	         | meter |		 |
	                 |            ---------       |
	                 |    		                   |
                    |                            |
                    |                            |
                    |                            |
 	                 |                            |
	             NEG |----------------------------| AGND
					     |									 |
	-----------------|									 |---------------------------


	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.
	4. Vary voltage (0-2.5v) on power supply to see the CURRENT meter track
	what's being displayed by Dynamic C (4-20ma).

	Note: For best results use a 4 1/2 digit current meter
***************************************************************************/

#class auto	 // Change local var storage default to "auto"

void main ()
{
	auto long data1, data2;
	auto unsigned int rawdata;
	auto float locurrent, hicurrent;
	auto float currentequ;
	auto char buffer[64];
	auto int key, i;
	auto int channel;

   // Initialize the controller
	brdInit();

   // Configure channel pair 0 & 1 for 4- 20ma mode of operation
   anaInConfig(0, mAMP_MODE);

   // Configure channel pair 2 & 3 for 4- 20ma mode of operation
   anaInConfig(1, mAMP_MODE);

	while (1)
	{
		printf("\nChoose the ADC 4-20ma channel (0-3) = ");
		do
		{
			channel = getchar();
		} while (!( (channel >= '0') && (channel <= '3')) );
		channel = channel - 0x30;
		printf("%d", channel);
		while(kbhit()) getchar();

		/////Get two data points using known currents
		printf("\n\nAdjust the current to 5.0ma and then ENTER the actual\n");
		printf("current being measured, (floating point value) = ");
		gets(buffer);
		while(kbhit()) getchar();
		locurrent = atof(buffer);
		if(!(locurrent >= 4.00 && locurrent <= 20.0))
		{
			printf("Current value must be within 4.0 - 20.0 amps\n\n");
			exit(1);
		}
		data1 = 0;
		for(i=0; i<10; i++)
			data1 += anaIn(channel,  mAMP_GAINCODE);
		data1 = data1/10;
		printf("data1 = %d\n", data1);

		printf("\n\nAdjust the current to ~19.0ma and ENTER the actual\n");
		printf("current being measured, (floating point value) = ");
		gets(buffer);
		while(kbhit()) getchar();
		hicurrent = atof(buffer);
		if(!(hicurrent >= 4.00 && hicurrent <= 20.0))
		{
			printf("Current value must be within 4.0 - 20.0 amps\n\n");
			exit(1);
		}

		data2 = 0;
		for(i=0; i<10; i++)
			data2 += anaIn(channel, mAMP_GAINCODE);
		data2 = data2/10;
		printf("data2 = %d\n", data2);

		/////Calculate gains and offsets
		anaInCalib(channel, mAMP_MODE, mAMP_GAINCODE,
		           (int)data1, locurrent, (int)data2, hicurrent);

		printf("Vary current on channel %d\n", channel);
		do
		{
			currentequ = anaInmAmps(channel);
			printf("Current at CH%d is %.2fma\n", channel, currentequ);
			printf("\nPress ENTER key to read value again or 'Q' to calibrate another channel\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();

		}	while(key != 'q' && key != 'Q');
	}

}	//end main