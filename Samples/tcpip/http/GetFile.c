/**
 * 	Samples/TcpIp/Http/GetFile.c
 * 	Z-world, 2002
 *
 *    Note added Jan 2004: Dynamic C 8.50 introduces a much simpler
 *    way of doing what this sample does.  Basically, you would not
 *    want to do it this way any more, so this sample is effectively
 *    deprecated.  See samples\tcpip\zserver\updpages.c for the
 *    new way of doing things.  (Compare the size of the samples
 *    and consider that updpages is more flexible to see what I
 *    mean).
 *
 *    When compiling this sample under DC8.50 and above, you will see
 *    a warning message "FTP_WRITABLE_FILES is deprecated.  Set write
 *    access using zserver.lib functions.".  This warning may be
 *    ignored (but see the above paragraph).
 *
 * 	This sample allows one file to be uploaded with FTP.  The page is
 * 	then available as a web page.  This sample is rather large.  It runs
 * 	both HTTP and FTP servers.  See DATAFILE_NAME for the file name,
 * 	seen from both servers.
 *
 * 	This file is added dynamically (at runtime) by calling SSPEC's
 * 	sspec_addfsfile().  The file DATAFILE_NAME is created empty when
 * 	the program starts, and can be updated only through FTP.
 * 	Attempting to upload a file with a different name gives error
 * 	"550 File Not Found."  If the file is too big for the File System,
 * 	the client sees "452 Transfer incomplete" message.
 *
 * 	Always use the BINARY transfer mode.  Many FTP clients default to
 * 	ASCII mode.  This affects the newline chars.  If you upload a file,
 * 	then download it (PUT and GET), the size will be different.  The
 * 	file's contents have also been corrupted.
 *
 * 	From an FTP client program, you can display the file's latest contents
 * 	by doing "get data.txt -" , which sends it to STDOUT rather than
 * 	saving it locally.
 *
 * 	This application uses the file system, web server, FTP server and
 * 	zserver library.  This application is large, and uses 3 TCP server
 * 	libraries.  There are many configuration options.
 *
 * 	Since the file is stored using the File System, make sure there is
 * 	enough space there to store it!  (See below on adjust "RabbitBios.c")
 * 	When the FS refuses, fwrite() will return an error, and we return
 * 	an error indication back to the FTP server library.
 *
 *
 * 	--------------->>>  NOTE  <<<---------------
 *
 * 	(0) If you have problems accessing memory for the file system, then
 * 		recompile the bios (^Y) and run Samples\FileSystem\FS2INFO.C
 *
 * 	(1) Upon startup, if you get the error message:
 *
 * 		"ERROR: No extents have room for file system."
 *
 * 		Then you need to set aside memory for the File System.  This can
 * 		be done in one of two ways, depending on which memory area you
 * 		want the file system to use.  Modify your .\lib\bioslib\memconfig.lib
 * 		file with ONE of these changes:
 *
 * 			#define XMEM_RESERVE_SIZE 0x4000L // FLASH: Amount of space
 * 	..or..
 * 			#define FS2_RAM_RESERVE  4			 // SRAM: Number of 4096-byte blocks
 *
 * 	(2) If the server has problems uploading any files, please force a
 *			reformat.  Can do this with MY_FORCE_FORMAT define.
 *
 ************************************************************************/
#class auto

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

/*
 * Web server configuration
 */

/*
 * One HTTP server, and one FTP server
 */
#define HTTP_MAXSERVERS 1
#define MAX_TCP_SOCKET_BUFFERS 3

/** OVERRIDE **/
#define SAUTH_MAXUSERS 	2


/**
 *  FS_FORCE_FORMAT - Define non-zero to wipe clean the file system, or 0 to
 * 		use whatever is there.  FS2 is very good at  noticing RAM always
 *  		needs formatting, but Flash can hold its contents.
 */
#define  MY_FORCE_FORMAT	0


/**
 *   FTP_USER_NAME, FTP_USER_PASSWORD - Name and password for FTP user that
 * 	can update the data file.  These are compile-time constants, and the
 * 	password is part of the image in plaintext.  Name size is limited by
 * 	the SAUTH_MAXNAME define.  See ftp_set_anonymous() for how to set an
 * 	anonymous user.
 */
#define  FTP_USER_NAME  	 "samwise"
#define  FTP_USER_PASSWORD  "shire"

/**
 * 	FTP_WRITABLE_FILES - Permit some users to update files on the FTP site.
 * 	 Our init code calls sauth_setwritaccess() for those users.
 *    NOTE: under DC8.50 and above, this macro causes a compiler warning.
 */
#define  FTP_WRITABLE_FILES 	1

#define  FTP_DEBUG
#define  FTP_VERBOSE
#define  ZSERVER_DEBUG
#define  ZSERVER_VERBOSE


/**
 *   DATAFILE_NAME, DATA_FILENUMBER - The name and file number of where to
 * 	store the "data.txt" file.  Name shows up for web and FTP servers.
 * 	Choose some number. The high-byte can select an FS2 extent explicitly.
 */
#define  DATAFILE_NAME 	"data.txt"
#define  DATAFILE_NUMBER	(FileNumber)99



/**
 * 	FS2_USER_PROGRAM_FLASH - Set aside this many kilobytes in the program
 * 		flash.  You will also need to edit BIOS\RABBITBIOS.C to set an
 * 		appropriate value to XMEM_RESERVE_SIZE.  The definition here may
 * 		be larger or smaller - the minimum of both specifications will
 * 		be used.
 */
#define FS2_USE_PROGRAM_FLASH 	20

// #define FS_DEBUG
// #define FS_DEBUG_FLASH


//#define ZSERVER_DEBUG

// In addition, you can edit BIOS\RABBITBIOS.C to set a value for
// FS2_RAM_RESERVE to reserve some RAM for use by the filesystem.


/********************************
 * End of configuration section *
 ********************************/

#memmap xmem
#use "fs2.lib"  		//  Always pull in FS MkII.
#use "dcrtcp.lib"
#use "http.lib"
#use "ftp_server.lib"

#ximport "samples/tcpip/http/pages/getfile.html"   index_html
#ximport "samples/tcpip/http/pages/rabbit1.gif"    rabbit1_gif
#ximport "samples/tcpip/http/GetFile.c"            sourcecode_c

FSLXnum	fs_ext;


/**
 *  In this case the .html is not the first type in the type table.
 *  This causes the default (no extension) to assume the shtml_handler().
 */

/* the default for unknown extensions must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", ""),
	SSPEC_MIME(".c", "text/plain"),
	SSPEC_MIME(".txt", "text/plain")
SSPEC_MIMETABLE_END


/* -------------------------------------------------------------------- */


SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/", index_html),
   	/*  "/index.html" and "data.txt" added dynamically  */
	SSPEC_RESOURCE_XMEMFILE("/rabbit1.gif", rabbit1_gif),
	SSPEC_RESOURCE_XMEMFILE("/GetFile.c", sourcecode_c)
SSPEC_RESOURCETABLE_END





/* -------------------------------------------------------------------- */

/**
 * 	Connect an SSPEC object to a named file in the file system.  It caller
 * 	wants, it can be marked writable.  First sets the FS2 extends wherein
 * 	it stores the data contents and meta-file information.
 */
void setup_fs_ftp_file( FileNumber fnumb, /*const*/ char * fname, int userID, int writable )
{
	auto File 	data_file;
	auto int 	index;
   auto word   groups;

	/*
	 *  Default to use 'fs_ext' extent as data and metadata for the shared
	 *  file.  When file is created, it is placed in these ex
	 */
	fs_set_lx(fs_ext, fs_ext);

	/*
	 *  The FTP and HTTP servers share a single file.  Make sure it exists.
	 * 	fcreate() will fail is already there, or succeed if not -- we
	 * 	don't care which occurs, as long as FS2 is working!
	 */
//	fdelete( fnumb );
   fcreate( & data_file, fnumb );
   fclose( & data_file );
	index = sspec_addfsfile(fname, fnumb, SERVER_HTTP | SERVER_FTP);
	if( index < 0 ) {
		printf( "ERROR: Can't add data file \"%s\" (err=%d)!\n", fname, errno );
		exit(2);
	}
   if (writable) {
   	// Get this user's groups
   	sauth_getusermask(userID, &groups, NULL);
      // Set read acces to all groups, write access just for this user's groups.
      // The realm is set to NULL so that the HTTP server will not bug the user for a userid/password
      // (which is required, since none of the userids are known to the HTTP server).
	   sspec_setpermissions(index, NULL, 0xFFFF, groups, SERVER_HTTP|SERVER_FTP, 0, NULL);
      // Allow write access only via FTP server
		sauth_setwriteaccess( userID, SERVER_FTP );
	}

}   /* end setup_fs_ftp_file() */


/**
 * 	Connect an SSPEC object to a named file in extended memory.
 * 	Object is shared between FTP and HTTP servers, read only access.
 */
void
setup_xmem_ftp_file( /*const*/ char * fname, long xmemptr, int userID )
{
	auto int 	index;

	index = sspec_addxmemfile( fname, xmemptr,
									SERVER_HTTP | SERVER_FTP );
	if( index < 0 ) {
		printf( "ERROR: Can't add data file \"%s\" (err=%d)!\n", fname, errno );
		exit(2);
	}

	sspec_setuser( index, userID );

}   /* end setup_xmem_ftp_file() */


/**
 * 	You can use fs_get_flash_lx() (for the 2nd flash, or
 * 		first flash if only one installed); or fs_get_ram_lx() (for
 * 		SRAM if configured in BIOS\RABBITBIOS.C).  See manual for others.
 *  		(see 	fs_set_lx()  call below).
 * 	Abort if no place for file system.
 */
void
select_fs_extent()
{

	/*  First try Flash, then RAM if room for filesystem. */
	fs_ext = fs_get_flash_lx();
	if(0 == fs_ext) {
		fs_ext = fs_get_ram_lx();
		if( 0 == fs_ext ) {
			printf("ERROR: No extents have room for file system.\n");
			exit(2);
		}
	}

	printf("Using device LX# %d...\n", (int)fs_ext);
}   /* end setup_fs_extent() */


/* -------------------------------------------------------------------- */

void main()
{
	int 	index, ftpUserID;
	int 	rc;

	select_fs_extent();

	/*
	 *  fs_init() function performs complete consistency checks and,
	 *  if necessary, fixups for each LX.  It may take up to several
	 *  seconds to run.  It should only be called once at application
	 *  initialization time.
	 */
	rc = fs_init( 0, 0 );
	if( rc != 0 ) {
		printf( "Error: can't get filesystem library initialized.\n" );
		exit(2);
	}

	/*
	 * Format the filesystem if requested.  Note that formatting
	 * must be done _after_ the call to fs_init().
	 *
	 * This demo has compile-time constant of whether the flash should be
	 * formatted or not.
	 */
#if MY_FORCE_FORMAT
		printf( "Note: Formatting MkII File System.\n" );
		if( 0 != lx_format(fs_ext, 0) ) {
			printf( "ABORT: Can't Format MkII File System, errno=%d\n", errno );
			exit(2);
		}
#endif

   sock_init();

   /*
    *  Create an "authorized user" into the system.  To update the file,
    *  external clients must use this name/password.  User "anonymous"
    *  can see the file, but not change its contents.
    */
	ftpUserID = sauth_adduser(FTP_USER_NAME, FTP_USER_PASSWORD, SERVER_FTP);
	if( ftpUserID < 0 ) {
		printf( "ERROR: Can't add FTP user \"" FTP_USER_NAME "\"!\n" );
		exit(2);
	}
   sauth_setwriteaccess(ftpUserID, SERVER_FTP);
	setup_fs_ftp_file( DATAFILE_NUMBER, DATAFILE_NAME, ftpUserID, TRUE );

	/* Put "index.html" under anonymous access. */
	ftpUserID = sauth_adduser( "anonymous", "", SERVER_FTP );
	if( ftpUserID < 0 ) {
		printf( "ERROR: Can't add anonymous FTP user!\n" );
		exit(2);
	}
	ftp_set_anonymous( ftpUserID );
	setup_xmem_ftp_file( "index.html", index_html, ftpUserID );

	/*
	 * 	Setup the FTP operation handlers.  Don't reserve the FTP command
	 * 	port.  "It's just to allow additional queued connections.  There
	 *  	is no real performance reason. [Using resserve_port()] depends
	 * 	on the application.  For HTTP servers you want the queue; for
	 *    sockets with human interaction you probably don't." (SJH)
	 */
   ftp_init(NULL);  				// Use all default handlers.

   http_init();
	tcp_reserveport(80);
	printf( "setup complete.\n" );

   while (1) {
      http_handler();
      ftp_tick();
   }

}   /* end main() */