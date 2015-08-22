/*************************************************************************
 FS2DEMO2.C Filesystem Mk II sample program.
 Z-World, 2001

 Extends FS2DEMO1.C to introduce the concept of "partitioning"
 a flash device to allow multiple data extents.  Some possible
 uses for this would be:
   . Reserving space on the device for "critical files".  For
     example, configuration file on one partition and data log
     file on another.  This would allow the config file to
     be updated or extended even if the log file was full.
   . Allow files with different characteristics to reside on
     partitions (called Logical eXtents or LXs) that best
     suit the file.  For example, a log file would be best
     on an LX with large logical sector sizes, whereas a
     configuration file (which needed frequent overwrites)
     would be better on an LX with small LSs.
   . Reserve a range of flash sectors which are not used
     by the filesystem.  For example, the application may
     wish to directly manipulate some of the flash memory.

 Reserving space for selected files can be important in many
 applications.  Note that overwriting of an existing file
 can cause an out-of-space condition, even if the overwrite
 does not change the overall file size.  This is because the
 filesystem tries to maintain integrity of the data by
 ensuring that the new data is written before the old (replaced)
 data is deleted.  Thus, there needs to be at least one free
 LS for any update operation.

 This demo benchmarks the read, append and overwrite speed for
 the current flash device.  You can use it to experiment with
 different LS sizes in order to optimize performance for your
 intended application.

 The benchmark models two types of file.  A "configuration" file
 which is a relatively small file of fixed length (CONFIG_SIZE)
 is placed on LX #1.  This file is tested for it overwrite and
 readback speed.  A "log" file is placed on LX #2 and tested for
 its append speed with fixed size records of LOG_RECORD_SIZE.
 Its readback speed with varying record sizes is also tested.

 Note: If you change the LS sizes etc., recompile, and re-run this
 demo program, you should reformat the flash device by entering
 F at the initial prompt.  Otherwise, the filesystem may think
 that many of the sectors are "bad" and will not use them.  This
 is also recommended if you have been running any of the other
 filesystem demo programs.

 Sit back, relax, and watch the numbers "flash" by...

 Change the initial #define's as desired.
*************************************************************************/

#class auto
#memmap xmem

#define FS_MAX_LX			4	// Number of logical extents ("partitions") - this
									// is more than the default since we want to
									// create an extra partition.
#define FS_MAX_FILES		12	// Maximum expected existing files on all LXs

// Amount of 1st flash to use (becomes 2nd or 3rd basic LX). - enter in kilobytes.
#define FS2_USE_PROGRAM_FLASH	512

// Logical sector (LS) sizes.  Specify as the logarithm of the desired
// size.  The value can range between 6 and 13 inclusive.  Smaller values
// can be better for files with higer overwrite activity, however this
// should not me too much smaller than the underlying physical sector
// size.  Larger LS sizes are better for append and readback performance.
// For sector-writable flash devices (as opposed to byte-writable),
// the LS size _must_ be greater than or equal to the PS size, but
// perferably no more than 8 or 16 times the PS size.
#define MY_LS_SHIFT1		8	// Log(base 2) of the LS size for LX #1.  Run the
									// FS2INFO sample program to check out
									// suitable values.
#define MY_LS_SHIFT2		12	// Larger LS for LX #2.

#define MY_LS_SIZE1		(1<<MY_LS_SHIFT1)	// Don't change this definition.
#define MY_LS_SIZE2		(1<<MY_LS_SHIFT2)	// Don't change this definition.

// We add in ext1 to get a unique name for each base LX tested.
#define CONFIG_FILE_NAME	(20+ext1)	// Filename for LX #1 (config)
#define LOG_FILE_NAME		(25+ext1)	// Filename for LX #2 (log)

#define CONFIG_SIZE		2000	// Total file size of "config".
#define LOG_RECORD_SIZE	240	// Record size for our "log" entries.

// Define how to partition the flash device.  The given values are rounded
// to the nearest whole number of physical sectors.
#define PERCENT_LX2		80		// This percentage of device reserved for LX#2
	// Any remaining space will be left for LX#1

#define APPEND_TESTS		2		// Number of times to do "append" test.  This
										// first empties out the log file, then
										// appends records until the LX becomes full.
										// The total time taken to do this is recorded.

#define READ_TESTS		1		// Number of read tests.  Once the log file is
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

#ifdef FS2_NO_PREMAIN
	_fs_premain();			// For debugging: explicit call to premain function
#endif

	errno = 0;
	log_open = 0;
	
	/*
	 * Step 1: define partitioning.  Since we are using non-default partitioning,
	 * it is necessary to call fs_setup() before calling fs_init().
	 * We assume that ext1 is the 2nd flash device, which will be the default
	 * if you have one.  Otherwise, ext1 will be set to the program flash.
	 */
	ext1 = fs_get_flash_lx();
	if (!ext1) {
		printf("No flash device found!\n");
		exit(1);
	}
	ext2 = fs_setup(ext1, MY_LS_SHIFT1, 0, NULL, FS_PARTITION_FRACTION,
	              PERCENT_LX2 * (65536/100),
	              MY_LS_SHIFT2, 0, NULL);
	if (!ext2) {
		printf("Could not partition filesystem, error number %d\n", errno);
		exit(2);
	}
	ext_log = ext2;
	
	/*
	 * Step 2: initialize the filesystem.  A real application must
	 * decide whether to format a new flash device, or just start
	 * up an existing filesystem.  Formatting should not normally
	 * be needed unless, say, a user commands it.  A factory-fresh
	 * flash device will usually be all erased so there is no need
	 * to format.  Likewise, a flash with completely random data
	 * will be recognised as such and automatically formatted.
	 */
	printf("Initializing file system.  Please wait...\n");
	rc = fs_init(0,0);
	if (rc) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}

	// Here we make some use of the filesystem internals in order to print
	// some useful info.
	printf("Extent table:\n");
	printf("LXD# ps_size  num_ps  num_ls  ls_size  ls_use  dev_offs  ps/ls  ls/ps\n");
	printf("---- -------  ------  ------  -------  ------  --------  -----  -----\n");
	for (i = 1; i <= _fs.num_lx; i++)
		printf("%4d %7ld  %6u  %6u  %7u  %6u  %8ld  %5u  %5u\n",
			i, _fs.lx[i].ps_size, _fs.lx[i].num_ps, _fs.lx[i].num_ls,
			_fs.lx[i].ls_size, _fs.lx[i].d_size, _fs.lx[i].dev_offs,
			_fs.lx[i].ps_per_ls, _fs.lx[i].ls_per_ps);

	// Sanity check for demo to work.  We use the lxd->d_size field to
	// determine the usable space per LS.  This is only available after
	// calling fs_init().
	if (_fs.lx[ext1].num_ls < (CONFIG_SIZE/_fs.lx[ext1].d_size + 2)) {
		printf("LX #%d is probably undersized for this demo.  Either reduce\n", (int)ext1);
		printf("CONFIG_SIZE or decrease PERCENT_LX2.\n");
		exit(20);
	}
	if (_fs.lx[ext_log].num_ls < (LOG_RECORD_SIZE/_fs.lx[ext_log].d_size + 2)) {
		printf("LX #%d is probably undersized for this demo.  Either reduce\n", (int)ext_log);
		printf("LOG_RECORD_SIZE or increase PERCENT_LX2.\n");
		exit(21);
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
		printf("Formatting device.  May take several seconds...\n");
		rc = lx_format(ext1, 0);
		if (rc) {
			printf("Format for LX# %d failed, error code %d\n", (int)ext1, errno);
			exit(3);
		}
		rc = lx_format(ext2, 0);
		if (rc) {
			printf("Format for LX# %d failed, error code %d\n", (int)ext2, errno);
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
	 * Files are stored in two parts, called "metadata" and "data".
	 * Metadata is a small amount of journaling information which
	 * is associated with each file.  It consumes one LS for each
	 * file.  You can think of it as a "directory entry", but it
	 * is more than that.  The data itself can take up any number
	 * of LSs.  The data and metadata can be stored on the same or
	 * on different LXs.  This demo uses the same LX for both.
	 */
	fs_set_lx(ext1, ext1);	// Specify LX #1 for metadata and data.
	rc = fcreate(&config, CONFIG_FILE_NAME);
	if (rc && errno == EEXIST) {
		printf("Config file exists.\n");
		rc = fopen_wr(&config, CONFIG_FILE_NAME);
	}
	if (rc) {
		printf("Couldn't create/open file %d: errno = %d\n", (int)CONFIG_FILE_NAME, errno);
		exit(33);
	}

	fs_set_lx(ext_log, ext_log);	// Specify LX #2 for metadata and data (log file creation).

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

		printf("Appended %ld bytes in %ld ms; %ld bytes/sec\n",
			length, time, length*1000/time);
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

		printf("Read (by record of %u) %ld bytes in %ld ms; %ld bytes/sec\n",
			sizeof(log_buf), length, time, length*1000/time);
			
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

		printf("Read (by byte) %ld bytes in %ld ms; %ld bytes/sec\n",
			length, time, length*1000/time);
			
		fseek(&log, 0, SEEK_SET);
		
		start_ms = MS_TIMER;
		length = 0;
		
		do {
			rc = fread(&log, big_buf, _fs.lx[ext_log].d_size);
			length += rc;
		} while (rc == _fs.lx[ext_log].d_size);

		end_ms = MS_TIMER;
		time = end_ms - start_ms;

		printf("Read (by LS of %u) %ld bytes in %ld ms; %ld bytes/sec\n",
			_fs.lx[ext_log].d_size, length, time, length*1000/time);
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

		printf("Overwrote %ld bytes (1 at a time) in %ld ms; %ld bytes/sec\n",
			length, time, length*1000/time);
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

		printf("Overwrote %ld bytes (64 at a time) in %ld ms; %ld bytes/sec\n",
			length, time, length*1000/time);
		fseek(&config, 0, SEEK_SET);
		fread(&config, config_buf, sizeof(config_buf));
		if (memcmp(big_buf, config_buf, sizeof(config_buf)))
			printf("!!! config data corrupted!\n");

	}

#endif
	
	
	return 0;
}

