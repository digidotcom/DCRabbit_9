/*******************************************************************************
        1stflash.c
        Z-World, 2001

        A basic example of using the flash filesystem in the FIRST flash,
        instead of the 2nd flash.

*******************************************************************************/

/*
 * !!! IMPORTANT NOTE !!!
 *
 * This example (and all first flash filesystems) requires a change
 * to the BIOS!
 *
 * The macro XMEM_RESERVE_SIZE must be modified to allow room in
 * the first flash for the File System.
 *
 * See note at the top of \lib\filesystem\fs_flash_single.lib
 * for more information.
 */

#define FS_FLASH_SINGLE
//#define FS_DEBUG
//#define FS_DEBUG_FLASH
#use "filesystem.lib"

#define FORMAT

#define BLOCKS  8
#define TESTFILE 1

main()
{
	File file;
	static char buffer[256];
	long fs_start; // where to start the filesystem

	fs_start = END_OF_XMEMORY; // start at the end of xmem 
	fs_start = fs_start / FS_BLOCK_SIZE; // divide out the blocksize, to meet the requirements for fs_format
	if((fs_start * FS_BLOCK_SIZE) != END_OF_XMEMORY) {
		// rounding error - we need to move up one block so we don't clobber the
		// end of the xmem area
		fs_start++;
	}
		
	printf("---> Starting at: %lx\n",fs_start);
	
#ifdef FORMAT
	if(fs_format(fs_start,BLOCKS,1)) {
		printf("Error formating FS!\n");
		return -1;
	}
	printf("---> Creating file...\n");
	if(fcreate(&file,TESTFILE)) {
		printf("error creating TESTFILE\n");
		return -1;
	}
#else
	if(fs_init(fs_start,BLOCKS)) {
		printf("Error initing FS!\n");
		return -1;
	}
	printf("---> Creating file...\n");
	if(fopen_wr(&file,TESTFILE)) {
		printf("error opening TESTFILE\n");
		return -1;
	}
#endif

	printf("---> Writing test data (1)\n");
	fwrite(&file,"hello",6);
	printf("---> Writing test data (2)\n");
	fwrite(&file,"12345",6);
	printf("---> Writing test data (3)\n");
	fwrite(&file,"67890",6);

	printf("---> Closing the file...\n");
	fclose(&file);

	printf("---> Re-opening in read mode\n");
	fopen_rd(&file,TESTFILE);
	
	printf("---> Reading it back...\n");
	while(fread(&file,buffer,6)>0) {
		printf("%s\n",buffer);
	}

	printf("---> All done!\n");
	fclose(&file);
}
