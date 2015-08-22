/***************************************************************************
	adc_rd_external.c
	Z-World, 2004

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	Description
	===========
   Reads and displays the voltage of the external A/D located on the
   PowerCoreFLEX prototyping board. The voltage is calculated from coefficients
   read from simulated EEPROM in flash.

   Power supply Connections
   ========================
   1. Connect the positive power supply lead to the A/D channel input
      located on J3 pin 7 on the prototyping board.
	2.	Connect the negative power supply lead to GND located on J3 pin 8
      on the prototyping board.

	Instructions
	============
	1. Compile and run this program.
	2. Follow the prompted directions of this program during execution.
	3. Voltage will be continuously displayed for the external A/D channel.

***************************************************************************/
#class auto

#use "PowerCoreFLEX.lib"

// Use the common ISR library for the triac and the ADC
// ramp circuit.
#use "adctriac_isr.lib"
#use "adcramp.lib"


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
	auto unsigned int rawdata;
	auto int channel, keypress;
	auto int key;
	auto float voltage;
   auto char s[256];

   // The brdInit function is only used to configure the I/O
   // on the prototyping board and is not required for the A/D
   // library which will configure everything required to access
   // the A/D circuit.
	brdInit();

   // Initialize the A/D ramp circuit and low-level driver.
   anaInRampInit();

   // Initialize the core module to read external A/D circuit.
   anaInExternalInit(3, 2, 1);

	while (1)
	{
      blankScreen(0, 20);
		DispStr(1, 2,  "External A/D input voltage for channel 0");
		DispStr(1, 3,  "----------------------------------------");
      DispStr(1, 6,  "Press Q or q to exit program.");

    	while(1)
      {
   		// Notes:
      	// 1. The adc_conversion_done flag is predefined and will be set
   		// by the ADC low-level driver.
      	// 2. If the adc_conversion_done flag isn't used, and if you read
      	// the external A/D channel faster than the A/D conversion rate,
      	// then the data will be from the previous A/D conversion until a
      	// new A/D conversion is completed.
      	if(adc_conversion_done)
         {
            adc_conversion_done = FALSE;
      		voltage = anaInVolts(0);
            if(voltage != ADOVERFLOW)
         		sprintf(s, "Voltage = %.3f               ", voltage);
            else
               sprintf(s, "Voltage = Exceeded Range!!!  ");
            DispStr(1, 0 + 4, s);
         }
         if(kbhit())
			{
				key = getchar();
				if (key == 'Q' || key == 'q')
				{
            	anaInDisable();
      			exit(0);
     			}
			}
      }
   }
}

