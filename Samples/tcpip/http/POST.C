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
        post.c

        This program accepts and parses POST style form submission, to
        get information from the user.
*******************************************************************************/
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


/*
 * Web server configuration
 */

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


/********************************
 * End of configuration section *
 ********************************/

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

#ximport "samples/tcpip/http/pages/form.html"      index_html

/* the default for / must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END

#define MAX_FORMSIZE	64
typedef struct {
	char *name;
	char value[MAX_FORMSIZE];
} FORMType;
FORMType FORMSpec[2];

/*
 * parse the url-encoded POST data into the FORMSpec struct
 * (ie: parse 'foo=bar&baz=qux' into the struct
 */
int parse_post(HttpState* state)
{
	auto int retval;
	auto int i;

	// state->s is the socket structure, and state->p is pointer
	// into the HTTP state buffer (initially pointing to the beginning
	// of the buffer).  Note that state->p was set up in the submit
	// CGI function.  Also note that we read up to the content_length,
	// or HTTP_MAXBUFFER, whichever is smaller.  Larger POSTs will be
	// truncated.
	retval = sock_aread(&state->s, state->p,
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
		for(i=0; i<(sizeof(FORMSpec)/sizeof(FORMType)); i++) {
			http_scanpost(FORMSpec[i].name, state->buffer, FORMSpec[i].value,
			              MAX_FORMSIZE);
		}

		// Finished processing--returning 1 indicates that we are done
		return 1;
	}
	// Processing not finished--return 0 so that we can be called again
	return 0;
}

/*
 * Sample submit.cgi function
 */
int submit(HttpState* state)
{
	auto int i;

	if(state->length) {
		/* buffer to write out */
		if(state->offset < state->length) {
			state->offset += sock_fastwrite(&state->s,
					state->buffer + (int)state->offset,
					(int)state->length - (int)state->offset);
		} else {
			state->offset = 0;
			state->length = 0;
		}
	} else {
		switch(state->substate) {
		case 0:
			strcpy(state->buffer, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n");
			state->length = strlen(state->buffer);
			state->offset = 0;
			state->substate++;
			break;

		case 1:
			strcpy(state->buffer, "<html><head><title>Results</title></head><body>\r\n");
			state->length = strlen(state->buffer);
			state->substate++;
			break;

		case 2:
			/* init the FORMSpec data */
			FORMSpec[0].value[0] = '\0';
			FORMSpec[1].value[0] = '\0';

			state->p = state->buffer;
			state->substate++;
			break;

		case 3:
			/* parse the POST information */
			if(parse_post(state)) {
				sprintf(state->buffer, "<p>Username: %s<p>\r\n<p>Email: %s<p>\r\n",
					FORMSpec[0].value, FORMSpec[1].value);
				state->length = strlen(state->buffer);
				state->substate++;
			} else {
			}
			break;

		case 4:
			strcpy(state->buffer, "<p>Go <a href=\"/\">home</a></body></html>\r\n");
			state->length = strlen(state->buffer);
			state->substate++;
			break;

		default:
			state->substate = 0;
			return 1;
		}
	}

	return 0;
}

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/index.html", index_html),
	SSPEC_RESOURCE_FUNCTION("/submit.cgi", submit)
SSPEC_RESOURCETABLE_END

void main()
{
	/* init FORM searchable names - must init ALL FORMSpec structs! */
	FORMSpec[0].name = "user_name";
	FORMSpec[1].name = "user_email";

	sock_init();
	http_init();
	tcp_reserveport(80);

	while (1) {
		http_handler();
	}
}