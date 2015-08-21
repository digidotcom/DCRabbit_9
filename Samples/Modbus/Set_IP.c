/*
   set_ip.c - Sample program used to set the desired IP address for a target
              loaded with one of the supplied "modbus_rabweb (xxxxx).bin" files.
*/

#define TCPCONFIG 6

#define MYIP	"10.10.6.101"	// set modbus_rabweb.bin target's desired IP addr
//#define MYIP	"255.255.255.255"	// NB: aton("255.255.255.255") == -1L

#memmap xmem
#use "dcrtcp.lib"

void main()
{
   int result;
   long my_ip;

   sock_init;	// required only to enable the aton() call below
   tcp_tick;	// required only to enable the aton() call below

   my_ip = aton(MYIP);
   result = writeUserBlock(0, &my_ip, 4);
   if(!result)
      printf("my_ip is set to:  %s (0x%LX).\n", MYIP, my_ip);
   else
      printf("Error %d when writing to the UserBlock.\n", result);
}

