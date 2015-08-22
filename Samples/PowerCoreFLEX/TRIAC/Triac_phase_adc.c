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
/********************************************************************
	triac_phase_adc.c

	Description
	-----------
   This program demonstrates phase-angle triac control and reading
   the A/D ramp circuit with using the PowerCoreFLEX series core module
   and prototyping board.

   Phase-angle triac control provides you with the ability to
   fire a triac at a given phase angle of an 50/60 Hz A/C
   sinewave to provide the desired control for your hardware
   application.

   PowerCoreFLEX Prototyping Board Setup:
   --------------------------------
   1. Solder-in incandescent lamps provided in your kit into
      DS1 and DS2 (This step not needed if you're going to
      monitor with oscilloscope or you're using some other
      load circuit).


	Instructions:
	-------------
   1. Compile and run this program.
   2. Select the desired triac via STDIO.
   3. Set the phase angle of where to fire the selected triac via
      STDIO. Then you can monitor the triac control pin with an
      oscilloscope to see the control pin going active at the
      phase angle you have selected. Control pins are located
      on the connector J4 of the prototyping board as follows:

      J4 Pin#   Signal name
      -------	 -----------
        41      PF2_SCR-0
        40      PF3_SCR-1
   4. If incandescent lamps are installed, visually monitor
   	the lamps with option 7 (ramping triacs) selected, you
      should see lamps go from being full ON to dimming down
      to being OFF.
   5. View the A/D ramp output voltages via STDIO for the 2.5v
   	voltage reference, end-of-ramp-voltage, and thermistor.

*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
//#use "PowerCoreFLEX.lib"

// Define phase-angle triac control method for proper
// library compilation.
#define PHASECONTROL

// Set the max number of triacs for application control
#define MAX_TRIACS 2

// Define the triac custom function ON/OFF function names
// for proper library compilation.
#define TRIAC_GATE_ON 	triac_gate_on
#define TRIAC_GATE_OFF 	triac_gate_off

// Function proptypes for the custom I/O routines.
void triac_gate_on(int triac);
void triac_gate_off(int triac);


// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"


// Use the common ISR library for the triac and the ADC
// ramp circuit.
#use "adctriac_isr.lib"

// Use the triac and A/D ramp library
#use "adcramp.lib"
#use "triac.lib"

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

main()
{
 	auto int counter, key, i, j;
   auto int triac0_cntrl, triac1_cntrl;
	auto int increment, decrement;
   auto int triac;
   auto int onOff0, onOff1;
   auto char s[256];
   auto char triac0_state[10];
   auto char triac1_state[10];
   auto int triac0_auto_sweep, triac1_auto_sweep;
   auto float Vread0, Vread1, Vread2;


   // The brdInit function is only used to configure the I/O
   // on the prototyping board and is not required for the triac
   // library which will configure everything required for triac
   // control.
   brdInit();

 	triac_PhaseInit(1, 3);
   triac_PhaseCntrlPin(0, PFDR, 2, 0);
	triac_PhaseCntrlPin(1, PFDR, 3, 0);
   triac_PhaseInitPWM(0, 0, 300, 0, 10000ul);



   // Initialize the A/D ramp circuit and low-level driver.
   anaInRampInit();


   triac0_auto_sweep = triac1_auto_sweep = TRUE;
   onOff0 = onOff1 = 1;
   triac0_cntrl = 0;
   triac1_cntrl = 50;
   triac = 0;
   strcpy(triac0_state, "ON");
   strcpy(triac1_state, "ON");


   DispStr(1, 1,  "ADC/Triac Status");
   DispStr(1, 2,  "----------------");
   DispStr(1, 3, "RefVolt = 0.000 RampVolts = 0.000 TempVolt = 0.000    \n");
   sprintf(s,"Triac selected = %d", triac);
   DispStr(1, 4, s);
   sprintf(s,"Triac 0 ON, phase angle = %d   ", triac0_cntrl);
   DispStr(1, 5, s);
   sprintf(s,"Triac 1 ON, phase angle = %d   ", triac1_cntrl);
   DispStr(1, 6, s);

   DispStr(1, 9,  "Triac control Menu");
	DispStr(1, 10,  "------------------");
   DispStr(1, 11, "1. Select triac 0 for control");
   DispStr(1, 12, "2. Select triac 1 for control");
   DispStr(1, 13, "3. Turn triac OFF");
   DispStr(1, 14, "4. Set triac to a phase-angle of 90");
	DispStr(1, 15, "5. Decrement phase angle of selected triac");
   DispStr(1, 16, "6. Increment phase angle of selected triac");
   DispStr(1, 17, "7. Enable phase-angle ramping of selected triac");
   DispStr(1, 19, "Select Option 1 - 7 > ");
   counter = 0;
   ref_conversion_done = FALSE;
   ramp_conversion_done = FALSE;
   temp_conversion_done = FALSE;
   while (1)
	{

   	costate
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
      		// Continually read and display the voltage for ramp A/D channels.
      		Vread0 = anaInRampVolts(0);
      		Vread1 = anaInRampVolts(1);
      		Vread2 = anaInRampVolts(2);
      	 	sprintf(s, "RefVolt = %.3f RampVolts = %.3f TempVolt = %.3f Counter %d\n",
                Vread0, Vread1, Vread2, counter++);
            DispStr(1, 3, s);
         	ref_conversion_done = FALSE;
         	ramp_conversion_done = FALSE;
         	temp_conversion_done = FALSE;
      	}
			waitfor(DelayMs(1000));
		}

		costate
		{
         // Do asynchronous control of triac's
         triac_PhaseCntrl(0, onOff0, triac0_cntrl, 0);
         triac_PhaseCntrl(1, onOff1, triac1_cntrl, 0);
 		}

      costate
   	{
   		if(triac0_auto_sweep)
      	{
      		for(i=0; i < 180; i++)
      		{
            	sprintf(s,"Triac 0 %s, phase angle = %d  ", triac0_state, triac0_cntrl);
   				DispStr(1, 5, s);

            	if(!triac0_auto_sweep || (triac0_cntrl >= 180))
               	break;
        			triac0_cntrl++;
        			yield;
      		}
      		for(i=0; i < 180; i++)
      		{
            	sprintf(s,"Triac 0 %s, phase angle = %d  ", triac0_state, triac0_cntrl);
   				DispStr(1, 5, s);

            	if(!triac0_auto_sweep)
               	break;
        			triac0_cntrl--;
        			if(triac0_cntrl == 0)
        			{
        				triac0_cntrl = 1;
           			break;
          		}
        			yield;
         	}
      	}
   	}
   	costate
   	{
     		if(triac1_auto_sweep)
      	{
      		for(j=0; j < 180; j++)
      		{
            	sprintf(s,"Triac 1 %s, phase angle = %d  ", triac1_state, triac1_cntrl);
   				DispStr(1, 6, s);

            	if(!triac1_auto_sweep || (triac1_cntrl >= 180))
               	break;
        			triac1_cntrl++;
        			yield;
      		}
      		for(j=0; j < 180; j++)
      		{
            	sprintf(s,"Triac 1 %s, phase angle = %d  ", triac1_state, triac1_cntrl);
   				DispStr(1, 6, s);
            	if(!triac1_auto_sweep)
               	break;
        			triac1_cntrl--;
        			if(triac1_cntrl == 0)
        			{
        				triac1_cntrl = 1;
           			break;
          		}
        			yield;
      		}
      	}
   	}

      costate
		{
         if(kbhit())
			{
				key = getchar();
            while(kbhit()) getchar();

            if( key >= '1' && key <='8')
				{

            	sprintf(s, "Select Option 1 - 7 > %c  ", key);
            	DispStr(1, 19, s);
            }
            switch(key)
            {

               case '1':
               	triac = 0;
                  break;

               case '2':
               	triac = 1;
               	break;

               case '3':
               	switch(triac)
                  {
                   	case 0:
                     	triac0_auto_sweep = FALSE;
                     	onOff0 = 0;
                        strcpy(triac0_state, "OFF");
                        break;
                     case 1:
                     	triac1_auto_sweep = FALSE;
                     	onOff1 = 0;
                        strcpy(triac1_state, "OFF");
                        break;
                  }
                  break;

               case '4':
                 	switch(triac)
                  {
                   	case 0:
                     	strcpy(triac0_state, "ON");
                     	triac0_auto_sweep = FALSE;
                     	onOff0 = 1;
                        triac0_cntrl = 90;
                        break;
                     case 1:
                     	strcpy(triac1_state, "ON");
                     	triac1_auto_sweep = FALSE;
                     	onOff1 = 1;
                        triac1_cntrl = 90;
                        break;
                  }
                  break;


               case '5':
             		switch(triac)
               	{
                		case 0:
                     	strcpy(triac0_state, "ON");
                     	triac0_auto_sweep = FALSE;
                   		if (triac0_cntrl > 0 )
                        	triac0_cntrl--;
                           onOff0 = 1;
                     	break;
                  	case 1:
                       	strcpy(triac1_state, "ON");
                     	triac1_auto_sweep = FALSE;
                  		if (triac1_cntrl > 0 )
                        	triac1_cntrl--;
                           onOff1 = 1;
                    	 	break;
               	}
                  break;

               case '6':
               	switch(triac)
               	{
                		case 0:
                     	strcpy(triac0_state, "ON");
                     	triac0_auto_sweep = FALSE;
                        if(triac0_cntrl < 179)
                        	triac0_cntrl++;
                           onOff0 = 1;
                     	break;

                  	case 1:
                     	strcpy(triac1_state, "ON");
                     	triac1_auto_sweep = FALSE;
                     	if(triac1_cntrl < 179)
                  			triac1_cntrl++;
                           onOff1 = 1;
                     	break;
               	}
                  break;

               case '7':
               	switch(triac)
               	{
                		case 0:
                     	strcpy(triac0_state, "ON");
                     	onOff0 = 1;
                        triac0_cntrl = 0;
                        triac0_auto_sweep = TRUE;
                     	break;

                  	case 1:
                     	strcpy(triac1_state, "ON");
                     	onOff1 = 1;
                     	triac1_cntrl = 0;
                        triac1_auto_sweep = TRUE;
                     	break;
               	}
                  break;

     			}
            sprintf(s,"Triac selected = %d", triac);
   			DispStr(1, 4, s);
   			sprintf(s,"Triac 0 %s, phase angle = %d  ", triac0_state, triac0_cntrl);
   			DispStr(1, 5, s);
   			sprintf(s,"Triac 1 %s, phase angle = %d  ", triac1_state, triac1_cntrl);
   			DispStr(1, 6, s);
			}
   	}
	}
}


//----------------------------------------------------------------------
// Custom triac control routines, triac_gate_on() and triac_gate_off().
// These routines are already set up for the PowerCoreFLEX series core module
// prototyping board. If you have a custom design you will need to change
// the routines to match the I/O port pins that you're using.
//
// The following registers are available for use by this routine:
// Registers...af, bc, de, hl, ix, and iy.
//------------------------------------------------------------------------
#asm root nodebug
triac_gate_on::
   ld		a,l
   ld 	de,PFDR
	ld 	hl,PFDRShadow
   or		a,(hl)
   ld    (hl),a
	ioi 	ldd
   ret
#endasm


#asm root nodebug
triac_gate_off::
   ld		a,l
   cpl
   ld 	de,PFDR 	; Port address
	ld 	hl,PFDRShadow
   and	a,(hl)
   ld    (hl),a
	ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port
   ret
#endasm



