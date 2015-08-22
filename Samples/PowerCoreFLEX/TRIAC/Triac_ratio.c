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
	triac_ratio.c

	Description
	-----------
   This program demonstrates time-proportional triac control with
   using the PowerCoreFLEX series core module and prototyping board.

   Time-proportional triac control provides you with the ability
   to control a triac over a fixed number of 50/60 Hz A/C cycles.
   With setting the triac ON/OFF ratio you will be able to obtain
   the desired control for your hardware application.

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
   3. Set the ON/OFF ratio of the selected triac via STDIO, and
   	monitor the triacs control pin with an oscilloscope, you
      should see the triacs cycle per the ON/OFF ratio you have
      selected.
	4. If the incandescent lamps are installed, you should see the
      lamps cycle the ON/OFF ratio per your selection. When you
      first activate the lamps you should see DS2 blink twice as
      fast as DS1.
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
   sprintf(s,"Triac 0 ON/OFF ratio = %d/10", triac_cntrl0);
   DispStr(1, 3, s);

   sprintf(s,"Triac 1 ON/OFF ratio = %d/5", triac_cntrl1);
   DispStr(1, 4, s);
   sprintf(s,"Triac selected = %d", triac);
   DispStr(1, 5, s);

   DispStr(1, 7,  "Triac control Menu");
	DispStr(1, 8,  "------------------");
   DispStr(1, 9,  "1. Select triac 0 for control");
   DispStr(1, 10, "2. Select triac 1 for control");
	DispStr(1, 11, "3. Increment number of ON cycles");
	DispStr(1, 12, "4. Decrement number of ON cycles");
   DispStr(1, 13, "Select Option 1 - 4 > ");

	while (1)
	{
      costate
      {
      	switch(triac)
         {
         	case 0:
      			triac_TimePropCntrl(0, triac_cntrl0, 10);
               break;

            case 1:
               triac_TimePropCntrl(1, triac_cntrl1, 5);
               break;
         }
      }
		costate
		{
         if(kbhit())
			{
				key = getchar();
            while(kbhit()) getchar();
            msDelay(500);
            if( key >= '0' && key <='4')
				{
            	sprintf(s, "Select Option 1 - 4 > %c  ", key);
            	DispStr(1, 13, s);
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
                        if(triac_cntrl0 < 10)
                        	triac_cntrl0++;
                     	break;

                  	case 1:
                     	if(triac_cntrl1 < 5)
                  			triac_cntrl1++;
                     	break;
               	}
                  break;

               case '4':
             		switch(triac)
               	{
                		case 0:
                   		if (triac_cntrl0 > 0 )
                        	triac_cntrl0--;
                     	break;
                  	case 1:
                  		if (triac_cntrl1 > 0 )
                        	triac_cntrl1--;
                    	 	break;
               	}
                  break;


     			}
            sprintf(s,"Triac 0 ON/OFF ratio = %d/10   ", triac_cntrl0);
   			DispStr(1, 3, s);
         	sprintf(s,"Triac 1 ON/OFF ratio = %d/5    ", triac_cntrl1);
   			DispStr(1, 4, s);
         	sprintf(s,"Triac selected = %d", triac);
   			DispStr(1, 5, s);
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


