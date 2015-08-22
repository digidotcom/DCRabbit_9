/******************************
ipdev_pktdemo.c

packet demo, bounces a message back and forth between a TCP/IP dev kit board
and another controller running a version of this program.
The RS485 ports of both boards should be tied together..
RS485-  <--->  RS485-
RS485+  <--->  RS485+
GND     <--->  GND  (optional)

Similar results can be obtained on boards without RS485 tranceivers
by wiring serial port D outputs together.
PC0  <--->  PC1
PC1  <--->  PC0

Hit a button on the dev board to send an initial packet

receives packets on port B
using ':' as a start character,

format is ':', then 2 bytes for length in hex, followed by data
e.g.
:13Thisisapacketofdata

Uncomment different versions of the pktBopen call below to try out different
packet schemes. Just make sure that both boards are running in the same mode.

Note: defining the label PKTB_USEPORTD causes serial port D to use
PD4 and PD5 for TX and RX which are connected to the RS485 tranceiver on
the dev kit board

*****************************/

#define PKTB_USEPORTD
#use packet.lib

int packet_test(void *packet, int length)
{
	char *bytes;
	char **tailptr;	//not really used
	char length_field[3];
	long length_read;
	
	if (length > 2) {
		bytes = (char *)packet;
		strncpy(length_field, bytes+1, 2);
		length_field[2] = 0; //make sure null terminator is there	
		length_read = strtol(length_field, tailptr, 16);
		if (length_read == length-3) {
			return 1;
		}
	}
	return 0;
}	

void main()
{
	char packet[51];
	char init_packet[50];
	int packet_size;
	char errors;
	int packet_count;
	char switch_state;
	int i, temp;
	
	pktBinitBuffers(5, 50);
	//uncomment one of the open statements depending on the packet type		
	//pktBopen(19200, PKT_CHARMODE, ':', packet_test);
	//pktBopen(19200, PKT_9BITMODE, PKT_LOWSTARTBYTE, packet_test);
	pktBopen(19200, PKT_GAPMODE, 3, NULL);

	printf("port B opened\n");

	//uncomment to set parity mode
	//pktBsetParity(PKT_OPARITY);
		
	packet_count = 0;
	strcpy(init_packet, ":13Thisisapacketofdata");

	pktBsend(init_packet, strlen(init_packet), 3);
	printf("Init packet sent\n");
	
	loopinit();
	while (1) {
	
		costate {
			waitfordone {
				packet_size = cof_pktBreceive(packet, 50);
			}
			if(packet_size < 0)
			{
				printf("Error from cof_pktBreceive: %d\n", packet_size);
			}
			else if(packet_size > 50)
			{
				printf("Packet too big:%d\n", packet_size);
			}
			else
			{
				packet[packet_size] = 0; //add null terminator
				printf("Got packet '%s' - size:%d\n", packet, packet_size);
			}
			errors = pktBgetErrors();
			if (errors & PKT_PARITYERROR) {
				printf("Parity error\n");
			}
			if(errors)
				printf("error: 0x%x\n", errors);			
			
			//echo the packet right back
			waitfordone { 
				cof_pktBsend(packet, packet_size, 3);
			}
		}
		
		costate {
			// another activity here, if desired
		}
	}
}	

//The tranceiver init function handles board-specific
//initialization of the tranceiver hardware.
//In this case the RS485 TX enable pin must be setup as an output
void pktBinit()
{
	bpkt_disableport(); //start with port disabled
}

#define DRS485ENABLE 0	//PC0 is data enable on the dev kit board

//These functions are defined to handle board-specific actions
//when transmitting or receiving data
void pktBrx();
void pktBtx();

#asm root

pktBtx::
	 ld	 a, (PCDRShadow)
	 set	 DRS485ENABLE, a
ioi ld	 (PCDR), a
	 ld	 (PCDRShadow), a
	 ret
	 
pktBrx::
	 ld	 a, (PCDRShadow)
	 res	 DRS485ENABLE, a
ioi ld	 (PCDR), a
	 ld	 (PCDRShadow), a
	 ret

#endasm