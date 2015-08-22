/**********************************************************************
   FS_FLASH.C
   Z-World, 2001
  
   This is the same as FS.C, except it
   utilizes a flash-filesystem in the 1st (program)
   flash device, as is appropriate for the RCM2200.
  
   A change to the BIOS is necessary, to reserve some
   of the first flash for this use. Find the line:
  		#define XMEM_RESERVE_SIZE 0x0000L
   and change it to:
  		#define XMEM_RESERVE_SIZE 0x8000L
   to reserve 32K of space for the flash. Not all
   of this space will be used in this example, to
   account for the System IDBlock, and rounding
   error of flash sector boundaries.

   Due to the limited space in the first flash, it is important to use
   a flash that has a sufficient number of sectors avaliable for the
   filesystem. Using a device with a sector size of around 256 bytes is
   ideal. If a large-sector flash device is necessary, it is highly
   recomended to use a 2nd flash device to store the filesystem, as
   shown by the example programs in the /RCM2250 directory.

**********************************************************************/

#define TC_I_AM_DEVMATE		/* necessary for all DeviceMates */

/*
 * Enable debug messages if this is uncommented
 */
#define TC_FS_DEBUG

/*
 * Necessary to define the network parameters.
 */
#define TCPCONFIG 1

#memmap xmem

/*
 * Pull in the filesystem and the webserver!
 */

// Amount of 1st flash to use (becomes 2nd or 3rd basic LX).
#define FS2_USE_PROGRAM_FLASH	16

#define FS_MAX_FILES		12
#use "fs2.lib"
#use "dcrtcp.lib"
#use "http.lib"

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

/*
 * The Logical Extent (LX) that the target can access in FS2.
 * This MUST be defined! As the actuall LX number is
 * determined at run time, a variable will be used. It will be
 * filled in before the target communications starts!
 */
FSLXnum	fs_ext;
#define TARGETPROC_FS_USELX	(fs_ext)

/*
 * Choose filesystem services.
 */ 
#define USE_TC_FS
#use "tc_conf.lib"

void main(void )
{
	/* init the network */
	sock_init();
	http_init();
	
	/* setup the target communications */
	targetproc_init();

	/* setup the filesystem for the targetproc_fs.lib driver */
	fs_ext = fs_get_flash_lx(); // now the variable is filled in for the TC_FS_USELX above
	printf("Initializing the filesystem... ");
	if(fs_init(0,0)) {
		printf("Couldn't init the FS, errno = %d\n",errno);
		exit(0);
	} else {
		printf("done!\n");
	}
	printf("Formating the 1st FLASH lx... ");
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