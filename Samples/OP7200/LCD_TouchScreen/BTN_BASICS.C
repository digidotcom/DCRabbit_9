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
/******************************************************************************

	btn_basics.c

	This sample program is for the OP7200 series controllers.

	Description
	===========
	This program demonstrates the basic functionality of the buttons
	for the touchscreen.
	
	- Button 1 setup to demo default button operation.
	- Button 2 setup to demo audio response of button being pressed
	  and released.
	- Button 3 setup to demo the repeat feature.
	
	All button responses will be displayed in Dynamic C STDIO window.


	Instructions
	============
	1. Compile and run program.

	2. Press/release button "1" to see normal button operation.

	3. Press/release button "2" for normal opertion with audio response. 	
	
	4. Quickly press/release button "3" to see normal button operation
	   on a button with the repeat option enabled.

	5. Press button "3" for at least 2 seconds to see it repeat.

	6. Press button "2" and release outside of the button area, do as
	   follows:
	    a) Press button
	    b) Slide finger to a point outside of the button area.
	    c) Release touchscreen.

      >>>> Button Not selected.
	   
	7. Press button, slide in/out of button region and then release, do
	   as follows:
		a) Press button.
		b) Slide finger to a point outside of the button area.
	   c) Slide finger back into the valid button area.
	   d) Release button.

		>>>> Button selected.
	   
	8. Do the same as in step 5 but slide into another valid button area
	   and release.

		>>>> No buttons selected.	   		   
******************************************************************************/
#memmap xmem

unsigned long xmemArea;
#define	NUMBER_OF_BUTTONS	10

//----------------------------------------------------------------------------------
// 	The following MACRO's can be used to override the overall button audio
//    response. The default macro settings are as follows:
//
//		#define BTN_PRESSPERIOD    0	// Initial button press audio is OFF
//		#define BTN_RELEASEPERIOD  2  // Button release audio is set to ~2ms
//		#define BTN_REPEATPERIOD   1  // Button repeat period is set to ~1ms
//
//
//	   BTN_PRESSPERIOD.....Macro to control audio response for all buttons, 
//                        for when it has been initially pressed.
//    0        = Disables audio.
//    >0       = Number of ticks for audio on-period, approx. 1ms/tick
//                             
//		BTN_RELEASEPERIOD...Macro to control audio response for all buttons, 
//                        for when it has been released.
//    0        = Disables audio.
//    >0       = Number of ticks for audio on-period, approx. 1ms/tick
//
//		BTN_REPEATPERIOD....Macro to control audio response for all buttons,
//                        for when it is in repeat mode.
//    0        = Disables audio.
//    >0       = Number of ticks for audio on-period, approx. 1ms/tick
//
//
//    Notes:
//    ------
//    1. BTN_REPEATPERIOD should be less the button RepeatDelay parameter
//       in the btnAttributes function.
//
//		2. The above macro's configure the audio response of all buttons. The
//       parameter BuzzerCntrl in the btnAttributes function then can be set
//       to Enable/Disable audio on a given button.
//
//    3. If all 3 macro's are set to zero then the btnAttributes BuzzerCntrl
//       Enable/Disable parameter will have no effect.  
//
//    Example of changing a MACRO:
//    ----------------------------
//		#undef  BTN_REPEATPERIOD	
//		#define BTN_REPEATPERIOD   3  // Changed repeat audio on-period to ~3ms
//
//----------------------------------------------------------------------------------
#class auto


#undef  BTN_REPEATPERIOD	
#define BTN_REPEATPERIOD   3
 
void main ()
{
	fontInfo fi8x10,fi10x16,fi12x16;
	int btn;
		
	// Initialize controller and graphic driver
	brdInit();

	glInit();
	glBackLight(1);
	glSetContrast(22);

	// Initialize Font structures for later use
	glXFontInit(&fi10x16, 10, 16, 32, 127, Font10x16);
	glXFontInit(&fi12x16, 12, 16, 32, 127, Font12x16);

	// Initial button driver
	xmemArea = btnInit(NUMBER_OF_BUTTONS);

	// Create buttons 
	btnCreateText(xmemArea, 0,60,  40, 180, 50, 1, 0, &fi10x16,"1.DEFAULT BUTTON");
	btnCreateText(xmemArea, 1,50, 100, 200, 50, 1, 0, &fi10x16,"2.BUTTON W/AUDIO");
	btnCreateText(xmemArea, 2,40, 160, 225, 50, 1, 0, &fi12x16,"3.BUTTON W/REPEAT");

	// Button 1...normal operation, no audio or repeat.
	btnAttributes(xmemArea, 0, 0,   0,   0, 0);

	// Button 2...normal operation with audio, no repeat.
	btnAttributes(xmemArea, 1, 0,   0,   0, 1);

	// Button 3...repeat operation with audio response
	btnAttributes(xmemArea, 2, 1, 1000, 100, 1);

	// Display buttons
	btnDisplay(xmemArea,0);
	btnDisplay(xmemArea,1);
	btnDisplay(xmemArea,2);

	glPrintf(50, 10, &fi12x16, "BASIC BUTTON DEMO");
	while(1)
	{
		btn = btnGet(xmemArea);
		if(btn >= 0)
		{
			switch(btn)
			{
				case 0:
					printf("Button 1 was pressed\n");
					break;
				case 1:
					printf("Button 2 was pressed\n");
					break;
				case 2:
					printf("Button 3 was pressed\n");
					break;
			}
		}
	}
}
