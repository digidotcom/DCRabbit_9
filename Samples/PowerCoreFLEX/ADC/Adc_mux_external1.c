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
	adc_mux_external1.c

 	This program is used with PowerCoreFLEX series core modules with the
   prototyping board.

	Description
	===========
   This program demonstrates how to enable the A/D interrupt MUX routine
   for designs with multiple external A/D channels. This program will also
   read and display the voltage of the external A/D channel that is located
   on the PowerCoreFLEX prototyping board.

   Since there's only one A/D on the prototyping board, the MUX control
   will be simulated by using the prototyping board LED0 (labeled DS5 on
   prototyping board) controlled by I/O bit PD5.

   LED = OFF = A/D Channel0
   LED = ON  = A/D channel1

   Implementation of Round-Robin MUX control
   =========================================
   1. Define function prototype.
   void adc_mux(void);

   2. Define ADC_MUX_CNTRL as follows to call the adc_mux routine.
	#define ADC_MUX_CNTRL call adc_mux

   3. Define number of A/D channels. change default from 1 to number of
   A/D channels in your design.
   #define MAX_ADCHANNELS <Number Channels>

   4. Create adc_mux assembly routine as follows:
   #asm root nodebug
   adc_mux::
   // Insert Mux control code
   ret
   #endasm

   The above assembly routine will be automatically called by the ADC ramp
   interrupt service routine (ISR).

   5. Add I/O initialization code for port pins used for MUX control at
   the beginning of your application program.

	6. End of steps.

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
   4. Can monitor I/O bit PD5 with oscilloscope to view the simulated
      mux control signal.

***************************************************************************/
#class auto

#use "PowerCoreFLEX.lib"

// Steps 1, 2 and 3 for MUX control
void adc_mux(void);
#define ADC_MUX_CNTRL call adc_mux
#define MAX_ADCHANNELS 2
// End of step


// Use the common ISR library for the triac and the ADC
// ramp circuit.
#use "adctriac_isr.lib"
#use "adcramp.lib"


// Step 4 for MUX control, A/D mux control routine. The
// following registers are available for use by this
// routine...af, bc, de, hl, ix, and iy, the ISR handles
// saving and retrieving these registers.

#asm root nodebug
adc_mux::
	; Set conversion flag for current channel TRUE
   ld		a,(_adc_mux_channel)
   ld		c,a
   ld		b,0
   ld		de,0x0002
   mul
   ld		ix,mux_conversion_done
   add	ix,bc
   ld		(ix),0xFFFF

	; Increment to next A/D channel
	ld		a,(_adc_mux_channel)
	inc	a

	; Check for MAX A/D channel, if MAX wrap back to ch0
	cp		a,MAX_ADCHANNELS
	jr		nz,.adc_mux_ch1

   ld		a,0x00
 	ld 	de,PDDR 	; Port address
	ld 	hl,PDDRShadow
   set   5,(hl)   ; Set bit 5
   ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port
	jr    .adc_mux_exit

.adc_mux_ch1:
 	ld 	de,PDDR 	; Port address
	ld 	hl,PDDRShadow
   res   5,(hl)   ; res bit 5
   ioi 	ldd   	; [io](de) <- (hl)  copy shadow to port

.adc_mux_exit:
	ld		(_adc_mux_channel),a
	ret
#endasm
// End of step


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

   // Step 5 for MUX control
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

   mux_conversion_done[0] = FALSE;
   mux_conversion_done[1] = FALSE;
	while (1)
	{
      blankScreen(0, 20);
		DispStr(1, 2,  "External A/D input voltage for channels 0 and 1");
		DispStr(1, 3,  "-----------------------------------------------");
      DispStr(1, 6,  "Press Q or q to exit program.");

    	while(1)
      {
   		// Notes:
      	// 1. The mux_conversion_done flag is predefined and will be set
   		// by the ADC low-level driver.
      	// 2. If the mux_conversion_done flag isn't used, and if you read
      	// the external A/D channel faster than the A/D conversion rate,
      	// then the data will be from the previous A/D conversion until a
      	// new A/D conversion is completed.
      	if(mux_conversion_done[0] && mux_conversion_done[1])
         {
            mux_conversion_done[0] = FALSE;
            mux_conversion_done[1] = FALSE;
            for(channel=0; channel < MAX_ADCHANNELS; channel++)
            {
            	voltage = anaInVolts(channel);
            	if(channel >= MAX_ADCHANNELS)
            		channel = 0;
            	if(voltage != ADOVERFLOW)
         			sprintf(s, "Voltage = %.3f               ", voltage);
            	else
               	sprintf(s, "Voltage = Exceeded Range!!!  ");
            	DispStr(1, channel + 4, s);
         	}
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

