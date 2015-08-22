/*******************************************************************************
        skeleton.c
        Z-World, 2000

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
