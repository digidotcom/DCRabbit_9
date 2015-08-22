/*******************************************************************************
        ftp_srv2.c
        Z-World, 2000

        Demonstration of a simple FTP server, using the ftp library.
        This extends the ftp_server.c example to demonstrate more advanced
        use of the FTP server via use of customized handler functions.

        This is a complex and complete example.  It contains the seeds of some
        very useful functionality, such as the ability to read and write the
        Rabbit log (uses LOG.LIB), read and write FS2 files (uses FS2.LIB)
        and get a dump of the entire physical address space.  This sample
        also shows how to set up multi-interface servers.  For example, you
        can run the server on both ethernet and serial PPP.

        Since this example uses FS2, you will need to ensure that you configure
        the filesystem options correctly.  For example, you might need to
        edit RABBITBIOS.C to define some space for a RAM filesystem.  See the
        FS2 documentation.

        Note that the FTP server library itself knows nothing about the complex
        resource hierarchy that we set up.  This is an artifact of the way the
        custom handler functions present information to the FTP client.

   How it works...
   
   We try to look like a simple filesystem.  In the root directory, the normal
   sspec_* resources are available (as per FTP_SERVER.C).  There are some added
   directory entries:
     /var
           log
           log_rev
           log_append
           README
     /dev
           kmem
           fs2_ext_1
           fs2_ext_2   etc.
           random
           fs2_image
           README

     /fs2
           file1
           file2       etc.
           README

   /var contains resources to access the log file (as configured above).  The
   log can be retrieved in forward (oldest first) or reverse (newest first) order.
   README is a short description of the directory contents.  log_append is
   a writable file.  Anything uploaded here is appended to the log.  The format of
   each line in the uploaded log file is an optional digit (0-7) immediately
   followed by the log line (max 115 chars).  Longer lines are truncated.  If no
   initial digit, uses the default message priority (6 = INFO).  The downloaded
   log contains a standard system-V Unix look-alike prefix, so you could grep
   for various things of interest.

   /dev contains a kmem "file" which is actually the entire physical address space
   of the Rabbit (all 1Mbyte of it).  /dev also contains raw images of all of the
   FS2 "logical extents".  "random" is an infinite file which reads out as
   pseudo-random garbage.  "fs2_image" is an image of the entire FS2 filesystem,
   i.e. the concatenation of all LX's in numerical ascending order.
   [Note: reading the raw filesystem images is not yet implemented - will read
   back as a zero-length file].

   /fs2 is a "mount point" which contains all of the FS2 files, by number.
   These files can be read, written and deleted.

   Since the FTP server only deals with integer "file descriptors", we have to
   map the above resource presentation into a distinct integer for each
   resource.  We do this by dividing the integer into bitfields.  The high
   3 bits (not counting the sign bit) specifies the directory number:
     0 for the root, 1 for /var, 2 for /dev and 3 for /fs2.  Others (4-7) not used.
   The remaining 12 bits specify the file in the directory.

*******************************************************************************/
#class auto

//#define FTP_VERBOSE
//#define FTP_DEBUG

#define TCPCONFIG		1		// 1 = static ethernet, 4 = multiple interface

#define FTP_INTERFACE IF_ANY		// Allow FTP via any interface

/*
 * FTP configuration
 */

#define FTP_NODEFAULTHANDLER		// We are using our own handlers
#define FTP_EXTENSIONS				// Support the DELE, SIZE and MDTM commands (required for this demo)

/*
 * We are not defining any static resources.  Defining the following macro
 * tells ZSERVER.LIB (used by the FTP server) not to look for these static
 * resources.
 */
#define SSPEC_NO_STATIC

/*
 * Logging and FS2 config.  See LOG.LIB and LOG_DEFS.LIB (in "filesystem".)
 */
#define LOG_USE_XMEM						// Either LOG_USE_FS2 or LOG_USE_XMEM
#define LOG_XMEM_SIZE		16384L	// Memory quota for XMEM log
#define LOG_XMEM_CIRCULAR	1			// XMEM log wraps around

#define MY_LOG		LOG_DEST_XMEM	// Tell this demo what log dest to use
#define MY_FAC		0
#define MY_FACPRI	LOG_MAKEPRI(MY_FAC, LOG_INFO)	// Log facility/priority to use

#define FS_MAX_FILES		12	// Maximum expected existing files on all LXs

// Amount of program flash to use (becomes 2nd or 3rd basic LX, depending on whether
// you have a 2nd flash).  May need to change some BIOS settings e.g. XMEM_RESERVE_SIZE.
#define FS2_USE_PROGRAM_FLASH	256

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "dcrtcp.lib"
#use "ftp_server.lib"
#use "fs2.lib"
#use "log.lib"

#ximport "samples/tcpip/http/pages/rabbit1.gif" rabbit1_gif

/********************************
 * Custom FTP handlers          *
 ********************************/


#define DIR_MASK	0x7000
#define FILE_MASK 0x0FFF
#define DIR2NUMBER(fd) (((fd) & DIR_MASK) >> 12)
#define FILENUMBER(cwd, n) ((cwd)<<12 | (n))
#define NUMBER2DIR(cwd) ((cwd) << 12 & DIR_MASK)
#define NUM_DIRS 4
#define README_FILENO  (FILE_MASK-8)		// consistent file number for the "README"
#define DIR_FILENO  (FILE_MASK - 7)
#define IS_A_DIR(fd) (((fd) & FILE_MASK) >= DIR_FILENO)
#define README_NAME "README"

char * const dirnames[NUM_DIRS] =
{
	""
  ,"var"
  ,"dev"
  ,"fs2"
};

char * const readmes[NUM_DIRS] =
{
	"This is the top level directory.  Please feel free to explore me.\r\n"
  ,"You can download or append to the log files herein.\r\n"
  ,"You can access raw memory and filesystems here.\r\nrandom sends back random data forever so you will have to terminate the download manually.\r\n"
  ,"This directory contains all of the FS2 files, by number.\r\nTo create new file, upload to file<n> where <n> is the file number in decimal (1-255).\r\n"
};

#define NUM_VARS	3
char * const varnames[NUM_VARS] =
{
	"log"
  ,"log_rev"
  ,"log_append"
};
#define FN_VAR	0
#define FN_LOG			(FN_VAR+0)
#define FN_LOG_REV	(FN_VAR+1)
#define FN_LOG_APPEND (FN_VAR+2)

#define NUM_DEVS	3
char * const devnames[NUM_DEVS] =
{
	"kmem"
  ,"random"
  ,"fs2_image"
};
// File numbers 0-255 are reserved for FS2 logical extent numbers
#define FN_DEV			256
#define FN_KMEM		(FN_DEV+0)
#define FN_RANDOM		(FN_DEV+1)
#define FN_FS2_IMAGE	(FN_DEV+2)


long starttime;
long logmodtime;
int writing;
int reading;

int filename2number(char * name, int cwd, int want_dir, unsigned options)
{
	// Return +ve for normal file, -1 for not found, -2 if root directory specified
	// If want_dir is true, then only returns +ve "cwd" number if directory specified.
	int fd;
	int d;
	int lxn;
	char * dir;		// Point to directory part of name (or NULL)
	char * fname;	// Point to actual name
	int spec;

	if (*name == '/') {
		cwd = 0;			// absolute path
		name++;
		if (!*name)
			return want_dir ? 0 : -2;	// Can't be called with just "/" (indicate this special case).
	}
repeat:
	fd = NUMBER2DIR(cwd);	// Init directory bits
	if (fname = strchr(name, '/')) {
		dir = name;
		*fname++ = 0;
	}
	else {
		dir = NULL;
		fname = name;
	}
	printf("looking for dir=%s, name=%s, cwd=%d\n",
	  dir ? dir : "<null>", fname, cwd);
	if (dir) {
		if (cwd) {
			if (!strcmp(dir, "..")) {
				cwd = 0;
				name = fname;
				goto repeat;
			}
			return -1;	// No directories under anything but root
		}
		for (d = 1; d < NUM_DIRS; d++)
			if (!strcmp(dir, dirnames[d])) {
				fd = NUMBER2DIR(d);
				cwd = d;
				break;
			}
		if (d == NUM_DIRS)
			return -1;	// No such directory (under root).
	}
	if (!strcmp(fname, ".."))
		return want_dir ? 0 : -2;	// Must be specifying root directory
	if (!strcmp(fname, README_NAME))
		fd |= README_FILENO;
	else switch (cwd) {
		case 0:
			// In the root dir we have the server spec files, plus the directory names.
			// dir names are given pseudo-file numbers starting at DIR_FILENO
			for (d = 1; d < NUM_DIRS; d++)
				if (!strcmp(fname, dirnames[d]))
					return want_dir ? d : (DIR_FILENO + d);
			// Otherwise, locate a server spec resource
			fd = sspec_findname(name, SERVER_FTP);
			break;
		case 1: /* var */
			for (d = 0; d < NUM_VARS; d++)
				if (!strcmp(fname, varnames[d])) {
					fd |= (FN_VAR + d);
					break;
				}
			if (d == NUM_VARS)
				fd = -1;
			break;
		case 2: /* dev */
			for (d = 0; d < NUM_DEVS; d++)
				if (!strcmp(fname, devnames[d])) {
					fd |= (FN_DEV + d);
					break;
				}
			if (d < NUM_DEVS)
				break;
			if (!memcmp(fname, "fs2_ext_", 8) && isdigit(fname[8]) && !fname[9]) {
				fd |= lxn = (fname[8] - '0');
				if (lxn < 0 || lxn > _fs.num_lx)
					fd = -1;
			}
			else
				fd = -1;
			break;
		case 3: /* fs2 */
			if (!memcmp(fname, "file", 4)) {
				d = 0;
				fname += 4;
				while (isdigit(*fname) && d < 256) {
					d = d * 10 + (*fname - '0');
					fname++;
				}
				if (*fname || !d || d > 255)
					return -1;
				if (!FS_EXISTS(d) &&
				    (options != O_WRONLY))
					return -1;
				fd |= d;
			}
			else
				fd = -1;
			break;
		default:
			fd = -1;
	}
	if (fd < 0)
		return fd;
	if (want_dir) {
		if (IS_A_DIR(fd))
			return (fd & FILE_MASK) - DIR_FILENO;
		else
			return -3;	// not a directory
	}
	return fd;
}


int my_open(char *name, int options, int uid, int cwd)
{
	auto int fd, n;
	auto File f;

	if (writing)
		return FTP_ERR_UNAVAIL;
	printf("my_open: name=%s...\n", name);
	fd = filename2number(name, cwd, 0, options);
	printf("...cwd=%d, fd=%d\n", cwd, fd);
	if (fd < 0)
		return FTP_ERR_NOTFOUND;
	if (IS_A_DIR(fd))
		return FTP_ERR_NOTFOUND;
	if (options == O_WRONLY) {
		if (reading)
			return FTP_ERR_UNAVAIL;
		// Write access requested.  OK for /var/log_append.
		if (fd == FILENUMBER(1, FN_LOG_APPEND)) {
			writing = 1;
			return fd;
		}
		if (fd > FILENUMBER(3, 0) && fd <= FILENUMBER(3, 255)) {
			// Writing to FS2 file
			n = fd & FILE_MASK;
			if (!FS_EXISTS(n)) {
				// Try to create.
				if (fcreate(&f, n))
					return -1;
				fclose(&f);
			}
			else {
				// Exists: truncate it to zero length
				if (fdelete(n) || fcreate(&f, n))
					return -1;
				fclose(&f);
			}
			writing = 1;
			return fd;
		}
	}
	if (options != O_RDONLY)
		return FTP_ERR_BADMODE;
	reading++;
	return fd;
}

long my_getfilesize(int fd)
{
	auto int cwd;
	auto int n;
	auto FS_lxd * lxd;
	auto FS_ef * ef;
	auto long sum;

	if (fd < 0)
		return 0;
	cwd = DIR2NUMBER(fd);
	n = fd & FILE_MASK;
	if (n == README_FILENO)
		return strlen(readmes[cwd]);
	if (IS_A_DIR(fd))
		return 10;	// Dummy amount for directories
	switch (cwd) {
		case 0:
			return sspec_getlength(n);
		case 1: /* var */
			if (n == FN_LOG_APPEND)
				return 0;				// This is a write-only file
			return LOG_XMEM_SIZE<<1;	// Others maximum log size (but expanded to account for
											// formatting characters).
		case 2: /* dev */
			switch (n) {
				case FN_KMEM:
					return 0x100000;	// 1M address space
				case FN_RANDOM:
					return -1;			// Indefinite
				case FN_FS2_IMAGE:
					sum = 0;
					for (n = 1; n <= _fs.num_lx; n++) {
						lxd = _fs.lx + n;
						sum += lxd->ps_size * lxd->num_ps;
					}
					return sum;
				default:
					if (n < 1 || n > _fs.num_lx)
						return 0;	// Should not happen
					lxd = _fs.lx + n;
					return lxd->ps_size * lxd->num_ps; 
			}
			break;
		case 3: /* fs2 */
			if (FS_EXISTS(n)) {
				ef = FS_EF_OF_FILE(n);
				return ef->eof;
			}
	}
	return 0;
}

long my_mdtm(int fd)
{
	// Return modification date/time.  Most files we just fake it as the start time
	// of the demo, except for the log we update the mod time.
	int cwd;
	int n;

	if (fd < 0)
		return 0;	// If not valid, return 1980 (should never happen)
	cwd = DIR2NUMBER(fd);
	n = fd & FILE_MASK;
	if (cwd == 1) {
		// var
		if (n != README_FILENO)
			return logmodtime;
	}
	return starttime;
}

// This must be a 64-byte string, exactly repeated once, to 128 bytes.
const char random_data[129] =
"The quick brown fox jumps over the lazy dog. !@#$%^&*()-=+[]{}\r\nThe quick brown fox jumps over the lazy dog. !@#$%^&*()-=+[]{}\r\n";

int my_read(int fd, char *buf, long offset, int len)
{
	auto LogEntry le;
	auto File f;
	auto int cwd, n;
	auto unsigned i, j;

	cwd = DIR2NUMBER(fd);
	n = fd & FILE_MASK;

	if (n == README_FILENO) {
		memcpy(buf, readmes[cwd] + (int)offset, len);
		return len;
	}
	switch (cwd) {
		case 0:
			return sspec_readfile(n, buf, offset, len);
		case 1: /* var */
			switch (n) {
				case FN_LOG:
					if (len < 192)
						return 192;		// Tell server we need this much buffer
					if (!offset)
						// 1st request
						log_seek(MY_LOG, 0);
					if (log_next(MY_LOG, &le) < 0)
						return 0;
					log_format(&le, buf, len, 1);
					n = strlen(buf);
					strcpy(buf+n, "\r\n");
					return n+2;
				case FN_LOG_REV:
					if (len < 192)
						return 192;		// Tell server we need this much buffer
					if (!offset)
						// 1st request
						log_seek(MY_LOG, 1);
					if (log_prev(MY_LOG, &le) < 0)
						return 0;
					log_format(&le, buf, len, 1);
					n = strlen(buf);
					strcpy(buf+n, "\r\n");
					return n+2;
					
				case FN_LOG_APPEND:
					break;
			}
			return 0;
		case 2: /* dev */
			switch (n) {
				case FN_KMEM:
					// Read the entire address space
					xmem2root(buf, offset, len);
					return len;
				case FN_RANDOM:
					// Random data returned.  We don't really generate random data.
					// Instead, we return a repeated string.  Since this goes forever,
					// the client will need to abort the download when they have seen
					// enough (e.g. press the STOP botton on a browser, or Ctl-C on a
					// command-line client).
					j = (unsigned)offset & 63;
					for (n = 0; n < len; n += 64) {
						i = (unsigned)len - n;
						if (i > 64)
							i = 64;
						memcpy(buf + n, random_data + j, i);
					}
					return len;
				case FN_FS2_IMAGE:
					// Entire FS2 file system.
					return 0;
				default:
					// FS2 logical extent, as a raw dump
					return 0;
			}
		case 3: /* fs2 */
			// Read a FS2 file in normal mode.
			if (fopen_rd(&f, n))
				return 0;	// should not happen
			fseek(&f, offset, SEEK_SET);
			fread(&f, buf, len);
			fclose(&f);
			return len;
	}
	return 0;
}

int my_write(int fd, char *buf, long offset, int len)
{
	static char log[LOG_MAX_MESSAGE+10];
	static int loglen;
	static int logpri;
	static int discard;
	auto File f;
	auto int bl, c;
	auto int n;

	if (!offset) {
		loglen = -1;
		discard = 0;
	}
	bl = len;
	
	// If this is called, we know file is OK for write.
	if (fd > FILENUMBER(3, 0) && fd <= FILENUMBER(3, 255)) {
		// Normal FS2 file write
		n = fd & FILE_MASK;
		if (fopen_wr(&f, n))
			return -1;
		fseek(&f, offset, SEEK_SET);
		bl = fwrite(&f, buf, len);
		fclose(&f);
		return bl;
	}
	else if (fd == FILENUMBER(1, FN_LOG_APPEND)) {
		// Append messages to the log.  Each message is of the form
		//  [optional digit] [string] [optional CR] LF
		// If the first char is a digit 0-7, this is assumed to be the priority of
		// the message.  The rest of the characters up to the end of line are put into
		// the log.  If a CR preceeds the LF, it is discarded.  The total length of the
		// line is limited to 115 (LOG_MAX_MESSAGE) characters; any others are discarded.
		// This process is complicated by the fact that the FTP server insists that we
		// process all data.  We must therefore maintain some state information about where
		// we were up to, since we can't log partial messages.  The information is kept in
		// static variables.  This makes the code non-reentrant, but that doesn't matter
		// because we only support one write operation at a time.
		while (bl) {
			if (discard) {
				while (bl) {
					buf++;
					bl--;
					if (buf[-1] == '\n') {
						discard = 0;
						loglen = -1;
						break;
					}
				}
			}
			c = 0;
			while (loglen < LOG_MAX_MESSAGE+2 && bl) {
				if (loglen < 0) {
					if (isdigit(*buf)) {
						logpri = *buf - '0';
						if (logpri > 7) logpri = 7;
						buf++;
						bl--;
					}
					else
						logpri = LOG_INFO;
					loglen = 0;
					continue;
				}
				c = log[loglen++] = *buf;
				buf++;
				bl--;
				if (c == '\n')
					break;
			}
			if (c == '\n') {
				// Got legitimate EOL
				loglen--;
				if (loglen && log[loglen-1] == '\r')
					loglen--;
				log_put(LOG_MAKEPRI(MY_FAC, logpri), 0, log, loglen);
				logmodtime = SEC_TIMER;
				loglen = -1;
			}
			else if (bl) {
				discard = 1;
				log_put(LOG_MAKEPRI(MY_FAC, logpri), 0, log, LOG_MAX_MESSAGE);
				logmodtime = SEC_TIMER;
			}
		}
		return len;
	}
	return -1;
}

int my_close(int fd)
{
	printf("Closed %s transfer\n", writing ? "write" : "read");
	if (writing)
		writing = 0;
	else
		reading--;
	return 0;
}

int my_cd(int cwd, char * dir, int uid)
{
	int rc;
	rc = filename2number(dir, cwd, 1, O_RDONLY);
	printf("dir: %s, cwd: %d, new cwd: %d\n", dir, cwd, rc);
	return rc;
}

int my_pwd(int cwd, char * buf)
{
	strcpy(buf, "/");
	strcat(buf, dirnames[cwd]);
	return 0;
}

int my_dirlist(int item, char *line, int listing, int uid, int cwd)
{
	// This is not quite as easy as it might be: we want to return the ".." and
	// README entries first in the list, so a bit of reordering must be done.
	// The ".." entry is probably superfluous, but we want to make the illusion
	// complete that we are a Unix machine :-)  Maybe we should do "." as well?
	int attribs, fd, spec;
	long length;
	char name[80];
	char attrstr[11];
	char buf[16];
	long modtm;
	int lxn;

	attribs = 0444;
	length = 0;
	
	switch (cwd) {
		case 0:	/* root */
			switch (item) {
				case 0:
					fd = README_FILENO;
					strcpy(name, README_NAME);
					break;
				case 1:
				case 2:
				case 3:
					/* Directories */
					fd = item + DIR_FILENO;
					attribs = 01555;
					strcpy(name, dirnames[item]);
					length = 10;
					break;
				default:
					// sspec names
					spec = sspec_findnextfile(item-NUM_DIRS, SERVER_FTP);
					if (spec < 0)
						return -1;
					fd = spec;
					item = spec+NUM_DIRS;
					strcpy(name, sspec_getname(spec));
					break;
			}
			break;
		default:
			fd = NUMBER2DIR(cwd);
			if (item == 0) {
				strcpy(name, "..");
				attribs = 01555;
				length = 10;
				break;
			}
			if (item == 1) {
				strcpy(name, README_NAME);
				fd |= README_FILENO;
				break;
			}
			switch (cwd) {
				case 1: /* var */
					if (item-2 < NUM_VARS) {
						strcpy(name, varnames[item-2]);
						fd |= item-2 + FN_VAR;
						if (item-2 == FN_LOG_APPEND)
							attribs = 0222;
					}
					else
						return -1;
					break;
				case 2: /* dev */
					if (item-2 < NUM_DEVS) {
						strcpy(name, devnames[item-2]);
						fd |= item-2 + FN_DEV;
					}
					else {
						lxn = item-1-NUM_DEVS;
						if (lxn > _fs.num_lx)
							return -1;
						sprintf(name, "fs2_ext_%d", lxn);
						fd |= lxn;
					}
					break;
				case 3: /* fs2 */
					while (item <= 256 && !FS_EXISTS(item-1))
						item++;
					if (item > 256)
							return -1;
					sprintf(name, "file%d", item-1);
					fd |= item-1;
					attribs = 0666;
					break;
			}
	}

	if (!length && fd >= 0)
		length = my_getfilesize(fd);
	if (fd >= 0)
		modtm = my_mdtm(fd);
	else
		modtm = 0;
		
	if (listing)
		sprintf(line, "%s\r\n", name);
	else {
		strcpy(attrstr, "---");
		if (attribs & 0400)
			attrstr[0] = 'r';
		if (attribs & 0200)
			attrstr[1] = 'w';
		if (attribs & 0100)
			attrstr[2] = 'x';
		sprintf(line, "%c%s%s%s   1 anonymous\tanonymous\t%10lu %s %s\r\n",
			attribs & 01000 ? 'd' : '-',
			attrstr, attrstr, attrstr, length, ftp_ls_date(modtm, buf), name);
	}
	return item;
}

int my_delete(char * name, int uid, int cwd)
{
	auto int fd, n;
	
	if (writing || reading)
		return FTP_ERR_UNAVAIL;
	fd = filename2number(name, cwd, 0, O_RDONLY);
	if (fd < 0)
		return FTP_ERR_NOTFOUND;
	if (fd > FILENUMBER(3, 0) && fd <= FILENUMBER(3, 255)) {
		n = fd & FILE_MASK;
		if (fdelete(n))
			return FTP_ERR_UNAVAIL;
		return 0;
	}
	return FTP_ERR_BADMODE;
}

FTPhandlers hnd;

/********************************
 * Main program                 *
 ********************************/


main()
{
	int file;
	int user;
	int rc;

	printf("Initializing file system.  Please wait...\n");
	rc = fs_init(0,0);
	if (rc) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}
	fs_print_lxs(0);
	
	log_open(MY_LOG, 1);
	log_put(MY_FACPRI, 0, "----log start-----", 18);
	log_put(MY_FACPRI, 0, "FTP Server started", 18);
	log_put(MY_FACPRI, 0, "------------------", 18);

	starttime = logmodtime = SEC_TIMER;
	writing = 0;
	reading = 0;

	hnd.open = my_open;
	hnd.read = my_read;
	hnd.write = my_write;
	hnd.close = my_close;
	hnd.getfilesize = my_getfilesize;
	hnd.dirlist = my_dirlist;
	hnd.cd = my_cd;
	hnd.pwd = my_pwd;
	hnd.mdtm = my_mdtm;
	hnd.delete = my_delete;

	// Set up the first file and user
	file = sspec_addxmemfile("rabbitA.gif", rabbit1_gif, SERVER_FTP);
	user = sauth_adduser("anonymous", "", SERVER_FTP);
	ftp_set_anonymous(user);
	sspec_setuser(file, user);
	sspec_setuser(sspec_addxmemfile("test1", rabbit1_gif, SERVER_FTP), user);
	sspec_setuser(sspec_addxmemfile("test2", rabbit1_gif, SERVER_FTP), user);

	// Set up the second file and user
	file = sspec_addxmemfile("rabbitF.gif", rabbit1_gif, SERVER_FTP);
	user = sauth_adduser("foo", "bar", SERVER_FTP);
	sspec_setuser(file, user);
	sspec_setuser(sspec_addxmemfile("test3", rabbit1_gif, SERVER_FTP), user);
	sspec_setuser(sspec_addxmemfile("test4", rabbit1_gif, SERVER_FTP), user);
	
	sock_init();
	ip_print_ifs();

	ftp_init(&hnd); /* use custom handlers */

	tcp_reserveport(FTP_CMDPORT);	// Port 21

	// Now we sit back and relax...
	while(1) {
		ftp_tick();
	}
}