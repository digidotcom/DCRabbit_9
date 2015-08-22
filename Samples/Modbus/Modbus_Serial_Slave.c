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
/* Modbus_Serial_Slave.c
	Version 1.02

Modifications:
TG		Nov 2006 1.02	added LP3500
JLC	Jun 2006			added USE_MODBUS_CRC

This is a MODBUS slave device specifically for the BL2600

	This is a MODBUS slave device specifically for the BL2600 or LP3500
*/

#define MODBUS_DEBUG_PRINT 0	// 0xFC
#define MODBUS_SLAVE_DEBUG nodebug
#define USE_MODBUS_CRC
#define BYTE_TIME		32		// maximum number of byte times to wait between received bytes
									// this is necessary because the 9XCite does not transmit all
									// the bytes in a packet at once.
#use Modbus_Slave.lib


#if((_BOARD_TYPE_ & 0xFF00) == (BL2600A & 0xFF00))
   #define  MY_MODBUS_ADDRESS 	2
   #define  SERIAL_MODE		2	//do a <ctrl><h> on serMode for more info
   #define	MODBUS_PORT		E
	#define  EINBUFSIZE		127
	#define  EOUTBUFSIZE		127
   #define  MODBUS_BAUD		9600
	#use Modbus_slave_bl26xx.LIB
#elif((_BOARD_TYPE_ & 0xFF00) == (LP3500 & 0xFF00))
   #define  MY_MODBUS_ADDRESS 3
   #define  SERIAL_MODE		0	//do a <ctrl><h> on serMode for more info
   #define	MODBUS_PORT		C
//	#define  USE_RS485      // if using serial port F, uncomment since using RS485 port
   #define  CINBUFSIZE		127
	#define  COUTBUFSIZE		127
	#define  MODBUS_BAUD    9600
	#define INPUT_ONE		LOW	// sets whether a logic one is high or low
	#define OUTPUT_ONE   LOW	// sets whether a logic one is high or low
	#use Modbus_slave_lp35xx.LIB
#else
   #error "This board (" _BOARD_TYPE_ ") does not have a Modbus Slave driver."
#endif

main ()
{
	int i;
	brdInit();
#if((_BOARD_TYPE_ & 0xFF00) == (LP3500 & 0xFF00))
	pwmOutConfig(10000ul);
#elif((_BOARD_TYPE_ & 0xFF00) == (BL2600A & 0xFF00))
	digOutConfig ( 0x00F0 );		// DIO 4-7 = output
// init the analog system
  	anaOutConfig ( 1, 0 );			// these two statements MUST be executed in
  	anaOutPwr(1);						// this order for the D/A to operate
// init the Hout channels
	digHTriStateConfig(0);			// disable tri-state
   digHoutConfig ( 0 );				// default Houts to sinking
   for ( i=0; i<=3; i++ ) digHout (i,1); // Houts off
#endif
	MODBUS_Serial_Init ();
	while (1)
   		MODBUS_Serial_tick();
}