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
/******************************
jr_pktdemo.c

packet demo, bounces a message back and forth between a JackRabbit board and
another controller running a version of this program.
The RS485 ports of both boards should be tied together..
RS485-  <--->  RS485-
RS485+  <--->  RS485+
GND     <--->  GND  (optional)

Similar results can be obtained on boards without RS485 tranceivers
by wiring serial port D outputs together.
PC0  <--->  PC1
PC1  <--->  PC0

Hit a button on the dev board to send an initial packet

receives packets on port D
using ':' as a start character,

format is ':', then 2 bytes for length in hex, followed by data
e.g.
:13Thisisapacketofdata

Uncomment different versions of the pktDopen call below to try out different
packet schemes. Just make sure that both boards are running in the same mode.

*****************************/

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
	
	pktDinitBuffers(5, 50);
	//uncomment one of the open statements depending on the packet type		
	//pktDopen(19200, PKT_CHARMODE, ':', packet_test);
	//pktDopen(19200, PKT_9BITMODE, PKT_LOWSTARTBYTE, packet_test);
	pktDopen(19200, PKT_GAPMODE, 3, NULL);

	printf("port D opened\n");

	//uncomment to set parity mode
	//pktDsetParity(PKT_OPARITY);
		
	packet_count = 0;
	strcpy(init_packet, ":13Thisisapacketofdata");

	loopinit();
	while (1) {
	
		costate {
			waitfordone {
				packet_size = cof_pktDreceive(packet, 50);
			}
			if(packet_size < 0)
			{
				printf("Error from cof_pktDreceive: %d\n", packet_size);
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
			errors = pktDgetErrors();
			if (errors & PKT_PARITYERROR) {
				printf("Parity error\n");
			}
			if(errors)
				printf("error: 0x%x\n", errors);			
			
			//echo the packet right back
			waitfordone { 
				cof_pktDsend(packet, packet_size, 3);
			}
		}
		costate {
			//send a packet if a button is pressed
			if ( (RdPortI(PBDR) & 0x3c) != 0x3c) {
				waitfordone {
					cof_pktDsend(init_packet, strlen(init_packet), 3);
				}
				printf("Init packet sent\n");
			}
			waitfor(DelayMs(1000));

		}			
	}
}	

//The tranceiver init function handles board-specific
//initialization of the tranceiver hardware.
//In this case the RS485 TX enable pin must be setup as an output
void pktDinit()
{
	dpkt_disableport(); //start with port disabled
	BitWrPortI(PDDDR, &PDDDRShadow, 1, 5); //make DE output
}

#define DRS485ENABLE 5

//These functions are defined to handle board-specific actions
//when transmitting or receiving data
//In the case of the JackRabbit, they either enable or disable
//the DE line(PD5) on the RS485 transceiver.
void pktDrx();
void pktDtx();

#asm root

pktDtx::
	 ld	 a, (PDDRShadow)
	 set	 DRS485ENABLE, a
ioi ld	 (PDDR), a
	 ld	 (PDDRShadow), a
	 ret
	 
pktDrx::
	 ld	 a, (PDDRShadow)
	 res	 DRS485ENABLE, a
ioi ld	 (PDDR), a
	 ld	 (PDDRShadow), a
	 ret

#endasm