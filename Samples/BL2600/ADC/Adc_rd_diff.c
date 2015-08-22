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
	adc_rd_diff.c

	This sample program is for the BL2600 series controller.

	Description
	===========
	This program reads and displays voltage and equivalent values
   of a differential ADC channel. The A/D rawdata and equivalent
   voltage is selectable by the user for display.

	Connections
	===========
	For this sample program you will need a power supply that has
   floating outputs.

	NOTE:	Before doing the following steps, set the power supply
	      to zero volts and then turn it OFF.

	1. Initially connect the positive side of the power supply to
	   the positive side to one of the following ADC differential
	   channel pairs.

	    Channel   DIFF Pairs
	    -------  ------------
	      0 		 +AIN0  -AIN1
	      2		 +AIN2  -AIN3
	      4		 +AIN4  -AIN5
	      6		 +AIN6  -AIN7

	2.	Connect the negative side of the power supply to the
	   negative side to one of the following ADC differential
	   channel pairs. (Same DIFF pair from step 1)

	    Channel    DIFF Pairs
	    -------   ------------
	      0		  +AIN0   -AIN1
	      2		  +AIN2   -AIN3
	      4		  +AIN4   -AIN5
	      6 		  +AIN6   -AIN7


	Instructions
	============
	1. Power-on the controller.
	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.

***************************************************************************/

#class auto	 // Change local var storage default to "auto"


// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

void  blankScreen(int start, int end)
{
	auto char buffer[256];
   auto int i;

   memset(buffer, 0x00, sizeof(buffer));
 	memset(buffer, ' ', sizeof(buffer));
   buffer[sizeof(buffer)-1] = '\0';
   for(i=start; i < end; i++)
   {
   	DispStr(start, i, buffer);
   }

}
void printrange()
{
	printf("\n\n");
	printf(" Gain Code\tVoltage range\n");
	printf(" ---------\t-------------\n");
	printf("    0     \t +- 20 \n");
	printf("    1     \t +- 10\n");
	printf("    2     \t +- 5\n");
	printf("    3     \t +- 4\n");
	printf("    4     \t +- 2.5\n");
	printf("    5     \t +- 2\n");
	printf("    6     \t +- 1.25\n");
	printf("    7     \t +- 1\n\n");
}


main ()
{
	auto int channel, gaincode;
	auto int rawdata;
	auto float voltage;
	auto int key, valid;
	auto int display_mode;
	auto char s[64];

   // Initialize the controller
	brdInit();

   // Configure channel pair 0 & 1 for differential mode of operation
   // (Max voltage range is ±20V)
   anaInConfig(0, DIFF_MODE);

   // Configure channel pair 2 & 3 for differential mode of operation
   // (Max voltage range is ±20V)
   anaInConfig(1, DIFF_MODE);

   // Configure channel pair 4 & 5 for differential mode of operation
   // (Max voltage range is ±20V)
   anaInConfig(2, DIFF_MODE);

   // Configure channel pair 6 & 7 for differential mode of operation
   // (Max voltage range is ±20V)
   anaInConfig(3, DIFF_MODE);

	while (1)
	{
     	printrange();
		printf(" Choose gain code (0-7) =  ");
		do
		{
			gaincode = getchar();
		} while (!( (gaincode >= '0') && (gaincode <= '7')) );
		gaincode = gaincode - 0x30;
		printf("%d", gaincode);
		while(kbhit()) getchar();

      blankScreen(0, 20);
		DispStr(1, 2, "A/D voltage for channel pairs 0&1, 2&3, 4&5 and 6&7");
		DispStr(1, 3, "---------------------------------------------------");
   	DispStr(1, 14, "Press Q or q to exit program.");

   	while(1)
      {
			for(channel = 0; channel < 8; channel+=2)
			{
      		voltage = anaInDiff(channel, gaincode);
            if(voltage != ADOVERFLOW)
         		sprintf(s, "Channel = %d&%d Voltage = %.3f               ",
                           channel, channel+1, voltage);
            else
               sprintf(s, "Channels = %d Voltage = Exceeded Range!!!  ", channel);
            DispStr(1,channel + 4, s);
			}
         if(kbhit())
			{
				key = getchar();
            if(key == 'Q' || key == 'q')
            {
      			blankScreen(0, 20);
   				DispStr(0,0, "\r");
               while(kbhit()) getchar();
               break;
            }
			}
		}
   }
}
///////////////////////////////////////////////////////////////////////////

