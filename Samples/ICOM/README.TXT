The programs tcp_send.c and tcp_respond.c are meant to be 
executed on two different Intellicom boards so that the two 
boards communicate with each other.  In the absence of a 
second board, however, the pcsend and pcreceive programs in 
the windows\ directory can be used.  Both the executables 
and the C source code are located in the windows\ 
directory.

The PC programs were developed under Borland C++ 5.0,
although they may work with other versions or even other
compilers.  To compile under Borland C++ 5.0, load the file
you wish to compile, and then select Project/Build All from
the menu.  The executable should then be generated in the
same directory as the C file.

Using pcsend:
-------------

pcsend runs from the console.  The command line is as 
follows:

>pcsend <ipaddr> <port>

where <ipaddr> is the IP address to send the message to, 
and <port> is the port number.  The arguments are optional--
if they are omitted, the IP address defaults to 10.10.6.112, 
and the port defaults to 4040.  A message will display on 
the remote machine, and then the response will be displayed 
on the console.  pcsend exits when it is finished sending a 
message.  

Using pcrespond:
----------------

pcrespond runs from the console.  The command line is as 
follows:

>pcrespond <port>

where <port> is the port to listen on for messages.  The 
argument is optional--if omitted, the port defaults to 4040.  
Any message received will be displayed on the console, and a 
response will be send to the remote machine.  pcrespond does 
not exit unless the user types a <ctrl>-c to stop the 
program.