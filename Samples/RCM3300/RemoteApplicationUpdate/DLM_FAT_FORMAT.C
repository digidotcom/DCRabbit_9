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
	DLM_FAT_FORMAT.C

	THIS PROGRAM IS PROVIDED AS A SAMPLE ONLY!

	It is intended for use ONLY with RCM3300 family boards that are
	equipped with on-board serial flash.

   This program will format an unformatted serial flash to support
   one FAT partition and one Download Manager (DLM) partition.

   The size of the DLM partition is by default 1MB in size.  You may
   uncomment one of the two defines below to change this to 2MB or
   3MB.  The balance of the space on the serial flash will be set
   up as a single FAT partition.

   Upon completion the FAT partition will be formatted and will be
   Partition 0 and the DLM partition will be partition 1.  If the
   flash already contains an MBR then the program will abort without
   modifying the flash.  To remove an MBR use the SFLASH_INSPECT.C
   demo and clear page 0.

**********************************************************************/
#memmap xmem

// Download Manager Partition size defaults to 1MB
//#define DLM_PARTITION_TWO_MB			// Uncomment for 2MB DLM Partition
//#define DLM_PARTITION_THREE_MB       // Uncomment for 3MB DLM Partition

#define FAT_BLOCK
#define TCPCONFIG 1
#use "dcrtcp.lib"
#use "downloadmanager.lib"
#use "fat.lib"

// Variables needed here to use just part of downloadmanager.lib
// If you're using dlm_init() in your code, these are not be needed
char dlm_sflashpresent;
unsigned long dlm_simages_start;

void main()
{
	int rc;
	mbr_drvr driver;
	mbr_dev dev;

   sock_init();
   // Initialize driver and see if flash has an MBR
   memset(&driver, 0, sizeof(mbr_drvr));
   memset(&dev, 0, sizeof(mbr_dev));
   rc = sf_InitDriver(&driver, NULL);
   while((rc=mbr_EnumDevice(&driver, &dev, 0, NULL)) == -EBUSY);

   // If no MBR, then go ahead and format it
  	if (rc == -EUNFORMAT || rc == -EBADPART)
   {
     	rc = sf_EnumDevice(&driver, &dev, 0);

		// Set the FAT partition size based on size of device and DLM partition
#ifdef DLM_PARTITION_TWO_MB
   	dev.part[0].partsecsize = dev.seccount - (0x200000 / dev.byte_sec);
#else
  #ifdef DLM_PARTITION_THREE_MB
   	dev.part[0].partsecsize = dev.seccount - (0x300000 / dev.byte_sec);
  #else
   	dev.part[0].partsecsize = dev.seccount - (0x100000 / dev.byte_sec);
  #endif
#endif

      // Set the rest of the FAT partition parameters
	   dev.part[0].bootflag    = 0;
   	dev.part[0].starthead   = 0xfe;   // Sets to LBA addressing
	   dev.part[0].startseccyl = 0;
   	dev.part[0].parttype    = 6;      // FAT 16 Partition
	   dev.part[0].endhead     = 0xfe;
   	dev.part[0].endseccyl   = 0;
	   dev.part[0].startsector = dev.byte_page / dev.byte_sec; // Page align

      // Write updated MBR with FAT partition parameters
		rc = mbr_FormatDevice(&dev);
      if(rc == -EBUSY || rc == -EAGAIN)
        	while((rc = sf_WriteContinue(&dev)) == -EBUSY);

      // Automount will now format the above partition
	   rc = fat_AutoMount(FDDF_MOUNT_DEV_0 | FDDF_MOUNT_PART_0 |
      							FDDF_UNCOND_PART_FORMAT | FDDF_COND_PART_FORMAT);

      // Unmount the device from the fat system
      fat_UnmountDevice(&dev);

		if (rc)
      	printf("ERROR on fat_AutoMount = %d, FAT Partition not Formatted.\n");
      else
      {
      	printf("FAT Partition 0 Formatted.\n");
         // Download Manager will find empty space and create DLM Partition
			if (dlm_initserialflash())
	      	printf("DLM Partition 1 Formatted.\n");
         else
	      	printf("ERROR on DLM Partition Format.\n");
      }
   }
   else
   	printf("Device is already Formatted, no changes made.\n");
}