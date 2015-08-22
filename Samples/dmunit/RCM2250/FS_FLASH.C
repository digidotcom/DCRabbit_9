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
/**********************************************************************
   FS_FLASH.C

   This is the same as FS.C, except that the filesystem
   resides in the 2nd flash chip, instead of RAM. Because it
   uses the 2nd flash, a board with two flash devices installed
   is necessary, such as the RCM2250.  
  
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