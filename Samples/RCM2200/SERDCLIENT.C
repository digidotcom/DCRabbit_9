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

	serdclient.c

	serdclient.c is intended to be run in conjunction
	with console.c

	Example of a target board creating a web page while
	connected to an RCM2200 using serial port D.

	This program demonstrates the ability of a Rabbit-
	based target board to update files on the web
	server of the RCM2200 board it is connected to
	via serial port D.

 ********************************************************/
#class auto

#define	DINBUFSIZE	255
#define	DOUTBUFSIZE	255

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
	auto int		done, len;
	auto unsigned long	t0;

	t0 = MS_TIMER + timeout;

	done = 0;
	len = 0;
	while (!done && ((long) (MS_TIMER - t0) < 0)) {
		len += serDread(buf, buflen, 500);
		if (len > 0) {
			buf[len] = '\0';
			done = 1;
		}
	}
	
	return (len);
}

/********************************************************/

#define	CTRL_D	0x04

void main()
{
	char	s[250];
	int	len;
	long	secs;
	struct tm t;	
		
	// This baud rate is slow because of issues writing
	// to the first flash while reading from the serial
	// port--see console.c for more details
	serDopen(300);

	// make sure we are not in the middle of a command
	serDputs("\n");

	// turn the RCM2200's local echo off
	serDputs("echo off\n");
	
	get_ser_response(s, 200, 1500);
	printf("RCM2200 returned:  %s", s);

	// delete old web page (if it exists)
	serDputs("delete demo.html\n");

	get_ser_response(s, 200, 1500);
	printf("RCM2200 returned:  %s", s);
		
	// create the web page
	serDputs("put demo.html\r");
	serDputs("<H2>A demonstration web page</H2>\n");
	
	// get the current time	
	tm_rd(&t);
	sprintf(s, "Created on %02d/%02d/%04d at %02d:%02d:%02d\n",
				t.tm_mon, t.tm_mday, t.tm_year+1900,
				t.tm_hour, t.tm_min, t.tm_sec);
	serDputs(s);

	// end web page
	sprintf(s, "%c", CTRL_D);
	serDputs(s);

	// this response takes a bit longer
	get_ser_response(s, 200, 15000);
	printf("RCM2200 returned:  %s", s);	
}
