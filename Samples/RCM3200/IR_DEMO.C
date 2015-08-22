/**********************************************************
	ir_demo.c
 	Z-World, 2002

	This program is used with RCM3200 series controllers
	with prototyping boards.

	The sample library, rcm3200.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program demonstrates sending packets (Modbus ASCII)
	back and forth over an IR link.  This demo requires two
	RCM3200 prototyping boards to be set up so that their IR
	transceivers are facing each other.

	This program should be run in standalone mode on one
	RCM3200 and then in debug mode on the other to observe
	the results.

	Instructions
	============
	1. Compile and run this program on one controller and
		then on the second controller.
	2. Press switch S2 on the proto-board to transmit a
		packet.
	3. If a test packet is received, the program will send
		back a response packet and displayed in STDIO window.
		The test packets and response	packets have different
		codes.

**********************************************************/
#use rcm3200.lib			//sample library used for this demo

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
	auto int i;
	auto char byte_value;

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

/**
 * 	Inspect incoming MODBUS packet.  Takes the ASCII codes and converts
 * 	them to binary.  Reads "packet_str" filled with hex digits and returns
 * 	"packet".  String starts with ':' and is terminated with two hexdigit
 * 	checksm (binary sum of address, function, data) and CR-LF.
 * 	"packet_str" doesn't need a NUL char terminator.
 *
 * 	RETURN: 	0 == format error, +1 == OK.
 */
int parse_modbus(ModbusPacket *packet, char *packet_str, int packet_len)
{
	auto char temp;
	auto int data_index, str_index, i;
	auto unsigned char packet_lrc, calc_lrc;

	if(packet_str[0] != ':')
	{
		return 0;
	}
	packet->address = parse_hex(packet_str+1);
	packet->function = parse_hex(packet_str+3);

	data_index = 0; //start of data
	str_index = 5;
	//look ahead to next pair.  "+2" skips over LCR to terminator.
	while(strncmp(packet_str+str_index+2, "\r\n", 2)
			&& str_index < packet_len)
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
	auto int i;
	auto unsigned char nibble;

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
	auto unsigned char lrc;
	auto int i;
	auto int str_index;

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


void main()
{
	auto char rx_packet[50];
	auto char tx_packet[50];
	auto char test_packet[50];
	auto unsigned char tx_data[20];
	auto int rx_size, tx_size, test_size;
	auto ModbusPacket rx_modbus;

	brdInit();			//initialize for this demo

	pktFinitBuffers(BUFFER_COUNT, BUFFER_SIZE);
	pktFopen(IR_BAUDRATE, PKT_CHARMODE, ':', modascii_check);
	WrPortI(SFER, NULL, 0x10); //turn on IrDA encoding

	while(1)
	{
		costate
		{
			if(rx_size = pktFreceive(rx_packet, 50))
			{
				//print out raw packet
				rx_packet[rx_size] = 0; //add null
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
			if((RdPortI(PGDR) & 0x02) != 0x02)
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



void pktFinit()
{
	BitWrPortI(PEDDR, &PEDDRShadow, 1, 4);
	BitWrPortI(PEDR, &PEDRShadow, 0, 4); //set tranceiver to SIR mode

	WrPortI(PGDR, &PGDRShadow, PGDRShadow & 0xcf); //set MD0, MD1 low
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

