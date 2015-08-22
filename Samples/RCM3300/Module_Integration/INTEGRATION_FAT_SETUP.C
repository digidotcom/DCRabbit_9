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
 		 INTEGRATION_FAT_SETUP.C

        ** NOTE ** This is the FIRST part of a 2-part sample. You will need
        to run this program before the actual sample, INTEGRATION.C, can run.
        See the section on the FAT Filesystem in INTEGRATION.C for more
        information.

		  This program copies some #ximported files into the FAT filesystem.
		  You should make sure that the FAT filesystem is already initialized
		  (see the FAT sample FMT_DEVICE.C for how to do this).

		  The sample program demonstrates the use of ZSERVER.LIB and FAT
		  filesystem functionality, with RabbitWeb dynamic HTML content,
		  all secured using SSL. This sample also supports dynamic updates
        of both the application and its resources using the Rabbit
        Download Manager (DLM) and HTTP upload capability, respectively
        (neither of these currently supports SSL security).

        This sample will only run on the RCM3300 family, RCM3700, RCM3720 and
        PowerCoreFLEX modules with serial flash.

        The DLM portion of the sample will only work on the RCM3300 family.

        ** NOTE **
        THIS IS AN ADVANCED SAMPLE THAT COMBINES MANY SOFTWARE ELEMENTS FROM
        VARIOUS MODULES. YOU WILL NEED TO HAVE ALL THE MODULES LISTED ABOVE,
        OR THIS SAMPLE WILL NOT COMPILE.

        Before running this sample, you should look at and run samples for
        ZServer, the FAT, RabbitWeb, SSL, the DLM, and HTTP upload to become
        familiar

		  Note that the RabbitWeb ZHTML pages are stored in the 8.3 FAT format
		  with the extension .ZTM.

*******************************************************************************/
/*
 * 	By default, have compiler make function variables storage class
 * 	"auto" (allocated on the stack).
 */
#class auto

// Necessary for zserver.
#define FAT_USE_FORWARDSLASH
#define FAT_BLOCK
#define FAT_FAT16

// Include libraries
#if (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || \
    _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
	#use "rcm33xx.lib"	// sample library to use with this application
#endif
#use "fat.lib"

// Required for this sample, since using anonymous user to put files in the FAT
#define SSPEC_ALLOW_ANONYMOUS_WRITE

#use "zserver.lib"

// Import these files into program flash (xmem).  They will be copied to
// the FAT filesystem when the program is run.
// These pages contain the ZHTML portion of the demonstration.  The first has
// a status page, while the second has a configuration interface.
#ximport "pages/humidity_monitor.zhtml"	monitor_zhtml
#ximport "pages/humidity_admin.zhtml"		admin_zhtml

// This shows the new way of initializing the flashspec.  No need to
// worry about read/write permissions, since we are not serving these
// in this sample.
SSPEC_RESOURCETABLE_START
   SSPEC_RESOURCE_XMEMFILE("/monitor.zhtml", monitor_zhtml),
   SSPEC_RESOURCE_XMEMFILE("/admin.zhtml", admin_zhtml)
SSPEC_RESOURCETABLE_END

// Create a directory with a specified path, if possible
int mkdir(fat_part* part, char* dir)
{
	int rc;
	FATfile file;

   // Call fat_Open with CREATE mode and allocation size
   rc = fat_Open(part, dir, FAT_DIR, FAT_MUST_CREATE, &file, NULL);
   if (rc) {
   	if (rc == -EEXIST) {
      	printf("'%s' already exists\n", dir);
         return 0;
      }
   	printf("Open '%s' failed rc=%d\n", dir, rc);
      return rc;
   }

   printf("Directory '%s' created\n", dir);
	return fat_Close(&file);
}

// Copy a file from SSPEC resource (#ximported files) to files in the FAT
ServerContext dummy_context;

void copy(char * from, char * to)
{
   char buf[256];
   int len, len2;
	int handle, handle2;
   long total;
   long filelen;

   printf("\nCopying from %s to %s...\n", from, to);

   handle = sspec_open(from, &dummy_context, O_READ, 0);
   if (handle < 0) {
   	printf("Could not open %s for reading, rc=%d\n", from, handle);
      return;
   }
   filelen = sspec_getlength(handle);
   if (filelen < 0)
   	filelen = 0;	// Don't know size of compressed files
   // Create with pre-allocation in terms of 1k blocks
   handle2 = sspec_open(to, &dummy_context, O_WRITE|O_CREAT, (word)(filelen >> 10) );
   if (handle2 < 0) {
   	printf("Could not open %s for writing, rc=%d\n", to, handle2);
      sspec_close(handle);
      return;
   }
   total = 0;
	do {
		len = sspec_read(handle, buf, sizeof(buf));
      if (len > 0) {
			len2 = sspec_write(handle2, buf, len);
         if (len2 < len) {
         	printf("Could not write to %s, rc=%d\n", to, len2);
            break;
         }
         else
         	total += len2;
      }
   } while (len > 0);

   sspec_close(handle2);
   sspec_close(handle);
   printf("...done (copied %ld bytes into preallocation of %ld)\n", total, filelen);
}

// --== MAIN ==--
int main()
{
	char buf[80];
   int rc;
	fat_part* part;

#if (_BOARD_TYPE_ & 0xFF00) == (RCM3300A & 0xFF00) || \
    _BOARD_TYPE_ == RCM3309 || _BOARD_TYPE_ == RCM3319
   brdInit();
#endif

   // Set up a dummy server context, so we can use zserver
   // functions to do the copying.
   dummy_context.userid = -1;			// No userid
   dummy_context.server = SERVER_ANY;// All servers
   dummy_context.rootdir = "/";		// Access from root
   strcpy(dummy_context.cwd, "/");
   dummy_context.dfltname = NULL;	// No default filename


	printf("Initializing filesystem...\n");
	// Note: sspec_automount automatically initializes all known filesystems.
   // We assume that the first partition on the drive will be a valid FAT12 or
   // FAT16 partition which will be mounted on '/A'.
   rc = sspec_automount(SSPEC_MOUNT_ANY, NULL, NULL, NULL);
   if (rc) {
   	printf("Failed to initialize, rc=%d\nProceeding anyway...\n", rc);
   }

   // Check that our FAT partition is registered
	part = sspec_fatregistered(0);
   if(!part) {
    	printf("FAT partition 0 not registered, exiting...\n");
      exit(1);
   }

	// First, create our admin dir in our partition
	mkdir(part, "admin");

	//    "flashspec" name   FAT file name
   //   -----------------   ---------------
   copy("/monitor.zhtml",   "/A/monitor.ztm");
   copy("/admin.zhtml",     "/A/admin/index.ztm");
   printf("All done.\n");

   // You should always unmount the device on exit to flush cache entries.
	fat_UnmountDevice(sspec_fatregistered(0)->dev);
	return 0;
}