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
	adrd_se_all.c
	
	This sample program is used with OP7200 series controllers.
	
	Description
	===========
	This program is setup to display voltage and/or rawdata for each
	single-ended ADC channel. 
		
	Instructions
	============
	1. Connect a power supply of 0-20 volts to input channels.
	2.	Connect power supply grounds to AGND on the controller.
	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.

***************************************************************************/
#class auto


void printrange( void )
{
	printf("\n");
	printf("\n gain_code\tVoltage range\n");
	printf("-----------\t-------------\n");
	printf("\t0\t0 - 20v\n");
	printf("\t1\t0 - 10v\n");
	printf("\t2\t0 - 5v\n");
	printf("\t3\t0 - 4v\n");
	printf("\t4\t0 - 2.5v\n");
	printf("\t5\t0 - 2v\n");
	printf("\t6\t0 - 1.25v\n");
	printf("\t7\t0 - 1v\n\n");
}


//---------------------------------------------------------
//	displays both the raw data count and voltage equivalent
//	_adsCalibS is address for single ended channels
//---------------------------------------------------------
void anaInInfo (int channel, int gaincode, unsigned int *rawdata, float *voltage)
{
	*rawdata = anaIn(channel, SE_MODE, gaincode);
	*voltage  = anaInVolts(channel, gaincode);	
}


void main ()
{
	auto unsigned int rawdata;
	auto int channel, gaincode;
	auto int display_mode, key;
	auto float voltage;

	brdInit();			//read constants if AD device installed

	while (1)
	{

		printrange();
		printf("Choose gain code (0-7) = ");
		do
		{
			gaincode = getchar();
		} while (!( (gaincode >= '0') && (gaincode <= '7')) );
		gaincode = gaincode - 0x30;
		printf("%d", gaincode);
		while(kbhit()) getchar();

		printf("\n\n");
		printf("Display Mode:\n");
		printf("-------------\n");
		printf("1. Display raw data only\n");
		printf("2. Display voltage only\n");
		printf("3. Display both\n");
		printf("Select Mode (1-3) = ");
		do
		{
			display_mode = getchar();
		} while (!( (display_mode >= '1') && (display_mode <= '3')) );
		display_mode = display_mode - 0x30;
		printf("%d\n\n", display_mode);
		while(kbhit()) getchar();

		do
		{
			switch (display_mode)
			{
				case 1: 
					for (channel=0; channel<=7; channel++)
					{
						rawdata = anaIn(channel, SE_MODE, gaincode);
						printf("CH%2d raw data %d\n", channel, rawdata);
					}	
					break;
				case 2:
					for (channel=0; channel<=7; channel++)
					{
						voltage = anaInVolts(channel, gaincode);
						printf("CH%2d is %.5f V\n", channel, voltage);
					}	
					break;
				case 3:
					for (channel=0; channel<=7; channel++)
					{
						anaInInfo(channel, gaincode, &rawdata, &voltage);
						printf("CH%2d is %.5f V from raw data %d\n", channel, voltage, rawdata);
					}	
					break;

				default:
				break;
			}
			printf("\nPress ENTER key to read value again or 'Q' or for different gaincode\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();

		}while(key != 'q' && key != 'Q');
	}		
}	//end main

