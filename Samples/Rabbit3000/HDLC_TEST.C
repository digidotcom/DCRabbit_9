/**********************************************************
	hdlc_test.c
 	Z-World, 2002

	This program is used with Rabbit 3000 based controllers.

	Description
	===========
	This program demonstrates a simple loopback test on
	either serial port E or serial port F in HDLC mode.

	Instructions
	============
	1. Simply connect PG6 (TXE) to PG7 (RXE) to use serial port E,
	   or connect PG2 (TXF) to PG3 (RXF) to use serial port F.
	2. Compile and run this program.
	3. Packets are sent out	and received on the same port.

*************************************************************/
#class auto

#use hdlc_packet.lib

// uncomment only one of the following two macro definitions
#define USE_HDLC_SERIALPORTE
//#define USE_HDLC_SERIALPORTF

#ifdef USE_HDLC_SERIALPORTE
#ifdef USE_HDLC_SERIALPORTF
#warnt "Not set up to use both serial ports E and F, using only serial port E."
#endif
#define HDLCopenX HDLCopenE
#define HDLCsendingX HDLCsendingE
#define HDLCsendX HDLCsendE
#define HDLCerrorX HDLCerrorE
#define HDLCpeekX HDLCpeekE
#define HDLCdropX HDLCdropE
#else
#ifdef USE_HDLC_SERIALPORTF
#define HDLCopenX HDLCopenF
#define HDLCsendingX HDLCsendingF
#define HDLCsendX HDLCsendF
#define HDLCerrorX HDLCerrorF
#define HDLCpeekX HDLCpeekF
#define HDLCdropX HDLCdropF
#else
#warnt "Must use either serial port E or serial port F."
#endif
#endif

#define BUFFER_COUNT 4
#define BUFFER_SIZE 100
#define BAUD_RATE 100000L

void main()
{
	char tx_packet[100];
	char rx_packet[100];
	int packet_size;
	unsigned long xbuffers;
	char errors;
	int counter;
	unsigned long t;
	int temp_flags;
	int temp_length;
	int i;
	int recv_count;
	unsigned long peekaddr;
	int peeklen;

	xbuffers = xalloc(BUFFER_COUNT * (BUFFER_SIZE + 4));
	HDLCopenX(	BAUD_RATE,
					HDLC_NRZ,
					xbuffers,
					BUFFER_COUNT,
					BUFFER_SIZE );
	counter = 1;
	recv_count = 0;

	while(1)
	{
		if(HDLCsendingX() == 0)
		{
			for(i = 0;i < 10;i++)
			{
				tx_packet[i] = '0' + (counter % 10);
			}
			tx_packet[10] = 0;

			strcpy(tx_packet, "CatMouse987654321");
			printf("Sending packet '%s', length:%d\n", tx_packet, strlen(tx_packet));

			if(HDLCsendX(tx_packet, strlen(tx_packet)) == 1)
			{
				counter++;
			}
			else
			{
				printf("Packet not sent\n");
			}
		}

		errors = HDLCerrorX();
		if(errors)
		{
			printf("ERROR 0x%x\n", errors);
		}

		//use the more efficient peek method
		if(HDLCpeekX(&peekaddr, &peeklen))
		{
			xmem2root(rx_packet, peekaddr, peeklen);
			packet_size = peeklen;
			HDLCdropX();
		}
		else
		{
			packet_size = 0;
		}

		if(packet_size > 0)
		{
			rx_packet[packet_size] = 0;
			printf("Got packet(%d): '%s'\n", packet_size, rx_packet);
		}
	}
}