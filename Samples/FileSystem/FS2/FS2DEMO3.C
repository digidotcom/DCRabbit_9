/*************************************************************************
 FS2DEMO3.C Filesystem Mk II sample program.
 Z-World, 2001

 Extends FS2DEMO2.C to use battery-backed RAM as a secondary storage
 device.

 !!! NOTE !!! -----------------------------------------------------------
 Before you can run this program, you will need to define a suitable
 value for the FS2_RAM_RESERVE near the top of .\lib\bioslib\memconfig.lib.
 This is necessary in order that the BIOS can reserve RAM for the
 filesystem.  A value of about 4k should be sufficient i.e.
   #define FS2_RAM_RESERVE 1  // Number of 4096-byte blocks
 After running this program, remember to set the value back to zero
 so that memory is not wasted.
 ------------------------------------------------------------------------

 You can run this demo even if you do not actually have a backup battery
 installed, so long as you do not interrupt power to the target board.

 Battery-backed SRAM (called NVRAM below) is a useful resource since it is
 much faster than flash or EEPROM memory, and does not suffer from limited
 erase cycles.  However, since it is usually a scarce resource, it would not
 normally be used to store the majority of data.  The flash filesystem can
 make use of a relatively small amount of NVRAM to hold the file metadata.

 This has the following advantages:
  . Data modification (append, overwrite, shift) is faster: up to two
    times improvement.  This is especially advantageous with sector-
    writable flash.
  . Large sector byte-writable flash can be better utilized if the
    metadata is separated.
  . Lower overall wear (erase cycles), especially for sector-writable
    flash.

 The size of the required amount of NVRAM can be calculated quite
 easily:
   Let F = the maximum number of files whose metadata is to be stored
           in NVRAM.
   Let L = the logical sector size of the NVRAM.
 Then the amount of space (S) required is:
   S = (F+1) * L

 This assumes that no file _data_ is being stored in the NVRAM.
 The logical sector size for the NVRAM can be any power of 2 between
 64 and 8192, however the larger sizes would be wasteful.  It is
 recommended that a size of either 128 or 256 be used, with 128 being
 better for most applications.

 If space is available, then NVRAM is useful for holding highly
 dynamic files which either change so frequently that normal flash
 memory would exceed its guaranteed erase cycle limit within the
 expected product lifetime, or require higher throughput rates than
 can be provided with flash.  (Most byte-writable flash devices take
 about 10-20us per byte, with several ms required for a sector erase).

 This demo program benchmarks the performance achieved by files with
 metadata only in NVRAM, as well as a 'config' file stored entirely
 in NVRAM.

 Change the initial #define's as desired.
*************************************************************************/

#class auto
#memmap xmem

#define FS_MAX_FILES		12	// Maximum expected existing files on all LXs

// Amount of 1st flash to use (becomes 2nd or 3rd basic LX).
#define FS2_USE_PROGRAM_FLASH	16

// Logical sector (LS) sizes.  Specify as the logarithm of the desired
// size.  The value can range between 6 and 13 inclusive.  For NVRAM,
// the recommended log2 LS size is 7 (i.e. 128 bytes).
//
#define MY_LS_SHIFT1		13	// Larger LS for LX #1 (flash).
#define MY_LS_SHIFT2		7	// Log(base 2) of the LS size for LX #2 (NVRAM)

#define MY_LS_SIZE1		(1<<MY_LS_SHIFT1)	// Don't change this definition.
#define MY_LS_SIZE2		(1<<MY_LS_SHIFT2)	// Don't change this definition.

#define CONFIG_FILE_NAME	(30+ext1)	// Filename for "config"
#define LOG_FILE_NAME		(35+ext1)	// Filename for "log"

#define CONFIG_SIZE		2000	// Total file size of "config" file (in NVRAM).
#define LOG_RECORD_SIZE	240	// Record size for our "log" entries.


#define APPEND_TESTS		2		// Number of times to do "append" test.  This
										// first empties out the log file, then
										// appends records until the LX becomes full.
										// The total time taken to do this is recorded.

#define READ_TESTS		2		// Number of read tests.  Once the log file is
										// full, it is read back one record at a time,
										// one character at a time, and one LS at a
										// time.  (This requires APPEND_TESTS to be
										// at least 1).

#define OVERWRITE_TESTS	2		// Each overwrite test overwrites a single
										// character in the config file, 50
										// times.  This is the worst-case; this
										// test also overwrites 64 chars at a time.

#use "fs2.lib"

char big_buf[8192];			// Largest possible LS size (2^13)
char config_buf[CONFIG_SIZE];
char log_buf[LOG_RECORD_SIZE];
char log_test_data[LOG_RECORD_SIZE];
char buf[80];


int main()
{
	int rc;						// Return code from filesystem functions

	File config;				// File handle for our "config" file
	File log;					// ...and for the "log".

	FSLXnum ext1, ext2, ext_log;		// Extent numbers

	int test_num;				// Test iteration counter
	long start_ms, end_ms;	// Timers
	long time;					// Test time
	long length;				// Data length
	int i;
	long pos;					// File position (ftell)
	int log_open;

	log_open = 0;

	ext1 = fs_get_flash_lx();		// Default LX# for 2nd flash device
	ext2 = fs_get_ram_lx();			// ...and for NVRAM.
	ext_log = ext1;	// Log data extent

	/*
	 * Step 1:  Ensure enough resources available for demo to run.
	 */

	if (!ext2 || FS2_RAM_RESERVE*4096L < CONFIG_SIZE + ((FS_MAX_FILES+1)<<MY_LS_SHIFT2)) {
		printf("Not enough RAM reserved for this program.\n");
		printf("You will need about %ld bytes.\n",
		  (long)(CONFIG_SIZE + ((FS_MAX_FILES+1)<<MY_LS_SHIFT2)));
		printf("%ld bytes are currently reserved for the filesystem.\n", FS2_RAM_RESERVE*4096L);
		printf("Either reduce CONFIG_SIZE in this demo, or increase\n");
		printf("the value of FS2_RAM_RESERVE in \\BIOS\\RABBITBIOS.C.\n");
		printf("Approximately %ld bytes are available.\n", xavail(NULL) - 4096);
			// Estimate 4k usage by the filesystem itself
		exit(1);
	}


	/*
	 * Step 2: initialize the filesystem.  Call fs_setup() first, to
	 * change the default LS sizes, then call fs_init().
	 */
	printf("Initializing file system.  Please wait...\n");
	fs_setup(ext1, MY_LS_SHIFT1, 0, NULL, FS_MODIFY_EXTENT, 0, 0, 0, NULL);
	fs_setup(ext2, MY_LS_SHIFT2, 0, NULL, FS_MODIFY_EXTENT, 0, 0, 0, NULL);
	rc = fs_init(0,0);
	if (rc) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}

	printf("Extent table:\n");
	printf("LXD# ps_size  num_ps  num_ls  ls_size  ls_use  dev_offs  ps/ls  ls/ps\n");
	printf("---- -------  ------  ------  -------  ------  --------  -----  -----\n");
	for (i = 1; i <= _fs.num_lx; i++)
		printf("%4d %7ld  %6u  %6u  %7u  %6u  %8ld  %5u  %5u\n",
			i, _fs.lx[i].ps_size, _fs.lx[i].num_ps, _fs.lx[i].num_ls,
			_fs.lx[i].ls_size, _fs.lx[i].d_size, _fs.lx[i].dev_offs,
			_fs.lx[i].ps_per_ls, _fs.lx[i].ls_per_ps);
	printf("Using LX# %d for 'config', LX# %d for 'log'.\n", (int)ext2, (int)ext_log);

	if (_fs.lx[1].num_ls < 4 || _fs.lx[_fs.num_lx].num_ls < 4) {
		printf("There is probably insufficient logical sectors to run this program;\n");
		printf("num_ls is less than 4 in the above table.  Continuing anyway...\n");
	}

	/*
	 * Step 3: format the filesystem if requested.  Note that formatting
	 * must be done _after_ the call to fs_init().
	 *
	 * This demo initially asks whether the flash should be formatted
	 * via the stdio connection.  It then prints the available free LSs
	 * on each LX.
	 */

	printf("Do you want to:\n");
	printf("  <enter>       re-use existing filesystem -or-\n");
	printf("  F <enter>     format the filesystem -or-\n");
	printf("  X <enter>     format the filesystem and halt?\n");
	gets(buf);
	if (toupper(buf[0]) == 'F' || toupper(buf[0]) == 'X') {
		printf("Formatting devices.  May take several seconds...\n");
		rc = lx_format(ext1, 0);
		if (rc) {
			printf("Format failed for LX# %d, error code %d\n", (int)ext1, errno);
			exit(3);
		}
		rc = lx_format(ext2, 0);
		if (rc) {
			printf("Format failed for LX# %d, error code %d\n", (int)ext2, errno);
			exit(3);
		}
		if (toupper(buf[0]) == 'X')
			exit(0);
	}

	/*
	 * Step 4: open or create files for writing.  When more than
	 * one LX is available, it is wise to specify the LXs in
	 * which the file will be created.  If the file already exists,
	 * then the LXs are already determined and can't be changed
	 * for that file.
	 *
	 */
	fs_set_lx(ext2, ext2);	// Specify LX #2 (NVRAM) for metadata and data.
	rc = fcreate(&config, CONFIG_FILE_NAME);
	if (rc && errno == EEXIST) {
		printf("Config file exists.\n");
		rc = fopen_wr(&config, CONFIG_FILE_NAME);
	}
	if (rc) {
		printf("Couldn't create/open file %d: errno = %d\n", (int)CONFIG_FILE_NAME, errno);
		exit(33);
	}

	fs_set_lx(ext2, ext_log);	// Specify LX #1 (flash) for log file data, NVRAM for metadata.

	/*
	 * The benchmark code below is identical to that in FS2DEMO2.C
	 */

#if (APPEND_TESTS > 0)

	printf("Performing %d append tests...\n", APPEND_TESTS);
	for (test_num = 1; test_num <= APPEND_TESTS; test_num++) {

		// Delete and recreate log file
		if (log_open)
			fclose(&log);		// Close logfile in case open (else delete will fail EBUSY).
		rc = fdelete(LOG_FILE_NAME);
		if (rc && errno != ENOENT) {
			printf("Could not delete log file, error code %d\n", errno);
			break;
		}
		rc = fcreate(&log, LOG_FILE_NAME);
		if (rc) {
			printf("Could not create log file, error code %d\n", errno);
			break;
		}
		log_open = 1;
		start_ms = MS_TIMER;
		length = 0;
		i = 0;
		do {
			rc = fwrite(&log, log_test_data, sizeof(log_test_data));
			length += rc;
			i++;
			if (rc < sizeof(log_test_data) && errno != ENOSPC)
				printf("  run terminated on error code %d, iteration %d\n", errno, i);
		} while (rc == sizeof(log_test_data));

		end_ms = MS_TIMER;
		time = end_ms - start_ms;

		if (time)
			printf("Appended %ld bytes in %ld ms; %ld bytes/sec\n",
				length, time, length*1000/time);
		else
			printf("Appended %ld bytes in %ld ms\n",
				length, time);
	}

#if (READ_TESTS > 0)

	printf("Performing %d read tests...\n", READ_TESTS);
	for (test_num = 1; test_num <= READ_TESTS; test_num++) {
		fseek(&log, 0, SEEK_SET);

		start_ms = MS_TIMER;
		length = 0;

		do {
			rc = fread(&log, log_buf, sizeof(log_buf));
			length += rc;
		} while (rc == sizeof(log_buf));

		end_ms = MS_TIMER;
		time = end_ms - start_ms;

		if (time)
			printf("Read (by record of %u) %ld bytes in %ld ms; %ld bytes/sec\n",
				sizeof(log_buf), length, time, length*1000/time);
		else
			printf("Read (by record of %u) %ld bytes in %ld ms\n",
				sizeof(log_buf), length, time);

		fseek(&log, 0, SEEK_SET);

		start_ms = MS_TIMER;
		length = 0;

		// Read from the start and end of file to get an average
		do {
			rc = fread(&log, log_buf, 1);
			length += rc;
		} while (rc == 1 && length < 500);
		fseek(&log, -500, SEEK_END);
		do {
			rc = fread(&log, log_buf, 1);
			length += rc;
		} while (rc == 1 && length < 1000);

		end_ms = MS_TIMER;
		time = end_ms - start_ms;

		if (time)
			printf("Read (by byte) %ld bytes in %ld ms; %ld bytes/sec\n",
				length, time, length*1000/time);
		else
			printf("Read (by byte) %ld bytes in %ld ms\n",
				length, time);

		fseek(&log, 0, SEEK_SET);

		start_ms = MS_TIMER;
		length = 0;

		do {
			rc = fread(&log, big_buf, _fs.lx[ext_log].d_size);
			length += rc;
		} while (rc == _fs.lx[ext_log].d_size);

		end_ms = MS_TIMER;
		time = end_ms - start_ms;

		if (time)
			printf("Read (by LS of %u) %ld bytes in %ld ms; %ld bytes/sec\n",
				_fs.lx[ext_log].d_size, length, time, length*1000/time);
		else
			printf("Read (by LS of %u) %ld bytes in %ld ms\n",
				_fs.lx[ext_log].d_size, length, time);
	}

#endif // READ_TESTS
#endif // APPEND_TESTS

#if (OVERWRITE_TESTS > 0)

	printf("Performing %d overwrite tests...\n", OVERWRITE_TESTS);
	memset(config_buf, 0, sizeof(config_buf));
	memset(big_buf, 0, sizeof(config_buf));
	rc = fwrite(&config, config_buf, sizeof(config_buf));
	if (rc < sizeof(config_buf)) {
		printf("Failed to write config file: rc=%d, error code=%d\n",
			rc, errno);
		exit(50);
	}

	for (test_num = 1; test_num <= OVERWRITE_TESTS; test_num++) {
		start_ms = MS_TIMER;
		length = 0;

		for (i = 0; i < 50; i++) {
			fseek(&config, pos = (long)(sizeof(config_buf)-1) * i / 50, SEEK_SET);
			rc = fwrite(&config, &test_num, 1);
			if (rc < 1) {
				printf("  run terminated on error code %d\n", errno);
				break;
			}
			big_buf[(int)pos] = (char)test_num;
			length += rc;
		}

		end_ms = MS_TIMER;
		time = end_ms - start_ms;

		if (time)
			printf("Overwrote %ld bytes (1 at a time) in %ld ms; %ld bytes/sec\n",
				length, time, length*1000/time);
		else
			printf("Overwrote %ld bytes (1 at a time) in %ld ms\n", length, time);
		fseek(&config, 0, SEEK_SET);
		fread(&config, config_buf, sizeof(config_buf));
		if (memcmp(big_buf, config_buf, sizeof(config_buf)))
			printf("!!! config data corrupted!\n");

		start_ms = MS_TIMER;
		length = 0;

		for (i = 0; i < 50; i++) {
			fseek(&config, pos = (long)(sizeof(config_buf)-64) * i / 50, SEEK_SET);
			memset(buf, (char)test_num, 64);
			rc = fwrite(&config, buf, 64);
			if (rc < 64) {
				printf("  run terminated on error code %d\n", errno);
				break;
			}
			memcpy(big_buf + (int)pos, buf, 64);
			length += rc;
		}

		end_ms = MS_TIMER;
		time = end_ms - start_ms;
		if (time)
			printf("Overwrote %ld bytes (64 at a time) in %ld ms; %ld bytes/sec\n",
				length, time, length*1000/time);
		else
			printf("Overwrote %ld bytes (64 at a time) in %ld ms\n", length, time);
		fseek(&config, 0, SEEK_SET);
		fread(&config, config_buf, sizeof(config_buf));
		if (memcmp(big_buf, config_buf, sizeof(config_buf)))
			printf("!!! config data corrupted!\n");

	}

#endif


	return 0;
}

