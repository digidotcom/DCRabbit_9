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
	daccalib.c

	This sample program is for the BL2500 series controllers.
	
	Description
	===========
	This program demonstrates how to recalibrate one single-ended analog
	output channel	using two known voltages to generate constants for that
	channel and	rewritten into user block data area.  This will overwrite
	previous factory calibrations.

	This program must be compiled to Flash.

	Connections
	===========
	Connect a voltmeter positive lead to DA0 or DA1 channels and
	the negative lead to ground at J1 pin 3 on the controller.

	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
#class auto

#define LOCOUNT 310		//approx. 1.0V
#define HICOUNT 930		//approx. 3.0V

main ()
{
	auto int channel, value1, value2;
	auto float voltage, volts1, volts2;
	auto unsigned int rawdata, gaincode;
	auto char buffer[64];
	
	brdInit();

	while (1)
	{
		printf("\n\nChoose the 0 for DA0 or 1 for DA1 ..... ");
		gets(buffer);
		channel = atoi(buffer);

		pwmOut(channel, LOCOUNT);
		printf("\nEnter first voltage reading from meter =  ");
		gets(buffer);
		volts1 = atof(buffer);
	
		pwmOut(channel, HICOUNT);
		printf("\nEnter second voltage reading from meter =  ");
		gets(buffer);
		volts2 = atof(buffer);
	
		anaOutCalib(channel, LOCOUNT, volts1, HICOUNT, volts2);
	
		printf("\nstore constants to flash\n");
		anaOutEEWr(channel);		
	
		printf("\nread back constants\n");
		anaOutEERd(channel);		

		while (strcmp(buffer,"q") && strcmp(buffer,"Q"))
		{
			printf("\nType a desired voltage (0 to 3.2 Volts) or Q to exit  ");
			voltage = atof(gets(buffer));
			printf("Observe voltage on meter.....\n");
			pwmOutVolts(channel, voltage);
		}
	}
}
///////////////////////////////////////////////////////////////////////////

