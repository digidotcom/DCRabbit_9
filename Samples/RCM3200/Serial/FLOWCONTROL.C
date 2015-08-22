/*******************************************************************
	flowcontrol.c
	Z-World 2002

	This program is used with RCM3200 series controllers
	with prototyping boards.

	The sample library, \Samples\RCM3200\rcm3200.lib, contains a
	brdInit() function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program demonstrates hardware flow control by sending a
	pattern of '*' characters out of serial port B, TXB (PC4), at
	115200 baud.

	One character at a time is received from port B and is displayed.
	In this example RXC (PC3) is configured as the CTS input, detecting
	a clear to send condition, and TXC (PC2) is configured as the RTS
	output, signaling a ready condition. This demonstration can be
	performed with either one or two core module boards.

	Refer to the function description for serBflowcontrolOn()
	for a general description on how to set up flow control lines.

	Prototyping Board Connections
	=============================

	On J5, RS232 connector, one or two boards
	-----------------------------------------

	   (CTS) RXC <-------> TXC (RTS)
	         RXB <-------> TXB
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
#class auto 			// Change default storage class for local variables: on the stack

#use rcm3200.lib 	//sample library used for this demo

#define BOUTBUFSIZE 31
#define BINBUFSIZE 15

//see serBflowcontrolOn() function description
#define SERB_CTS_PORT PCDR
#define SERB_CTS_BIT 3
#define SERB_RTS_PORT PCDR
#define SERB_RTS_SHADOW PCDRShadow
#define SERB_RTS_BIT 2

const long baud_rate = 115200L;

void main()
{
	auto char send_buffer[128];
	auto char received;
	auto char fc_flag;
	auto int i;
	auto int j;

	brdInit();				//initialize board for this demo

	serBopen(baud_rate);

	printf("Starting...\n");

	serBflowcontrolOn();
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
					cof_serBputs(send_buffer);
				}
			}
			//toggle flow control
			if (fc_flag)
			{
				serBflowcontrolOff();
				fc_flag = 0;
				printf("Flow Control Off\n");
			}
			else
			{
				serBflowcontrolOn();
				fc_flag = 1;
				printf("Flow Control On\n");
			}
		}
		costate
		{
			//receive characters in a leisurely fashion
			waitfordone
			{
				received = cof_serBgetc();
			}
			putchar(received);
	   }
	}
}

