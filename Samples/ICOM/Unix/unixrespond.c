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
/********************************************************************
	pcrespond.c

 	This is the source code for "unixrespond" used on the PC console
 	side to communicate with an Intellicom Series board.
 	
	The executable unixrespond" is similar to tcp_respond.c but is run at the
	command prompt to communicate	with an Intellicom Series board running
	tcp_send.c
	
	The excutable "unixrespond" is run at the shell prompt
	to communicate with an Intellicom Series board running tcp_send.c
	
	Using pcrespond:
	----------------

	pcrespond runs from the console.  The command line is as
	follows:

	% unixrespond <port>

	where <port> is the port to listen on for messages.  The
	argument is optional--if omitted, the port defaults to 4040.
	Any message received will be displayed on the console, and a
	response will be send to the remote machine.  "unixrespond" does
	not exit unless the user types a <ctrl>-c to stop the
	program.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


/*  Things to smell like MS-Windows: */
typedef int   SOCKET;
typedef struct sockaddr_in  SOCKADDR_IN;
#define  closesocket(s)   close(s)


#define PORT 4040

int main(int argc, char* argv[])
{
	int err;
   SOCKET ssock, csock;
   SOCKADDR_IN sin;
   char data[1024];
   char response[] = "I hear ya";
   unsigned short int port;
   int     ivalue;

   if (argc > 1) {
   	port = (unsigned short int)atoi(argv[1]);
   } else {
   	port = PORT;
   }

   ssock = socket(PF_INET, SOCK_STREAM, 0);
   if (ssock < 0 ) {
   	printf("socket() failed!\n");
   	return 1;
   }

   /*  We want to keep using this address (call before bind()). */
   ivalue = 1;
   setsockopt( ssock, SOL_SOCKET, SO_REUSEADDR, & ivalue, sizeof(ivalue) );

   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = htonl(INADDR_ANY);   // use my IP address
   sin.sin_port = htons(port);
   err = bind(ssock, &sin, sizeof(sin));
   if (err < 0 ) {
   	printf("bind() failed!\n");
   	return 1;
   }

   /*  Accept one message and send response.  The close and re-open socket. */
   while (1) {
      err = listen(ssock, 1);
      if (err < 0 ) {
      	printf("listen() failed!\n");
      	return 1;
      }

      csock = accept(ssock, NULL, NULL);
      if (csock < 0 ) {
      	printf("accept() failed!\n");
      	return 1;
      }

      err = recv(csock, data, sizeof(data), 0);
      if (err < 0 ) {
      	printf("recv() failed!\n");
         return 1;
      }

      data[err] = '\0';
      printf("message -> \"%s\"\n", data);
      sleep(5);			// Sleep 5 seconds

      err = send(csock, response, strlen(response), 0);
      if (err < 0 ) {
      	 printf("send() failed!\n");
         return 1;
      }
      printf( "(responded)\n" );
      sleep(1);     /* Allow fragment to be xmitted (defeat VJ) */

      err = closesocket(csock);
      if (err < 0 ) {
      	 printf("closesocket() failed!\n");
         return 1;
      }
   }
}   /* end main() */

