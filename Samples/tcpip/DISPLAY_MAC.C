/*******************************************************************************
		Samples\TCPIP\display_mac.c
		Z-World, 2001

		This program prints out the MAC address(es) for your board.
*******************************************************************************/

#class auto

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 *
 * Grabs IP address out of "TCP_CONFIG.LIB" so you can ping the board.
 */
#define TCPCONFIG 1


#use "dcrtcp.lib"

///////////////////////////////////////////////////////////////////////

void main()
{
	char buffer[6];
   word iface;

	sock_init();

   for (iface = 0; iface < IF_MAX; ++iface)
   	if (is_valid_iface(iface)) {
      	printf("Interface %u is valid.\n");
         if (!ifconfig(iface, IFG_HWA, buffer, IFS_END))
         	printf("  Hardware address is %02x:%02x:%02x:%02x:%02x:%02x\n",
	            buffer[0],buffer[1],buffer[2],
	            buffer[3],buffer[4],buffer[5]);
         else
         	printf("  No hardware address for this interface.\n");

      }

	printf("\nCalling ip_print_ifs()...\n\n");
   ip_print_ifs();

	for(;;) {
		tcp_tick(NULL);
	}
}