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
/*
		Samples\TCPIP\http\flashlog.c

		This program can be used for boards that have a second flash.
		It is set up to work on the TCP/IP Dev Kit, the NetGateway and
		the Ethernet Core.

		The program logs users that request a controller's page.  The
		log can be viewed online.
*/
#class auto



#memmap xmem

#ifdef USE_2NDFLASH_CODE
   #error "Sample requires second flash!"
#endif

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1


/*
 *		FORM_ERROR_BUF is required for the zserver form interface in
 *		init_dateform()
 *
 */

#define FORM_ERROR_BUF 	256


/**
 * 	Use the second flash to store the log.  This is the base address
 * 	for storing our log.  (A size limit is not implemented.)
 * 	If battery backed up SRAM is good enough, you could change this
 * 	to a variable and allocate a chunk of XMEM.
 */

#define  FLASH_BASE_ADDR	0x40000L


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


/********************************
 * End of configuration section *
 ********************************/

#use "dcrtcp.lib"
#use "http.lib"

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""


/*
 *		Load files into flash
 *
 */

#ximport "samples/tcpip/http/pages/log.shtml"	index_shtml
#ximport "samples/tcpip/http/pages/rabbit1.gif"	rabbit1_gif
#ximport "samples/tcpip/http/flashlog.c"			flashlog_c

/*
 *		log_cgi(HttpState*)
 *
 *		This cgi function is called from a log.shtml #exec ssi.
 *		Each time the page is requested it calls this function which
 *		records the IP address and time of the access.
 *
 */

int count;

int log_cgi(HttpState* state)
{
	int x;
	char buffer[512];
	int my_xpc;
	struct tm time;
	sockaddr sock_addr;

	#GLOBAL_INIT {	count=0;	}

	memset(buffer,0,sizeof(buffer));

	x=sizeof(sock_addr);
	getpeername((sock_type*)&state->s,&sock_addr,&x);

	tm_rd(&time);
	sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d - %d.%d.%d.%d<br>",
		time.tm_mon,
		time.tm_mday,
		time.tm_year+1900,
		time.tm_hour,
		time.tm_min,
		time.tm_sec,
		*(((char*)&sock_addr.s_ip)+3),
		*(((char*)&sock_addr.s_ip)+2),
		*(((char*)&sock_addr.s_ip)+1),
		*(((char*)&sock_addr.s_ip)+0));

	x = WriteFlash2(FLASH_BASE_ADDR + count*sizeof(buffer),
				buffer, sizeof(buffer));

	count++;

   return 1;
}

/*
 *		int resetlog_cgi(HttpState*)
 *
 *		This cgi function clears the log.
 *
 */

int resetlog_cgi(HttpState* state)
{
	count=0;

	cgi_redirectto(state,REDIRECTTO);
   return 0;
}

/*
 *		int printlog_cgi(HttpState*)
 *
 *		This cgi function prints the contents of the log.  It uses
 *		the HttpState substate, buffer, offset, and p variables to
 *		maintain the current state.
 *
 */

#define PRTLOG_INIT		0
#define PRTLOG_HEADER	1
#define PRTLOG_PRTITEM	2
#define PRTLOG_FOOTER	3

const char prtlog_header[] =
	"HTTP/1.0 200 OK\r\n" \
	"Content-Type: text/html\r\n\r\n" \
	"<html><head><title>prtlog.cgi</title></head><body>\r\n" \
	"<h2>Web Log</h2>\r\n";

const char prtlog_footer[] = "</body></html>\r\n";

int printlog_cgi(HttpState* state)
{
	auto int bytes_written;

	switch(state->substate) {
		case PRTLOG_INIT:
			state->p=&prtlog_header[0];
			state->substate=PRTLOG_HEADER;

			// intentionally no break

		case PRTLOG_HEADER:
			if(*state->p) {
				bytes_written=sock_fastwrite(&state->s,state->p,strlen(state->p));
				if(bytes_written>0)
					state->p+=bytes_written;
			} else {
				state->p=NULL;
				state->offset=0;
				state->substate=PRTLOG_PRTITEM;
			}
			break;

		case PRTLOG_PRTITEM:
			if(state->p==NULL || *state->p=='\0' || *state->p=='\xff') {
				if(state->offset<count) {

					xmem2root(state->buffer,
								FLASH_BASE_ADDR + (state->offset * 512), 128);

					state->p=state->buffer;
					state->p[127]=0;
				} else {
					state->offset=0;
					state->p=&prtlog_footer[0];
					state->substate=PRTLOG_FOOTER;
				}
				state->offset++;
			} else {
				bytes_written=sock_fastwrite(&state->s,state->p,strlen(state->p));
				if(bytes_written>0) {
					state->p+=bytes_written;
				}
			}
			break;

		case PRTLOG_FOOTER:
			if(*state->p) {
				bytes_written=sock_fastwrite(&state->s,state->p,strlen(state->p));
				if(bytes_written>0)
					state->p+=bytes_written;
			} else
				return 1;			// done

			break;
	}
	return 0;
}

SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", ""),
	SSPEC_MIME(".c", "text/plain")
SSPEC_MIMETABLE_END

// The static resource table is initialized with these macros...
SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", index_shtml),
	SSPEC_RESOURCE_XMEMFILE("/index.shtml", index_shtml),
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif),
	SSPEC_RESOURCE_XMEMFILE("/flashlog.c", flashlog_c),
	SSPEC_RESOURCE_ROOTVAR("count", &count, INT16, "%d"),
	SSPEC_RESOURCE_FUNCTION("/log.cgi", log_cgi),
	SSPEC_RESOURCE_FUNCTION("/printlog.cgi", printlog_cgi),
	SSPEC_RESOURCE_FUNCTION("/resetlog.cgi", resetlog_cgi)
SSPEC_RESOURCETABLE_END

/*
 *		void setdate();
 *
 *		Change the board time to the user specified time
 *		and redirect the user to the front page.
 *
 */

int hour, minute, second, month, day, year, date_lock;

int set_date(HttpState* state)
{
	auto struct tm time;

	if (state->cancel) {
		return 1;
	}
	time.tm_sec=second;
	time.tm_min=minute;
	time.tm_hour=hour;
	time.tm_mon=month;
	time.tm_mday=day;
	time.tm_year=year-1900;

	tm_wr(&time);
	SEC_TIMER=mktime(&time);

	date_lock=0;

	cgi_redirectto(state,REDIRECTTO);
   return 0;
}


/*
 *		void update_date()
 *
 *		Update the date from the board clock
 *
 */

void update_date()
{
	auto struct tm time;

	#GLOBAL_INIT { date_lock=0; }

	if(date_lock) return;

	tm_rd(&time);

	month=time.tm_mon;
	day=time.tm_mday;
	year=time.tm_year+1900;
	hour=time.tm_hour;
	minute=time.tm_min;
	second=time.tm_sec;
}

/*
 *		void lock_date()
 *
 *		Lock the date structure so we can update it atomically.
 *
 */

void lock_date(HttpState* state)
{
	date_lock=1;
}

/*
 *		void init_dateform()
 *
 *		Initialize the date form using ZServer primatives.
 *
 */

void init_dateform()
{
    auto int var,form;
    static FormVar dateform[7];

    /*
     *	Set up the date form.  To set up your own form you follow a
     *	similar set of steps.  First call sspec_addform.  Then add
     *	the variables creating the variable and adding it to the form.
     *	If you want to restrict the values of the variables you can
     *	use the sspec_setfvlen, sspec_setfvrange, or define a custom
     *	validation function with sspec_fvcheck.
     *
     */

    form = sspec_addform("date.html", dateform, 7, SERVER_HTTP);

    // Set the title of the form
    sspec_setformtitle(form, "Set Date");

    var = sspec_addvariable("hour", &hour, INT16, "%02d", SERVER_HTTP);
    var = sspec_addfv(form, var);
    sspec_setfvname(form, var, "Hour");
    sspec_setfvlen(form, var, 2);
    sspec_setfvrange(form, var, 0, 24);

    var = sspec_addvariable("minute", &minute, INT16, "%02d", SERVER_HTTP);
    var = sspec_addfv(form, var);
    sspec_setfvname(form, var, "Minute");
    sspec_setfvlen(form, var, 2);
    sspec_setfvrange(form, var, 0, 60);

    var = sspec_addvariable("second", &second, INT16, "%02d", SERVER_HTTP);
    var = sspec_addfv(form, var);
    sspec_setfvname(form, var, "Second");
    sspec_setfvlen(form, var, 2);
    sspec_setfvrange(form, var, 0, 60);

    var = sspec_addvariable("month", &month, INT16, "%02d", SERVER_HTTP);
    var = sspec_addfv(form, var);
    sspec_setfvname(form, var, "month");
    sspec_setfvlen(form, var, 2);
    sspec_setfvrange(form, var, 0, 12);

    var = sspec_addvariable("day", &day, INT16, "%02d", SERVER_HTTP);
    var = sspec_addfv(form, var);
    sspec_setfvname(form, var, "day");
    sspec_setfvlen(form, var, 2);
    sspec_setfvrange(form, var, 0, 31);

    var = sspec_addvariable("year", &year, INT16, "%04d", SERVER_HTTP);
    var = sspec_addfv(form, var);
    sspec_setfvname(form, var, "year");
    sspec_setfvlen(form, var, 4);
    sspec_setfvrange(form, var, 1990, 2010);

    /*
     *	lock_date sets a flag that disables the automatic updating
     *	of the hour, minute, second, month, day, and year variables.
     *	The prolog gets called after the form is parsed correctly,
     *	but before the variables are updated.  The set_date function
     *	gets called after the variables are updated.  The lock_date
     *	function is necessary because the update_date function in
     *	main could be called between the time the variables are
     *	updated and the prolog completes its processing.
     *
     */

    var = sspec_addfunction("lock_date", lock_date, SERVER_HTTP);
	 sspec_setformprolog(form, var);
    var = sspec_addfunction("set_date", set_date, SERVER_HTTP);
	 sspec_setformepilog(form, var);

}

/*
 *		main()
 *
 *		Initialize the flash, set up the date form and start the web
 *		server.
 *
 */

void main()
{
	sock_init();
	http_init();
	init_dateform();

	tcp_reserveport(80);			// mark port 80 as a server port.

	for(;;) {
		update_date();
		http_handler();
	}
}