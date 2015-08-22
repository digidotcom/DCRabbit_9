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
/*****************************************************

	xstring.c

	Example of using xstring.

	This program demonstrates how to access the
	strings of an xstring declaration.

	See the Dynamic C manual for more information
	about how Dynamic C stores xstrings in XMEM.

******************************************************/
#class auto

xstring	mystrings {	"string1", "string2", "string3" };


/**
 *  Fetch a string from an "xstring" array.  Assumes the string will
 *  completely inside the caller's "data" buffer.  Fetches "max_data"
 *  bytes, which might be more than what the string really is.
 */
int get_xstring(unsigned long xstring_addr, int index, char *data, int max_data)
{
	unsigned long	string_addr, index_addr;

	// calculate address in string index
	xstring_addr += index * sizeof(long);

	// get address of desired string
 	xmem2root((void *)&string_addr, xstring_addr, sizeof(long));

	// load string into data
	xmem2root((void *)data, string_addr, max_data); 
}

 
void main()
{
	char data[20];
	int	i;
	
	for (i=0; i<3; i++) {
		get_xstring(mystrings, i, data, sizeof(data));
		printf("Element %d of mystrings = \"%s\"\n", i, data);
	}
}
