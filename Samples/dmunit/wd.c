/*****************************************************************************

	wd.c
	Z-World, 2001
 
	This program is intended to be run on a DeviceMate.  It starts the
	watchdog application.
 
	See \samples\dmtarget\wd.c for the program intended to be run on target
	processor.
  
*****************************************************************************/

#define TC_I_AM_DEVMATE		/* necessary for all DeviceMates */

/*
 * Choose watchdog services.
 */
#define USE_TC_WD
#use "tc_conf.lib"

main()
{
	targetproc_init();

	while(1)
		targetproc_tick();
}