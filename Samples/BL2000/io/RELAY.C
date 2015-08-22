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
/**************************************************************************

	Samples\BL2000\io\relay.c

	This sample program is used with BL20XX series controllers.
	
	This program demonstrates how to control the relay. You can use a
	ohmmeter to verify the connections.
	
	1. You should have low impedences between the Common contact and the
	   Normally Closed contact when in the COM_NC state. To verify, set
	   a breakpoint(using F2) on the following statement in the mainline
	   function and use the ohmmeter to verify the connection.

	   	printf("Relay_COM is connected to Relay_NC contact\n");
		   
	2. You should have low impedences between the Common contact and the 
	   Normally Open contact when in the COM_NO state. To verify, set a
	   breakpoint(using F2) on the following statement in the mainline
	   function and use the ohmmeter to verify the connection.

	   	printf("Relay_COM is connected to Relay_NO contact\n");

	Note: When turning the relay ON and OFF, LED0 will also be toggling
	      because its tied to the same control signal.

**************************************************************************/
#class auto


#define COM_NC		0
#define COM_NO		1


nodebug
void sdelay(unsigned int delay)
{
	auto unsigned long done_time;
	
	done_time = SEC_TIMER + delay;
   while( (long) (SEC_TIMER - done_time) < 0 );
} 


///////////////////////////////////////////////////////////////////////////

void main()
{
	brdInit();

	for (;;) // Loop forever
	{
		relayOut(0,	COM_NC);		//connect contact COM to NC
		printf("Relay_COM is connected to Relay_NC contact\n");
		sdelay(2);					//delay for 2 seconds

		relayOut(0, COM_NO);		//turn relay off
		printf("Relay_COM is connected to Relay_NO contact\n");
		sdelay(2);							//delay for 2 seconds
	}
		
}
