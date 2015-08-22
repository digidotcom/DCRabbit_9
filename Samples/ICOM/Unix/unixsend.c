/***************************************************************************
	unixsend.c
   Z-World, 2001
   
 	This is the source code for "unixsend" used on the PC console
 	side to communicate with an Intellicom Series board.
 	
	The executable "unixsend" is similar to tcp_send.c but is run at the
	command prompt to communicate	with an Intellicom Series board running
	tcp_respond.c
	
   To compile this file in Borland C++ 5.0, load this file and
   select Project/Build All from the menu.  The executable will
   be generated in the same directory as this file.

	Using "unixsend":
	-----------------

	unixsend runs from the shell.  The command line is as follows:

	% unixsend <ipaddr> <port>

	where <ipaddr> is the IP address to send the message to,
	and <port> is the port number.  The arguments are optional--
	if they are omitted, the IP address defaults to 10.10.6.112,
	and the port defaults to 4040.  A message will display on
	the remote machine, and then the response will be displayed
	on the console.  "unixsend" exits when it is finished sending a
	message.

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

/**
 *   Where to find the Rabbit.
 */
#define IPADDR "10.10.6.177"
#define PORT 4040


/*  Things to smell like MS-Windows: */
typedef int   SOCKET;
#define  closesocket(s)   close(s)


int main(int argc, char* argv[])
{
	int err;
   SOCKET       sock;
   struct sockaddr_in  sin;
   char data[1024];
   const char message[] = "How are you today?";
   char ip_addr[20];
   unsigned short int port;

   if (argc > 1) {
   	strncpy(ip_addr, argv[1], 20);
   } else {
   	strncpy(ip_addr, IPADDR, 20);
   }

   if (argc > 2) {
   	port = (unsigned short int)atoi(argv[2]);
   } else {
   	port = PORT;
   }

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock < 0 ) {
   	printf("socket() failed!\n");
   	return 2;
   }

   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = inet_addr(ip_addr);
   sin.sin_port = htons(port);
   err = connect(sock, (struct sockaddr *) &sin, sizeof(sin));
   if (err < 0 ) {
   	perror("connect() failed!");
   	return 2;
   }

   err = send(sock, message, strlen(message), 0);
   if (err < 0 ) {
   	perror("send() failed!");
      return 2;
   }

   err = recv(sock, data, sizeof(data), 0);
   if (err < 0 ) {
   	perror("recv() failed!");
      return 1;
   }
   data[err] = '\0';
   printf("message -> \"%s\"\n", data);

   err = closesocket(sock);
   if (err < 0 ) {
   	perror("closesocket() failed!");
      return 1;
   }

   return 0;
}   /* end main() */

