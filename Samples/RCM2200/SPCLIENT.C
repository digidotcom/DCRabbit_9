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
/********************************************************

	spclient.c

	This program must run on the master!

	spclient.c is intended to be run in conjunction
	with console.c

	Example of a target board creating a web page while
	connected to an RCM2200 using the stream handlers of
	the slave port.

	This program demonstrates the ability of a Rabbit-
	based target board to update files on the web
	server of the RCM2200 board it is connected to
	via the slave port.

 ********************************************************/
#class auto


#use "master_serial.lib"

// This channel must correspond to the channel in the
// console.
#define SP_CHANNEL 0x42

/********************************************************/

/*
	Read a response from the RCM2200.  Blocks until
	some characters are read or the timeout limit passes.
	Make sure that the buffer passed to it is large
	enough to contain the entire response or characters
	may be lost.
	
		parameter 1 = pointer to char buffer
		parameter 2 = length of buffer
		parameter 3 = timeout in milliseconds
 */
 
int get_ser_response(char *buf, int buflen, int timeout)
{
	auto int  	done, len;
	auto unsigned long	t0;

	t0 = MS_TIMER + timeout;

	done = 0;
	while (!done && ((long)(MS_TIMER - t0) <  0)) {
		len = MSread(SP_CHANNEL, buf, buflen, 10);
		if (len > 0) {
			buf[len] = '\0';
			done = 1;
		}
	}
	
	return (len);
}

/********************************************************/

#define	CTRL_D	0x04

// There is no MSputs function, so we create one here
int MSputs(char* s)
{
	auto int len;
	auto int i;
	auto int retval;

	len = strlen(s);
	i = 0;

	while (i < len) {
		retval = MSwrite(SP_CHANNEL, s + i, strlen(s) - i);
		i += retval;
	}
	return i;
}

main()
{
	char	s[250];
	int	len;
	long	secs;
	struct tm t;
	int retval;

	MSinit(0);

	// make sure we are not in the middle of a command
	MSputs("\n");

	// turn the RCM2200's local echo off
	MSputs("echo off\n");
	
	get_ser_response(s, 200, 250);
	printf("RCM2200 returned:  %s", s);

	// delete old web page (if it exists)
	MSputs("delete demo.html\n");

	get_ser_response(s, 200, 250);
	printf("RCM2200 returned:  %s", s);
		
	// create the web page
	MSputs("put demo.html\n");
	MSputs("<H2>A demonstration web page</H2>\n");
	
	// get the current time	
	tm_rd(&t);
	sprintf(s, "Created on %02d/%02d/%04d at %02d:%02d:%02d\r",
				t.tm_mon, t.tm_mday, t.tm_year+1900,
				t.tm_hour, t.tm_min, t.tm_sec);
	MSputs(s);

	// end web page
	sprintf(s, "%c", CTRL_D);
	MSputs(s);

	// this response takes a bit longer
	get_ser_response(s, 200, 1500);
	printf("RCM2200 returned:  %s", s);	
}
