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
/*************************************************************************
 CREATE.C    Filesystem Mk II sample program.

 Demonstrates use of fcreate_unused().

 Change the initial #define's as desired.
*************************************************************************/
#class auto

#memmap xmem

// Set aside this many kilobytes in the program flash.
// You will also need to edit .\lib\bioslib\memconfig.lib to set an appropriate
// value to XMEM_RESERVE_SIZE.  The definition here may be larger or
// smaller - the minimum of both specifications will be used.
#define FS2_USE_PROGRAM_FLASH	16

// You can use fs_get_flash_lx (for the 2nd flash); fs_get_ram_lx (for
// RAM, if configured in .\lib\bioslib\memconfig.lib); or fs_get_other_lx
// (for program flash if configured in .\lib\bioslib\memconfig.lib).
#define LX_2_USE			fs_get_flash_lx()

#define MY_LS_SHIFT		9	// Log(base 2) of the desired LS size.  Run the
									// FS2INFO sample program to check out
									// suitable values.  The LS (logical sector)
									// size is the internal buffer unit used
									// by the filesystem.

#define DELETE_AMOUNT	10	// Demo amount to shift out of file.
#define NUM_2_ADD			1	// Number of files to create

#define FS_MAX_FILES		32		// Maximum files in filesystem
#define FS_API_TRACE				// Open/close/create/delete tracing to stdout.
//#define LOTSA_DATA				// Add larger amount of test data to files

#use "fs2.lib"


void print_file(File * f);

int main()
{
	int rc;						// Return code from filesystem functions
	int fcount;

	File f1;						// Demo file handle
	FileNumber f1n;			// Corresponding file number
	File f2;						// Demo file handle
	FileNumber f2n;			// Corresponding file number

	static char buf[1024];	// General-purpose buffer.
	char * p;
	int print_range;

	errno = 0;
	print_range = 0;

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
	printf("  P n1 n2 <enter>     Print existing files numbered n1..n2?\n");
	gets(buf);
	if (toupper(buf[0]) == 'F') {
		rc = lx_format(LX_2_USE, 0);
		if (rc) {
			printf("Format failed, error code %d\n", errno);
			exit(3);
		}
	}
	else if (toupper(buf[0]) == 'P') {
		print_range = 1;
		p = buf + 1;
		while (isspace(*p)) p++;
		if (isdigit(*p)) {
			f1n = atoi(p) & 0x00FF;
			while (isdigit(*p)) p++;
			while (isspace(*p)) p++;
			if (isdigit(*p))
				f2n = atoi(p) & 0x00FF;
			else
				f2n = f1n;
		}
		else
			print_range = 0;
		if (print_range && f1n > f2n) {
			rc = f1n;
			f1n = f2n;
			f2n = rc;
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

	if (print_range) {
		for ( ; f1n <= f2n; f1n++) {
			rc = fopen_rd(&f1, f1n);
			if (!rc) {
				printf("File %d found:\n", (int)f1n);
				print_file(&f1);
			}
		}
	}

	for (fcount = 0; fcount < NUM_2_ADD; fcount++) {

	f1n = fcreate_unused(&f1) & 0x00FF;
	if (!f1n) {
		printf("Error creating 1st file (errno=%d)\n", errno);
		exit(3);
	}
	printf(">>> Next file number = %d\n", (int)f1n);

	/*
	 * Seek to the end of the file then print the current length.
	 */
	fseek(&f1, 0, SEEK_END);
	printf("File length is %ld\n", ftell(&f1));
	printf("Initial contents...\n");
	print_file(&f1);

	/*
	 * Add some data to the end.
	 */
	sprintf(buf, "Test pattern ________[%d]________", (int)f1n);
#ifdef LOTSA_DATA
	strcat(buf, "===========================================================================F===");
	strcat(buf, "================================================================E==============");
	strcat(buf, "==============A================================================================");
	strcat(buf, "===================================================D===========================");
	strcat(buf, "===========================B===================================================");
	strcat(buf, "=======================================C=======================================");
#endif
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

	fclose(&f1);

	}

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