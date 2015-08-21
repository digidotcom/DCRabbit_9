/*
	http client.c
	Digi International, Copyright © 2008.  All rights reserved.

	Description
	===========
	This sample program demonstrates the use of httpc.lib to request files from
	a remote web server and dump it to stdout.

	In addition, it shows how you would read the headers of the HTTP server's
	response, and use the HTTP server's clock to update the Rabbit's RTC.

*/

//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// default functions to xmem
#memmap xmem

/*
 * NETWORK CONFIGURATION
 * Please see the function help (Ctrl-H) on TCPCONFIG for instructions on
 * compile-time network configuration.
 */
#define TCPCONFIG 1
#use "dcrtcp.lib"

// turn on verbose debug output from the HTTP Client library
//#define HTTPC_VERBOSE

#use "httpc.lib"

// ----- options for this demo program -----

// define SHOW_HEADERS to display the HTTP headers in addition to the body
//#define SHOW_HEADERS

// define UPDATE_RTC to sync the Rabbit's clock to the server's
#define UPDATE_RTC

// example of how to build x-www-form-urlencoded data to submit to cgi scripts
void buildPost()
{
	int len;

	char post[256];

	len = 0;
	len += sprintf (&post[len], "real=%5.3f", 3.1415);
	len += sprintf (&post[len], "&int=%d", 12345);
	len += urlencodestr (&post[len], "&string=", "Some string :&$@?");

	printf ("%s\n", post);
	// httpc_posturl (&hsock, "http://www.domain.com/cgi-bin/post.cgi",
	//		post, len, NULL);

	return;
}

void httpc_demo(tcp_Socket *sock)
{
	httpc_Socket hsock;
	int err;

	char url[256];

   err = httpc_init (&hsock, sock);
   if (err)
   {
      printf ("error %d calling httpc_init()\n", err);
   }
   else
   {
      printf ("\nEnter a URL to retrieve using the following format:\n");
      printf ("[http://][user:pass@]hostname[:port][/file.html]\n");
      printf ("Items in brackets are optional.  Examples:\n");
      printf ("  http://www.google.com/\n");
      printf ("  www.google.com\n");
      printf ("  google.com\n");
      printf ("  http://tomlogic.com/fortune.cgi\n");
      while (1)
      {
         printf ("\n\nenter URL (blank to exit): ");
         gets (url);
         if (*url == '\0') break;
         printf ("Connecting to server...\n");
         err = httpc_get_url (&hsock, url);
         if (err)
         {
            printf ("error %d calling httpc_get_url()\n", err);
         }
         else
         {
            #ifdef SHOW_HEADERS
               while (hsock.state == HTTPC_STATE_HEADER)
               {
                  err = httpc_read_header (&hsock, url, sizeof(url), 0);
                  if (err > 0)
                  {
                  	#ifndef HTTPC_VERBOSE
                  		// echo headers if HTTPC library didn't already do so
                     	printf (">%s\n", url);
                     #endif
                  }
                  else if (err < 0)
                  {
                     printf ("error %d calling httpc_read_header()\n", err);
                  }
               }
            #else
               httpc_skip_headers(&hsock, 0);
            #endif
            printf ("Headers were parsed as follows:\n");
            printf ("  HTTP/%s response = %d, filesize = %lu\n",
               (hsock.flags & HTTPC_FLAG_HTTP10) ? "1.0" :
               (hsock.flags & HTTPC_FLAG_HTTP11) ? "1.1" : "???",
               hsock.response, hsock.filesize);
            if (hsock.flags & HTTPC_FLAG_CHUNKED)
            {
               printf ("  body will be sent chunked\n");
            }
            printf ("  Rabbit's RTC is %ld second(s) off of server's time\n",
               hsock.skew);

            #ifdef UPDATE_RTC
            	if (labs (hsock.skew) > 10)
            	{
            		// only update if off by more than 10 seconds
	               printf ("  Updating Rabbit's RTC to match web server.\n");
	               write_rtc (SEC_TIMER + hsock.skew);
	               // CAUTION: Updating SEC_TIMER outside of the periodic ISR
	               //          may cause problems for applications which depend
	               //          on SEC_TIMER's usual unit-increase monotonicity.
	               SEC_TIMER = read_rtc();
	            }
            #endif

            printf ("\nBody:\n");
            while (hsock.state == HTTPC_STATE_BODY)
            {
               // limit reads to 127 bytes to match printf limitation
               err = httpc_read_body (&hsock, url, 127);
               if (err > 0)
               {
                  url[err] = '\0';
                  printf ("%s", url);
               }
               else if (err < 0)
               {
                  printf ("error %d calling httpc_read_body()\n", err);
               }
            }
            httpc_close (&hsock);
            tcp_tick(NULL);
         }
      }
   }

}

// It's safer to keep sockets as globals, especially when using uC/OS-II.  If
// your socket is on the stack, and another task (with its own stack, instead
// of your task's stack) calls tcp_tick, tcp_tick won't find your socket
// structure in the other task's stack.
// Even though this sample doesn't use uC/OS-II, using globals for sockets is
// a good habit to be in.
tcp_Socket demosock;

void main()
{
	printf ("httpc v" HTTPC_VERSTR "\n");

	printf ("Initializing TCP/IP stack...\n");
	sock_init_or_exit(1);

   httpc_demo(&demosock);
}