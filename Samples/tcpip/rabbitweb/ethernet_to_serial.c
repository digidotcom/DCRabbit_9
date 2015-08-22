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
/*******************************************************************************
        Samples\TcpIp\RabbitWeb\ethernet_to_serial.c

        Uses the RabbitWeb HTTP enhancements to configure a simple 
        Ethernet-to-serial converter.  This sample only supports listening 
        TCP sockets, meaning that Ethernet-to-serial devices can only be 
        started by another device initiating the network connection to the 
        Rabbit.

        Each serial port can be associated with a specific TCP port.  The Rabbit
        will listen on each of these TCP ports for a connection, which will
        then be associated with a specific serial port.  Data will then be
        shuttled between the serial and Ethernet connections.
*******************************************************************************/

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

/*
 * The following array defines which serial ports are used in this sample.
 * Please check the documentation for this board to find which serial ports
 * are convenient to use.  In particular, some Rabbit 3000-based boards make
 * ports E and F available rather than B and C.  Note, also, that you can have
 * any number of serial ports defined below, not just two.
 */
const char ports_config[] = { 'B', 'C' };

#define E2S_BUFFER_SIZE 1024

/*
 * Only one server is really needed for the HTTP server as long as
 * tcp_reserveport() is called on port 80.
 */
#define HTTP_MAXSERVERS 1

/*
 * Define the number of TCP socket buffers to the number of sockets needed for
 * the HTTP server (HTTP_MAXSERVERS) + the number of serial ports configured
 * (sizeof(ports_config)).
 */
#define MAX_TCP_SOCKET_BUFFERS (HTTP_MAXSERVERS + sizeof(ports_config))

/*
 * Define the sizes of the input and output buffers for each of the serial
 * ports
 */
#define SERINBUFSIZE		127
#define SEROUTBUFSIZE	127

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem

/*
 * This is needed to be able to use the RabbitWeb HTTP enhancements and the 
 * ZHTML scripting language.
 */
#define USE_RABBITWEB 1

#use "dcrtcp.lib"
#use "http.lib"

/*
 * This page contains the configuration interface for the serial ports.
 */
#ximport "samples/tcpip/rabbitweb/pages/config.zhtml"    config_zhtml

/* The default mime type for '/' must be first */
SSPEC_MIMETABLE_START
   // This handler enables the ZHTML parser to be used on ZHTML files...
	SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif")
SSPEC_MIMETABLE_END

/* Associate the #ximported files with the web server */
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", config_zhtml),
	SSPEC_RESOURCE_XMEMFILE("/index.zhtml", config_zhtml)
SSPEC_RESOURCETABLE_END

/*
 * Function declarations
 */
void restart_socket(int i);
void update_tcp(void);
void restart_serial(int i);
void update_serial(void);
void serial_open(int i);
void e2s_init(void);
void e2s_tick(void);

/*
 * This structure contains the configuration information for each serial port /
 * TCP port pair.
 */
struct SerialPort {
	word tcp_port;
	struct {
		char port;
		long baud;
		int databits;
      int parity;
		int stopbits;
	} ser;
};

/*
 * This stores the configuration information on the serial ports.  The members
 * will be registered with the #web statement so that the HTTP enhancements can
 * be used.
 */
struct SerialPort serial_ports[sizeof(ports_config)];

/*
 * This will function as a copy of the above.  It is used to determine which
 * port information changed when the update function is called (this will be
 * explained later in the program).
 */
struct SerialPort serial_ports_copy[sizeof(ports_config)];

/*
 * #web statements
 */

// A #web registration for the TCP port.  Note that the only rule in the guard
// is that the new value must be greater than 0.
#web serial_ports[@].tcp_port ($serial_ports[@].tcp_port > 0)
//#web serial_ports[@].ser_port groups=all(ro)
// The character ('B', 'C', etc.) representing the serial port.  This is a
// read-only variable.
#web serial_ports[@].ser.port
// The following two #web statements correspond to the baud rate.  The guards
// are split into two so that the WEB_ERROR() feature can be used.  WEB_ERROR()
// will indicated why the guard statement failed; the string message can later
// be used in the ZHTML scripting.
#web serial_ports[@].ser.baud (($serial_ports[@].ser.baud >= 300)? \
                               1:WEB_ERROR("too low"))
#web serial_ports[@].ser.baud (($serial_ports[@].ser.baud <= 115200)? \
                               1:WEB_ERROR("too high"))
// Each of the following are selection variables, since each of the variables
// can only take on a few values.
#web serial_ports[@].ser.databits select("7" = 7, "8" = 8)
#web serial_ports[@].ser.parity select("None" = 0, "Even", "Odd")
#web serial_ports[@].ser.stopbits select("1" = 1, "2" = 2)

// The #web_update feature will initiate a function call when the corresponding
// variables are updated.  Note that update_tcp() will be called when the TCP
// port changes, and update_serial() will be called when any of the other
// members are updated.
#web_update serial_ports[@].tcp_port update_tcp
#web_update serial_ports[@].ser.baud,serial_ports[@].ser.databits,\
            serial_ports[@].ser.stopbits update_serial

// The following simply sets the buffer sizes for the serial ports based on the
// user configuration above.
#define AINBUFSIZE	SERINBUFSIZE
#define AOUTBUFSIZE	SEROUTBUFSIZE
#define BINBUFSIZE	SERINBUFSIZE
#define BOUTBUFSIZE	SEROUTBUFSIZE
#define CINBUFSIZE	SERINBUFSIZE
#define COUTBUFSIZE	SEROUTBUFSIZE
#define DINBUFSIZE	SERINBUFSIZE
#define DOUTBUFSIZE	SEROUTBUFSIZE
#define EINBUFSIZE	SERINBUFSIZE
#define EOUTBUFSIZE	SEROUTBUFSIZE
#define FINBUFSIZE	SERINBUFSIZE
#define FOUTBUFSIZE	SEROUTBUFSIZE

// The following symbols represent different states in the Ethernet-to-serial
// state machine
enum {
	E2S_INIT,
	E2S_LISTEN,
	E2S_PROCESS
};

// This is the core of the Ethernet-to-serial state machine.
struct {
	int state;			// Current state of the the state machine
	tcp_Socket sock;	// Socket associated with this serial port
	// The following members are function pointers for accessing this serial
	// port
	int (*open)();
	int (*close)();
	int (*read)();
	int (*write)();
	int (*setdatabits)();
	int (*setparity)();
} e2s_state[sizeof(ports_config)];

// A temporary buffer for copying data between the serial ports and TCP ports.
char e2s_buffer[E2S_BUFFER_SIZE];

// Aborts and restarts the given socket (index i).
void restart_socket(int i)
{
	printf("Restarting socket %d\n", i);
   // Abort the socket
   sock_abort(&(e2s_state[i].sock));
   // Set up the state machine to reopen the socket
   e2s_state[i].state = E2S_INIT;
}

// This function is called when a TCP port is updated via the HTML interface.
// It determines which TCP port(s) changed, and then restarts them with the new
// parameters.
void update_tcp(void)
{
	auto int i;

	// Check which TCP port(s) changed
	for (i = 0; i < sizeof(ports_config); i++) {
		if (serial_ports[i].tcp_port != serial_ports_copy[i].tcp_port) {
			// This port has changed, restart the socket on the new port
			restart_socket(i);
			// Save the new port, so we can check which one changed on the next
			// update
			serial_ports_copy[i].tcp_port = serial_ports[i].tcp_port;
		}
	}
}

// Closes and reopens the given serial port (index i).
void restart_serial(int i)
{
	printf("Restarting serial port %d\n", i);
   // Close the serial port
	e2s_state[i].close();
   // Open the serial port
   serial_open(i);
}

// This function is called when a serial port is updated via the HTML interface.
// It determines which serial port(s) changed, and then restarts them with the
// new parameters.
void update_serial(void)
{
	auto int i;

	// Check which serial port(s) changed
	for (i = 0; i < sizeof(ports_config); i++) {
		if (memcmp(&(serial_ports[i].ser), &(serial_ports_copy[i].ser),
		    sizeof(serial_ports[i].ser))) {
			// This serial port has changed, so re-open the serial port with the
			// new parameters
			restart_serial(i);
			// Save the new parameters, so we can check which one changed on the
			// next update
			memcpy(&(serial_ports_copy[i].ser), &(serial_ports[i].ser),
			       sizeof(serial_ports[i].ser));
		}
	}
}

// This function does all of the work necessary to open a serial port, including
// setting the number of data bits, stop bits, and parity.
void serial_open(int i)
{
	// Open the serial port
	e2s_state[i].open(serial_ports[i].ser.baud);

	// Set the data bits
	if (serial_ports[i].ser.databits == 7) {
		e2s_state[i].setdatabits(PARAM_7BIT);
	}
	else {
		e2s_state[i].setdatabits(PARAM_8BIT);
	}

	// Set the stop bits
	if (serial_ports[i].ser.stopbits == 1) {
   	if (serial_ports[i].ser.parity == 0) {
      	// No parity
			e2s_state[i].setparity(PARAM_NOPARITY);
      }
      else if (serial_ports[i].ser.parity == 1) {
      	// Even parity
      	e2s_state[i].setparity(PARAM_EPARITY);
      }
      else {
      	// Odd parity (== 2)
      	e2s_state[i].setparity(PARAM_OPARITY);
      }
	}
	else {
   	// 2 stop bits
		e2s_state[i].setparity(PARAM_2STOP);
	}
}

// Initialize the Ethernet-to-serial state machine.
void e2s_init(void)
{
	auto int i;

	for (i = 0; i < sizeof(ports_config); i++) {
	   // Initialize the state
	   e2s_state[i].state = E2S_INIT;

	   // Initialize the serial function pointers
	   switch (ports_config[i]) {
	   case 'A':
	   	e2s_state[i].open = serAopen;
	   	e2s_state[i].close = serAclose;
	   	e2s_state[i].read = serAread;
	   	e2s_state[i].write = serAwrite;
	   	e2s_state[i].setdatabits = serAdatabits;
	   	e2s_state[i].setparity = serAparity;
	   	break;
	   case 'B':
	   	e2s_state[i].open = serBopen;
	   	e2s_state[i].close = serBclose;
	   	e2s_state[i].read = serBread;
	   	e2s_state[i].write = serBwrite;
	   	e2s_state[i].setdatabits = serBdatabits;
	   	e2s_state[i].setparity = serBparity;
	   	break;
	   case 'C':
	   	e2s_state[i].open = serCopen;
	   	e2s_state[i].close = serCclose;
	   	e2s_state[i].read = serCread;
	   	e2s_state[i].write = serCwrite;
	   	e2s_state[i].setdatabits = serCdatabits;
	   	e2s_state[i].setparity = serCparity;
	   	break;
	   case 'D':
	   	e2s_state[i].open = serDopen;
	   	e2s_state[i].close = serDclose;
	   	e2s_state[i].read = serDread;
	   	e2s_state[i].write = serDwrite;
	   	e2s_state[i].setdatabits = serDdatabits;
	   	e2s_state[i].setparity = serDparity;
	   	break;
#if (CPU_ID_MASK(_CPU_ID_)) >= R3000
	   case 'E':
	   	e2s_state[i].open = serEopen;
	   	e2s_state[i].close = serEclose;
	   	e2s_state[i].read = serEread;
	   	e2s_state[i].write = serEwrite;
	   	e2s_state[i].setdatabits = serEdatabits;
	   	e2s_state[i].setparity = serEparity;
	   	break;
	   case 'F':
	   	e2s_state[i].open = serFopen;
	   	e2s_state[i].close = serFclose;
	   	e2s_state[i].read = serFread;
	   	e2s_state[i].write = serFwrite;
	   	e2s_state[i].setdatabits = serFdatabits;
	   	e2s_state[i].setparity = serFparity;
	   	break;
#endif
	   default:
	   	// Error--not a valid serial port
	   	exit(-1);
	   }

	   // Open each serial port
	   serial_open(i);
	}
}

// Drive the Ethernet-to-serial state machine.  This largely concerns itself
// with handling each of the TCP sockets (the different states correspond to
// the state of the TCP socket).  In particular, in the E2S_PROCESS state, it
// does the copying of data from TCP socket to serial port and vice versa.
void e2s_tick(void)
{
	auto int i;
   auto int len;
   auto tcp_Socket *sock;

	for (i = 0; i < sizeof(ports_config); i++) {
   	sock = &(e2s_state[i].sock);
		switch (e2s_state[i].state) {
		case E2S_INIT:
			tcp_listen(sock, serial_ports[i].tcp_port, 0, 0, NULL, 0);
			e2s_state[i].state = E2S_LISTEN;
			break;
		case E2S_LISTEN:
      	if (!sock_waiting(sock)) {
         	// The socket is no longer waiting
            if (sock_established(sock)) {
            	// The socket is established
            	e2s_state[i].state = E2S_PROCESS;
            }
            else if (!sock_alive(sock)) {
            	// The socket was established but then aborted by the peer
               e2s_state[i].state = E2S_INIT;
            }
            else {
            	// The socket was opened, but is now closing.  Just go to the
               // PROCESS state to read off any data.
               e2s_state[i].state = E2S_PROCESS;
            }
         }
			break;
		case E2S_PROCESS:
      	// Check if the socket is dead
         if (!sock_alive(sock)) {
         	e2s_state[i].state = E2S_INIT;
         }
			// Read from TCP socket and write to serial port
			len = sock_fastread(sock, e2s_buffer, E2S_BUFFER_SIZE);
         if (len < 0) {
         	// Error
            sock_abort(sock);
            e2s_state[i].state = E2S_INIT;
         }
         if (len > 0) {
         	// Write the read data to the serial port--Note that for simplicity,
            // this code will drop bytes if more data has been read from the TCP
            // socket than can be written to the serial port.
            e2s_state[i].write(e2s_buffer, len);
         }
         else {
         	// No data read--do nothing
         }
         // Read from the serial port and write to the TCP socket
         len = e2s_state[i].read(e2s_buffer, E2S_BUFFER_SIZE, (unsigned long)0);
			if (len > 0) {
         	// Write the read data to the TCP port--Note that for simplicity,
            // this code will drop bytes if more data has been read from the
            // serial port than can be written to the TCP socket.
            len = sock_fastwrite(sock, e2s_buffer, len);
            if (len < 0) {
            	// Error
               sock_abort(sock);
               e2s_state[i].state = E2S_INIT;
            }
         }
			break;
		}
	}
}

void main(void)
{
	auto int i;

	// Initialize the serial_ports data structure
	for (i = 0; i < sizeof(ports_config); i++) {
		serial_ports[i].tcp_port = 1234 + i;
		serial_ports[i].ser.port = ports_config[i];
		serial_ports[i].ser.baud = 9600;
		serial_ports[i].ser.databits = 8;
      serial_ports[i].ser.parity = 0;
		serial_ports[i].ser.stopbits = 1;
	}
	// Make a copy of the configuration options to be compared against when
	// the update functions are called
	memcpy(serial_ports_copy, serial_ports, sizeof(serial_ports));

	// Initialize the TCP/IP stack, HTTP server, and Ethernet-to-serial state
	// machine.
   sock_init();
   http_init();
   e2s_init();

	// This is a performance improvement for the HTTP server (port 80),
	// especially when few HTTP server instances are used.
   tcp_reserveport(80);

   while (1) {
		// Drive the HTTP server
      http_handler();
      // Drive the Ethernet-to-serial state machine
      e2s_tick();
   }
}

