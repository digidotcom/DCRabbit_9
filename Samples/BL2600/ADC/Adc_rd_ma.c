/***************************************************************************
	adc_rd_ma.c
	Z-World, 2004

	This sample program is for the BL2600 series controller.

	Description
	===========
	This program demonstrates how to read an ADC milli-Amp channel using
	previously defined coefficients. It will also continuously display
	the current that is being monitored.


	Instructions
	============
	1. Connect a current meter in series as shown below with it set to
   read 4 to 20 milli-amps of current.

	-----------------|									 |-------------------------
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
	-----------------|									 |-------------------------


	2. Compile and run this program.
	3. Follow the prompted directions of this program during execution.
	4. Vary voltage (0-2.5v) on power supply to see the CURRENT meter track
	what's being displayed by Dynamic C (4-20ma).

	Note: For best results use a 4 1/2 digit current meter
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

void main ()
{
	auto int channel;
	auto float current;
	auto int key;
	auto char s[128];

   // Initialize the controller
	brdInit();

   // Configure channel pair 0 & 1 for 4- 20ma mode of operation
   anaInConfig(0, mAMP_MODE);

   // Configure channel pair 2 & 3 for 4- 20ma mode of operation
	anaInConfig(1, mAMP_MODE);

   blankScreen(0, 20);
	DispStr(2, 2, "A/D input current for channels 0 - 3");
	DispStr(2, 3, "------------------------------------");
   DispStr(2, 10, "Press Q or q to exit program.");

   while(1)
   {
		for(channel = 0; channel < 4; channel++)
		{
     		current = anaInmAmps(channel);
         if(current != ADOVERFLOW)
        		sprintf(s, "Channel = %2d Current = %.3f                  ", channel, current);
         else
            sprintf(s, "Channel = %2d Current = Exceeded Range!!!     ", channel);
         DispStr(2,channel + 4, s);
		}
      if(kbhit())
		{
			key = getchar();
			if (key == 'Q' || key == 'q')		// check if it's the q or Q key
			{
				while(kbhit()) getchar();
      		break;
     		}
		}
   }
}	//end main

