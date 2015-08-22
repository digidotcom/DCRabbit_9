/********************************************************************
	triac_phase_flash.c
	Z-World, 2004

	Description
	-----------
   This program demonstrates phase-angle triac control with
   writing to application code flash (Does not apply to serial
   flashes).


   PowerCoreFLEX Prototyping Board Setup:
   --------------------------------
   1. Solder-in incandescent lamps provided in your kit into
      DS1 and DS2 (This step not needed if you're going to
      monitor with oscilloscope or you're using some other
      load circuit).


	Instructions:
	-------------
   1. Compile and run this program.
 	2. Then you can monitor the triac control pin with an
      oscilloscope to see the control pin going active at
      phase-angles 0 - 180. Control pins are located on
      connector J4 of the prototyping board as follows:

      J4 Pin#   Signal name
      -------	 -----------
        41      PF2_SCR-0
        40      PF3_SCR-1

   3. If incandescent lamps are installed, visually monitor
   	the lamps, you should see lamps go from being full ON
      to dimming down to being OFF.

   4. Select menu option 1 to observe (via steps 2 and/or 3
      listed above) the triac control being disabled when
      writing to application code flash.

*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

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

// Use the common ISR library for the triac and the ADC
// ramp circuit.
#use "adctriac_isr.lib"

// Use the triac library
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
 	auto int key, i, j;
   auto int triac0_cntrl, triac1_cntrl;
   auto char s[256];
   auto int userblock_flashwrite;

   // The brdInit function is only used to configure the I/O
   // on the prototyping board and is not required for the triac
   // library which will configure everything required for triac
   // control.
   brdInit();

 	triac_PhaseInit(1, 3);
   triac_PhaseCntrlPin(0, PFDR, 2, 0);
	triac_PhaseCntrlPin(1, PFDR, 3, 0);
   triac_PhaseInitPWM(0, 0, 300, 0, 10000ul);

   triac0_cntrl = 0;
   triac1_cntrl = 50;

   DispStr(1, 1,  "Triac Status");
   DispStr(1, 2,  "------------");
   sprintf(s,"Triac 0 ON, phase angle = %d   ", triac0_cntrl);
   DispStr(1, 3, s);
   sprintf(s,"Triac 1 ON, phase angle = %d   ", triac1_cntrl);
   DispStr(1, 4, s);


   DispStr(1, 6,  "Option Menu");
	DispStr(1, 7,  "-----------");
   DispStr(1, 8, "1. Write to flash");
   DispStr(1, 9, "2. Exit program");

  	userblock_flashwrite = FALSE;
   while (1)
	{
		costate
		{
         // Do asynchronous control of triac's
         if(!userblock_flashwrite)
         {
         	triac_PhaseCntrl(0, 1, triac0_cntrl, 0);
         	triac_PhaseCntrl(1, 1, triac1_cntrl, 0);
 			}
         else
         {
         	sprintf(s,"Triac 0 OFF, phase angle = 0  ");
   		  	DispStr(1, 3, s);
           	sprintf(s,"Triac 1 OFF, phase angle = 0  ");
   			DispStr(1, 4, s);

            // Disable triac driver...Its a non-blocking function which
            // takes approx. 50 ms to execute.
            waitfor(triac_PhaseDisable());

            // Write to application code flash
            writeUserBlock(0, "test", 4);

            // Re-enable triac driver...Its a non-blocking function which
            // takes approx. 750 ms to execute.
            waitfor(triac_PhaseEnable());

            // Delay so you can see STDIO display being updated
            waitfor(DelayMs(1000));
            DispStr(1, 10, "Select Option 1 - 2 >     ");
            userblock_flashwrite = FALSE;
            triac0_cntrl = 0;
   			triac1_cntrl = 50;
         }
      }

      costate
   	{
   		if(!userblock_flashwrite)
      	{
      		for(i=0; i < 180; i++)
      		{
            	if(triac0_cntrl > 180 || userblock_flashwrite)
               {
               	triac0_cntrl = 180;
               	break;
               }
            	sprintf(s,"Triac 0 ON, phase angle = %d  ", triac0_cntrl);
   				DispStr(1, 3, s);
               triac0_cntrl++;
        			yield;
      		}
      		for(i=0; i < 180; i++)
      		{
            	if(triac0_cntrl <= 0 || userblock_flashwrite)
        			{
        				triac0_cntrl = 0;
           			break;
          		}
            	sprintf(s,"Triac 0 ON, phase angle = %d  ", triac0_cntrl);
   				DispStr(1, 3, s);
        			triac0_cntrl--;\
        			yield;
         	}
      	}
   	}
   	costate
   	{
     		if(!userblock_flashwrite)
      	{
      		for(j=0; j < 180; j++)
      		{
            	if(triac1_cntrl > 180 || userblock_flashwrite)
               {
                 	triac1_cntrl = 180;
                	break;
               }
            	sprintf(s,"Triac 1 ON, phase angle = %d  ", triac1_cntrl);
   				DispStr(1, 4, s);
               triac1_cntrl++;
        			yield;
      		}
      		for(j=0; j < 180; j++)
      		{
            	if(triac1_cntrl <= 0 || userblock_flashwrite)
        			{
        				triac1_cntrl = 0;
           			break;
          		}
            	sprintf(s,"Triac 1 ON, phase angle = %d  ", triac1_cntrl);
   				DispStr(1, 4, s);
        			triac1_cntrl--;
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

            if( key >= '1' && key <='2')
				{
            	sprintf(s, "Select Option 1 - 2 > %c  ", key);
            	DispStr(1, 10, s);
            }
            switch(key)
            {
               case '1':
               	userblock_flashwrite = TRUE;
                 	break;
               case '2':
               	// Disable triac control driver before exiting
                 	waitfor(triac_PhaseDisable());
               	exit(0);
            }
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



