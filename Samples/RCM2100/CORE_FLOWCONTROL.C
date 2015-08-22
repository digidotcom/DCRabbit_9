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
/*****************************************
   Samples\RCN2100\core_flowcontrol.c

Demonstration of hardware flow control.
Sends a pattern of '*' characters out of serial port B(PC4, PC5)
at 115200 baud.
Receives one character at a time from port B, and displays it.
In this example PC3 is configured as the CTS input, detecting a clear
to send condition, and PC2 is configured as the RTS output, signaling a
ready condition. This demonstration can be performed with either one
or two core module boards.

In the case of a single board, simply connect PC4 to PC5 and connect
PC2 to PC3.

With two boards, run the program on the sending board and then disconnect
and power it back up so that the program is running in stand-alone mode.
Connect to the second board, which will be the receiver and run this same
program normally. Connect Board1:PC4 to Board2:PC5, connect ground pins
together, and connect Board1:PC3 to Board2:PC2.

A repeating triangular pattern should now be printing out in the STDIO
window. The program will periodically switch flow control on or off to
demonstrate the effect of no flow control for a very slow receiver.

You can also observe the signals with a scope to see
flow control operating.

You may refer to the function descripition for serBflowcontrolOn()
for a general description on how to set up flow control lines.

*******************************************/
// #class auto


#define BOUTBUFSIZE 31
#define BINBUFSIZE 15

//see serBflowcontrolOn() function description
#define SERB_CTS_PORT PCDR
#define SERB_CTS_BIT 3
#define SERB_RTS_PORT PCDR
#define SERB_RTS_SHADOW PCDRShadow
#define SERB_RTS_BIT 2

#define BAUD_RATE 	115200L

void main()
{
	auto char send_buffer[128];
	auto char received;
	auto char fc_flag;
	auto int i;
	auto int j;
					
	serBopen(BAUD_RATE);
	
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
	
