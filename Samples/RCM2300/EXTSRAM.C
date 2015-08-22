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
	Samples\RCM2300\extsram.c

	This sample program demonstrates setup and simple addressing
	to an	external sram.
	
	Use with RCM2300 series controller.  Connect circuitry as
	described in your User Manual Sample Circuit Section.

	This program first maps the external sram to I/O Bank 7 register
	with 15 wait states (max), chip select strobe (PE7), and permits
	writes.

	The first 16 bytes of sram are cleared and read back.  Values
	are the written to the same area and read back.  STDIO window
	will indicate if writes and reads did not occur.
	
********************************************************************/
#class auto

#define STARTADDR 0xE000

void main()
{
	unsigned int i, waddr;
	char rbuf[256];

	///// Map external sram to use PE7 as a chip select with 15 waitstates,
	///// and permits writes
	WrPortI ( SPCR, &SPCRShadow, 0x84 );						// Disable slave port function
	WrPortI ( PEFR, &PEFRShadow, (PEFRShadow | 0x80));		// Set PE7 to be external I/O Chip Select	
	WrPortI ( PEDDR, &PEDDRShadow, (PEDDRShadow | 0x80)); // Set PE7 as an output		
	WrPortI ( IB7CR, &IB0CRShadow, 0x08 ); 					// Set PE7  = 15 wait state, /CS, write enabled

	//// Clear 16 bytes starting at address 0xE000
	for (i = 0, waddr=STARTADDR; i < 16; i++, waddr++)
	{
		printf("Clear address 0x%04x", waddr);
	
		WrPortE(waddr, NULL, 0x00);	//write 0 to external address
		rbuf[i]=RdPortE(waddr);	//read from external address
			
		if (rbuf[i] != 0x00)
			printf("...can't clear\n");
		else
			printf("\n");
	}

	//// Write 16 bytes starting at address 0xE000
	for (i = 0, waddr=STARTADDR; i < 16; i++, waddr++)	{
		printf("Writing %x to address 0x%04x", i, waddr);
	
		WrPortE(waddr, NULL, i);	//write byte to external address
		rbuf[i]=RdPortE(waddr);		//read from external address
		if (rbuf[i] != i)
			printf("...can't write\n");
		else
			printf("\n");
	}
}
