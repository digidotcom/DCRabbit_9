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
/*
	This program is intended to be used to setup RabbitSys to be programmed via
   an ethernet crossover cable connected directly from a PC to a RabbitSys
   enabled board.

	This program brings down the ethernet interface, turns off DHCP, sets the
   device's IP address to 10.10.6.100, the netmask to 255.255.255.0 and the
   default gateway to 10.10.6.1.  Note that the ip and gateway addresses can
   be changed in the two macros below.

   To restore RabbitSys to default DHCP behavior uncomment the ENABLE_DHCP
   macro.

   *** WARNING ***
   Do not run this program over a TCP/IP debugging session.    
*/

#define _IPADDR		"10.10.6.100"
#define _GATEWAY		"10.10.6.1"

//#define ENABLE_DHCP

#use "dcrtcp.lib"

void main()
{
	sock_init();
#ifndef ENABLE_DHCP
	if(ifconfig(IF_ETH0,
   				IFS_DOWN,
            	IFS_DHCP,       0,
            	IFS_IPADDR,     aton(_IPADDR),
            	IFS_NETMASK,    0xFFFFFF00uL,
            	IFS_ROUTER_SET, aton(_GATEWAY),
            	IFS_UP,
            	IFS_END) == 0)
   {
   	printf("Static IP address set.\n");
   }
   else
   {
   	printf("Could not set static IP address\n");
   }
#else
	if(ifconfig(IF_ETH0,
   				IFS_DOWN,
               IFS_ROUTER_DEL, 0L,
            	IFS_DHCP,       1,
            	IFS_UP,
            	IFS_END) == 0)
   {
   	printf("DHCP enabled.\n");
   }
   else
   {
   	printf("Could not enable DHCP\n");
   }
#endif
}