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
/*
   set_ip.c - Sample program used to set the desired IP address for a target
              loaded with one of the supplied "modbus_rabweb (xxxxx).bin" files.
*/

#define TCPCONFIG 6

#define MYIP	"10.10.6.101"	// set modbus_rabweb.bin target's desired IP addr
//#define MYIP	"255.255.255.255"	// NB: aton("255.255.255.255") == -1L

#memmap xmem
#use "dcrtcp.lib"

void main()
{
   int result;
   long my_ip;

   sock_init;	// required only to enable the aton() call below
   tcp_tick;	// required only to enable the aton() call below

   my_ip = aton(MYIP);
   result = writeUserBlock(0, &my_ip, 4);
   if(!result)
      printf("my_ip is set to:  %s (0x%LX).\n", MYIP, my_ip);
   else
      printf("Error %d when writing to the UserBlock.\n", result);
}

