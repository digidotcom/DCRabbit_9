/*****************************************************************************\

	MOTOR.C - Author: Michael Shelley

   This sample demonstrates remote I/O control.  It implements rabbit's SSI and
   CGI capabilities to control a stepper motor connected to an RCM3300 proto-
   board.  It will run under rabbitsys and non-rabbitsys modes.

   Instructions:

   	Connect the L293D H-bridge chip to the U2 socket on the RCM3300
      protoboard.

      Connect a stepper motor to MDA1-4 pin outs according to the stepper's
      specifications.

      For external power, change the jumpers on JP1 from 1-2,9-10 to 3-4,7-8
      and connect VMA+ and VMA- to your power supply.

      Set the TCP/IP settings int LIB\TCPIP\TCP_CONFIG.LIB to your liking and
      compile and run this program.

      Depending on the strength of your motor, you may wish to change the
      ABS_MIN_DELAY and TOG_MIN_DELAY defines if the fastest possible speeds do
      not work.

\*****************************************************************************/

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
//#define TCPCONFIG 1

/*
 * TCP/IP modification - increase TCP socket buffer
 * size, for increased performance.  Note that this buffer size is split in
 * two for TCP sockets-- half for send and half for receive.
 */
#define TCP_BUF_SIZE 2048

/*
 * Only one server is needed for a reserved port
 */
#define HTTP_MAXSERVERS 1
#define MAX_TCP_SOCKET_BUFFERS 1

/*
 * Our web server as seen from the clients.
 * This should be the address that the clients (netscape/IE)
 * use to access your server. Usually, this is your IP address.
 * If you are behind a firewall, though, it might be a port on
 * the proxy, that will be forwarded to the Rabbit board. The
 * commented out line is an example of such a situation.
 */
#define REDIRECTHOST		_PRIMARY_STATIC_IP
//#define REDIRECTHOST	"proxy.domain.com:1212"

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

/*
 *  REDIRECTTO is used by the cgi function to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */
#define REDIRECTTO 		"http://" REDIRECTHOST ""

///////////////////////////////////////////////////////////////////////////////
// MOTOR SETUP

int motor_msd, motor_dir, pos, err;
char motor_str[10];

// The motor must delay ABS_MIN_DELAY ms between steps or risk getting stuck.
// when moving quickly, it is dangerous to switch directions, so TOG_MIN_DELAY
// is the minimum delay between steps during a toggle of direction.
#define ABS_MIN_DELAY 2
#define TOG_MIN_DELAY 5
#define MOTOR_FORWARD 0
#define MOTOR_BACKWARD 1
#define MOTOR_STOP 2

// The sequence below is what is written to PADR to step the motor.
const char seq[] = { 0x0A, 0x09, 0x05, 0x06 };
const char* const errors[] = {
	"",
   "<p>Error - Delay must be greater than 1.</p>",
   "<p>Error - Cannot change from forward <-> backward with delay < 5.</p>"
};

// END SETUP SECTION
///////////////////////////////////////////////////////////////////////////////

// This function is used to print out the motor direction options and select
// the chosen option with dynamic HTML generation.

int options(HttpState* state)
{
	static char option_list[256];
	sprintf(option_list, "<OPTION VALUE=\"0\" ");
   if(motor_dir == MOTOR_FORWARD)
   	strcat(option_list, "SELECTED=\"SELECTED\"");
   strcat(option_list, ">Forward<OPTION VALUE=\"1\" ");
   if(motor_dir == MOTOR_BACKWARD)
   	strcat(option_list, "SELECTED=\"SELECTED\"");
   strcat(option_list, ">Backward<OPTION VALUE=\"2\" ");
   if(motor_dir == MOTOR_STOP)
   	strcat(option_list, "SELECTED=\"SELECTED\"");
   strcat(option_list, ">Stop");
   sprintf(state->buffer, option_list);
	state->headerlen = strlen(state->buffer);
	state->headeroff = 0;
   return 1;
}

// This function prints out any error messages pending and clears err.

int errmsg(HttpState* state)
{
	#GLOBAL_INIT { err = 0; }
	sprintf(state->buffer, errors[err]);
	state->headerlen = strlen(state->buffer);
	state->headeroff = 0;
   err = 0;
   return 1;
}

// This submit function was edited from post.c's parse_post function.
// When the submit button is pushed in the web browser, the data sent is
// handled here, and the page is redirected back to _PRIMARY_STATIC_IP

int submit(HttpState* state)
{
	auto int retval;
	auto int i, old_val;
   auto char temp[21];

	// state->s is the socket structure, and state->p is pointer
	// into the HTTP state buffer (initially pointing to the beginning
	// of the buffer).  Note that state->p was set up in the submit
	// CGI function.  Also note that we read up to the content_length,
	// or HTTP_MAXBUFFER, whichever is smaller.  Larger POSTs will be
	// truncated.
	retval = sock_aread(&(state->s), state->p,
	                    (state->content_length < HTTP_MAXBUFFER-1)?
	                     (int)state->content_length:HTTP_MAXBUFFER-1);
	if (retval < 0) {
		// Error--just bail out
		return 1;
	}

	// Using the subsubstate to keep track of how much data we have received
	state->subsubstate += retval;

	if (state->subsubstate >= state->content_length) {
		// NULL-terminate the content buffer
		state->buffer[(int)state->content_length] = '\0';

		// Scan the received POST information into the FORMSpec structure
		old_val = motor_msd;
		http_scanpost("delay", state->buffer, temp, 40);
      motor_msd = atoi(temp);
      if(motor_msd < ABS_MIN_DELAY) {
      	motor_msd = old_val;
         err = 1;
      }

      old_val = motor_dir;
		http_scanpost("direction", state->buffer, temp, 40);
      motor_dir = atoi(temp);
      // check for toggle between forward and backward with low delay
      if(motor_dir != MOTOR_STOP && old_val != MOTOR_STOP &&
      	motor_dir != old_val &&	motor_msd < TOG_MIN_DELAY) {
      	motor_dir = old_val;
         err = 2;
      }

      // redirect to the main page
	   cgi_redirectto(state, REDIRECTTO);
	}

	return 0;
}

#ximport "pages1/index.html" index_html
#ximport "pages1/rabbit1.gif" rabbit1_gif

SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", index_html),
	SSPEC_RESOURCE_XMEMFILE("/index.shtml", index_html),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif),
	SSPEC_RESOURCE_ROOTVAR("motor_msd", &motor_msd, INT16, "%d"),
	SSPEC_RESOURCE_FUNCTION("/options", options),
	SSPEC_RESOURCE_FUNCTION("/error_message", errmsg),
	SSPEC_RESOURCE_FUNCTION("/submit.cgi", submit)
SSPEC_RESOURCETABLE_END

// Initialize the I/O pins to enable and start the stepper motor.

void motor_init()
{
	pos = 0;
   motor_msd = 20;
   motor_dir = MOTOR_FORWARD;
   // Enable AUX/IO
   WrPortI(SPCR, &SPCRShadow, 0x84);
   // Initialize Port A (motor drive port)
   WrPortI(PADR, &PADRShadow, seq[pos]);
   // H-Bridge enable on
   WrPortI(PFDR, &PFDRShadow, PFDRShadow | 0x30);
   // AUX/IO Chip select
   WrPortI(PGDDR, &PGDDRShadow, PGDDRShadow | 0x10);
   WrPortI(PGDR, &PGDRShadow, PGDRShadow & ~0x10);
   // AUX/IO CP bit
   WrPortI(PEDDR, &PEDDRShadow, PEDDRShadow | 0x08);
}

void main()
{
	int user, page;

	motor_init();
   sock_init();
   http_init();
   tcp_reserveport(80);

   // Authentication setup
	http_setauthentication(HTTP_BASIC_AUTH);

	user = sauth_adduser("admin", "password", SERVER_HTTP);

   page = sspec_addxmemfile("/", index_html, SERVER_HTTP);
   sspec_adduser(page, user);
	sspec_setrealm(page, "Admin");

   page = sspec_addxmemfile("index.html", index_html, SERVER_HTTP);
   sspec_adduser(page, user);
	sspec_setrealm(page, "Admin");

   sspec_addxmemfile("rabbit1.gif", rabbit1_gif, SERVER_HTTP);

   // main loop
   printf("Press any key to quit.\n");
   while(1)
   {
		costate {
      	// PA0-3 feed the byte sequence to the H-Bridge circuit.
         WrPortI(PADR, &PADRShadow, seq[pos]);
         // PE3 acts as the clock to the state carrying flip flops on the
         // RCM33XX protoboard.  These flipflops feed their output to the
         // H-Bridge circuit.  Toggle PE3 to update the state.
   	   WrPortI(PEDR, &PEDRShadow, PEDRShadow | 0x08);
      	WrPortI(PEDR, &PEDRShadow, PEDRShadow & ~0x08);
	      http_handler();
      }
      costate {
			waitfor(DelayMs(motor_msd));
	      switch(motor_dir) {
            case MOTOR_FORWARD:
            	pos = (pos + 1) % 4;
               break;
            case MOTOR_BACKWARD:
					pos = (pos + 3) % 4;
               break;
            case MOTOR_STOP:
               break;
            default:
            	printf("Error - unknown direction\n");
               motor_dir = MOTOR_STOP;
               // shouldn't happen
               break;
			}
      }
      costate {
      	if(kbhit()) {
         	getchar();
            // disable AUX/IO chip select
			   WrPortI(PGDR, &PGDRShadow, PGDRShadow | 0x10);
            // disable H-Bridge chip
				WrPortI(PFDR, &PFDRShadow, PFDRShadow & ~0x30);
            exit(0);
         }
      }
   }
}