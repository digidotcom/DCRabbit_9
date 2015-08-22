/*******************************************************************************
        pingme.c
        Z-World, 2000

        A very basic TCP/IP program, that will initilize the TCP/IP
        interface, and allow the device to be 'pinged' from another
        computer on the network.
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
 */
#define TCPCONFIG 1


/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use dcrtcp.lib

void main()
{
   sock_init();
   for (;;) { 
      tcp_tick(NULL);
   }
}
