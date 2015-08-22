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
        ftp_fs2.c

        Demonstration of a simple FTP server, using the ftp library, that
        allows file uploads and deletion.  This is a version of the old
        ftp_server_full.c sample, but shows how to do it using the new
        zserver resource manager.

        If you have already run ftp_server_full.c, and uploaded some
        files, then the files you uploaded will be visible in this
        sample except that the names will be changed!  If you executed
        the FTP command "put my.file" then that file will appear in
        this sample to be in /fs2/file<n> where <n> is a number from
        1 to 255.  Unfortunately, you will not be able to tell which
        of the files relates to the files you uploaded (except by
        downloading them and comparing).

        The user "anonymous" may download the file "rabbitA.gif", but
        not "rabbitF.gif".  The user "foo" (with password "bar") may
        download "rabbitF.gif", and also "rabbitA.gif".  "foo" can also
        upload files, which will be stored in the FS2 filesystem.

        When a file is uploaded, you must call it
        /fs2/file<n>
        where <n> is a decimal number 1..255 inclusive (you choose the
        number).  From a command-line FTP client, you may use a command
        something like
          put  my.file  /fs2/file55
        or maybe
          cd /fs2
          put  my.file  file55

        Each user may execute the "dir" or "ls" command to see a listing of
        the available files.  The listing shows only the files that the
        logged-in user can access.

        When you initially connect, an ls command will show a directory
        called /fs2.  You can CD to that directory in order to list the
        files therein (and upload new ones if you are the foo user).

        This sample also shows how to create an xmem file resource
        _without_ using #ximport.  A small "README" file is generated.


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
 * FTP and FS2 configuration
 *
 * See the sspec_* and sauth_* functions below to see how to set up
 * files and users for the FTP server.
 */


/*
 * This defines the maximum number of files that FS2 can handle.
 */
#define FS_MAX_FILES 20

/*
 * This is the size of the structure that keeps track of files for
 * the FTP server in the dynamic resource table.  Unlike the
 * ftp_server_full.c sample, there is no need to include one
 * count for each FS2 file, since zserver manages these without
 * requiring an additional dynamic resource table entry.  This is
 * set just high enough to contain the "fixed" entries added in
 * main().
 */
#define SSPEC_MAXSPEC 10

/*
 * This must be defined to notify the library that there is no
 * static resource table (used to be known as "flashspec").
 * This macro is required for FTP-only applications since
 * Dynamic C 8.50.
 */
#define SSPEC_NO_STATIC

/*
 * Define this because we are using a static rule table.
 */
#define SSPEC_FLASHRULES


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


#define ANON_GROUP	0x0001		// Group mask for anonymous user(s)
#define FOO_GROUP		0x0002		// Group mask for user(s) which can perform uploads

// This is the access permissions "rule" table.  It associates userid
// information with files in the filesystem.  This is necessary because
// FS2 does not support the concept of owner userids.  Basically,
// this is a simple prefix-matching algorithm.
SSPEC_RULETABLE_START
	// You need to be in user group FOO_GROUP to write resources starting with "/fs2"
   // Everyone can read these files (0xFFFF), but only users in group FOO_GROUP can
   // write (or create) them.
	SSPEC_MM_RULE("/fs2", "foo", 0xFFFF, FOO_GROUP, SERVER_FTP, SERVER_AUTH_BASIC, NULL),
   	// Note: the 2nd string parameter is not relevant to FTP, however it gets
      // printed in the "group" field of the directory listing.  Thus, we may as
      // well set it to "foo".  If set to "" or NULL, then the group field prints
      // as "anon" which may be confusing to the user (since it is not related to
      // the anonymous user).
      // SERVER_AUTH_BASIC means plaintext userid/password matching - this is the only
      // authentication currently supported by FTP.

   // Just for fun, we add another rule for files starting with "/fs2/file9".  This
   // allows write-only access i.e. foo can upload it, but nobody can read it back.
	SSPEC_MM_RULE("/fs2/file9", "foo", 0, FOO_GROUP, SERVER_FTP, SERVER_AUTH_BASIC, NULL),

   // Plug a possible security hole: in the /fs2 directory you will find sub-dirs
   // /flash, /ram, /ext1 etc.  These directories (which provide an extent-specific
   // view of the filesystem) could be used to bypass the above rule.  So, we block
   // access.
	SSPEC_MM_RULE("/fs2/ram", "none", 0, 0, 0, 0, NULL),
	SSPEC_MM_RULE("/fs2/flash", "none", 0, 0, 0, 0, NULL),
	SSPEC_MM_RULE("/fs2/pflash", "none", 0, 0, 0, 0, NULL),
	SSPEC_MM_RULE("/fs2/ext", "none", 0, 0, 0, 0, NULL),
SSPEC_RULETABLE_END


void main()
{
	int anon_user, foo_user;
	faraddr_t  xdest;
	long len;
	char buf[20];

   // Get zserver to handle the details of initializing the FS2 filesystem
   if (sspec_automount(SSPEC_MOUNT_FS, NULL, NULL, NULL)) {
   	printf("Failed to setup FS2.  Please refer to FS2 samples/documentation.\n");
   	exit(1);
   }

   /*
    * Set up dynamic resources and users.  These have nothing to do with FS2 files.
    */

	// Set up anonymous user
	anon_user = sauth_adduser("anonymous", "", SERVER_FTP);
	sauth_setusermask(anon_user, ANON_GROUP, NULL);
   // FTP makes special arrangements for this user...
	ftp_set_anonymous(anon_user);

   // Set up foo user.
	foo_user = sauth_adduser("foo", "bar", SERVER_FTP);
	sauth_setusermask(foo_user, FOO_GROUP, NULL);
	sauth_setwriteaccess(foo_user, SERVER_FTP);		// Must do this to allow write access

	// Set up the first few files.  The sspec_setuser() function has slightly changed
   // semantics from DC8.50.  It sets the access permissions to be
   //   Readable to all groups which the specified user is a member of (and only those).
   //   Write permissions denied to all.
   // Note that we have initially set each user to be in its own, unique, group.
	sspec_setuser(sspec_addxmemfile("rabbitA.gif", rabbit1_gif, SERVER_FTP), anon_user);
	sspec_setuser(sspec_addxmemfile("test1", rabbit1_gif, SERVER_FTP), anon_user);
	sspec_setuser(sspec_addxmemfile("test2", rabbit1_gif, SERVER_FTP), anon_user);

	/*
	 *  This shows how to set up an xmem file (without #ximport).
	 *  Copy the string from one place in XMEMORY to another.
	 */
	xdest = xmem_strdup( xtext );
	sspec_setuser( sspec_addxmemfile( "README", xdest, SERVER_FTP ), anon_user );

	// Set up the next set of files.
	sspec_setuser(sspec_addxmemfile("rabbitF.gif", rabbit1_gif, SERVER_FTP), foo_user);
	sspec_setuser(sspec_addxmemfile("test3", rabbit1_gif, SERVER_FTP), foo_user);
	sspec_setuser(sspec_addxmemfile("test4", rabbit1_gif, SERVER_FTP), foo_user);

   // Having done all the sspec_setuser() calls (for fixed files), since the user "foo"
   // is able to do everything that "anonymous" can, we now make foo a member of _both_
   // groups.
	sauth_setusermask(foo_user, FOO_GROUP | ANON_GROUP, NULL);

	sock_init();
	ftp_init(NULL); /* use default handlers */

	tcp_reserveport(FTP_CMDPORT);	// Port 21

   printf("Ready: FTP to %s\n\n", inet_ntoa(buf, MY_ADDR(IF_DEFAULT)));

	while(1) {
		ftp_tick();
	}
}