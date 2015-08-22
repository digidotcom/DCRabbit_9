/************************************************************************
serial_flashlog.c
ZWorld, 2003

Runs a simple web server and stores a log of hits on the home page of the
server serial flash. The log should be viewed and cleared from a browser.

***********************************************************************/

#class auto

#memmap xmem

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


//setup serial flash chip select for PE7 (RCM3700)
#define SF_SPI_CSPORT PEDR
#define SF_SPI_CSSHADOW PEDRShadow
#define SF_SPI_CSDD PEDDR
#define SF_SPI_CSDDSHADOW PEDDRShadow
#define SF_SPI_CSPIN 6


/********************************
 * End of configuration section *
 ********************************/

#use "dcrtcp.lib"
#use "http.lib"
#use "sflash.lib"

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""


#ximport "/pages/log.shtml"  	index_shtml
#ximport "/pages/rabbit1.gif"	rabbit1_gif
#ximport "serial_flashlog.c"	flashlog_c

/*
 *		log_cgi(HttpState*)
 *
 *		This cgi function is called from a log.shtml #exec ssi.
 *		Each time the page is requested it calls this function which
 *		records the IP address and time of the access.
 *
 */

int count;

//routines and global variables for accessing log in serial flash

typedef struct
{
	char marker_string[4];
   int datalen;
} log_block_header;

int log_start_block, log_end_block, log_block_datasize;
long log_read_offset;


int init_log()
{
	int err, i, j;
   int found_start, found_end;
   log_block_header header;

	sfspi_init();
   if(err = sf_init())
   {
      printf("ERROR - Serial Flash init failed\n");
    	return err;
   }
   printf("Serial Flash Initialized\n");
   printf("# of blocks: %d\n", sf_blocks);
   printf("size of blocks: %d\n", sf_blocksize);

   //calculate space for data in a block
   log_block_datasize = sf_blocksize - sizeof(log_block_header);

   //find beginning of log(if it exists)
   found_start = 0;
   for(i = 0;i < sf_blocks;i++)
   {
   	sf_pageToRAM(i);
      sf_readRAM((char*)&header, 0, sizeof(header));
      if(!strncmp(header.marker_string, "SLOG", 4))
      {
      	log_start_block = i;
         printf("Found start at block %d\n", i);
         found_start = 1;
         break;
      }
   }
   if(found_start)
   {
   	//look for end
      found_end = 0;
      for(i = 0;i < sf_blocks;i++)
      {
      	j = (i + log_start_block) % (int)sf_blocks;
         sf_pageToRAM(j);
         sf_readRAM((char *)&header, 0, sizeof(header));
         if(!strncmp(header.marker_string, "ELOG", 4))
         {
         	log_end_block = j;
            printf("Found end at block %d\n", j);
            found_end = 1;
            break;
         }
      }
      if(!found_end)
      {
      	log_end_block = log_start_block; //no end found, must be first block
      }
   }
   else
   {
   	//no start found, begin new log at block 0
      memcpy(header.marker_string, "SLOG", 4);
      header.datalen = 0;
      sf_writeRAM((char *)&header, 0, sizeof(header));
      sf_RAMToPage(0);
      log_start_block = 0;
      log_end_block = 0;
   }
   return 0; //init OK
}

void read_log_reset()
{
	log_read_offset = 0;
}

//fill buffer with string containing next log contents up to bufsize
int read_log(char *buffer, int bufsize)
{
	int current_block, string_index, block_index, readsize;
   log_block_header header;


   current_block = (log_start_block +
   					 (int)(log_read_offset/log_block_datasize)) % (int)sf_blocks;
                   string_index = 0; //start copying to beginning of string
	block_index = (int)(log_read_offset % log_block_datasize);
	string_index = 0;
   while(string_index < bufsize)
   {
   	sf_pageToRAM(current_block);
   	sf_readRAM((char *)&header, 0, sizeof(header));
      if( (header.datalen - block_index) >= (bufsize - string_index) )
      {
      	//can fill string with data from this buffer
         readsize = bufsize - string_index;
         sf_readRAM(buffer + string_index,
         			  block_index + sizeof(header),
                    readsize);
      	log_read_offset += readsize;
         //filled string, were done
         buffer[bufsize] = 0; //add NULL to end
         return bufsize;
      }
   	else
      {
      	//go to end of block
         readsize = header.datalen - block_index;
         if(readsize)
         {
         	sf_readRAM(buffer + string_index,
         				  block_index + sizeof(header),
               	     readsize);
         	log_read_offset += readsize;
         	string_index += readsize;
         }
         if(current_block == log_end_block)
         {
         	//reached end of log
            buffer[string_index] = 0; //add NULL
            return string_index;
         }
         else
         {
         	current_block = (current_block + 1) % (int)sf_blocks;
            block_index = 0;
         }
      }
   }
   //shouldn't reach here
   printf("ERROR - read_log() failed\n");
   buffer[0] = 0; //return NULL
   return 0;
}

void clear_log()
{
   log_block_header header;

   //kill start block header
   memset(&header, 0, sizeof(header));
   sf_writeRAM((char *)&header, 0, sizeof(header));
   sf_RAMToPage(log_start_block);
   sf_RAMToPage(log_end_block);
   //start at next 'free' block
   log_start_block = (log_end_block + 1) % (int)sf_blocks;
   log_end_block = log_start_block;
	//setup header for new start block
   memcpy(header.marker_string, "SLOG", 4);
   header.datalen = 0;
   sf_writeRAM((char *)&header, 0, sizeof(header));
   sf_RAMToPage(log_start_block);
}

int append_log(char *buffer)
{
   int string_index, string_size, writesize;
   log_block_header header;

   string_size = strlen(buffer);
   string_index = 0;

   sf_pageToRAM(log_end_block);
   sf_readRAM((char *)&header, 0, sizeof(header));
   while(string_index < string_size)
   {
      if((log_block_datasize - header.datalen) >= (string_size - string_index))
      {
      	//rest of string can fit into this block
         writesize = string_size - string_index;
      }
      else
      {
      	//fill this block and go to next
         writesize = log_block_datasize - header.datalen;
         if(log_end_block != log_start_block)
         {
         	//remove end block marker
         	memset(&header, 0, 4);
         }
      }
      sf_writeRAM(buffer + string_index,
      				sizeof(header) + header.datalen,
                  writesize);
      header.datalen += writesize;
      sf_writeRAM((char *)&header, 0, sizeof(header)); //update header
      sf_RAMToPage(log_end_block);
      string_index += writesize;
      if(string_index < string_size)
      {
      	//next block
         log_end_block = (log_end_block + 1) % (int)sf_blocks;
         if(log_end_block == log_start_block)
         {
            return -1; //ran out of room
         }
         memcpy(header.marker_string, "ELOG", 4);
         header.datalen = 0;
      }
   }
   return 0; //write OK
}



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

	x = append_log(buffer);

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

   clear_log();

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
         read_log_reset();

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
         	read_log(state->buffer, 128);

         	if(strlen(state->buffer))
            {
            	//still reading out log contents
				 	state->p=state->buffer;
               state->offset = 0;
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

const HttpType http_types[] =
{
   { ".shtml", "text/html", shtml_handler},	// ssi
   { ".cgi", "", NULL},                     	// cgi
   { ".gif", "image/gif", NULL},					// gif
   { ".c", "text/plain", NULL}
};

const HttpSpec http_flashspec[] =
{
   { HTTPSPEC_FILE,  "/",              index_shtml,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/index.shtml",   index_shtml,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "/rabbit1.gif",   rabbit1_gif,   NULL, 0, NULL, NULL},
   { HTTPSPEC_FILE,  "flashlog.c",  	flashlog_c,    NULL, 0, NULL, NULL},

   { HTTPSPEC_VARIABLE, "count", 0, &count, INT16, "%d", NULL},

   { HTTPSPEC_FUNCTION, "/log.cgi",			0, log_cgi, 		0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/printlog.cgi",	0, printlog_cgi,	0, NULL, NULL},
   { HTTPSPEC_FUNCTION, "/resetlog.cgi",	0, resetlog_cgi,	0, NULL, NULL},
};

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
   brdInit();
	sock_init();
	http_init();
	init_dateform();
   init_log();

	tcp_reserveport(80);			// mark port 80 as a server port.

	for(;;) {
		update_date();
		http_handler();
	}
}