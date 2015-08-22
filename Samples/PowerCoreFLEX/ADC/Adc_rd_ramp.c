/***************************************************************************
	adc_rd_ramp.c
	Z-World, 2004

	This sample program is for the PowerCoreFLEX series core modules.

	Description
	===========
   Reads and displays the voltage of the following PowerCoreFLEX core module
   ramp A/D channels.

   CH Function
   -- --------------
   0  2.5V reference.
   1  End-of-ramp voltage.
   2  Thermistor.

   The voltage is calculated from coefficients read from simulated
   EEPROM in flash that were written there from the calibration
   process.


	Instructions
	============
	1. Compile and run this program.
	2. Follow the prompted directions of this program during execution.
	3. Voltage will be continuously displayed of the PowerCoreFLEX A/D channels.

***************************************************************************/

#class auto	 // Change local var storage default to "auto"

// Sample library for PowerCoreFLEX series core modules
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
	auto int channel;
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

	while (1)
	{
      blankScreen(0, 20);
		DispStr(1, 2,  "Ramp A/D voltages for channels 0 - 2");
		DispStr(1, 3,  "------------------------------------");
   	DispStr(1, 6, "Press Q or q to exit program.");
      ref_conversion_done = FALSE;
      ramp_conversion_done = FALSE;
      temp_conversion_done = FALSE;
   	while(1)
      {
   		// Notes:
      	// 1. The conversion flags are predefined and will be set
   		// by the ADC low-level driver.
      	// 2. If the conversion flags aren't used, and if you read
      	// the ramp A/D channel faster than the A/D conversion rate,
      	// then the data will be from the previous A/D conversion until a
      	// new A/D conversion is completed.
      	if(ref_conversion_done && ramp_conversion_done && temp_conversion_done)
      	{
				for(channel = 0; channel < 3; channel++)
				{
      			voltage = anaInRampVolts(channel);
            	if(voltage != ADOVERFLOW)
         			sprintf(s, "Channel%2d Voltage = %.3f               ", channel, voltage);
            	else
               	sprintf(s, "Channel%2d Voltage = Exceeded Range!!!  ", channel);
            	DispStr(1,channel + 4, s);
				}
            ref_conversion_done = FALSE;
         	ramp_conversion_done = FALSE;
         	temp_conversion_done = FALSE;
         }
         if(kbhit())
			{
				key = getchar();
				if (key == 'Q' || key == 'q')		// check if it's the q or Q key
				{
            	anaInDisable();
            	exit(0);
          	}
			}
      }
   }
}

