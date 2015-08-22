/********************************************************************
	triac_ratio_flash.c
	Z-World, 2004

	Description
	-----------
   This program demonstrates time-proportional triac control with
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
   2. Monitor the triacs cycle ON and OFF automatically incrementing
      through the following ratio ranges

      DS1 = 0/5 - 5/5
      DS2 = 0/10 - 10/10

      Monitor triacs either visually via DS1 and DS2 or by monitoring
      the triac control pins with an oscilloscope located on connector
      J4 of the prototyping board as follows:

      J4 Pin#   Signal name
      -------	 -----------
        41      PF2_SCR-0
        40      PF3_SCR-1

   3. Select menu option 1 to observe (via step 2 listed above) the
   	triac control being disabled when writing to application code
      flash.

*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Sample library for PowerCoreFLEX series core modules
#use "PowerCoreFLEX.lib"

// Define time-proportional triac control method for proper
// library compilation.
#define TIMEPROPORTIONAL

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
	auto int key, i;
   auto int triac_cntrl0, triac_cntrl1;
	auto int increment, decrement;
   auto int triac;
   auto char s[256];
   auto int userblock_flashwrite;

   // The brdInit function is only used to configure the I/O
   // on the prototyping board and is not required for the triac
   // library which will configure everything required for triac
   // control.
   brdInit();

   triac_TimePropInit(1, 3);
   triac_TimePropCntrlPin(0, PFDR, 2, 0);
   triac_TimePropCntrlPin(1, PFDR, 3, 0);
   triac_cntrl0 = 0;
   triac_cntrl1 = 0;
   triac = 0;

   DispStr(1, 1,  "Triac Status");
   DispStr(1, 2,  "------------");
   sprintf(s,"Triac 0 ON/OFF ratio = %d/10  ", triac_cntrl0);
   DispStr(1, 3, s);

   sprintf(s,"Triac 1 ON/OFF ratio = %d/5   ", triac_cntrl1);
   DispStr(1, 4, s);
   sprintf(s,"Triac selected = %d", triac);
   DispStr(1, 5, s);

  	DispStr(1, 7,  "Option Menu");
	DispStr(1, 8,  "-----------");
   DispStr(1, 9,  "1. Write to flash");
   DispStr(1, 10, "2. Exit program");

  	userblock_flashwrite = FALSE;
	while (1)
	{
      costate
      {
      	waitfor(DelayMs(1000));
         sprintf(s,"Triac 0 ON/OFF ratio = %d/10  ", triac_cntrl0);
   		DispStr(1, 3, s);
   		sprintf(s,"Triac 1 ON/OFF ratio = %d/5   ", triac_cntrl1);
   		DispStr(1, 4, s);
   		sprintf(s,"Triac selected = %d", triac);
   		DispStr(1, 5, s);
      	if(!userblock_flashwrite)
         {
      		triac_TimePropCntrl(0, triac_cntrl0, 10);
            triac_TimePropCntrl(1, triac_cntrl1, 5);
            if(++triac_cntrl0 > 10)
                triac_cntrl0 = 0;
            if(++triac_cntrl1 > 5)
           		triac_cntrl1 = 0;
         }
   		else
         {
      		sprintf(s,"Triac 0 ON/OFF ratio = 0/10");
   			DispStr(1, 3, s);

   			sprintf(s,"Triac 1 ON/OFF ratio = 0/5");
   			DispStr(1, 4, s);

            // Disable triac driver...Its a non-blocking function which
            // takes approx. 50 ms to execute.
            waitfor(triac_TimePropDisable());

            // Write to application code flash
            writeUserBlock(0, "test", 4);

            // Re-enable triac driver...Its a non-blocking function which
            // takes approx. 750 ms to execute.
            waitfor(triac_TimePropEnable());

            // Delay so you can see STDIO display being updated
            waitfor(DelayMs(5000));
            DispStr(1, 11, "Select Option 1 - 2 >     ");
            userblock_flashwrite = FALSE;
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
            	DispStr(1, 11, s);
            }
            switch(key)
            {
               case '1':
               	userblock_flashwrite = TRUE;
                 	break;
               case '2':
               	// Disable triac control driver before exiting
                 	waitfor(triac_TimePropDisable());
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
// Must save-off (push/pop) registers used by this routine.
//------------------------------------------------------------------------

#asm root nodebug
triac_gate_on::
	push	af
   push	bc
  	push	de

  	ld		a,l
   cp		a,0x00
   jr		nz, .triac1
.triac0:
   ld		a,0x04
   ld 	de,PFDR 	; Port address
	ld 	hl,PFDRShadow
  	or		a,(hl)
  	ld 	(hl),a
   ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port
   jr		.triac_exit

.triac1:
   ld		a,0x08
   ld 	de,PFDR 	; Port address
	ld 	hl,PFDRShadow
   or		a,(hl)
   ld 	(hl),a
	ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port

.triac_exit:
	pop	af
   pop	bc
  	pop	de

   ret
#endasm

#asm root nodebug
triac_gate_off::
	push	af
   push	bc
  	push	de

  	ld		a,l
   cp		a,0x00
   jr		nz, .triac1

.triac0:
   ld		a,0x04
   ld 	de,PFDR 	; Port address
 	ld 	hl,PFDRShadow
   cpl
   and	a,(hl)
   ld    (hl),a
   ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port
   jr		.triac_exit

.triac1:
   ld		a,0x08
   ld 	de,PFDR 	; Port address
	ld 	hl,PFDRShadow
   cpl
   and	a,(hl)
   ld    (hl),a
   ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port

.triac_exit:
	pop	af
   pop	bc
  	pop	de

   ret
#endasm


