/********************************************************************
   thermistor.c
   Rabbit Semiconductor, 2006

   This program is used with RabbitFLEX SBC40 boards.

   Description
   ===========
   This program demonstrates how to read the thermistor sensor
   on a PowerCoreFLEX series core module that is on a RabbitFLEX
   SBC40.

   Instructions
   ============
   1. Compile and run this program.
   2. Apply heat and cold air to the thermistor to observe
      change in temperature in the stdio window.

********************************************************************/
#class auto

// set the STDIO cursor location and display a string
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}

main()
{
   auto float temperature;
   auto float Vread;
   auto int thermistor_rawdata;
   auto int adchannel, units;
   auto int medium_rate, slow_rate;
   auto unsigned long med_delay, slow_delay;
   auto char s[256];
   auto char u[3];

   // Initialize the I/O on the RabbitFLEX SBC40 board
   brdInit();

   // Initialize the A/D ramp circuit and low-level driver.
   anaInRampInit();


   // Initialize variables
   units = 0;
   adchannel = 2;
   medium_rate = slow_rate = 0;
   temperature = Vread = 0.0;
   thermistor_rawdata = 0;
   u[0] = 'C';
   u[1] = 'F';
   u[2] = 'K';

   // Clear temperature conversion flag.
   // Note: The temp_conversion_done flag is predefined and will be
   // set by the ADC low-level driver.
   temp_conversion_done = FALSE;

   printf(" Read Thermistor Example\n");
   printf(" -----------------------\n");
   DispStr(1,7,"Press 'F' for Farenheit, 'C' for Celcius, or 'K' for Kelvin");

   while (1)
   {
      //******************************************************
      // Fast conversion rate method
      //******************************************************
      // Set temperature updates for 9ms intervals.
      //
      // Notes:
      // 1. The temp_conversion_done flag is predefined and will be
      // set by the ADC low-level driver.
      // 2. If the temp_conversion_done flag isn't used, and if you read
      // the thermistor A/D channel faster than the A/D conversion rate,
      // then the data will be from the previous A/D conversion until a
      // new A/D conversion is completed.

      if(temp_conversion_done)
      {
         // Clear temperature conversion flag
         temp_conversion_done = FALSE;
         temperature = thermReading(units);
         thermistor_rawdata = anaInRamp(adchannel);
         Vread = anaInRampVolts(adchannel);
         sprintf(s, "Fast Update...Temp = %.1f%c TempRawdata = %d TempVoltage = %.3f        \r",
                 temperature, u[units], thermistor_rawdata, Vread);
         DispStr(1, 2, s);
      }


      //******************************************************
      // Medium conversion rate method
      //******************************************************
      switch(medium_rate)
      {
         case 0:
         {
            // Set temperature updates for 100ms intervals
            med_delay = MS_TIMER + 100;
            medium_rate = 1;
            break;
         }

         case 1:
         {
            if((long)(MS_TIMER-med_delay) >= 0 )
            {
               temperature = thermReading(units);
               thermistor_rawdata = anaInRamp(adchannel);
               Vread = anaInRampVolts(adchannel);
               sprintf(s, "Med  Update...Temp = %.1f%c TempRawdata = %d TempVoltage = %.3f        \r",
                       temperature, u[units], thermistor_rawdata, Vread);
               DispStr(1, 3, s);
               medium_rate = 0;
            }
            break;
         }
      }

      //******************************************************
      // Slow conversion rate method
      //******************************************************
      switch(slow_rate)
      {
         case 0:
         {
            // Set temperature updates for 2 seconds intervals
            slow_delay = SEC_TIMER + 2;
            slow_rate = 1;
            break;
         }

         case 1:
         {
            if((long)(SEC_TIMER-slow_delay) >= 0 )
            {
               temperature = thermReading(units);
               thermistor_rawdata = anaInRamp(adchannel);
               Vread = anaInRampVolts(adchannel);
               sprintf(s, "Slow Update...Temp = %.1f%c TempRawdata = %d TempVoltage = %.3f        \r",
                       temperature, u[units], thermistor_rawdata, Vread);
               DispStr(1, 4, s);
               slow_rate = 0;
            }
            break;
         }
      } //switch
      if (kbhit())
      {
         *s = toupper(getchar());
         if (*s=='K')
            units = 2;
         else if (*s=='C')
            units = 0;
         else if (*s =='F')
            units = 1;
      }
   }
}