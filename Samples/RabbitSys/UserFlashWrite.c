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

   UserFlashWrite.c

	Sample program for writing to and reading from
	RabbitSys user flash area.
******************************************************/
char data[8192];

main() {

	int i, j;
   int base;

   base = 0;
   for (i=0; i<32; ++i)
   {
  		memset(data+i*256, i+base, 256);
   }
   j = _sys_userFlashWrite(data,0,RS_USERFLASH_SIZE);
   memset(data,0,8192);
   i = _sys_userFlashRead(data,0,RS_USERFLASH_SIZE);
   for (i=0; i<RS_USERFLASH_SIZE; ++i)
   {
		if ( data[i] != (i/256)+base )
		{
			printf("Flash Readback failed at byte %d..\n", i);
			printf("Expected 0x%2.2x but read 0x2.2%x\n",(i/256)+base,data[i]);
			exit(1);
		}
   }
	printf("Flash write and readback successful\n");
}