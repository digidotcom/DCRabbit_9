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
/* Modbus_Serial_Master.c

This program is a limited function MODBUS master.  It supports the
following commands which are in Modbus_Master.lib:
	0x01 Read Coils
   0x03 Read Holding Registers
   0x04 Read Input Registers
   0x05 Write Single Coil
   0x06 Write Single Register
	0x0F Write multiple Coils

Some customization will probably be required in the serial port functions:
	serInit, MBM_Send_ADU and MBM_Rcv_Resp
*/

#define MODBUS_SERIAL_MASTER

#define MODBUS_DEBUG_PRINT 0	// define to 1 to print transactions
#define BINBUFSIZE 63
#define BOUTBUFSIZE 63

#define _RS232	232
#define _RS485	485
#define SERIAL_PORT_MODE	_RS232

#use modbus_master.lib

int MBM_Send_ADU ( char *Packet, int ByteCount );
int MBM_Rcv_Resp ( char * Packet );
void serInit ( void );
void ser485Tx(void);
void ser485Rx(void);

main ()
{	int i, RegsValue[4];
	serInit();
	i = MBM_ReadRegs ( 2, &RegsValue[0], 0, 1 );	// read reg 0
   while (1);
}

/****************************************************
The following functions are board-specific and must be
implemented by the user.  Specifically, the correct
serial port must be referenced in the MBM_Send_ADU and
MBM_Rcv_Resp functions.  If RS485 is being used the
functions ser485Tx and ser485Rx must be set to enable
and disable the RS485 transmit enable hardware.  If
RS232 is being used then the function MBM_Send_ADU can
be simplified by removing the RS485-specific code.
/****************************************************/

void serInit ( void )
{	serBopen ( 9600 );						// open the serial port
}

void ser485Tx(void)
{
}

void ser485Rx(void)
{
}


/* START FUNCTION DESCRIPTION ********************************************
MBM_Send_ADU

SYNTAX:			int MBM_Send_ADU ( char *Packet, int ByteCount );

DESCRIPTION:	Transmit a Modbus packet to a "downstream" device.
					Calculate the CRC and append to the packet.
					There is a 50msec timeout hard coded in this function.

PARAMETER1:		address of packet - must have two byte pad at end for
					inclusion of CRC word

PARAMETER2:		number of bytes in the packet

RETURN VALUE:	MB_SUCCESS

Note: these functions do NOT implement the Modbus protocol delay of
3.5 byte times.

END DESCRIPTION **********************************************************/


int MBM_Send_ADU ( char *Packet, int ByteCount )
{	auto unsigned CRCvalue;
	auto unsigned long Endtime;
   int i;

// insert CRC
	CRCvalue = MODBUS_CRC ( Packet, ByteCount );
	Packet[ByteCount+1] = CRCvalue;		// store low byte
	Packet[ByteCount] = CRCvalue>>8;		// store high byte
	ByteCount+=2;								// adjust for CRC

#ifdef MODBUS_DEBUG_PRINT & 1
	printf ( "Tx:" );
	for ( i=0; i<ByteCount; i++ ) printf ( " %02X", Packet[i] );
	printf ( "\n\r" );
#endif

#if SERIAL_PORT_MODE	== _RS485
	ser485Tx();									// enable the RS485 transmitter
   serBrdFlush();								// clear the read FIFO
	serBwrite ( Packet, ByteCount );		// send the data
   while ( serBrdUsed() != ByteCount ); // wait for all bytes to be transmitted
	ser485Rx();									// disable the RS485 transmitter
   serBrdFlush();								// clear the read FIFO
#else
	serBwrite ( Packet, ByteCount );		// send the data
   serBrdFlush();								// clear the read FIFO
#endif

// receive the response into the same buffer used for the transmit data
	Endtime = MS_TIMER+50;					// allow up to 50msec for response to start
	while (  ( (long)(MS_TIMER - Endtime) < 0L )
      && ( (ByteCount = MBM_Rcv_Resp(Packet)) == 0 )  );
   return ByteCount;
}

/* START FUNCTION DESCRIPTION *********************************************
MBM_Rcv_Resp

DESCRIPTION:	Receive the response from the Modbus Slave
					It is the responsibility of the caller to handle
					a timeout if required.

PARAMETER1:		none

RETURN VALUE:	0 = no message
					+n = number of bytes with valid CRC
               MB_CRC_ERROR = invalid CRC

END DESCRIPTION **********************************************************/


int MBM_Rcv_Resp ( char * Packet )
{	auto unsigned CalcCRC;
	auto int ByteCount;
   auto int i;

	ByteCount = serBread( Packet, 50, 50L );
   if ( ByteCount )
   {	CalcCRC = MODBUS_CRC ( Packet, ByteCount );
	   if ( CalcCRC )
		{
			ByteCount = MB_CRC_ERROR;
		}
      else ByteCount = MB_SUCCESS;
// the calculated CRC which includes the received CRC should be 0
	#if MODBUS_DEBUG_PRINT & 1
		printf ( "Rx:" );
		for ( i=0; i<ByteCount; i++ ) printf ( " %02X", Packet[i] );
      printf ( "\n\r" );
	#endif
   }
   return ByteCount;
}

