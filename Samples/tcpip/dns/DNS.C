/*******************************************************************************
        dns.c
        Z-World, 2000

        Demonstration of how to look up an IP address through a DNS
        server.
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
	longword ip;
	char buffer[20];
	
	sock_init();

	ip=resolve("www.rabbitsemiconductor.com");
	if(ip==0)
		printf("couldn't find www.rabbitsemiconductor.com\n");
	else
		printf("%s is www.rabbitsemiconductor.com's address.\n",inet_ntoa(buffer,ip));
}
