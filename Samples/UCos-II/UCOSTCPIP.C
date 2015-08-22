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
/*****************************************************

      ucostcpip.c

      Example of using TCP/IP and uC/OS together.

      In this example uC/OS is monitoring the Ethernet
      link and reflecting the link light on the "LEDs"
      on the development board.  At the same time, this
      program has a ECHO server and HTTP server running.

      To use this program direct a web browser at the
      board.  To use the echo server telnet to port 7.

      When you unplug the Ethernet cable from the board,
      a message appears in the STDIO window.  If you have
      a board with PORTA hooked to the LEDs uncomment
      the "#define LEDS" below and the actual lights will
      change instead of the printed message.

******************************************************/
#class auto 			// Change default storage class for local variables: on the stack

/* uCOS configuration */
#define OS_MAX_TASKS           3  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_SEM_EN					 1			// Enable semaphores
#define OS_SEM_POST_EN			 1       // Enable old style post to semaphore
#define OS_TIME_DLY_HMSM_EN	 1
#define OS_MAX_EVENTS			 5			// MAX_TCP_SOCKET_BUFFERS + 2 + 1 (1 semaphore is used in this app)
#define STACK_CNT_512			 2			// LED update task + main()
#define STACK_CNT_2K         	 2			// TCP/IP needs a 2K stack

/* TCP/IP configuration */
#define TCPCONFIG              1			// default TCP/IP configuration

#define ETH_MTU                1500		// Set the MTU to the maximum Ethernet MTU
#define TCP_BUF_SIZE           8192		// Make the TCP tx and rx buffers 4K each
#define MAX_TCP_SOCKET_BUFFERS 2			// Two sockets, one for HTTP and one for echo.

/* HTTP configuration */
#define HTTP_MAXSERVERS        1			// Have only one active server.

/* Program configuration */
//#define LEDS									// PORT A has LEDs attached

#memmap xmem
#use "ucos2.lib"
#use "dcrtcp.lib"
#use "http.lib"

/*
 *		Simple HTTP setup from samples/tcpip/static.c
 *		For more information about setting up a HTTP server
 *		look at the comments in that sample program.
 */

#ximport "samples/tcpip/http/pages/static.html"    index_html
#ximport "samples/tcpip/http/pages/rabbit1.gif"    rabbit1_gif

const HttpType http_types[] =
{
   { ".html", "text/html", NULL},
   { ".gif", "image/gif", NULL}
};

const HttpSpec http_flashspec[] =
{
   { HTTPSPEC_FILE,  "/",              index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.html",    index_html,    NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/rabbit1.gif",   rabbit1_gif,   NULL, 0, NULL, NULL},
};

/*
 *		This task turns on the LEDs.  If you define LEDS at the
 *		top of the program it turns on and off leds on PORT A.
 *		This setting would be useful on a board like the RCM2100
 *		that has PORT A connected to the development board LEDs.
 *
 *		If you do not define LEDS it prints a message to the
 *		STDIO window when the LINK on the Ethernet goes up or
 *		down.
 */

void ledtask(void* ptr)
{
#ifdef LEDS
	WrPortI(SPCR,&SPCRShadow,0x84);

	while(1) {
		OSTimeDlyHMSM(0,0,0,250);
		if(pd_havelink(0))
			WrPortI(PADR,&PADRShadow,0x00);
		else
			WrPortI(PADR,&PADRShadow,0xff);
	}
#else
	static int	status;
   auto int 	newstatus;

	printf("LEDs initialized\n");

	status=-1;
	while(1) {
		OSTimeDlyHMSM(0,0,0,250);
		newstatus=pd_havelink(0);

		if(newstatus!=status) {
			if(newstatus)
				printf("Link Up\n");
			else
				printf("Link Down\n");

			status=newstatus;
		}
	}
#endif
}

/*
 *		This task listens for an incoming connection on
 *		the well define ECHO port.  When there is an active
 *		connection this task echos back whatever is sent
 *		to it.
 *
 */

void echotask(void* ptr)
{
	static char buffer[1024];
	static tcp_Socket echo_socket;
	auto int 	length;

	while(1) {
		tcp_listen(&echo_socket,7,0,0,NULL,0);

		while(!sock_established(&echo_socket) && sock_bytesready(&echo_socket)==-1) {
			tcp_tick(NULL);
			OSTimeDlyHMSM(0,0,1,0);
		}

		while(tcp_tick(&echo_socket)) {
			length=sock_rbused(&echo_socket);

			if(length>sock_tbleft(&echo_socket))
				length=sock_tbleft(&echo_socket);

			if(length>sizeof(buffer))
				length=sizeof(buffer);

			if(length) {
				sock_read(&echo_socket,buffer,length);
				sock_write(&echo_socket,buffer,length);
			} else
				OSTimeDlyHMSM(0,0,0,100);
		}
	}
}

/*
 *		Run the HTTP server in the background
 */

void httptask(void* ptr)
{
	http_init();

	while(1) {
		http_handler();
	}
}

void main()
{
	OSInit();				// init uC/OS
	sock_init();			// init the TCP/IP stack

	tcp_reserveport(7);	// enable SYN queuing on port 7 and 80.
	tcp_reserveport(80);

	/*
	 *		Start tasks.  ledtask runs every 250ms, echo every
	 *		second when not connected and every 100ms when
	 *		connected.  httptask runs otherwise.
	 */

	OSTaskCreate(ledtask,NULL,512,0);
	OSTaskCreate(echotask,NULL,2048,1);
	OSTaskCreate(httptask,NULL,2048,2);

	OSStart();				// start uC/OS
}