/***************************************************************************
	adc_rd_se_unipolar.c
	Z-World, 2004

	This sample program is for the BL2600 series controller.

	Description
	===========
   Reads and displays the voltage of all single-ended analog input
   channels. The voltage is calculated from coefficients read from
   the reserved eeprom storage device.

    Connections for unipolar mode of operation, 0 - 20V
   ----------------------------------------------------
   1. Connect the positive power supply lead to an input channel.
	2.	Connect the negative power supply lead to AGND on the controller.

	Instructions
	============
	3. Compile and run this program.
	4. Follow the prompted directions of this program during execution.
	5. Voltage will be continuously displayed for all channels.

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

void printrange( void )
{
	printf("\n\n");
   printf(" Gain code\tVoltage range\n");
	printf(" ---------\t-------------\n");
	printf("     0    \t0 - 20v\n");
	printf("     1    \t0 - 10v\n");
	printf("     2    \t0 - 5v\n");
	printf("     3    \t0 - 4v\n");
	printf("     4    \t0 - 2.5v\n");
	printf("     5    \t0 - 2v\n");
	printf("     6    \t0 - 1.25v\n");
	printf("     7    \t0 - 1v\n\n");
}


void main ()
{
	auto unsigned int rawdata;
	auto int channel, keypress;
	auto int key;
	auto int gaincode;
	auto float voltage;
   auto int mode;
   auto char s[128];

   // Initialize the controller
	brdInit();

   // Configure channel pair 0 & 1 for Single-Ended unipolar mode of operation.
   // (Max voltage range is 0 - 20v)
   anaInConfig(0, SE0_MODE);

   // Configure channel pair 2 & 3 for Single-Ended unipolar mode of operation
   // (Max voltage range is 0 - 20v)
   anaInConfig(1, SE0_MODE);

   // Configure channel pair 4 & 5 for Single-Ended unipolar mode of operation
   // (Max voltage range is 0 - 20v)
   anaInConfig(2, SE0_MODE);

   // Configure channel pair 6 & 7 for Single-Ended unipolar mode of operation
   // (Max voltage range is 0 - 20v)
   anaInConfig(3, SE0_MODE);

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
		DispStr(1, 2,  "A/D input voltage for channels 0 - 7");
		DispStr(1, 3,  "------------------------------------");
   	DispStr(1, 14, "Press Q or q to select another gain option.");

   	while(1)
      {
			for(channel = 0; channel < 8; channel++)
			{
      		voltage = anaInVolts(channel, gaincode);
            if(voltage != ADOVERFLOW)
         		sprintf(s, "Channel = %2d Voltage = %.3f               ", channel, voltage);
            else
               sprintf(s, "Channel = %2d Voltage = Exceeded Range!!!  ", channel);
            DispStr(1,channel + 4, s);
			}
         if(kbhit())
			{
				key = getchar();
				if (key == 'Q' || key == 'q')		// check if it's the q or Q key
				{
            	blankScreen(0, 20);
   				DispStr(0,0, "\r");
			  		while(kbhit()) getchar();
      			break;
     			}
			}
      }
   }
}	//end main

