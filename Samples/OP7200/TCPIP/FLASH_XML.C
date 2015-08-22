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
/***************************************************************************
        Samples\OP7200\TCPIP\FLASH_XML.c

        The basic idea for this program is to allow a html web page to create
         a separate active socket on a server which runs on the OP7200.
         This active socket will allow the client and server to exchange
         data on a socket that is always opened.  In particular, the server
         is able to send the status of Digital Inputs and Analog 	readings to
         the client and the client is able to control Digital 	  Outputs on the
         server (OP7200). When the flash_xml.c sample program is compiled it will
         ximport the following files:

			-	xml_socket.html
			-	xml_socket.swf

			Both of the above files were created with Macromedia Flash MX.
			The xml_socket.html file attempts to load the xml_socket.swf
			file in to the web page when accessed by the web browser.
			In order for the web browser to recognize this file it must
			have the Macromedia Flash MX plugin version 6 installed.  If the
			browser does not have this plugin it will be directed to the Macromedia
			Flash web site where it can be downloaded.

			Also included with this sample program is the source code for the
			Macromedia Flash MX html and swf files (xml_socket.fla).
			In order to open and edit this file you must obtain a licensed copy of
			Macromedia Flash MX.    www.macromedia.com


        IF USING A ZWORLD DEMO BOARD
        ============================

        Connections
        ===========
        Connect SW1-SW4 of the Demo Board to IN0-IN3 of the OP7200 respectively.
        Connect LED1-LED4 of the Demo Board to OUT0-OUT3 of the OP7200 respectively.
        Connect GND of the Demo Board to PWR - of the OP7200
        Connect K of the Demo Board to K+ of the OP7200
        Supply K of the OP7200 with a positive voltage between 5 and 24VDC.

        Testing
        =======
		  Press one of the SWx push buttons on the DEMO BOARD and you will see
		  DINx box change state.

		  Press one of the OUTx buttons on the Internet Page and watch LEDs turn on and off.


        ANALOG TESTING.
        ===============

        Connections
        ===========
        A variable power supply with a range of 0 to 10VDC will be needed.
		  Connect the positive to AINx and the negative to AGND.

        Testing
        =======
        As you vary the analog input of the power supply, you will see the bar
        graph on the web page match.



*************************************************************************************************/
#class auto


/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

#define MY_PORT	8888

//************************************************************************************************
#define	ETH_MTU		 600
#define  BUFF_SIZE	 (ETH_MTU-40)	//must be smaller than (ETH_MTU - (IP Datagram + TCP Segment))
#define  TCP_BUF_SIZE ((ETH_MTU-40)*4)	// sets up (ETH_MTU-40)*2 bytes for Tx & Rx buffers

#define	HTTP_MAXSERVERS		 	1
#define 	MAX_TCP_SOCKET_BUFFERS	2	// 1 for flash xml sockets and 1 for http server
#define	HTTP_MAXNAME				30 // had to change since .swf mime is longer than defaulf of 20
#define  SSPEC_MAXNAME				HTTP_MAXNAME
//************************************************************************************************

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

#define INCOMING_IP		0 			//except all connections
#define INCOMING_PORT	0			//except all ports
#define TCP_TIMEOUT		3000		// (disabled) if in any state for more than a X msecs re-initialize

#define ANALOG_INPUTS	8
#define DIGITAL_INPUTS	16
#define DIGITAL_OUTPUTS 8
#define REFRESH_RATE		1000		// the rate in which the xml socket will be updated

#define	RED		"\x1b[31m"	// foreground colors for printf
#define	BLUE		"\x1b[34m"	// foreground colors for printf
#define	BLACK		"\x1b[30m"	// foreground colors for printf

#ximport "\samples\op7200\tcpip\pages\xml_socket.html"	index_html
#ximport "\samples\op7200\tcpip\pages\xml_socket.swf"	xml_socket_swf

const HttpType http_types[] =	// mime types
{
	{ ".html", "text/html", NULL },
	{ ".swf", "application/x-shockwave-flash", NULL}
};

const HttpSpec http_flashspec[] =
{	// assocates the file we brought in with ximport with its name on the webserver
	{ HTTPSPEC_FILE,	"/",					 index_html,			NULL, 0,	NULL,	NULL},
	{ HTTPSPEC_FILE,	"/index.html",		 index_html,			NULL, 0,	NULL,	NULL},
	{ HTTPSPEC_FILE,	"/xml_socket.swf", xml_socket_swf,	NULL,	0,	NULL,	NULL}
};

typedef struct
{
	tcp_Socket sock;
	int txbytes, rxbytes;
	int nextstate;
	long statetime;
	char buff[BUFF_SIZE];
}My_Socket_Type;

typedef struct
{
	char  buff[BUFF_SIZE];
	long	rtime;
	float	ain[ANALOG_INPUTS];
	char  *din[DIGITAL_INPUTS];
	char  *dout[DIGITAL_OUTPUTS];
}My_InOuts_Type;

const char ANA0_7[]  = 	"<AINPUTS " \
								"ain00=\"%6.3f\" ain01=\"%6.3f\" ain02=\"%6.3f\" ain03=\"%6.3f\" " \
								"ain04=\"%6.3f\" ain05=\"%6.3f\" ain06=\"%6.3f\" ain07=\"%6.3f\" />";

const char DIN0_7[]  =	"<DINPUTS " \
								"din00=\"%s\" din01=\"%s\" din02=\"%s\" din03=\"%s\" " \
								"din04=\"%s\" din05=\"%s\" din06=\"%s\" din07=\"%s\" ";
const char DIN8_15[] = 	"din08=\"%s\" din09=\"%s\" din10=\"%s\" din11=\"%s\" " \
								"din12=\"%s\" din13=\"%s\" din14=\"%s\" din15=\"%s\" />";

const char DOUT[] 	=	"<DOUTPUTS " \
								"dout00=\"%s\" dout01=\"%s\" dout02=\"%s\" dout03=\"%s\" " \
								"dout04=\"%s\" dout05=\"%s\" dout06=\"%s\" dout07=\"%s\" />";

const char * const LOW_HIGH[]= {{"LOW"},{"HIGH"}};
unsigned long xmemBtn;
fontInfo fi10x12,fi14x16;
void My_IO_Ctrl(My_InOuts_Type *my_inouts);
void My_Load_Buffer(int selection,My_Socket_Type *my_sock,My_InOuts_Type *my_inouts);
void My_XML_Parser(char *buff,My_InOuts_Type *inouts);
int  My_Rcv_Pkt(My_Socket_Type *my_sock);
int  My_Snd_Pkt(My_Socket_Type *my_sock);
void My_Handler(My_Socket_Type *my_sock);

///////////////////////////////////////////////////////////////////////

void main()
{

	int channel;
	My_Socket_Type socket;
	My_InOuts_Type InOuts;
	char DispStrIn[100];
	long IOoutcnt,InCnt;
	// initializing
	InOuts.rtime	= REFRESH_RATE;
	brdInit();
	digOutConfig(0);
	glInit();
	glBlankScreen();
	glSetContrast(24);
	glBackLight(1);
	glXFontInit ( &fi10x12,10,12,0x20,0x7E,Terminal9 );
	glXFontInit ( &fi14x16,14,16,0x20,0x7E,Terminal12 );
	keyInit();
	IOoutcnt = InCnt = 0;
	xmemBtn = btnInit(20);
	for(channel=0;channel<DIGITAL_OUTPUTS;channel++)	// setting digital outputs to low state
		InOuts.dout[channel] = LOW_HIGH[0];
	memset(socket.buff,0x00,BUFF_SIZE);
	socket.nextstate 	= 0;

	sock_init();
	http_init();
	tcp_reserveport(80);

	while(1)
	{
		costate //TCP_HANDLES
		{
			My_Handler(&socket);
			http_handler();
		}

		costate //IO_Ctrl
		{
			waitfor(socket.nextstate == 2 && DelayMs(InOuts.rtime));
			My_IO_Ctrl(&InOuts);

		}

		costate //TCP_RX
		{	// THIS IS WHERE WE RECEIVE INFORMATION FROM THE XML SOCKET
			waitfor(socket.rxbytes || DelayMs(15000) );
			if (socket.rxbytes)
			{
				sprintf(DispStrIn,"Msg From Host\n%s",socket.buff);
				btnMsgBox(0,120,320,120,&fi14x16,DispStrIn,1,0);
					//printf("%sRcvd: %s\n",RED,socket.buff);
				socket.rxbytes = 0;
				My_XML_Parser(socket.buff,&InOuts);
			}
			else
			{
				btnClearRegion(0,120,320,120);
			}
		}

		costate //TCP_TX
		{	// THIS IS WHERE WE SEND INFORMATION TO THE XML SOCKET
			waitfor(socket.nextstate == 2 && DelayMs(InOuts.rtime));
			My_Load_Buffer(0,&socket,&InOuts);		// FILLING SOCKET BUFFER WITH ANALOG INPUTS DATA
			socket.nextstate = 3;						// SET STATEMACHINE TO THE SEND STATE
//			printf("%sSent: %s\n",RED,socket.buff);
			yield;

			My_Load_Buffer(1,&socket,&InOuts);		// FILLING SOCKET BUFFER WITH DIGITAL INPUTS
			socket.nextstate = 3;						// SET STATEMACHINE TO THE SEND STATE
//			printf("%sSent: %s\n",RED,socket.buff);
			yield;

			My_Load_Buffer(2,&socket,&InOuts);		// FILLING SOCKET BUFFER WITH DIGITAL OUTPUT STATUS
			socket.nextstate = 3;						// SET STATEMACHINE TO THE SEND STATE
//			printf("%sSent: %s",RED,socket.buff);
//			printf("\n\n");


		}
	}
}

/* My_IO_Ctrl */
/*********************************************************************************
 This function reads/writes the IO and fills the information in the my_inouts
 array, for example:
 	- Analog Inputs
 		read the channel then fill the my_inouts->ain array with the new data
 	- Digital Inputs:
 		read the channel then fill the my_inouts->din array with the new data
 	- Digital Outputs:
 		read the my_inouts->dout array then set the correct digital channel LOW or
 		HIGH depending what is in the array.
********************************************************************************/
void My_IO_Ctrl(My_InOuts_Type *my_inouts)
{
	static int channel;

	// ANALOG INPUTS: this is where we read the analog inputs............
	for(channel=0;channel<ANALOG_INPUTS;channel++)
	{
		my_inouts->ain[channel] = anaInVolts(channel,GAIN_X2);
		if (my_inouts->ain[channel] > 10 ) my_inouts->ain[channel] = 10.000;
		if (my_inouts->ain[channel] < 0 ) my_inouts->ain[channel] = 0.000;
	//	if(my_inouts->ain[channel] < 10)		// DEBUGGING
	//		my_inouts->ain[channel]++;			// DEBUGGING
	//	else											// DEBUGGING
	//		my_inouts->ain[channel] = 0;		// DEBUGGING
	}

	// DIGITAL INPUTS: this is where we read the digital inputs..........
	for(channel=0;channel<DIGITAL_INPUTS;channel++)
	{
		my_inouts->din[channel] = LOW_HIGH[digIn(channel)];

		//if(!strcmp(my_inouts->din[channel],LOW_HIGH[1]))// DEBUGGING
		//	my_inouts->din[channel] = LOW_HIGH[0];			// DEBUGGING
		//else															// DEBUGGING
		//	my_inouts->din[channel] = LOW_HIGH[1];			// DEBUGGING
	}

	// DIGITAL OUTPUTS: this is where we set the digital outputs.........
	for(channel=0;channel<DIGITAL_OUTPUTS;channel++)
	{
		if(!strcmp(my_inouts->dout[channel],LOW_HIGH[0]))
		{

			//my_inouts->dout[channel] = LOW_HIGH[0];		 // DEBUGGING
			digOut(channel,0);
			// at this point the data in the my_inouts->dout array is high so we
			// need to set the digital output to high, for example:
			//	see_digital_output(channel,1);
		}
		else
		{
			//my_inouts->dout[channel] = LOW_HIGH[1];		 // DEBUGGING
			digOut(channel,1);
			// at this point the data in the my_inouts->dout array is low so we
			// need to set the digital output to low, for example:
			//	see_digital_output(channel,0);
		}
	}
}

/* My_XML_Parser */
/*********************************************************************************
 This function parses the socket buffer then updates the digital outputs in the
  inouts buffer.
********************************************************************************/
void My_XML_Parser(char *buff,My_InOuts_Type *inouts)
{
	static char 	element[12],channel[12],state[12];
	static char 	*pelement,*pchannel,*pstate,*pDOUT,*pbuff;
	static int		loop_cnt;

	pelement = strtok(buff," ");	// element name
	strcpy(element,pelement+1);	// skipping the <

	pstate   = strtok(NULL,"="); 	// state attribute
	pstate   = strtok(NULL," ");	// actual state data
	strcpy(state,pstate+1);			// now we have, for example: HIGH" so we need to get rid of the quotes

	pchannel = strtok(NULL,"=");	// channel attribute
	pchannel = strtok(NULL," ");	// actual channel data
	strcpy(channel,pchannel+1);		// now we have, for example: dout00" so we need to get rid of the quotes

	strtok(state,"\"");			// we now have, for example: state = HIGH
	strtok(channel,"\"");		// we now have, for example: channel = dout00

	if(!strcmp(element,"CONFIG"))
	{
		// NOT IMPLEMENTED YET....
	}

	if(!strcmp(element,"DOUT"))	// must change digital output
	{
		pDOUT = strtok(DOUT," ");	// skipping the element name ( <DOUTPUTS )
		for(loop_cnt=0;loop_cnt<DIGITAL_OUTPUTS;loop_cnt++)
		{
			pDOUT = strtok(NULL,"=");	// locate the channel name from our constant local buffer
			if(strcmp(pDOUT,channel) == 61)
			{	// found the channel
				if(!strcmp(state,LOW_HIGH[1]))
					inouts->dout[loop_cnt] = LOW_HIGH[1];
				else
					inouts->dout[loop_cnt] = LOW_HIGH[0];

			}
			pDOUT = strtok(NULL," ");	// skipping ( \"%s\" ) in our constant local buffer
		}
	}
}

/* My_Load_Buffer */
/*********************************************************************************
 This function loads the socket buffer with updated values for a particular I/O.
********************************************************************************/
void My_Load_Buffer(int selection,My_Socket_Type *my_sock,My_InOuts_Type *my_inouts)
{
	switch(selection)
	{
		case 0:	// FILLING SOCKET BUFFER WITH ANALOG INPUTS DATA
			sprintf(my_inouts->buff,ANA0_7,my_inouts->ain[0],my_inouts->ain[1],my_inouts->ain[2],my_inouts->ain[3],
					  my_inouts->ain[4],my_inouts->ain[5],my_inouts->ain[6],my_inouts->ain[7]);
			strcpy(my_sock->buff,my_inouts->buff);
			break;
		case 1:  // FILLING SOCKET BUFFER WITH DIGITAL INPUTS
			sprintf(my_inouts->buff,DIN0_7,my_inouts->din[0],my_inouts->din[1],my_inouts->din[2],my_inouts->din[3],
					  my_inouts->din[4],my_inouts->din[5],my_inouts->din[6],my_inouts->din[7]);
			strcpy(my_sock->buff,my_inouts->buff);
			sprintf(my_inouts->buff,DIN8_15,my_inouts->din[8],my_inouts->din[9],my_inouts->din[10],my_inouts->din[11],
					  my_inouts->din[12],my_inouts->din[13],my_inouts->din[14],my_inouts->din[15]);
			strcat(my_sock->buff,my_inouts->buff);
			break;
		case 2:
			// FILLING SOCKET BUFFER WITH DIGITAL OUTPUT STATUS
			sprintf(my_inouts->buff,DOUT,my_inouts->dout[0],my_inouts->dout[1],my_inouts->dout[2],my_inouts->dout[3],
					  my_inouts->dout[4],my_inouts->dout[5],my_inouts->dout[6],my_inouts->dout[7]);
			strcpy(my_sock->buff,my_inouts->buff);
			break;
	}
	my_sock->buff[strlen(my_sock->buff)] = 0x00;		// XML SOCKET IS LOOKING FOR 0x00 AT THE END OF THE STRING
	my_sock->txbytes = strlen(my_sock->buff);			// NUMBER OF BYTES THAT NEED TO BE TRANSFERED
}

/* My_Rcv_Pkt */
/*********************************************************************************
 This function reads the xml socket, if there is no data it returns 2 so the
  My_Handler function calls state 2 again (state 2 is the receive state).
********************************************************************************/
int My_Rcv_Pkt(My_Socket_Type *my_sock)
{
	/* receive the packet */
	my_sock->rxbytes = sock_fastread(&my_sock->sock,my_sock->buff,BUFF_SIZE);
	switch(my_sock->rxbytes)
	{
		case -1:
			return 4; // there was an error go to state 4 (NO_WAIT_CLOSE)
		case  0:
			return 2; // connection is okay, but no data received
		default:
			(my_sock->statetime) = MS_TIMER+TCP_TIMEOUT;	// reset time to be in the SEND state
			return 3;	//now go to state 3 (SEND)
	}
}

/* My_Snd_Pkt */
/*********************************************************************************
 This function sends data out the xml socket.
********************************************************************************/
int My_Snd_Pkt(My_Socket_Type *my_sock)
{
	/* send the packet */
	my_sock->txbytes = sock_fastwrite(&my_sock->sock, my_sock->buff,my_sock->txbytes+1);
	switch(my_sock->txbytes)
	{
		case -1:
			return 4; // there was an error go to state 4 (NO_WAIT_CLOSE)
		default:
			memset(my_sock->buff,0x00,BUFF_SIZE);
			my_sock->txbytes = 0;
			(my_sock->statetime) = MS_TIMER+TCP_TIMEOUT;	// reset time to be in the RECEIVE state
			return 2;	//now go to state 2 (RECEIVE)
	}
}

/* My_Handler */
/*********************************************************************************
 This function handles and controls the xml socket.
********************************************************************************/
void My_Handler(My_Socket_Type *my_sock)
{
	static char ipaddr[25],DispStrOut[100];

	switch(my_sock->nextstate)
	{
		case 0:/*INITIALIZATION*/										// listen for incoming connection
			inet_ntoa( ipaddr, my_ip_addr );
			sprintf(DispStrOut,"OFFLINE\nCONNECT TO\nIP: %s\nPORT: %d",
					  ipaddr, MY_PORT);
			while(!btnMsgBox(40,0,240,90,&fi14x16,DispStrOut,1,1));
			if(tcp_listen(&my_sock->sock,MY_PORT,INCOMING_IP,INCOMING_PORT,NULL,0))
			{
				my_sock->rxbytes = my_sock->txbytes = 0;
				my_sock->statetime = 0L;
				(my_sock->nextstate)++;										// init complete move onto next state
				my_sock->statetime = MS_TIMER+TCP_TIMEOUT;				// reset the statetime
			}
			else
				exit(0);
			break;
		case 1://LISTEN//
			if(sock_established(&my_sock->sock))						// check for a connection
			{
				inet_ntoa(ipaddr,my_sock->sock.hisaddr);
				sprintf(DispStrOut,"CONNECTED\nIP: %s",ipaddr);
				while(!btnMsgBox(40,0,240,90,&fi14x16,DispStrOut,1,0));
				//printf("%s\nConnection Established.\n",BLACK);
				(my_sock->statetime) = MS_TIMER+TCP_TIMEOUT;			// reset statetime for RECEIVE state
				(my_sock->nextstate)++;										//  we have connection so move on
			}
			else if ((long)(MS_TIMER-(my_sock->statetime)) > 0)	// if X sec and no sock
				my_sock->nextstate = 4;										//	  abort and re-init
			break;
		case 2://RECEIVE//
			my_sock->nextstate = My_Rcv_Pkt(my_sock);					// see function for details
			if ((long)(MS_TIMER-(my_sock->statetime)) > 0)			// if X sec and still waiting
				my_sock->nextstate = 4;										//	  abort and re-init
			break;
		case 3://SEND//
			my_sock->nextstate = My_Snd_Pkt(my_sock);					// see function for details
			if ((long)(MS_TIMER-(my_sock->statetime)) > 0)			// if X sec and still waiting
				my_sock->nextstate = 4;										//	  abort and re-init
			break;
		case 4://NO WAIT_CLOSE//
			sock_abort(&my_sock->sock);									// close the socket
			my_sock->nextstate = 0;											// go back to the INIT state
	}
	if(!tcp_tick(&my_sock->sock))
		my_sock->nextstate = 0;
}