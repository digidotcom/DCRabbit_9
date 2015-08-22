/*******************************************************************
	flowcontrol.c
	Z-World 2003

	This program is used with RCM3600 series controllers
	with prototyping boards.

	Description
	===========
	This program demonstrates hardware flow control by sending a
	pattern of '*' characters out of serial port D, TXD (PC0), at
	115200 baud.

	One character at a time is received from port D and is displayed.
	In this example RXC (PC3) is configured as the CTS input, detecting
	a clear to send condition, and TXC (PC2) is configured as the RTS
	output, signaling a ready condition. This demonstration can be
	performed with either one or two core module boards.

	Refer to the function description for serDflowcontrolOn()
	for a general description on how to set up flow control lines.


	Prototyping Board Connections
	=============================

	On the RS232 connector, one or two boards
	-----------------------------------------

	   (CTS) RXC <-------> TXC (RTS)
	         RXD <-------> TXD
	         GND <-------> GND (two boards only)


	Instructions
	============
	1.	With two boards, run the program on the sending board and then
		disconnect and power it back up so that the program is running
		in stand-alone mode.
		Connect to the second board, which will be the receiver and
		run this same program normally.

	2.	A repeating triangular pattern should print out in
		the STDIO window. The program will periodically switch flow
		control on or off to	demonstrate the effect of no flow control
		for a very slow receiver.

	3.	You can also observe the signals with a scope to see flow
		control operating.

*******************************************************************/
#class auto

#define DOUTBUFSIZE 31
#define DINBUFSIZE 15

//see serDflowcontrolOn() function description
#define SERD_CTS_PORT PCDR
#define SERD_CTS_BIT 3
#define SERD_RTS_PORT PCDR
#define SERD_RTS_SHADOW PCDRShadow
#define SERD_RTS_BIT 2

const long baud_rate = 115200L;

main()
{
	auto char send_buffer[128];
	auto char received;
	auto char fc_flag;
	auto int i;
	auto int j;

	brdInit();				//initialize board for this demo

   BitWrPortI(PEDR, &PEDRShadow, 0, 5);	//set low to enable rs232 device

	serDopen(baud_rate);

	printf("Starting...\n");

	serDflowcontrolOn();
	fc_flag = 1;
	printf("Flow Control On\n");

	//prepare the pattern in the send buffer
	send_buffer[0] = 0;  //null terminator
	for (i = 0;i < 8;i++)
	{
		for (j=0; j <= i; j++)
		{
			strcat(send_buffer, "*");
		}
		strcat(send_buffer, "\r\n");
	}

	while (1)
	{
		costate
		{
			//send as fast as we can
			for(i = 0; i < 3;i++)
			{
				//do 3 rounds before switching
				//flow control
				waitfordone
				{
					cof_serDputs(send_buffer);
				}
			}
			//toggle flow control
			if (fc_flag)
			{
				serDflowcontrolOff();
				fc_flag = 0;
				printf("Flow Control Off\n");
			}
			else
			{
				serDflowcontrolOn();
				fc_flag = 1;
				printf("Flow Control On\n");
			}
		}
		costate
		{
			//receive characters in a leisurely fashion
			waitfordone
			{
				received = cof_serDgetc();
			}
			putchar(received);
	   }
	}
}


