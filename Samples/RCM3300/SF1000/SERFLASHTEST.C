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

	serflashtest.c

	This program is used with RCM3300 series controllers and
	prototyping boards.

	An optional SF1000 Serial Flash card is required to run this
   demonstration. The card is installed onto the prototyping board.


   Description
	===========
	This simple sample program demonstrates how to read and write
   from the SF1000 Serial Flash.  Please refer to SF1000 User
   Manual for further reference.

   Serial port D lines, TXD (PC0) and RXD (PC1), and serial clock
   line CLKD (PF0) are used.  PD3 is used as the chip select line.
   These lines are configured below.


	Instructions
	============
   1. Plug SF1000 Serial Flash card into RCM3300 prototyping board
   	on connector J11, matching pin 1 on each connector.

	2. Compile and run this program.

	3. You should see in STDIO Window:

		     "This serial flash test passed"

      This indicates a write and read were successful.

      OR

		     "This serial flash test failed"

      This indicates a write and read were not successful.


*********************************************************************/
#class auto

#use rcm33xx.lib		//sample library to use with this application

////////////////////////////////////////////////////
// The following configurations are required to
// run the SF1000 library with an RCM3300.
////////////////////////////////////////////////////
#define SPI_SER_D                      //use serial port D
#define SPI_CLK_DIVISOR			23			//clock divisor 23=57600 baud

#define SF1000_CS_BIT			2				//chip select bit
#define SF1000_CS_PORT			PDDR			//chip select register
#define SF1000_CS_PORTSHADOW	PDDRShadow  //chip select shadow register

#use sf1000.lib
////////////////////////////////////////////////////
// end of configurations
////////////////////////////////////////////////////


main()
{
	auto int msg1, msg2;
	auto char buf1[64];
	auto char buf2[64];

   brdInit();

	memset(buf1, 0x00, sizeof(buf1));
	memset(buf2, 0x00, sizeof(buf2));

	if (msg1 = SF1000Init())
   {
		printf("Serial flash initialize: error msg %d\n", msg1);
   }

	strcpy(buf1, "This serial flash test\0");
	msg1 = SF1000Write(0x1000, buf1, strlen(buf1)+1);
	msg2 = SF1000Read(0x1000, buf2, strlen(buf1)+1);

   if (strncmp(buf1, buf2, strlen(buf1)))
   {
		printf("%s failed: error msg write %d, read %d\n", buf1, msg1, msg2);
   }
   else
   {
		printf("%s passed\n", buf2);
   }

}