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
/*******************************************************************************
        skeleton.c

        This program can be used for boards that have a second flash.

        This is an example from the Dynamic C Users Manual and is used in
        the manual as a basic example for using the flash file system.

*******************************************************************************/

#define FS_FLASH
#use "filesystem.lib"

#define FORMAT

#define RESERVE 0L
#define BLOCKS  64
#define TESTFILE 1

main()
{
	File file;
	static char buffer[256];

#ifdef FORMAT
	fs_format(RESERVE,BLOCKS,1);
	if(fcreate(&file,TESTFILE)) {
		printf("error creating TESTFILE\n");
		return -1;
	}
#else
	fs_init(RESERVE,BLOCKS);
	if(fopen_wr(&file,TESTFILE)) {
		printf("error opening TESTFILE\n");
		return -1;
	}
#endif

	fwrite(&file,"hello",6);
	fwrite(&file,"12345",6);
	fwrite(&file,"67890",6);

	while(fread(&file,buffer,6)>0) {
		printf("%s\n",buffer);
	}

	fclose(&file);
}
