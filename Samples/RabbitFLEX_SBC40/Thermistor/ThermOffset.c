/***************************************************************************
	thermoffset.c
   Rabbit Semiconductor, 2006

 	This program is used with RabbitFLEX SBC40 boards.

	Description
	===========
   This program demonstrates how to set the PowerCoreFLEX thermistor
   (on the RabbitFLEX SBC40 board) to an external reference temperature.

	Instructions
	============
	1. Power-on the core module.
	2. Compile and run this program.
	3. Follow the instructions as displayed.

***************************************************************************/
#class auto

#define CELSIUS  		0
#define FAHRENHEIT  	1
#define KELVINS		2

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}


void main()
{
	auto int i;
   auto float temperature;
   auto char s[256];
   auto int key;
   auto int calib;
   auto int temp_units;

	// Initialize the I/O on the RabbitFLEX SBC40 board
	brdInit();

   while(1)
	{
		DispStr(1, 1,"!!!Caution this will overwrite the offset constant set at the factory.");
		DispStr(1, 2,"Do you want to continue(Y/N)?");
      while(!kbhit());
		key = getchar();
		if(key == 'Y' || key == 'y')
		{
      	calib = TRUE;
			break;
		}
		else if(key == 'N' || key == 'n')
		{
         calib = FALSE;
			break;
		}
	}
	while(kbhit()) getchar();
   if(calib)
   {
   	printf("\n\n");
      printf(" Select unit type you will be entering\n");
      printf(" -------------------------------------\n");
      printf(" 0 = Celsius\n");
      printf(" 1 = Fahrenheit\n");
      printf(" 2 = Kelvin\n");
		printf(" Select (0-2) =  ");
		do
		{
			temp_units = getchar();
		} while (!( (temp_units >= '0') && (temp_units <= '2')) );
		temp_units = temp_units - 0x30;
		printf("%d\n\n", temp_units);

		while(kbhit()) getchar();
		printf(" Enter the value from your temperature reference = ");
		gets(s);
		while(kbhit()) getchar();
		temperature = atof(s);

      // Create temperature offset constant and then write to flash.
      thermOffset(temp_units, temperature);
   }
   printf(" Thermistor adjustment completed...\n");
   printf(" Now display temperature continually...\n\n\n");
   while(1)
   {
     	sprintf(s, " Celsius=%.1f Fahrenheit=%.1f Kelvin=%.1f  \r",
      		  thermReading(0), thermReading(1), thermReading(2));
      printf("%s", s);
	}
}


