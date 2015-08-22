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
   sdflash_log.c

   This program is used with RCM3900 series controllers and RCM3300 prototyping
   boards.

   Description
   ===========
   This sample program demonstrates a simple web server which stores a log of
   hits in the SD card's data flash.  This log can be viewed and cleared from a
   web browser.

   Instructions
   ============
   1. If necessary, make changes below in the configuration section to match
      your requirements.

   2. Compile and run this sample program.  If running in debug mode then SD
      flash status and log information is displayed in the STDIO window.

   3. Use a web browser to open the log page (which updates the log), and view
      or clear the log.

   4. Click the "source code" link on the controller's web page to see this
      sample program's #ximported file content.
*******************************************************************************/

// These defines redirect run mode STDIO to serial port A at 57600 baud.
#define STDIO_DEBUG_SERIAL SADR
#define STDIO_DEBUG_BAUD 57600

// This define adds carriage returns ('\r') to each newline char ('\n') when
//  sending STDIO output to a serial port.
#define STDIO_DEBUG_ADDCR

// Uncomment this define to force both run mode and debug mode STDIO to the
//  serial port specified above.
//#define STDIO_DEBUG_FORCEDSERIAL

#class auto
#if !__SEPARATE_INST_DATA__
// When compiling with separate I&D space enabled, it is often best to allow
//  the compiler more leeway in placing code in root space.
#memmap xmem
#endif

#use "rcm39xx.lib"   // sample library to use with this sample program

// set an arbitrary upper limit on the number of flash log data blocks
#define MAX_FLASHLOG_BLOCKCOUNT 1024L

//#define SDFLASH_VERBOSE
//#define SDFLASH_DEBUG
#use "sdflash.lib"   // base SD card flash driver library

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

/*
 *    FORM_ERROR_BUF is required for the zserver form interface in
 *    init_dateform()
 *
 */
#define FORM_ERROR_BUF  256

/*
 * Our web server as seen from the clients.
 * This should be the address that the clients (netscape/IE)
 * use to access your server. Usually, this is your IP address.
 * If you are behind a firewall, though, it might be a port on
 * the proxy, that will be forwarded to the Rabbit board. The
 * commented out line is an example of such a situation.
 */
#define REDIRECTHOST    _PRIMARY_STATIC_IP
//#define REDIRECTHOST  "proxy.domain.com:1212"

/********************************
 * End of configuration section *
 ********************************/

#use "dcrtcp.lib"
#use "http.lib"

/*
 *  REDIRECTTO is used by resetlog_cgi and set_date to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */
#define REDIRECTTO myurl()

#ximport "pages/log.shtml"   index_shtml
#ximport "pages/rabbit1.gif" rabbit1_gif
#ximport "sdflash_log.c"     flashlog_c

// global variables
char sdflash_buf[512];
int count, date_lock, day, hour, minute, month, second, year;
unsigned log_block_datasize, log_read_offset;
long log_block_count, log_end_block, log_read_block, log_start_block;
sd_device *dev;

//flash log header typedef
typedef struct
{
   char marker_string[4];
   unsigned datalen;
} log_block_header;

//
// This routine returns our IP address in URL format
//
nodebug
char *myurl(void)
{
   static char URL[64];
   auto char tmpstr[32];
   auto long ipval;

   ifconfig(IF_DEFAULT, IFG_IPADDR, &ipval, IFS_END);
   sprintf(URL, "http://%s/index.shtml", inet_ntoa(tmpstr, ipval));
   return URL;
}

nodebug
void read_log_reset(void)
{
   log_read_block = log_start_block;
   log_read_offset = 0L;
}

/*
** 1) Attempt to intialize the SD card flash device.
** 2) If device is initialized successfully, look for an existing log.
** 3) If an existing log is not found, create a log.
*/
nodebug
int init_log(void)
{
   auto int err;
   auto long i, j;
   auto int found_start, found_end;
   auto log_block_header header;

   dev = &SD[0];
   if (err = sdspi_initDevice(0, &SD_dev0)) {
      printf("ERROR - SD Card Flash init failed\n");
      return err;
   }
   printf("SD Card Flash Initialized\n");
   printf("# of blocks: %ld\n", dev->sectors);
   printf("size of blocks: %d\n", 512);

   if (MAX_FLASHLOG_BLOCKCOUNT < dev->sectors) {
      printf("Limiting the Flash Log to use just %ld data blocks.\n",
             MAX_FLASHLOG_BLOCKCOUNT);
      log_block_count = MAX_FLASHLOG_BLOCKCOUNT;
   } else {
      printf("The Flash Log can grow to use all %ld data blocks.\n",
             dev->sectors);
      log_block_count = dev->sectors;
   }

   //calculate space for data in a block
   log_block_datasize = 512u - sizeof(log_block_header);

   //find beginning of log(if it exists)
   found_start = 0;
   for (i = 0; i < log_block_count; ++i) {
      if (!(i % 32L)) {
         printf("Checking block #%ld.\r", i);
      }
      if (err = sdspi_read_sector(dev, i, sdflash_buf)) {
         printf("ERROR - SD Card Flash init SLOG read failed\n");
         return err;
      }
      memcpy((void *) &header, (void *) sdflash_buf, sizeof(header));
      if (!strncmp(header.marker_string, "SLOG", 4)) {
         log_start_block = i;
         printf("Found start at block %ld.\n", i);
         found_start = 1;
         break;
      }
   }
   if (found_start) {
      //look for end
      found_end = 0;
      for (i = 0; i < log_block_count; ++i) {
         j = (i + log_start_block) % log_block_count;
         if (err = sdspi_read_sector(dev, j, sdflash_buf)) {
            printf("ERROR - SD Card Flash init ELOG read failed\n");
            return err;
         }
         memcpy((void *) &header, (void *) sdflash_buf, sizeof(header));
         if (!strncmp(header.marker_string, "ELOG", 4)) {
            log_end_block = j;
            printf("Found end at block %ld.\n", j);
            found_end = 1;
            break;
         }
      }
      if (!found_end) {
         printf("Log is completely contained in block %ld.\n", log_start_block);
         log_end_block = log_start_block; //no end found, must be first block
      }
   } else {
      // no log start found, begin new log at block 0
      printf("Creating a new log, starting at block #0.\n");
      memcpy(header.marker_string, "SLOG", 4);
      header.datalen = 0L;
      memcpy(sdflash_buf, &header, sizeof(header));
      if (err = sdspi_write_sector(dev, 0ul, sdflash_buf)) {
         printf("ERROR - SD Card Flash init SLOG write failed\n");
         return err;
      }
      log_start_block = 0;
      log_end_block = 0;
   }
   read_log_reset();
   return 0;   //init OK
}

/*
** Fill buffer with string containing next log contents up to bufsize.
*/
nodebug
int read_log(char *buffer, int bufsize)
{
   auto unsigned readsize, string_index;
   auto log_block_header header;

   string_index = 0u;	// start copying to beginning of string
   while (string_index < bufsize) {
      if (sdspi_read_sector(dev, log_read_block, sdflash_buf)) {
         buffer[0] = '\0'; // signal end of log read
         return -1;  // any read error ends log!
      }
      memcpy((char *) &header, sdflash_buf, sizeof(header));
      if (log_read_offset >= header.datalen) {
         // reached end of log, we're done (so return an empty buffer now)
         break;
      }
      if ((header.datalen-log_read_offset) >= (long) (bufsize-string_index)) {
         // can fill string with data from this buffer
         readsize = bufsize - string_index;
      } else {
         // read to end of data
         readsize = (unsigned) (header.datalen - log_read_offset);
      }
      memcpy(buffer + string_index,
             sdflash_buf + sizeof(header) + log_read_offset, readsize);
      log_read_offset += readsize;
      string_index += readsize;
      if (log_read_offset >= header.datalen) {
         if (log_read_block == log_end_block) {
            // end of log, we're done (except to return empty buffer next time)
            break;
         } else {
            // move on to log's next block
            ++log_read_block;
            log_read_block %= log_block_count;
            log_read_offset = 0L;
         }
      }
   }
   buffer[string_index] = '\0';  // add NULL to end of string
   return (int) string_index;
}

/*
** 1) Clear existing log by removing its header.
** 2) Start new log in next block.
*/
nodebug
void clear_log(void)
{
   auto log_block_header header;

   // kill start block header
   memset(&header, 0, sizeof(header));
   memcpy(sdflash_buf, &header, sizeof(header));
   sdspi_write_sector(dev, log_start_block, sdflash_buf);
   // start at next 'free' block
   log_start_block = (log_end_block + 1) % log_block_count;
   log_end_block = log_start_block;
   // set up header for new start block
   memcpy(header.marker_string, "SLOG", 4);
   header.datalen = 0L;
   memcpy(sdflash_buf, &header, sizeof(header));
   sdspi_write_sector(dev, log_start_block, sdflash_buf);
}

/*
** Append a new log entry to an existing log.
*/
nodebug
int append_log(char *buffer)
{
   auto unsigned string_index, string_size, writesize;
   auto log_block_header header;

   string_size = strlen(buffer);
   string_index = 0u;

   if (sdspi_read_sector(dev, log_end_block, sdflash_buf)) {
      return -1;  // read error
   }
   memcpy(&header, sdflash_buf, sizeof(header));
   while (string_index < string_size) {
      if (log_block_datasize - header.datalen >=
          (long) (string_size - string_index))
      {
         // rest of string can fit into this block
         writesize = string_size - string_index;
      } else {
         // fill this block and go to next
         writesize = (unsigned) (log_block_datasize - header.datalen);
         if (log_end_block != log_start_block) {
            // remove end block marker
            memset(&header, 0, 4);
         }
      }
      memcpy(sdflash_buf + sizeof(header) + header.datalen,
             buffer + string_index, writesize);
      header.datalen += writesize;
      memcpy(sdflash_buf, &header, sizeof(header));   // update header
      if (sdspi_write_sector(dev, log_end_block, sdflash_buf)) {
         return -2;  // write error
      }
      string_index += writesize;
      if (header.datalen == log_block_datasize) {
         // move end to next 'free' block
         ++log_end_block;
         log_end_block %= log_block_count;
         if (log_end_block == log_start_block) {
            return -3;  // ran out of room
         }
         memcpy(header.marker_string, "ELOG", 4);
         header.datalen = 0L;
         memcpy(sdflash_buf, &header, sizeof(header));   // update header
         if (sdspi_write_sector(dev, log_end_block, sdflash_buf)) {
            return -4;  // write error in new block
         }
      }
   }
   return 0;   //write OK
}

/*
 *    int log_cgi(HttpState*)
 *
 *    This cgi function is called from a log.shtml #exec ssi.
 *    Each time the page is requested it calls this function which
 *    records the IP address and time of the access.
 *
 */
nodebug
int log_cgi(HttpState* state)
{
   auto int x;
   static char buffer[512];
   auto int my_xpc;
   auto struct tm time;
   auto sockaddr sock_addr;

#GLOBAL_INIT { count = 0; }

   memset(buffer, 0, sizeof(buffer));

   x=sizeof(sock_addr);
   getpeername((sock_type*)&state->s,&sock_addr,&x);

   tm_rd(&time);
   sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d - %d.%d.%d.%d<br>",
           time.tm_mon, time.tm_mday, time.tm_year + 1900, time.tm_hour,
           time.tm_min, time.tm_sec, *(((char *) &sock_addr.s_ip) + 3),
           *(((char *) &sock_addr.s_ip) + 2), *(((char *) &sock_addr.s_ip) + 1),
           *(((char *) &sock_addr.s_ip) + 0));

   x = append_log(buffer);
   count++;
   return 1;
}

/*
 *    int resetlog_cgi(HttpState*)
 *
 *    This cgi function clears the log.
 *
 */
nodebug
int resetlog_cgi(HttpState* state)
{
   count = 0;
   clear_log();
   cgi_redirectto(state, REDIRECTTO);
   return 0;
}

#define PRTLOG_INIT    0
#define PRTLOG_HEADER  1
#define PRTLOG_PRTITEM 2
#define PRTLOG_FOOTER  3

const char prtlog_header[] =
   "<html><head><title>prtlog.cgi</title></head><body>\r\n" \
   "<h2>Web Log</h2>\r\n";

const char prtlog_footer[] = "</body></html>\r\n";

/*
 *    int printlog_cgi(HttpState*)
 *
 *    This cgi function prints the contents of the log.  It uses
 *    the HttpState substate, buffer, offset, and p variables to
 *    maintain the current state.
 *
 */
nodebug
int printlog_cgi(HttpState* state)
{
   auto int bytes_written;

   switch (state->substate) {
   case PRTLOG_INIT:
      state->p = &prtlog_header[0];
      state->substate = PRTLOG_HEADER;
      read_log_reset();

      // intentionally no break

   case PRTLOG_HEADER:
      if (*state->p) {
         bytes_written = sock_fastwrite(&state->s, state->p, strlen(state->p));
         if (bytes_written > 0)
            state->p += bytes_written;
      } else {
         state->p = NULL;
         state->offset = 0;
         state->substate = PRTLOG_PRTITEM;
      }
      break;

   case PRTLOG_PRTITEM:
      if (state->p == NULL || *state->p == '\0' || *state->p == '\xff') {
         read_log(state->buffer, 128u);
         if (strlen(state->buffer)) {
            //still reading out log contents
            state->p = state->buffer;
            state->offset = 0;
         } else {
            state->offset = 0;
            state->p = &prtlog_footer[0];
            state->substate = PRTLOG_FOOTER;
         }
         state->offset++;
      } else {
         bytes_written = sock_fastwrite(&state->s, state->p, strlen(state->p));
         if (bytes_written > 0) {
            state->p += bytes_written;
         }
      }
      break;

   case PRTLOG_FOOTER:
      if (*state->p) {
         bytes_written = sock_fastwrite(&state->s, state->p, strlen(state->p));
         if (bytes_written > 0)
            state->p += bytes_written;
      } else {
         return 1;   // done
      }
      break;
   }
   return 0;
}

const HttpType http_types[] = {
   { ".shtml", "text/html",  shtml_handler },   // ssi
   { ".cgi",   "",           NULL },            // cgi
   { ".gif",   "image/gif",  NULL },            // gif
   { ".c",     "text/plain", NULL }
};

const HttpSpec http_flashspec[] = {
   { HTTPSPEC_FILE, "/",             index_shtml, NULL, 0, NULL, NULL },
   { HTTPSPEC_FILE, "/index.shtml",  index_shtml, NULL, 0, NULL, NULL },
   { HTTPSPEC_FILE, "/rabbit1.gif",  rabbit1_gif, NULL, 0, NULL, NULL },
   { HTTPSPEC_FILE, "sdflash_log.c", flashlog_c,  NULL, 0, NULL, NULL },

   { HTTPSPEC_VARIABLE, "count", 0, &count, INT16, "%d", NULL },

   { HTTPSPEC_FUNCTION, "/log.cgi",      0, log_cgi,      0, NULL, NULL },
   { HTTPSPEC_FUNCTION, "/printlog.cgi", 0, printlog_cgi, 0, NULL, NULL },
   { HTTPSPEC_FUNCTION, "/resetlog.cgi", 0, resetlog_cgi, 0, NULL, NULL },
};

/*
 *    int set_date(HttpState*);
 *
 *    Change the board time to the user specified time
 *    and redirect the user to the front page.
 *
 */
nodebug
int set_date(HttpState* state)
{
   auto struct tm time;

   if (state->cancel) {
      return 1;
   }
   time.tm_sec = second;
   time.tm_min = minute;
   time.tm_hour = hour;
   time.tm_mon = month;
   time.tm_mday = day;
   time.tm_year = year - 1900;

   tm_wr(&time);
   SEC_TIMER = mktime(&time);

   date_lock = 0;

   cgi_redirectto(state, REDIRECTTO);
   return 0;
}

/*
 *    void update_date(void)
 *
 *    Update the date from the board clock
 *
 */
nodebug
void update_date(void)
{
   auto struct tm time;

#GLOBAL_INIT { date_lock = 0; }

   if (date_lock) return;

   tm_rd(&time);

   month = time.tm_mon;
   day = time.tm_mday;
   year = time.tm_year + 1900;
   hour = time.tm_hour;
   minute = time.tm_min;
   second = time.tm_sec;
}

/*
 *    void lock_date(void)
 *
 *    Lock the date structure so we can update it atomically.
 *
 */
nodebug
void lock_date(HttpState* state)
{
   date_lock = 1;
}

/*
 *    void init_dateform(void)
 *
 *    Initialize the date form using ZServer primitives.
 *
 */
nodebug
void init_dateform(void)
{
   auto int var, form;
   static FormVar dateform[7];

   /*
    *   Set up the date form.  To set up your own form you follow a
    *   similar set of steps.  First call sspec_addform.  Then add
    *   the variables creating the variable and adding it to the form.
    *   If you want to restrict the values of the variables you can
    *   use the sspec_setfvlen, sspec_setfvrange, or define a custom
    *   validation function with sspec_fvcheck.
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
    *   lock_date sets a flag that disables the automatic updating
    *   of the hour, minute, second, month, day, and year variables.
    *   The prolog gets called after the form is parsed correctly,
    *   but before the variables are updated.  The set_date function
    *   gets called after the variables are updated.  The lock_date
    *   function is necessary because the update_date function in
    *   main could be called between the time the variables are
    *   updated and the prolog completes its processing.
    *
    */

   var = sspec_addfunction("lock_date", lock_date, SERVER_HTTP);
   sspec_setformprolog(form, var);
   var = sspec_addfunction("set_date", set_date, SERVER_HTTP);
   sspec_setformepilog(form, var);
}

/*
 *    void main(void)
 *
 *    Initialize the flash, set up the date form and start the web
 *    server.
 *
 */
void main(void)
{
   // it's just good practice to initialize Rabbit's board-specific I/O
   brdInit();

   sock_init();
   http_init();
   init_dateform();
   init_log();

   tcp_reserveport(80); // mark port 80 as a server port

   for(;;) {
      update_date();
      http_handler();
   }
}

