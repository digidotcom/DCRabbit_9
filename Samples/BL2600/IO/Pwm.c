/********************************************************************
	pwm.c
	Z-World, 2004

	Description
	-----------
   This program demonstrates the use of four PWM channels on port F
   pins PF4 - PF7 going to connector pins DIN20 - DIN23 respectively.
   The PWM signals are set for a frequency of 10 khz with the duty
   cycle adjustable from 1 to 99% by the user.

   Notes:
   1. Once the PWM driver sets up a given PWM channel (PF4 - PF7)
      then the corresponding digIn channel is no longer available
      for use by the application program.

	2. Must move the resistor bias option to GND, see
   	instructions below.

	Instructions:
	-------------
   1. Set resistor divider option on JP4 to GND.
   2. Compile and run this program.
   3. Change duty cycle for a given PWM channel via STDIO and view
      with either an oscilloscope or a voltmeter. When monitoring
      with a voltmeter you can compute the expected voltage as
      follows:

	   Vout = PWM percentage * 1.65V

*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


//------------------------------------------------------------------
// Option flags are used to enable features on an individual PWM
// channel. Bit masks for these are:
// PWM_SPREAD    - sets pulse spreading. The duty cycle is spread
//                 over four seperate pulses to increase the pulse
//                 frequency.
//	PWM_OPENDRAIN - sets the PWM output pin to be open-drain instead
//                 of a normal push-pull logic output.
//
// Default setting is for all options to be disabled.
//------------------------------------------------------------------
#define PWM0_OPTION 0
#define PWM1_OPTION 0
#define PWM2_OPTION 0
#define PWM3_OPTION 0


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

void set_duty_cycle(int channel, int dutycycle)
{
	auto int pwm;

   pwm = (int)(dutycycle/100.0 * 1024);
	switch(channel)
   {
   	case 0:
      	pwm_set(0, pwm, PWM0_OPTION);
         break;

   	case 1:
   		pwm_set(1, pwm, PWM1_OPTION);
         break;

   	case 2:
   		pwm_set(2, pwm, PWM2_OPTION);
         break;

   	case 3:
   		pwm_set(3, pwm, PWM3_OPTION);
         break;
   }
}

void main()
{
	auto int channel;
   auto int done, option;
   auto int initial_display;
   auto long freq;
   auto int dutycycle[4];
   auto char s[256];

   // Initialize the controller
	brdInit();

   // Request 10kHz PWM cycle (will select closest possible value)
	freq = pwm_init(10000ul);
   sprintf(s, "Actual PWM frequency for all channels is %luHz\n", freq);
	DispStr(1, 2, s);

   // Start PWM with default duty cycle for each channel
   dutycycle[0] = 1;
   dutycycle[1] = 25;
   dutycycle[2] = 50;
   dutycycle[3] = 99;
   set_duty_cycle(0, dutycycle[0]);
   set_duty_cycle(1, dutycycle[1]);
   set_duty_cycle(2, dutycycle[2]);
   set_duty_cycle(3, dutycycle[3]);

	sprintf(s, "<<< Duty Cycle...ch0 = %d%%  ch1 = %d%%  ch2 = %d%%  ch3 = %d%% >>>   ",
            dutycycle[0], dutycycle[1],
            dutycycle[2], dutycycle[3]);
   DispStr(1, 15, s);

   initial_display = TRUE;
	// Generate a PWM signal where the user can change the dutycycle
	while (1)
	{

   	if(!initial_display)
      {
      	DispStr(1, 4, "Select PWM Channel 0 - 3 (DIN20 - DIN23)= ");

 			do
			{
				channel = getchar();
			} while (!((channel >= '0') && (channel <= '3')));
			printf("%d", channel-=0x30);
		}
      else
      {
      	// Default to channel 0
      	DispStr(1, 4, "Select PWM Channel 0 - 3 (DIN20 - DIN23)=  0 ");
         initial_display = FALSE;
         channel = 0;
      }

      done = FALSE;
      while(!done)
      {
   		// Display usage message
			DispStr(1, 6,  "PWM control Menu");
			DispStr(1, 7,  "----------------");
			DispStr(1, 8,  "1. Increment PWM duty cycle");
			DispStr(1, 9,  "2. Decrement PWM duty cycle");
      	DispStr(1, 10, "3. Select new PWM channel");
         DispStr(1, 12, "Select Option 1 - 3 = ");

 			do
			{
				option = getchar();
			} while (!((option >= '1') && (option <= '3')));
			printf("%d", option-=0x30);

         switch(option)
			{
				case 1: // Increase dutycycle
					if(dutycycle[channel] < 99)
               {
               	dutycycle[channel]+= 1;
               }
               set_duty_cycle(channel, dutycycle[channel]);
               break;

         	case 2: // Decrease dutycycle
               if(dutycycle[channel] > 1)
               	dutycycle[channel]-= 1;
					set_duty_cycle(channel, dutycycle[channel]);
               break;

            case 3:
            	done = TRUE;
               blankScreen(3, 20);
               break;
         }
         sprintf(s, "<<< Duty Cycle...ch0 = %d%%  ch1 = %d%%  ch2 = %d%%  ch3 = %d%% >>>    ",
                dutycycle[0], dutycycle[1],
                dutycycle[2], dutycycle[3]);
         DispStr(1, 15, s);
      }
	}
}





