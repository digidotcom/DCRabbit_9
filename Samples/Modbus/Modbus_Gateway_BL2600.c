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
/* Modbus_Gateway_Bl2600.c

Modifications:
JLC	June 2006	added USE_MODBUS_CRC

This sample is a complete program.  It enables the BL2600 to act as a
MODBUS slave and will only respond to commands received via MODBUS.

This is a MODBUS slave device therefore it is a TCP/IP Server listening for
requests from a MODBUS master which is a TCP/IP client.

This sample does not use the tcp_config.lib for setting up network parameters.
All the network parameters are set locally in this program via the IFCONFIG_ETH0
macro.  The network parameter default settings are as follows:
	IP Address: 10.10.6.102  		(IP Address of the device running this program)
   Gateway:		10.10.6.1
   Netmask:		255.255.255.0

This device can act as a MODBUS gateway by defining MODBUS_GATEWAY.  As
such, MODBUS messages which are not addresses to MY_MODBUS_ADDRESS will be
sent to downstream devices via RS485.
*/

// useful debugging macros
#define MODBUS_DEBUG nodebug
#define MODBUS_DEBUG_PRINT 0x0000
#define MODBUS_SLAVE_DEBUG nodebug
//#define DCRTCP_VERBOSE  				// tcp/ip debugging info

// The following three definitions are required for proper operation
// of the TCP/IP keepalive function.  If they are not defined here
// default values will be used.
#define	INACTIVE_PERIOD		5	//  period of inactivity, in seconds, before sending a
											//		keepalive, or 0 to turn off keepalives.
#define	KEEPALIVE_WAITTIME  	3	// number of seconds to wait between keepalives,
			  						  		//		after the 1st keepalive was sent
#define	KEEPALIVE_NUMRETRYS	3	// number of retrys

#define TCPCONFIG 		0
#define USE_ETHERNET		1
// set up my IP addresses
#define IFCONFIG_ETH0 \
		IFS_IPADDR,aton("10.10.6.102"), 		\
      IFS_ROUTER_SET, aton("10.10.6.1"),	\
		IFS_NETMASK,aton("255.255.255.0"),	\
		IFS_UP

#use dcrtcp.lib

#define MY_MODBUS_ADDRESS	1

#use Modbus_Slave.lib				// must be #use'd BEFORE Modbus_Slave_TCP
#define MY_MODBUS_PORT 		502	// default tcp/ip port for MODBUS is 502

// the following definitions are required only if this device is a gateway for
//		"downstream" devices via RS485
//#define	MODBUS_GATEWAY				// allow downstream devices via RS485

#ifdef MODBUS_GATEWAY
#define	SERIAL_MODE			1		// modes 1 or 3 enable RS485 on port E
#define	MODBUS_BAUD			115200
#define	MODBUS_PORT			E
#define  EINBUFSIZE			127
#define  EOUTBUFSIZE			127
#endif

#define USE_MODBUS_CRC				// use alternate CRC calculation

#use Modbus_Slave_TCP.lib			// use MODBUS TCP

#use Modbus_slave_bl26xx.LIB		// must be last MODBUS library #use'd

main ()
{	auto int i;

	brdInit();
	digOutConfig ( 0x00F0 );		// DIO 4-7 = output
  	anaOutConfig ( 1, 0 );			// these two statements MUST be executed in
  	anaOutPwr(1);						// this order for the D/A to operate
	digHTriStateConfig(0);			// disable tri-state
   digHoutConfig ( 0 );				// default Houts to sinking
   for ( i=0; i<=3; i++ ) digHout (i,1); // Houts off

	sock_init();
	MODBUS_TCP_Init ( MY_MODBUS_ADDRESS, MY_MODBUS_PORT );
	while (1)
   	MODBUS_TCP_tick();
}

