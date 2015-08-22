/**********************************************************************
   FS.C
   Z-World, 2001

   This will provide filesystem access to a target
   processor, that is connected to this devmate device.
   The target processor may make requests, that will be
   serviced by this library, such as uploading,
   removing (deleting), or renaming files.
  
   As the interface to the filesystem that is provided
   to the target processor is a rather different task
   from running the filesystem itself, the job of setting
   up and initializing the FS2.LIB library is left to the
   user. This allows for much greater flexibility, as
   the details of the filesystem, such as if it runs in
   RAM or FLASH, or the size that is allocated for it,
   can be set on a program-by-program basis, without
   having to change the TARGETPROC_FS.LIB library.
 
   Once a file is uploaded from the target, it will
   be available in the local filesystem for other
   applications to use, such as the web (HTTP) or
   FTP servers. This is accomplished through the
   ZSERVER.LIB libary, which provides a uniform interface
   to files stored anywhere on the system, that applications
   can access easily. As this sample program makes use
   of the web (HTTP) server, make sure the network
   information below is correct! After files are uploaded,
   you should be able to browse to:
       http://[MY_IP_ADDRESS]/myfilename
   and be able to find your file there. For more examples
   of using the ZSERVER.LIB library, there are several
   web examples that use it, specifically:
       samples\tcpip\http\static2.c
  
   IMPORTANT NOTE!!!
   This program requires a small change to the BIOS!
   As the filesystem used in this example is a RAM filesystem,
   (though any FS2 filesystem may be used) room in RAM must
   be reserved. You will find the line:
  		#define FS2_RAM_RESERVE 0 	// Number of 4096-byte blocks
   at the top of the BIOS. Change it to an appropriate value
   such that room is allocated in main system memory. I have
   been allocating 16K for these tests, but the exact ammount
   is not as important.
  		#define FS2_RAM_RESERVE 4 	// Number of 4096-byte blocks
   It is not always necessary to use the filesystem in this
   manner, as various forms of flash filesystems are avaliable
   as well, but for this sample program, a simple RAM based
   filesystem is suitable. For more examples on filesystem
   usage, see the sample programs in samples\FileSystem.
**********************************************************************/

#define TC_I_AM_DEVMATE		/* necessary for all DeviceMates */

/*
 * Enable debug messages if this is uncommented
 */
#define TC_FS_DEBUG

/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1

#memmap xmem

/*
 * Pull in the filesystem and the webserver!
 */
#define FS_MAX_FILES		12
#use "fs2.lib"
#use "dcrtcp.lib"
#use "http.lib"

/**
 * The Logical Extent (LX) that the target can access in FS2.
 * This MUST be defined! As the actual LX number is
 * determined at run time, a variable will be used. It will be
 * filled in before the target communications starts!
 */
FSLXnum	fs_ext;
#define TARGETPROC_FS_USELX	(fs_ext)

/*
 * Choose filesystem services from DeviceMate
 */ 
#define USE_TC_FS
#use "tc_conf.lib"

/*
 * MIME types for use by the web server.
 * The default mime type for '/' must be first!
 */
const HttpType http_types[] =
{
   { ".html", "text/html", NULL},
   { ".gif", "image/gif", NULL}
};
/*
 * The web server spec table. It must not be empty, and
 * the real files will be filled in later, so one dummy
 * entry is used for now.
 */
const HttpSpec http_flashspec[] =
{
  { HTTPSPEC_FILE,  "/dummy",             0,    NULL, 0, NULL, NULL},
};


/* -------------------------------------------------------------------- */

void main(void)
{
	/* init the network */
	sock_init();
	http_init();
	
	/* setup the target communications */
	targetproc_init();

	/* setup the filesystem for the targetproc_fs.lib driver */
	fs_ext = fs_get_ram_lx(); // now the variable is filled in for the TC_FS_USELX above
	printf("Initializing the filesystem... ");
	if(fs_init(0,0)) {
		printf("Couldn't init the FS, errno = %d\n",errno);
		exit(0);
	} else {
		printf("done!\n");
	}
	printf("Formating the RAM lx... ");
	if(lx_format(fs_ext,0)) {
		printf("Format failed! errno = %d\n",errno);
		exit(0);
	} else {
		printf("done!\n");
	}

	/* filesystem should be ready now - drive it all */	
	for(;;) {
		tcp_tick(NULL);
		http_handler();
		targetproc_tick();
	}
}