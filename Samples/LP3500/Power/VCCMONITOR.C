/***************************************************************************
	vccmonitor.c
	Z-World, 2002

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

