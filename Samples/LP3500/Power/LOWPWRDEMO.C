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
/**************************************************************************

	lowpwrdemo.c

	This sample program is intended for LP3500 series controllers.

	Description
	===========
	This program demonstrates low power mode with normal power source.


	Instructions
	============
	1. Press F5 to compile this program.
	2. Press ALT-F9 to to run without polling. Do not remove the cable so
		messages will be displayed in STDIO window.

**************************************************************************/
#class auto				// Change default storage for local variables to "auto"

#define OFF 0
#define ON 1
#define NUMLOOPS		25

void main()
{
	auto int	i;
	auto float brdvcc;
	unsigned long	data[NUMLOOPS], start_time, end_time;

	brdInit();

	VccMonitorInit(1);		//enables Vcc monitoring, AIN7 will be unavailable
	brdvcc=VccMonitor();
	printf("\nVcc is %.3fV\n\n", brdvcc);

	printf("Entering lowest power mode for %d loops...\n", NUMLOOPS);

	devPowerSet(ALLDEVICES, OFF);		//disable devices for less power
	start_time = MS_TIMER;
	powerMode(10);							//minimum power

	///next section demonstrates that program still runs
	///in low power mode
	i = 0;
	while (i < NUMLOOPS)
	{
		hitwd();					// no periodic interrupt!
		data[i] = i*i;			// store data
		i++;
	}

	powerMode(1);					//main oscillator
	end_time = MS_TIMER;
	printf("Now in maximum power mode\n");

	devPowerSet(ADCDEV, ON);	//enable AD converter
	printf("done in %ld ms (%ld ms/loop)\n",
			end_time - start_time, (end_time - start_time)/NUMLOOPS);

	printf("\nData:\n");
	for (i=0; i<NUMLOOPS; i++)
		printf("data[%2d] = %ld\n", i, data[i]);

	brdvcc=VccMonitor();
	printf("\nVcc is %.3fV\n", brdvcc);

}