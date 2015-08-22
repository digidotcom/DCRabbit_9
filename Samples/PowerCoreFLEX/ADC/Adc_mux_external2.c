/***************************************************************************
	adc_mux_external2.c
	Z-World, 2004

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	Description
	===========
   This program demonstrates how to implement MUX control for reading
   external A/D channels randomly in your application program. This
   program will also read and display the voltage of the external
   A/D channel that is located on the PowerCoreFLEX prototyping board.

   Since there's only one A/D on the prototyping board, the MUX control
   will be simulated by using the prototyping board LED0 (labeled DS5 on
   prototyping board) controlled by I/O bit PD5.

   LED = OFF = A/D Channel0
   LED = ON  = A/D channel1

   Implementation of Mux routine for random A/D channel selection
   ==============================================================
   1. Define number of A/D channels. change default from 1 to number of
   	A/D channels in your design.

   	#define MAX_ADCHANNELS <Number Channels>

   2. Provide a application MUX function to provide the following:

      a) ADC channel selection via your hardware MUX circuit.
      b) Change _adc_mux_channel index to the channel selected.
      c) Add delay for channel switching settling time.
      c) Set adc_conversion_done flag FALSE.


   3. Add I/O initialization code for port pins used for MUX control at
   the beginning of your application program.


   4. Then do:
   a) Call your routine with ADC channel selected.
   b) Wait in a non-blocking wait routine for the
      adc_conversion_done flag to become TRUE.
   c) Read ADC.
   d) Start sequence over again.

   5. End of steps.

   To find the above steps in this program search for "MUX control" to
   see the various steps implemented.

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

// Step 1 for MUX control.
#define MAX_ADCHANNELS 2
// End of step


// Use the common ISR library for the triac and the ADC
// ramp circuit.
#use "adctriac_isr.lib"
#use "adcramp.lib"


// Step 2 for MUX control, A/D mux control function.
void anaInMux(int channel)
{
   switch(channel)
   {
   	case 0:
      	BitWrPortI(PDDR, &PDDRShadow, 1, 5);
         _adc_mux_channel = channel;
         break;

      case 1:
      	BitWrPortI(PDDR, &PDDRShadow, 0, 5);
         _adc_mux_channel = channel;
         break;
   }
}

// End of step


int read_ad_channel(int channel, int samples, float *voltage)
{

	static unsigned long settling_time;
   static int state;
   static int index, max_cnt;
   static int status;
   static int mux_isr_sync;
   static int last_channel;

   #GLOBAL_INIT
   {
   	state = 0;
      last_channel = -1;
   }

   switch(state)
   {
   	case 0:
      	// Switch to the desired A/D channel to read
      	if(last_channel != channel)
         {
   			anaInMux(channel);
            settling_time = MS_TIMER + 1;
         	state = 1;
         	status = 0;
            last_channel = channel;
         }
         else // Reading previous selected A/D channel
         {
         	index = 0;
         	max_cnt = samples;
         	mux_isr_sync = TRUE;
         	state = 2;
         	status = 0;
         }
         break;

      case 1:
      	// Wait for the A/D mux settling time to finish
			if((long) (MS_TIMER - settling_time) >= 0 && adc_conversion_done)
         {
         	index = 0;
         	max_cnt = samples;
            adc_conversion_done = FALSE;
            mux_isr_sync = FALSE;
            state = 2;
         }
      	break;

      // Wait for conversion done flag to become true, then go to state 3
      case 2:
      	if(adc_conversion_done && mux_isr_sync)
         {
         	// Read selected A/D channel
            if(index < max_cnt)
         	{
         		voltage[index++] = anaInVolts(channel);
         	}
         	else  // Completed reading requested number of A/D samples
         	{
         		state = 0;
            	status = 1;
         	}
            adc_conversion_done = FALSE;
         }
         else if(adc_conversion_done)
         {
         	// Parital A/D cycle finished, now ready to read the selected A/D channel
            // on the next conversion cycle.
         	mux_isr_sync = TRUE;
         }
         break;

      default:
      	state = 0;
         status = 0;
         break;
   }
   return(status);
}



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
   auto char s[256];
   auto unsigned long settling_time;
   auto float voltages[10];
   auto float voltage;
   int index;


   // The brdInit function is only used to configure the I/O
   // on the prototyping board and is not required for the A/D
   // library which will configure everything required to access
   // the A/D circuit.
	brdInit();

   // Step 3 for MUX control
   // Make sure is low when the line is enabled.
   WrPortI(PDDR,  &PDDRShadow,  (PDDRShadow  & ~0x20));

   // Set Function for I/O opeartion
   WrPortI(PDFR,  &PDFRShadow,  (PDFRShadow  & ~0x20));

   // Make PD5 an output
   WrPortI(PDDDR, &PDDDRShadow, (PDDDRShadow | 0x20));

   // Set PD5 for active high/low operation
   WrPortI(PDDCR, &PDDCRShadow, 0x00);

   // Set Port D to be CLK'd by PCLK/2
   WrPortI(PDCR,  &PDCRShadow,  0x00);
   // End of step

   // Initialize the A/D ramp circuit and low-level driver.
   anaInRampInit();

   // Initialize the core module to read external A/D circuit.
   anaInExternalInit(3, 2, 1);


   // Copy calibration data from ch0 to ch1, since we are simulating the
   // 2nd channel....must be done after executing anaInExternalInit.
   _adcCalib[1][0] = _adcCalib[0][0];
   _adcCalib[1][1] = _adcCalib[0][1];

  	adc_conversion_done = FALSE;
	while (1)
	{
      blankScreen(0, 20);
		DispStr(1, 2,  "External A/D voltage for channel 0 and 1");
		DispStr(1, 3,  "----------------------------------------");

    	while(1)
      {
      	costate
         {
         	waitfor(read_ad_channel(0, 10, voltages));
            for(index = 0; index < 10; index++)
				{
      			voltage = voltages[index];
            	if(voltage != ADOVERFLOW)
         			sprintf(s, "Ch0 Voltage = %.3f               ", voltage);
            	else
               	sprintf(s, "Ch0 Voltage = Exceeded Range!!!  ");
           	 	DispStr(1,index + 4, s);
				}


         	waitfor(read_ad_channel(1, 10, voltages));
            for(index = 0; index < 10; index++)
				{
      			voltage = voltages[index];
            	if(voltage != ADOVERFLOW)
         			sprintf(s, "Ch1 Voltage = %.3f               ", voltage);
            	else
               	sprintf(s, "Ch1 Voltage = Exceeded Range!!!  ");
           	 	DispStr(1,index + 14, s);
				}
            DispStr(1, 26,  "Press Q or q to exit program.");
         }

         costate
         {
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
}

