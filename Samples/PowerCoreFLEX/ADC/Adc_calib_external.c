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
	adc_calib_external.c

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	Description
	===========
	This program demonstrates how to recalibrate an external A/D channel
   using two known voltages to generate two coefficients, gain and offset,
   which will be rewritten	into simulated EEPROM in flash. The program
   will then will also continuously	display the voltage that has been
   calibrated.

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
	3. Voltage will be continuously displayed the external A/D channel.

***************************************************************************/
#class auto

#use "PowerCoreFLEX.lib"

// Use the common ISR library for the triac and the ADC
// ramp circuit.
#use "adctriac_isr.lib"
#use "adcramp.lib"


nodebug
void msDelay(unsigned int delay)
{
   auto unsigned long done_time;

   done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


void main()
{
   auto int i;
   auto long value1, value2;
   auto unsigned int rawdata;
   auto int key;
   auto float voltage, volts1, volts2;
   auto int calib;
   auto char buffer[256];

   // The brdInit function is only used to configure the I/O
   // on the prototyping board and is not required for the A/D
   // library which will configure everything required to access
   // the A/D circuit.
	brdInit();

   // Initialize the A/D ramp circuit and low-level driver.
   anaInRampInit();

   // Initialize the core module to read external A/D circuit.
   anaInExternalInit(3, 2, 1);

   while(1)
   {
      printf("Would you like to calibrate?");
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
   printf("\n\n");

   if(calib)
   {
      printf("\nAdjust to approx. 1v and then enter actual voltage = ");
      gets(buffer);
      while(kbhit()) getchar();

      volts1 = atof(buffer);
      value1 = 0;
      adc_conversion_done = FALSE;
   	for(i=0; i < 10; i++)
      {
     		// Use the adc_conversion_done flag to get 10 new
         // A/D conversions values.
         while(!adc_conversion_done);
         adc_conversion_done = FALSE;
      	value1 += anaIn(0);
      }
      value1 = value1 / 10;
      printf("lo:  channel=%d raw=%d\n", 0, value1);
      printf("\nAdjust to approx. 9v and then enter actual voltage = ");
      gets(buffer);
      while(kbhit()) getchar();

      volts2 = atof(buffer);
      value2 = 0;
   	adc_conversion_done = FALSE;
      for(i=0; i < 10; i++)
      {
      	// Use the adc_conversion_done flag to get 10 new
         // A/D conversions values.
         while(!adc_conversion_done);
         adc_conversion_done = FALSE;
      	value2 += anaIn(0);
      }
      value2 = value2 / 10;
      printf("hi:  channel=%d raw=%d\n", 0, value2);

      anaInCalib(0, (int)value1, volts1, (int)value2, volts2);
      anaInEEWr(0);

      // !!! Attention !!!
      // Must delay accessing the ramp A/D circuit after doing a write to
      // the program code flash, so the ramp A/D circuit can re-sync due to
      // interrupts being disabled.
      msDelay(50);
   }
   anaInEERd(0);
   adc_conversion_done = FALSE;
   for(;;)
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
   		rawdata = anaIn(0);
      	voltage = anaInVolts(0);
      	printf("Rawdata = %d Voltage = %.3f\n", rawdata, voltage);
     	}
   }
}

