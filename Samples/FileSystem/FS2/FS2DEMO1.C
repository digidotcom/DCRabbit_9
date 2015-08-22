/*************************************************************************
 FS2DEMO1.C   Filesystem Mk II sample program.
 Z-World, 2001

 Demonstrates how to initialize the filesystem,
 create, open and delete files, plus the
 available operations of appending, overwriting,
 reading and "shifting".  Shifting means deleting
 data from the start of the file.

 Change the initial #define's as desired.
*************************************************************************/

#class auto
#memmap xmem

// Set aside this many kilobytes in the program flash.
// You will also need to edit .\lib\bioslib\memconfig.lib to set an appropriate
// value to XMEM_RESERVE_SIZE.  The definition here may be larger or
// smaller - the minimum of both specifications will be used.
#define FS2_USE_PROGRAM_FLASH	16

// You can use:
// fs_get_flash_lx (for the 2nd flash on a two-flash board, or for
//    the program flash on a one-flash board, if XMEM_RESERVE_SIZE
//    is configured in .\lib\bioslib\memconfig.lib);
// fs_get_ram_lx (for RAM, if FS2_RAM_RESERVE is configured in
//    .\lib\bioslib\memconfig.lib); or
// fs_get_other_lx (for the program flash on a two-flash board, if
//    XMEM_RESERVE_SIZE is configured in .\lib\bioslib\memconfig.lib).
#define LX_2_USE			fs_get_flash_lx()

#define MY_LS_SHIFT		9	// Log(base 2) of the desired LS size.  Run the
									// FS2INFO sample program to check out
									// suitable values.  The LS (logical sector)
									// size is the internal buffer unit used
									// by the filesystem.

#define MY_FILE_NAME		(1+LX_2_USE)
									// A file "name" to use.  The filesystem uses
									// numbers from 1 to 255 inclusive.  A particular
									// file name must be unique to the entire
									// filesystem.  Once created, it remains bound
									// to the logical extent(s) on which it was created.

#define DELETE_AMOUNT	10	// Demo amount to shift out of file.

#use "fs2.lib"

void print_file(File * f);

int main()
{
	int rc;						// Return code from filesystem functions

	File f1;						// Demo file handle

	static char buf[1024];	// General-purpose buffer.

	if (!LX_2_USE) {
		printf("The specified device (LX# %d) does not exist.  Change LX_2_USE.\n",
			(int)LX_2_USE);
		exit(1);
	}
	else
		printf("Using device LX# %d...\n", (int)LX_2_USE);

	/*
	 * Step 1: initialize the filesystem.  A real application must
	 * decide whether to format a new flash device, or just start
	 * up an existing filesystem.  Formatting should not normally
	 * be needed unless, say, a user commands it.  A factory-fresh
	 * flash device will usually be all erased so there is no need
	 * to format.  Likewise, a flash with completely random data
	 * will be recognised as such and automatically formatted.
	 */
	rc = fs_init(0,0);
	if (rc) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}

	/*
	 * Step 2: format the filesystem if requested.  Note that formatting
	 * must be done _after_ the call to fs_init().
	 *
	 * This demo initially asks whether the flash should be formatted
	 * via the stdio connection.
	 */

	printf("Do you want to:\n");
	printf("  <enter>       re-use existing filesystem -or-\n");
	printf("  F <enter>     format the filesystem logical extent?\n");
	gets(buf);
	if (toupper(buf[0]) == 'F') {
		rc = lx_format(LX_2_USE, 0);
		if (rc) {
			printf("Format failed, error code %d\n", errno);
			exit(3);
		}
	}


	/*
	 * Step 3: open a file for writing if it already exists, or
	 * create it if it does not.  Note that creation automatically
	 * opens the file for writing if it does not exist.  If it
	 * does exist, then EEXIST will be returned in errno.
	 */
	printf("Capacity of LX# %d is approximately %ld\n",
		(int)LX_2_USE, fs_get_lx_size(LX_2_USE, 0, 0));
	fs_set_lx(LX_2_USE, LX_2_USE);
	rc = fcreate(&f1, MY_FILE_NAME);
	if (rc && errno == EEXIST) {
		printf("File %d exists: shall I delete it? (y/n)\n", (int)MY_FILE_NAME);
		gets(buf);
		if (toupper(buf[0]) == 'Y') {
			fdelete(MY_FILE_NAME);
			rc = fcreate(&f1, MY_FILE_NAME);
		}
		else
			rc = fopen_wr(&f1, MY_FILE_NAME);
	}
	if (rc) {
		printf("Couldn't create/open file %d: errno = %d\n", (int)MY_FILE_NAME, errno);
		exit(3);
	}

do_it_again:
	/*
	 * Seek to the end of the file then print the current length.
	 */
	fseek(&f1, 0, SEEK_END);
	printf("File length is %ld\n", ftell(&f1));

	/*
	 * Add some data to the end.
	 */
	strcpy(buf, "Test pattern ________");
	rc = fwrite(&f1, buf, strlen(buf));
	if (rc < strlen(buf)) {
		printf("Append operation failed, error code %d\n", errno);
		// but we keep going...
	}

	printf("After append operation...\n");
	print_file(&f1);

	/*
	 * Seek backwards and overwrite something.
	 */
	fseek(&f1, -7, SEEK_END);
	sprintf(buf, "%ld", ftell(&f1));
	rc = fwrite(&f1, buf, strlen(buf));
	if (rc < strlen(buf)) {
		printf("Overwrite operation failed, error code %d\n", errno);
		// but we keep going...
	}

	printf("After overwrite operation...\n");
	print_file(&f1);

	/*
	 * Delete some data from the start.
	 */
	rc = fshift(&f1, DELETE_AMOUNT, buf);
	if (rc == 0) {
		printf("Shift operation failed, error code %d\n", errno);
		// but we keep going...
	}

	printf("After shifting out %d chars...\n", rc);
	print_file(&f1);
	buf[rc] = 0;
	printf("...and the deleted data was '%s'\n", buf);

	/*
	 * Another round, anyone?
	 */
	printf("Are you having too much fun? (y/n)\n");
	gets(buf);
	if (toupper(buf[0]) == 'Y')
		goto do_it_again;


	return 0;
}


void print_file(File * f)
{
#define READ_CHUNK 48
	long curr_pos;
	int rc;
	char buf[READ_CHUNK+1];

	curr_pos = ftell(f);			// Remember position so there's no nett disturbance
	fseek(f, 0, SEEK_SET);
	printf("File contents:\n");
	do {
		rc = fread(f, buf, READ_CHUNK);
		buf[rc] = 0;
		if (rc)
			printf("  '%s'\n", buf);
	} while (rc > 0);

	fseek(f, curr_pos, SEEK_SET);
}