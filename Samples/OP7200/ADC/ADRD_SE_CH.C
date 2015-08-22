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
	adrd_se_ch.c
	
	This sample program is used with OP7200 series controllers.
	
	Description
	===========
	This program reads and displays voltage and equivalent values of one
	single-ended ADC channel. The A/D rawdata and equivalent voltage is
	selectable by the user for display.
	
	Instructions
	============
	1. Connect a power supply of 0-20 volts to an input channel.
	2.	Connect power supply grounds to AGND on the controller.
	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Values will continuously display.

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

void main ()
{
	auto unsigned int rawdata;
	auto int channel, keypress;
	auto int key;
	auto int gaincode;
	auto float voltage;
	
	brdInit();			//read constants if AD device installed
	while (1)
	{
		printf("\n\nPlease enter an ADC channel (0-7) = ");
		do
		{
			channel = getchar();
		} while (!( (channel >= '0') && (channel <= '7')) );
		channel = channel - 0x30;
		printf("%d", channel);
		while(kbhit()) getchar();

		printrange();
		printf("Choose gain code (0-7) =  ");
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
			keypress = getchar();
		} while (!( (keypress >= '1') && (keypress <= '3')) );
		keypress = keypress - 0x30;
		printf("%d\n\n", keypress);
		while(kbhit()) getchar();
		
		do 
		{
			switch (keypress)
			{
				case 1: 
					rawdata = anaIn(channel, SE_MODE, gaincode);
					printf("CH%2d raw data %d\n", channel, rawdata);
					break;
				case 2:
					voltage = anaInVolts(channel, gaincode);
					printf("CH%2d is %.5f V\n", channel, voltage);
					break;
				case 3:
					rawdata  = anaIn(channel, SE_MODE, gaincode);
					voltage  = anaInVolts(channel, gaincode);
					printf("CH%2d is %.5f V from raw data %d\n", channel, voltage, rawdata);
					break;
				default:
				break;
			}
			printf("\nPress ENTER key to read value again or 'Q' or read another channel\n\n");
			while(!kbhit());
			key = getchar();
			while(kbhit()) getchar();

		}while(key != 'q' && key != 'Q');
	}		
}	//end main

