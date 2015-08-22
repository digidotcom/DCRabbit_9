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
	vccmonitor.c

	This sample program is for the LP3500 series controllers.

	Description
	===========
	This program demonstrates monitoring of Vcc.  This will only
	work if you have an A/C device installed on your controller.
	AIN7 will not be available for analog operations.

	Instructions
	============
	1. Compile and run this program.
	2. Follow directions in STDIO window.

***************************************************************************/
#class auto				// Change default storage for local variables to "auto"


void main ()
{
	auto float voltage;

	brdInit();

	VccMonitorInit(1);		//enables monitoring

	voltage = VccMonitor();
	printf ("Vcc Monitor On = %.2fV\n", voltage);

	VccMonitorInit(0);		//disables monitoring

	voltage = VccMonitor();
	printf ("Vcc Monitor Off = %.2fV\n", voltage);

}
///////////////////////////////////////////////////////////////////////////

