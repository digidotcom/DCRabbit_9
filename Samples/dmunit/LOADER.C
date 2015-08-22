/*******************************************************************************
        SAMPLES/DevMate/Loader.c
        Z-World, 2001

        Use DevMate as worldly connection for connected target processor.

        (I)  Choose an IP address and socket number where the DevMate can be
        reached.  This socket is given to DCR in the Options | Communication..
        dialog.  Download this program to the DevMate CPU.

        (II) Connect the DevMate's Ethernet port to your network.  This will be
        your "programming cable" between DCR and the target CPU.
        
        (III) In DynamicC IDE, select "Use TCP/IP Connection" and put in the
        IP and socket numbers (names may not work).  The "controller name"
        is for informational purposes only.  Note, if you have a firewall
        with proxy between DCR and the Rabbit DevMate, you may have to enter the
        *firewall's* proxy IP/port instead.

        (IV) Hit control-Y to reset the target.  Now "target" communications
        is achieved using the DevMate board as an intermediary.  It drives RESET,
        SMODE0, and SMODE1, reads STATUS, and does serial communications. A program
        may now be loaded as usual to the target processor.

        NOTE - Any program that is downloaded through the DevMate *must* have the
        following code in it, to force all communications to talk to the Ethernet
        Loader on the DevMate directly.
        
        		#define USE_TC_LOADER
        		#use "tc_conf.lib"
        		
        If this is not done, your program will not be able to communicate to the
        DevMate once it starts running!

*******************************************************************************/

#define TC_I_AM_DEVMATE

/**
 * 	Define to have LIB\DevMate\TargetProc_Loader.lib display debug and
 * 	progress messages.
 */
//#define TARGETPROC_LOADER_DEBUG

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

/**
 *  DevMate will listen on MY_IP_ADDRESS:TARGETPROC_LOADER_PORT for commands
 *  from the DynamicC IDE.  If going through a firewall or proxy, it might be
 *  necessary to modify this value to something allowed - see your network
 *  administrator for help, if necessary.
 *  Default value is 4244 - from DCR and the TC Loader subsystem.
 */
//#define TARGETPROC_LOADER_PORT	4244

#use "dcrtcp.lib"

/*
 * Select the loader application and pull in the necessary libraries
 */
#define USE_TC_LOADER
#use "tc_conf.lib"

/* --------------------------------------------------------------------- */

void main()
{
	/* init the TCP/IP stack and the Ethernet Loader */
	sock_init();
	targetproc_init();

	/* drive it all */
	while(1) {	
		tcp_tick(NULL);
		targetproc_tick();
	}
}

