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
/*****************************************************************************\

	MOTOR_RS.C - Author: Michael Shelley

   This is an example of an embedded application taking advantage of the
   built in RabbitSys http server.  This sample is converted from motor.c

   Things to note:
   *	The RabbitSys HTTP Server is run off of port 32080 instead of 80.
   		Direct your browser to http://xx.xx.xx.xx:32080/
	*	This will run under protected mode.
   *	Port E is used by rabbitsys, so accessing it requires using a handle.
   *	HTTP.LIB is not needed, so download time is greatly decreased.
   *	HTTP Resource registration is different.  There are no var resources.
   *	Authentication setup is not required because it is handled by RabbitSys.

   Instructions:

   	Connect the L293D H-bridge chip to the U2 socket on the RCM3300
      protoboard.

      Connect a stepper motor to MDA1-4 pin outs according to the stepper's
      specifications.

      For external power, change the jumpers on JP1 from 1-2,9-10 to 3-4,7-8
      and connect VMA+ and VMA- to your power supply.

      Depending on the strength of your motor, you may wish to change the
      ABS_MIN_DELAY and TOG_MIN_DELAY defines if the fastest possible speeds do
      not work.

      Set MY_IP_ADDRESS or comment it out to use DHCP

\*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// NETWORK SETUP

#use "dcrtcp.lib"
// This structure is used to pass data back to the internal HTTP server of
// RabbitSys.
rs_SSI_CGI_State st;
char mybuffer[512];
// Define your initial ip address here, or comment it out to use DHCP
#define MY_IP_ADDRESS "10.10.6.75"

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

// sequence of bits to write to H-Bridge circuit to run the motor
const char seq[] = { 0x0A, 0x09, 0x05, 0x06 };
const char* const errors[] = {
	"",
   "<p>Error - Delay must be greater than 1.</p>",
   "<p>Error - Cannot change from forward <-> backward with delay < 5.</p>"
};

// END SETUP SECTION
///////////////////////////////////////////////////////////////////////////////

// The following functions are mostly the same as in the original sample.
// However, note the change in the html file when calling them.
//	-	original html file:		<!--#exec cmd="options"-->
// -	new html file:				'options,;

// The get_delay function was added in place of the shared variable motor_msd.
// The rabbitsys http server does not support variables.  The following
// function is a simple workaround.

rs_SSI_CGI_State *get_delay()
{
   st.buffer = mybuffer;
   st.retval = RS_SSI_SENDDONE;

   sprintf(mybuffer, "%d", motor_msd);

	return &st;
}

// This function is used to print out the motor direction options and select
// the chosen option with dynamic HTML generation.

rs_SSI_CGI_State *option()
{
   st.buffer = mybuffer;
	st.retval = RS_SSI_SENDDONE;		//our default return code.

	sprintf(mybuffer, "<OPTION VALUE=\"0\" ");
   if(motor_dir == MOTOR_FORWARD)
   	strcat(mybuffer, "SELECTED=\"SELECTED\"");
   strcat(mybuffer, ">Forward<OPTION VALUE=\"1\" ");
   if(motor_dir == MOTOR_BACKWARD)
   	strcat(mybuffer, "SELECTED=\"SELECTED\"");
   strcat(mybuffer, ">Backward<OPTION VALUE=\"2\" ");
   if(motor_dir == MOTOR_STOP)
   	strcat(mybuffer, "SELECTED=\"SELECTED\"");
   strcat(mybuffer, ">Stop");

   return &st;
}

// This function prints out any error messages pending and clears err.

rs_SSI_CGI_State *errmsg()
{
	#GLOBAL_INIT { err = 0; }
   st.buffer = mybuffer;
	st.retval = RS_SSI_SENDDONE;		//our default return code.

	sprintf(mybuffer, errors[err]);
   err = 0;
   return &st;
}

// The submit function contains the greatest changes from the original motor.c
// This is because it must communicate with the rabbitsys http server.  It
// remains fairly simple because it is just a redirection.  All that is needed
// is to process the data, and call redirect.  See http.c for an example
// without redirection.

rs_SSI_CGI_State *submit(sysHttpState *h)
{
	int url_len, x302msg_len;

   int old_val;
   char ipaddr[40];

   st.buffer = mybuffer;
	switch(h->action)
   {
	   case RS_CGI_DATA:
      	// At this point, h->tag should contain a variable name while
         // h->buffer contains the value.
	      if(!strcmp(h->tag, "delay")) {
	         old_val = motor_msd;
	         motor_msd = atoi(h->buffer);
	         if(motor_msd < ABS_MIN_DELAY) {
	            motor_msd = old_val;
	            err = 1;
	         }
	      }
	      else if(!strcmp(h->tag, "direction")) {
	         old_val = motor_dir;
	         motor_dir = atoi(h->buffer);
	         // check for toggle between forward and backward with low delay
	         if(motor_dir != 2 && old_val != 2 && motor_dir != old_val &&
	            motor_msd < TOG_MIN_DELAY) {
	            motor_dir = old_val;
	            err = 2;
	         }
	      }

	      st.retval = RS_CGI_OK;
	      break;

	   case RS_CGI_EOF:
      	// redirect the browser to our main page
		   inet_ntoa(mybuffer, gethostid());
			sprintf(ipaddr, "http://%s:32080/", mybuffer);
         _sys_cgi_redirect(mybuffer, ipaddr);

	      st.retval = RS_CGI_SEND_DONE;
	      break;

	   default:
	      st.retval = RS_CGI_OK;
	      break;
	}
	return &st;
}

#ximport "pages2/index.html" index_html
#ximport "pages2/rabbit1.gif" rabbit1_gif

RS_HTTPRESOURCETABLE_START
	RS_HTTPRESOURCE_XMEMFILE_HTML("/", index_html),
	RS_HTTPRESOURCE_XMEMFILE_HTML("/index.html", index_html),
	RS_HTTPRESOURCE_XMEMFILE_GIF("/rabbit1.gif", rabbit1_gif),
	RS_HTTPRESOURCE_FUNCTION("get_delay", get_delay),
	RS_HTTPRESOURCE_FUNCTION("option", option),
	RS_HTTPRESOURCE_FUNCTION("error_message", errmsg),
	RS_HTTPRESOURCE_CGI("/submit.cgi", submit),
RS_HTTPRESOURCETABLE_END

// Initialize the I/O pins to enable and start the stepper motor.
//	-	Port E is used by rabbitsys, so a handle is needed for access to PEDDR.

void motor_init()
{
	handle peddrhandle;

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
   peddrhandle = _sys_open(_SYS_OPEN_INTERFACE, PEDDR);
   _sys_write(peddrhandle, PEDDRShadow | 0x08);
   _sys_close(peddrhandle);
}

// Changes from original:
//	-	Note the lack of authentication setup.  This is because authentication
// is built into rabbitsys.
//	-	Port E is used by rabbitsys, so a handle is needed for access.
//	-	RS_REGISTERTABLE(); is used to register the http resources
//	-	_sys_tick replaces http_handler

void main()
{
	handle pedrhandle;

#ifdef MY_IP_ADDRESS
   ifconfig(IF_ETH0, IFS_DOWN, IFS_DHCP, 0, IFS_IPADDR, aton(MY_IP_ADDRESS),
   	IFS_UP, IFS_END);
#endif

	motor_init();
	RS_REGISTERTABLE();
   pedrhandle = _sys_open(_SYS_OPEN_INTERFACE, PEDR);

   printf("Press any key to quit.\n");

   while(1)
   {
		costate {
         WrPortI(PADR, &PADRShadow, seq[pos]);

	   	// Toggle PE3 to change state
	      _sys_write(pedrhandle, PEDRShadow | 0x08);
	      _sys_write(pedrhandle, PEDRShadow & ~0x08);

			_sys_tick(1);
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
            // close PEDR handle
		      _sys_close(pedrhandle);
            exit(0);
         }
      }
   }
}