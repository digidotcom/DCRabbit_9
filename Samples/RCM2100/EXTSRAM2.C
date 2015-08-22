/********************************************************************
	Samples\RCM2100\extsram2.c
	Z-World, Inc 2001

	This sample program demonstrates setup and simple addressing
	to an	external sram.
	
	Use with RCM2100 series controller.  Connect circuitry as
	described in your User Manual Sample Circuit Section.

	This program first maps the external sram to I/O Bank 7 register
	with 15 wait states (max), chip select strobe (PE7), and permits
	writes.

	The first 256 bytes of sram are cleared and read back.  Values
	are the written to the same area and read back.  STDIO window
	will indicate if writes and reads did not occur.
	
********************************************************************/
#class auto


 void main()
{
	unsigned int i, waddr;
	char rbuf[256];

	///// Map external sram to use PE7 as a chip select with 15 waitstates,
	///// and permits writes
	WrPortI ( SPCR, &SPCRShadow, 0x84 );						// Disable slave port function
	WrPortI ( PEFR, &PEFRShadow, (PEFRShadow | 0x80));		// Set PE7 to be external I/O Chip Select	
	WrPortI ( PEDDR, &PEDDRShadow, (PEDDRShadow | 0x80)); // Set PE7 as an output		
	WrPortI ( IB7CR, &IB7CRShadow, 0x08 ); 					// Set PE7  = 15 wait state, /CS, write enabled

	//// Clear 256 bytes starting at address 0xE000
	for (i = 0, waddr=0xE000; i < 256; i++, waddr++)
	{
		printf("Clear address 0x%04x", waddr);
	
		WrPortE(waddr, NULL, 0x00);	//write 0 to external address
		rbuf[i]=RdPortE(waddr);	//read from external address
			
		if (rbuf[i] != 0x00)
			printf("...can't clear\n");
		else
			printf("\n");
	}

	//// Write 256 bytes starting at address 0xE000
	for (i = 0, waddr=0xE000; i < 256; i++, waddr++)	{
		printf("Writing %x to address 0x%04x", i, waddr);
	
		WrPortE(waddr, NULL, i);	//write byte to external address
		rbuf[i]=RdPortE(waddr);		//read from external address
		if (rbuf[i] != i)
			printf("...can't write\n");
		else
			printf("\n");
	}
}
