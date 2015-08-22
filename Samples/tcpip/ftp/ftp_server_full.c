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
        ftp_server_full.c

        Demonstration of a simple FTP server, using the ftp library, that
        allows file uploads and deletion.  This is an extension of the
        ftp_server.c sample program.

        The user "anonymous" may download the file "rabbitA.gif", but
        not "rabbitF.gif".  The user "foo" (with password "bar") may
        download "rabbitF.gif", and also "rabbitA.gif", since files owned
        by the anonymous user are world-readable.  "foo" can also
        upload files, which will be stored in the FS2 filesystem.

        Each user may execute the "dir" or "ls" command to see a listing of
        the available files.  The listing shows only the files that the
        logged-in user can access.

        This sample also shows how to create an xmem file resource
        _without_ using #ximport.  A small "README" file is generated.

   NOTE:
        As of DC 8.50, this sample is deprecated.  It still works, however
        new code should use the samples\tcpip\ftp\FTP_FS2.C program
        as a starting point.

        >>> This sample makes use of the "userblock" (which is a special
        area of the program flash memory) to store the mapping between
        file names and FS2 file numbers (1-255).  This is useful
        technique if you want to use _arbitrary_ filenames.  Otherwise,
        the new (DC8.50) way of doing things is that the FS2 filesystem
        is automatically managed by the Zserver library, which creates
        filenames of the form /fs2/file1, /fs2/file2 etc.  Since the mapping
        between filenames and FS2 file numbers is fixed, this may not always
        meet requirements, however it does eliminate the need to use part
        of the userblock (or other non-volatile storage) to store the
        mapping.

        Defining FTP_USE_FS2_HANDLERS, as this sample does, will generate
        largely redundant code in the FTP server, because most of the
        same functionality has been moved to zserver.lib.

*******************************************************************************/
#class auto

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your        *
 * preferences.                    *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

/*
 * FTP configuration
 *
 * See the sspec_* and sauth_* functions below to see how to set up
 * files and users for the FTP server.
 */

/*
 * Defining the following will format the filesystem and reset the
 * list of files in the userblock.  This should be done the first time
 * you run the program so that the filesystem is put in a known state.
 */
#define FORMAT

/*
 * The following must be defined to allow the FTP server to write
 * new files to the filesystem.  It modifies the default handlers
 * to allow filesystem writes.
 */
#define FTP_USE_FS2_HANDLERS

/*
 * This mask is used when new files are uploaded to the FTP server, and
 * is passed to the sspec_addfsfile() call.  This is used to give
 * access to the new file to the specified server(s).  Each server has
 * a predefined macro constant (SERVER_FTP, SERVER_HTTP etc.) which
 * may be bitwise-ORed together.  SERVER_ANY can also be used.
 */
#define FTP_CREATE_MASK  SERVER_FTP

/*
 * The structure that holds the correlation between filenames and file
 * locations (whether in the filesystem or in system memory) must be
 * saved to the userblock.  This macro specifies the offset into the
 * userblock at which this structure will be saved.  The amount of
 * information that will be saved can be checked by checking the value
 * of sizeof(server_spec).
 */
#define FTP_USERBLOCK_OFFSET 0

/*
 * This defines the maximum number of files that FS2 can handle.
 */
#define FS_MAX_FILES 50

/*
 * This is the size of the structure that keeps track of files for
 * the FTP server, whether they are in the filesystem or the dynamic
 * resource table.  It must be large enough for the number of files you need
 * in the FTP server, plus the "fixed" ones initialized in main().
 */
#define SSPEC_MAXSPEC (10+FS_MAX_FILES)

/*
 * This must be defined to notify the library that there is no
 * static resource table (used to be known as "flashspec").
 * This macro is required for FTP-only applications since
 * Dynamic C 8.50.
 */
#define SSPEC_NO_STATIC

/*
 * This must be defined for the FTP server to support the DELE
 * (delete) command.
 */
#define FTP_EXTENSIONS


/*
 * Optionally define debugging macros (for single-stepping in library)
 */

#define FTP_DEBUG
#define ZSERVER_DEBUG

/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "fs2.lib"
#use "dcrtcp.lib"
#use "ftp_server.lib"

#ximport "samples/tcpip/http/pages/rabbit1.gif" rabbit1_gif


/* The empty string at the end is used to calculate string length. */
xstring xtext { "Welcome to the Rabbit FTP server.  Download a file.\r\n", "" };


/**
 * 	Duplicate a string with a four-byte size in front of it.
 * 	Allocates xmem to store size and string.  The "xstring"
 * 	directive does NOT store the size.  To use sspec_addxmemfile(),
 * 	this is required.  Thus the copy which prepends a four-byte size
 * 	value (not including NUL char).  The final xmem2mem() copies the
 * 	string just after the size.
 *
 * 	RETURN: 	physical addr of 4 byte size (string follows, without
 * 			its NUl char termination).
 */
long
xmem_strdup( long xstr )
{
	auto long	xsrc, xnext;
	auto long 	siz;
	auto long 	xdest;

	xmem2root( & xsrc, xstr+0, 4 );
	xmem2root( & xnext, xstr+4, 4 );
	siz = xnext - xsrc - 1;   					// Don't store NUL char.

	xdest = xalloc( (int)siz + 4 );
	root2xmem( xdest, & siz, 4 );
	xmem2xmem( xdest+4, xsrc, (int)siz );

	return( xdest );
}   /* end xmem_strdup() */

/* -------------------------------------------------------------------- */

void main()
{
	int file;
	int user;
	faraddr_t  xdest;
	FSLXnum ext;
	long len;

	// Get the flash logical extent.  If you want the filesystem in
	// a different area, you will need to change this call (see
	// fs_get_ram_lx() and fs_get_other_lx()).
	ext = fs_get_flash_lx();

	fs_init(0, 0);
#ifdef FORMAT
	// Put the filesystem and userblock in a known initial state
	lx_format(ext, 0);
	len = 0;
	writeUserBlock(FTP_USERBLOCK_OFFSET, &len, sizeof(long));
#endif

	// ftp_load_filenames() loads the data structure that keeps track of
	// the correlation between filenames and the locations of the files.
	// Note that ftp_load_filenames() will blank out any entries that are
	// not fs2 files.
	if (ftp_load_filenames() < 0) {
		printf("Did not load filenames!  Saving the list now...\n");
		// Go ahead and save the list of filenames, even if there aren't
		// any now, just to get in a known initial state.
		ftp_save_filenames();
	}

	// Set up the first file and user
	file = sspec_addxmemfile("rabbitA.gif", rabbit1_gif, SERVER_FTP);
	user = sauth_adduser("anonymous", "", SERVER_FTP);
	ftp_set_anonymous(user);
	sspec_setuser(file, user);
	sspec_setuser(sspec_addxmemfile("test1", rabbit1_gif, SERVER_FTP), user);
	sspec_setuser(sspec_addxmemfile("test2", rabbit1_gif, SERVER_FTP), user);

	/*
	 *  Copy the string from one place in XMEMORY to another.
	 */
	xdest = xmem_strdup( xtext );
	sspec_setuser( sspec_addxmemfile( "README", xdest, SERVER_FTP ), user );

	// Set up the second file and user
	file = sspec_addxmemfile("rabbitF.gif", rabbit1_gif, SERVER_FTP);
	user = sauth_adduser("foo", "bar", SERVER_FTP);
	sauth_setwriteaccess(user, SERVER_FTP);
	sspec_setuser(file, user);
	sspec_setuser(sspec_addxmemfile("test3", rabbit1_gif, SERVER_FTP), user);
	sspec_setuser(sspec_addxmemfile("test4", rabbit1_gif, SERVER_FTP), user);

	sock_init();
	ftp_init(NULL); /* use default handlers */

	tcp_reserveport(FTP_CMDPORT);	// Port 21

	while(1) {
		ftp_tick();
	}
}