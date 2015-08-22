/*********************************************************************
	ir_demo.c
 	Z-World, 2002

	This program is used with RCM3400 series controllers
	with prototyping boards.
	
	Description
	===========
	This program demonstrates sending packets (Modbus ASCII)
	back and forth over an infrared link.  This demo requires another
	RCM3400 prototyping board or another controller with IR (such
	as an RCM3000) to be set up so that their IR transceivers are
	facing each other.
	
	Instructions
	============
	1. Compile and run the IR demo program that runs on the
		second controller and remove programming cable so that the
		controller is in run mode.
	2. Compile and run this program on an RCM3400 controller
		in debug mode to observe results.
	3. Press switch S2 on the proto-board to transmit a
		packet.
	4. If a test packet is received, the program will send
		back a response packet and displayed in STDIO window.
		The test packets and response	packets have different
		codes.
		 
*********************************************************************/
#class auto

#define S2 (BitRdPortI(PDDR, 5))		//switch S2 input port D bit 5

#define IR_BAUDRATE 57600

#define BUFFER_COUNT 4
#define BUFFER_SIZE 50

#define MAX_MODBUS_DATA 20

#define TEST_FUNCTION_CODE 0x01
#define RESPONSE_FUNCTION_CODE 0x02

#define NODE_ADDRESS 0x01

#define TEST_DATA "Testing"

typedef struct
	{
	char address;
	char function;
	unsigned char data[MAX_MODBUS_DATA];
	int datalen;
	} ModbusPacket;
	

#use "packet.lib"

int modascii_check(char *packet, int packet_size)
{
	if(packet_size < 5)
	{
		return 0;
	}
	if(strncmp(packet + packet_size - 2, "\r\n", 2))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//parses two characters of hex into 8bit number
unsigned char parse_hex(char *ascii)
{
	int i;
	char byte_value;
	
	for(i = 0;i < 2;i++)
	{
		byte_value = byte_value << 4;
		if(ascii[i] >= '0' && ascii[i] <= '9')
		{
			byte_value += (ascii[i] - '0');
		}
		else if(ascii[i] >= 'a' && ascii[i] <= 'f')
		{
			byte_value += (ascii[i] - 'a' + 10);
		}
		else if(ascii[i] >= 'A' && ascii[i] <= 'F')
		{
			byte_value += (ascii[i] - 'A' + 10);
		}
	}
	return byte_value;
}

int parse_modbus(ModbusPacket *packet, char *packet_str, int strlen)
{
	char temp;
	int data_index, str_index, i;
	unsigned char packet_lrc, calc_lrc;		
	if(packet_str[0] != ':')
	{
		return 0;
	}
	packet->address = parse_hex(packet_str+1);
	packet->function = parse_hex(packet_str+3);

	data_index = 0; //start of data
	str_index = 5;
	//look ahead to next pair
	while(strncmp(packet_str+str_index+2, "\r\n", 2)
			&& str_index < strlen)
	{
		if(data_index < MAX_MODBUS_DATA)
		{
			packet->data[data_index] = parse_hex(packet_str+str_index);
		}
		else
		{
			return 0; //too big
		}											 		
		data_index++;
		str_index += 2;	
	}
	packet->datalen = data_index;
	//check LRC	
	packet_lrc = parse_hex(packet_str+str_index);
	calc_lrc = packet->address;
	calc_lrc += packet->function;
	for(i = 0;i < packet->datalen;i++)
	{
		calc_lrc += packet->data[i];
	}
	if(calc_lrc == packet_lrc)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}	

write_hex_byte(char *str, unsigned char inbyte)
{
	int i;
	unsigned char nibble;
		
	for(i = 1; i >= 0;i--)
	{
		nibble = inbyte & 0x0f;
		if(nibble < 10)
		{
			str[i] = '0' + nibble;
		}
		else
		{
			str[i] = 'A' + nibble - 10;
		}
		inbyte = inbyte>>4;	
	}
}

int build_modbus(	char *packet_buf,
						unsigned char address,
						unsigned char function,
						unsigned char *data,
						int datalen)
{
	unsigned char lrc;
	int i;
	int str_index;
	
	packet_buf[0] = ':';
	write_hex_byte(packet_buf+1, address);
	lrc = address;
	write_hex_byte(packet_buf+3, function);
	lrc += function;

	str_index = 5;
	for(i = 0;i < datalen;i++)
	{
		write_hex_byte(packet_buf+str_index, data[i]);
		lrc += data[i];
		str_index += 2;
	}
	write_hex_byte(packet_buf+str_index, lrc);
	packet_buf[str_index+2] = '\r';			
	packet_buf[str_index+3] = '\n';
	return datalen*2+9;
}			
	
	
main()
{
	char rx_packet[50];
	char tx_packet[50];
	char test_packet[50];
	unsigned char tx_data[20];
	int rx_size, tx_size, test_size;
	ModbusPacket rx_modbus;

	brdInit();			//initialize for this demo

	pktFinitBuffers(BUFFER_COUNT, BUFFER_SIZE);
	pktFopen(IR_BAUDRATE, PKT_CHARMODE, ':', modascii_check);
	WrPortI(SFER, NULL, 0x10); 		//turn on IrDA encoding
		
	while(1)
	{
		costate
		{
			if(rx_size = pktFreceive(rx_packet, 50))
			{
				//print out raw packet
				rx_packet[rx_size] = '\x0'; //add null
				printf("Got packet: %s\n", rx_packet);
				printf("Size:%d\n", rx_size);
				
				if(parse_modbus(&rx_modbus, rx_packet, rx_size))
				{
					//packet OK
					switch(rx_modbus.function)
					{
						case TEST_FUNCTION_CODE:
							printf("Received test packet from 0x%x\n",         
										rx_modbus.address);
							printf("Sending response.\n");
							tx_size = build_modbus(tx_packet,
														  NODE_ADDRESS,
														  RESPONSE_FUNCTION_CODE,
														  rx_modbus.data,
														  rx_modbus.datalen);
							//wait if already sending something							
							while(pktFsending())
							{
								yield;
							}  
							//send out response, add in small turnaround delay
							pktFsend(tx_packet, tx_size, 5); 
							break;
						case RESPONSE_FUNCTION_CODE:
							printf("Received response from 0x%x\n",
										rx_modbus.address);
							break;
						default:
							printf("Got unknown function code: 0x%x\n",
									rx_modbus.function);
					}
				}
				else
				{
					printf("BAD PACKET\n");
				} 					
			}
		} //end of first costate

		costate
		{
			//check if S2 was pressed
			if (!S2)
			{
				//wait if a packet is being transmitted
				while(pktFsending())
				{
					yield;
				}  
				test_size = build_modbus(test_packet,
											  NODE_ADDRESS,
											  TEST_FUNCTION_CODE,
											  TEST_DATA,
											  strlen(TEST_DATA));
				//send out response, add in small turnaround delay
				pktFsend(test_packet, test_size, 5);
				test_packet[test_size] = 0;
				printf("Sent test packet: %s\n", test_packet);
				//don't send packets too fast
				waitfor(DelayMs(500));
			}
		}		
	}
}


///////
// Required functions
///////
void pktFinit()
{
	BitWrPortI(PEDDR, &PEDDRShadow, 1, 4);	//set to output
	BitWrPortI(PEDR, &PEDRShadow, 0, 4); 	//set tranceiver MIR_SEL to SIR mode

	WrPortI(PGDR, &PGDRShadow, PGDRShadow&0xcb);  		//set device MD0,MD1,TXD low
	WrPortI(PGDDR, &PGDDRShadow, PGDDRShadow|0x34);  	//set to output
}

void pktFrx();
void pktFtx();

#asm
pktFrx::
	 ld	 a, (PGDRShadow)
	 res   2, a					;make sure PG2 is low	
	 ld	 (PGDRShadow), a
ioi ld	 (PGDR), a
	 ret

pktFtx::
	 ld	 a, (PGDRShadow)
	 set   2, a					;make sure PG2 is high
	 ld	 (PGDRShadow), a
ioi ld	 (PGDR), a
	 ret
#endasm
	

