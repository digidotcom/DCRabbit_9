/*******************************************************************************
	flowcontrol.c
	Rabbit, 2007

	This program is used with RCM3900 series controllers and RCM3300 prototyping
	boards.

	Description
	===========
	This sample program demonstrates RS-232 serial communication with hardware
	flow control.  A pattern of '*' characters is sent out of TXE (serial port E
	transmit) at 115200 baud.  One character at a time is received on RXE (serial
	port E receive) and is displayed in the STDIO window.

	RXF (normally, serial port F receive) is configured as the hardware flow
	control CTS input, detecting a clear to send condition.  TXF (normally,
	serial port F transmit) is configured as the hardware flow control RTS
	output, signaling a request to send condition.

	This demonstration can be performed with either one or two core modules and
	prototyping boards.

	Please refer to the function description for serEflowcontrolOn() for general
	instructions on setting up RS-232 hardware flow control lines.

	Prototyping Board(s) Connections
	================================
	One board, on its RS-232 connector:
		(CTS) RXF <-------> TXF (RTS)
		      RXE <-------> TXE

	Two boards (call them A and B), on their respective RS-232 connectors:
		  Board A           Board B
		  -------           -------
		(CTS) RXF <-------> TXF (RTS)
		(RTS) TXF <-------> RXF (CTS)
		      RXE <-------> TXE
		      TXE <-------> RXE
		      GND <-------> GND

	Instructions
	============
	1. If running this sample with one board, skip the rest of this step.
	   Otherwise, make sure Dynamic C is *NOT* set to "Code and BIOS in RAM"
	   compile mode.  Connect the programming cable to the first of the two
	   boards, then compile this program to the board.  Press F4 to return
	   Dynamic C to edit mode, then power the board down.  Disconnect the
	   programming cable and then power the board back up again.  This sample
	   program is now executing in stand-alone run mode on the first board.

	2. Connect the programming cable to the (second or only) board, then compile
	   and run this sample program in debug mode.

	3. A repeating triangular pattern should print out in the STDIO window.  The
	   program will periodically toggle hardware flow control between on and off
	   to demonstrate the effect of no flow control on a very slow receiver.

	4. You can also observe the CTS / RTS signals with a scope to see flow
	   control operating.
*******************************************************************************/

#class auto
#use rcm39xx.lib	// sample library to use with this sample program

#define BAUD_RATE 115200L

#define EOUTBUFSIZE 31
#define EINBUFSIZE 15

// see serEflowcontrolOn() function description
#define SERE_RTS_PORT PGDR
#define SERE_RTS_SHADOW PGDRShadow
#define SERE_RTS_BIT 2	// PG2 is TxF on protoboard
#define SERE_CTS_PORT PGDR
#define SERE_CTS_BIT 3	// PG3 is RxF on protoboard

void main(void)
{
	// auto variables in main() permanently consume precious stack space
	static char send_buffer[128];
	static char received;
	static char fc_flag;
	static int i;
	static int j;

	// it's just good practice to initialize Rabbit's board-specific I/O
	brdInit();

	// PG2 & PG3 are configured as inputs in brdInit; set PG2 to output
	BitWrPortI(PGDR, &PGDRShadow, 1, 2);	// preset PG2 output high
	BitWrPortI(PGDDR, &PGDDRShadow, 1, 2);	// set PG2 to output

	serEopen(BAUD_RATE);

	printf("Starting...\n");

	serEflowcontrolOn();
	fc_flag = 1;
	printf("Flow Control On\n");

	/*
		prepare the following pattern in the send buffer:
			*<CR><LF>
			**<CR><LF>
			***<CR><LF>
			****<CR><LF>
			*****<CR><LF>
			******<CR><LF>
			*******<CR><LF>
			********<CR><LF>
	*/
	send_buffer[0] = 0;  // ensure strcat() sees an initial zero terminator
	for (i = 0; i < 8; ++i) {
		for (j = 0; j <= i; ++j) {
			strcat(send_buffer, "*");
		}
		strcat(send_buffer, "\r\n");
	}

	while (1) {
		costate {
			// send as fast as we can
			for (i = 0; i < 4; ++i) {
				// do 4 rounds before toggling flow control
				waitfordone {
					cof_serEputs(send_buffer);
				}
			}
			// toggle flow control
			if (fc_flag) {
				serEflowcontrolOff();
				fc_flag = 0;
				printf("\nFlow Control Off\n");
			} else {
				serEflowcontrolOn();
				fc_flag = 1;
				printf("\nFlow Control On\n");
			}
		}
		costate {
			// receive characters in a leisurely fashion
			waitfordone {
				received = cof_serEgetc();
			}
			putchar(received);
		}
	}
}

