/********************************************************************
	extsram.c
	Z-World, Inc 2000

	This sample program demonstrates setup and simple addressing
	to an	external sram.

	Use with Rabbit Core Modules.  Connect circuitry as described
	in your User Manual Sample Circuit Section.

	This program first maps the external sram to I/O Bank 0 register
	with max 15 wait states, chip select strobe (but is ignored
	because of circuitry), and permits writes.

	The first 256 bytes of sram are cleared and read back.  Values
	are the written to the same area and read back.  STDIO window
	will indicate if writes and reads did not occur.

********************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

void main()
{
	auto unsigned int waddr;
	auto char rbuf[256];

	///// Configure external sram to  I/O bank 0, 15 waitstates,
	///// chip select, permit write
	WrPortI(IB0CR, NULL, 0x08);
	WrPortI(GOCR,NULL,0x00);		//enable bufen

	//// Clear 256 bytes starting at address 0
	for (waddr=0x0000; waddr<256; waddr++)	{
		printf("Clear address 0x%04x", waddr);

		WrPortE(waddr, NULL, 0x00);	//write 0 to external address
		rbuf[waddr]=RdPortE(waddr);	//read from external address

		if (rbuf[waddr] != 0x00)
			printf("...can't clear\n");
		else
			printf("\n");
		}

	//// Write 256 bytes starting at address 0
	for (waddr=0x0000; waddr<256; waddr++)	{
		printf("Writing %x to address 0x%04x", waddr, waddr);

		WrPortE(waddr, NULL, waddr);	//write byte to external address
		rbuf[waddr]=RdPortE(waddr);	//read from external address

		if (rbuf[waddr] != waddr)
			printf("...can't write\n");
		else
			printf("\n");
		}
}
