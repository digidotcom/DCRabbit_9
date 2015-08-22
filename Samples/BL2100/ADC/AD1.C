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
	ad1.c
	
	This sample program is used with the BL2100 series controllers.

	This program demonstrates how to access the A/D internal test voltages
	in the TLC2543 A/D chips. The program reads the A/D internal voltages
	and then displays the RAW data in the STDIO window. 

	Instructions:
	-------------

	1. Compile and run this program.
	2. The program will display the raw data for a given internal test
	   channel.
	   
	   A/D Channel 								 TLC2543	  	
      ---------------------------------    -------		
	   channel 11  =  (Vref+  - Vref-)/2     0x800      
	   channel 12  =  Vref-						  0x000      
	   channel 13  =  Vref+						  0xFFF		 
	
***************************************************************************/
#class auto

#ifndef _FLASH_
#error "This program must be compiled to Flash."
#endif


///////////////////////////////////////////////////////////////////////////

void main ()
{
	auto int rawdata11, rawdata12, rawdata13;
	auto int numbits;

	brdInit();		// Required for BL2100 series boards
		
	printf("\n\n\r");
	printf("Displaying RAW data for the 12 bit A/D internal test voltages\n\r");
	printf("--------------------------------------------------------------\n\r");

	rawdata11 = anaIn(11);	//read A/D internal test voltage (Vref+ - Vref-)/2
	rawdata12 = anaIn(12);	//read A/D internal test voltage  Vref-
	rawdata13 = anaIn(13);  //read A/D internal test voltage  Vref+

	printf("Rawdata count for ch11 is 0x%03x (Vref+ - Vref-)/2 \r\n", rawdata11);
	printf("Rawdata count for ch12 is 0x%03x  Vref-\r\n", rawdata12);
	printf("Rawdata count for ch13 is 0x%03x  Vref+\r\n", rawdata13);
	
}	
///////////////////////////////////////////////////////////////////////////

