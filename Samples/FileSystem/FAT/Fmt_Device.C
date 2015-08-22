/***********************************************************************
	FMT_DEVICE.C

   Digi International, Copyright (c) 2007.  All rights reserved.

   Formats the configured device(s) for use with the FAT file system.

   Requires the FAT filesystem module be installed.

   Also requires that you run this on a board with a compatible
   storage medium (serial flash, NAND flash or SD card).

   IMPORTANT!!!!  Please use with caution.

   Allows formatting of existing FAT partitions (previously formatted
   or unformatted).  This will only affect the selected partition.

   Formatting a device will unconditionally ERASE ALL DATA on any
   FAT 12/16 partitions during formatting.  It will not affect non-FAT
   partitions or FAT 32 partitions except as noted below.

   If format device is selected and NO FAT 12/16 partitions are found, a
   prompt will appear allowing erasure of all existing partitions and
   then the creation of one large FAT partition using the entire device.
   This option WILL DESTROY FAT 32 PARTITIONS AND ALL DATA STORED THERE!

   This sample does not allow the creation of multiple partitions that
   do not already exist on the device.  To create multiple partitions,
   see the fat_write_mbr.c sample program.

   This sample deals with low level formatting of FAT devices and uses
   several 'internal' data structures which are not documented in the
   FAT user's manual.  This is especially true of the erase_MBR function,
   which destroys existing device formating.  This is NOT a function
   that should typically be included in an application, and is only
   included here to make initial formatting of preformatted commercial
   media easier for the Rabbit user.

************************************************************************/

// Map program to xmem if not compiling to separate I&D space.
#if !__SEPARATE_INST_DATA__
	#memmap xmem
#endif

// Set FAT library to blocking mode
#define FAT_BLOCK

// various library debug-enable macros
//#define FAT_DEBUG
//#define FATWTC_DEBUG
//#define NFLASH_DEBUG
//#define NFLASH_FAT_DEBUG
//#define SDFLASH_DEBUG
//#define SDFAT_DEBUG
//#define SFLASH_DEBUG
//#define SFLASH_FAT_DEBUG
//#define FATWTC_DEBUG_INIT

// Makes the device drivers and FAT filesystem libraries available
#use "fat.lib"

#ifdef _DRIVER_1
   #define EMBR_DRV1  _DRIVER_1
#else
   #define EMBR_DRV1
#endif

#ifdef __SDFLASH_LIB__
	#define USING_CHAR_FLASH_BUF
#else
 #ifdef __SFLASH_LIB__
	#define USING_CHAR_FLASH_BUF
 #endif
#endif

// This function DESTROYS the MBR on a device without warning.
// This also destroys DATA ACCESS TO ALL INFORMATION ON THE DEVICE.
// This function accesses several internal FAT data structures to
// determine the low level driver to use, and makes low level driver
// calls directly.
void erase_MBR(int dev)
{
	auto int i, j, rc;
#ifdef USING_CHAR_FLASH_BUF
   static char flash_buf[1056];
#endif
#ifdef __NFLASH_LIB__
	auto nf_device nandFlash;
   auto long fbuf, fbuf_size;
   auto long sbuf_size;
#endif

#ifdef USING_CHAR_FLASH_BUF
   // clear buffer to be written to erase MBR and BPB from the device
   memset (flash_buf, 0, 1056);
#endif

   // iterate through the drivers to find a match for the specified device
   for (i = 0; i < num_fat_drivers; ++i) {
      if (!_fatDevConfig[dev].InitDriver || !_fatConfig[i].InitDriver) {
         // if reached end of devices or drivers tables then indicate device
         //  not found and end the iteration
         i = num_fat_devices;
      }
      else if (_fatDevConfig[dev].InitDriver == _fatConfig[i].InitDriver) {
         // found the device's driver, i has the driver table index
         break;
      }
   }

	// default to unknown device type
   j = 0;
	// if device's driver found then classify device type
   if (i < num_fat_devices) {
      // set j to 1 for SD card, 2 for serial flash or 3 for nand flash
      if (!strcmp(_fatConfig[i].type, "SD")) {
         // device is SD Card flash
         j = 1;
      }
      else if (!strcmp(_fatConfig[i].type, "SF")) {
         // device is serial flash
         j = 2;
      }
      else if (!strcmp(_fatConfig[i].type, "NF")) {
         // device is nand flash
         j = 3;
      }
   }
   if (!j) {
      // unknown device number or type!
      exit(-ERR_BADPARAMETER);
   }

   for (i = 0; i < 3; i++) {
      switch (j) {

#ifdef __SDFLASH_LIB__
       case 1:
         if (rc = sdspi_write_sector(sd_getDevice(&SD[0],
                                                  _fatDevConfig[dev].deviceID),
                                     i, flash_buf))
         {
            printf("Failure clearing pages (%d) %ls.\n", rc, error_message(rc));
            printf("Format device failed.\n\n");
            exit(rc);
         }
         break;
#endif

#ifdef __SFLASH_LIB__
       case 2:
         rc = sf_writeRAM(flash_buf, 0, sf_blocksize);
         if( rc )  {
            printf("sf_writeRAM() write error on page %d (%d).\n", i, rc);
            exit(rc);
         }
         rc = sf_RAMToPage(i);
         if(rc)  {
            printf("sf_RAMToPage() write error on page %d (%d).\n", i, rc);
            exit(rc);
         }
         break;
#endif

#ifdef __NFLASH_LIB__
       case 3:
         fbuf_size = 16384;
         fbuf = _xalloc(&fbuf_size, 0, XALLOC_ANY);
         // clear buffer to be written to erase MBR and BPB from the device
         xmemset(fbuf, 0, (unsigned) fbuf_size);
         sbuf_size = ((long) _nf_defaultDevList[dev].sparesize <<
                      (_nf_defaultDevList[dev].erasebitshift -
                      _nf_defaultDevList[dev].pagebitshift)) +
                     (long) _nf_defaultDevList[dev].sparesize;

         _nf_defaultDevList[dev].sparebfbuf = _xalloc(&sbuf_size, 0,
                                                      XALLOC_ANY);
         rc = nf_writePage(&_nf_defaultDevList[dev], fbuf, 0L);
         xrelease(_nf_defaultDevList[dev].sparebfbuf, sbuf_size);
         xrelease(fbuf, fbuf_size);

         if( rc )  {
            printf("Write error to page 0 (%d).\n", rc);
            exit(rc);
         }
         i = 3;
         break;
#endif

       default:
         return;
      }
   }

   // Disconnect driver and device structures from tables
   // This forces re-scanning device for valid MBR
   _fat_config_init();

   return;
}


// format one partition or all partitions on specified device
int format(int dev, int part)
{
	auto int i, j, end, good_rc, rc;
	auto word flags_to_use;
   auto char s[9];

   if (dev < 0 || dev >= num_fat_devices) {
      printf("\nDevice %d does not exist.\n", dev);
      return -ENODEV;
   }

   if (part >= FAT_MAX_PARTITIONS) {
      printf("\nDevice partition %d is not recognized (maximum is %d).\n", part,
             FAT_MAX_PARTITIONS - 1);
      return -EBADPART;
   }

   // check for option, as partition letter or device number
   if (part < 0) {
      i = dev * FAT_MAX_PARTITIONS;
      end = i + FAT_MAX_PARTITIONS - 1;
      printf("This will destroy ALL DATA on partitions %c-%c!", 'A' + i,
             'A' + end);
   }
   else {
      i = end = dev * FAT_MAX_PARTITIONS + part;
      printf("This will destroy ALL DATA on partition %c!", 'A' + i);
   }
   printf("\nAre you sure you want to continue? (y/n)  ");
   gets(s);
   printf("\n\n");
   if (toupper(s[0]) != 'Y') return 0;

	good_rc = 0;	// not seen a good partition result so far
   j = 0;	// not counted any invalid partitions so far
   while (i <= end) {
      if (fat_part_mounted[i]) {
         // a previously mounted partition is a good result
         good_rc = 1;
		   // Unmount the selected mounted partition
		   printf("Unmounting partition %c.\n", 'A' + i);
		   rc = fat_UnmountPartition(fat_part_mounted[i]);
		   if (rc) {
			   printf("fat_UnmountPartition error: (%d) %ls.\n\n", rc,
			          error_message(rc));
            break;
		   }
      }
      else {
        // Find unmounted FAT partitions (if any)
        rc = fat_EnumPartition(&_fat_device_table[dev], i % FAT_MAX_PARTITIONS,
        						    &_fat_part_table[i]);
        if (!rc || rc == -EUNFORMAT) {
          fat_part_mounted[i] = &_fat_part_table[i];
          // a newly mounted partition is a good result
          good_rc = 1;
        }
        else {
          if (rc == -ENOPART || rc == -EBADPART || rc == -ENOSYS) {
            printf("Partition %c either doesn't exist or is neither", 'A' + i);
            printf(" a FAT12 nor a FAT16 partition.\n");

            if (good_rc) {
               // if ever a good partition result then always a good result at
               //  this point (don't report errors for non-existent partitions)
               rc = 0;
            }
            ++i;       // Not a FAT partition, move to next
            ++j;       // Count invalid partitions
            if (j < FAT_MAX_PARTITIONS)
              continue;

            printf("\n\nNo FAT12 or FAT16 partitions found.\n\n");
            printf("Erase device and create one large FAT partition? (y/n)  ");
            gets(s);
            printf("\n\n");
            if (toupper(s[0]) == 'Y') {
               erase_MBR(dev);
               // Set format flags for device to be formatted
               if (dev == 0) {
                  flags_to_use = FDDF_UNCOND_DEV_FORMAT | FDDF_MOUNT_DEV_0 |
                                 FDDF_UNCOND_PART_FORMAT | FDDF_MOUNT_PART_0;
               }
               else {
                  flags_to_use = FDDF_UNCOND_DEV_FORMAT | FDDF_MOUNT_DEV_1 |
                                 FDDF_UNCOND_PART_FORMAT | FDDF_MOUNT_PART_0;
               }
               // first use automount to format the device w/ one big partition
             	rc = fat_AutoMount(flags_to_use);
            	if (rc && rc != -EBADPART && rc != -EUNFORMAT) {
            		printf("\nfat_AutoMount() error (%d) %ls.\n", rc,
                                  error_message(rc));
            	}
            	else {
            	   // next use automount to mount all devices' partitions
            	   rc = fat_AutoMount(flags_to_use &
                              (FDDF_MOUNT_DEV_ALL | FDDF_MOUNT_PART_ALL));
            	   if (rc) {
            	      if (fat_part_mounted[dev * FAT_MAX_PARTITIONS]) {
            	         // success if mounted just-formatted partition,
            	         //  regardless of other partitions' state
            	         rc = 0;
            	      }
            	      else {
                        printf("\nfat_AutoMount() error (%d) %ls.\n", rc,
                               error_message(rc));
            	      }
            	   }
            	}
            }
            continue;
          }
          else break;
        }
      }
		printf("Formatting partition %c.\n", 'A' + i);
		rc = fat_FormatPartition(&_fat_device_table[dev], fat_part_mounted[i],
		                         i % FAT_MAX_PARTITIONS, 6, NULL, NULL);

		if (rc) {
  			printf("fat_FormatPartition error: (%d) %ls.\n\n", rc,
                          error_message(rc));
         break;
		}
      else {
  			printf("Mounting partition %c.\n", 'A' + i);
   		rc = fat_MountPartition(fat_part_mounted[i++]);
	   	if (rc) {
		   	printf("fat_MountPartition error: (%d) %ls.\n\n", rc,
                            error_message(rc));
            break;
  			}
      }
	}

	printf("\n");
   return rc;
}


void main(void)
{
   static int i, rc;
   static char s[80];
   static int dev;
   static char cmd;
   static word fmt_flags;

#ifdef FAT_DEBUG
	fat_print_config();
#endif

	rc = fat_AutoMount(FDDF_MOUNT_DEV_ALL | FDDF_MOUNT_PART_ALL);
	if (rc) {
		printf("\nfat_AutoMount() error (%d) %ls.\n", rc, error_message(rc));
      // If device initialization fails or medium not present, exit
      if (rc == -EIO || rc == -ENOMEDIUM) exit(rc);
	}

   printf("\nNote:  Formatting destroys all data.\n");
   printf("       Quit now or forever hold your peace!\n\n");

   for (;;) {
   	for (i = 0; i < num_fat_devices * FAT_MAX_PARTITIONS; ++i) {
   		if (fat_part_mounted[i]) {
   			printf("Partition %c is mounted.\n", 'A' + i);
         }
   	}
   	printf("\nEnter a command (single char only, then press Enter):\n");
      printf("  p  \tPurge cache (try this if getting -310 error codes)\n");
      for (i = 0; i < num_fat_devices; ++i) {
         printf("  %c-%c\tFormat partition 0, 1, 2 or 3 on device %d\n",
                'A' + i * FAT_MAX_PARTITIONS, 'D' + i * FAT_MAX_PARTITIONS, i);
      }
      for (i = 0; i < num_fat_devices; ++i) {
         printf("  %d  \tFormat entire device %d (all FAT 12/16 partitions)\n",
                i, i);
      }
      printf("  q  \tQuit (after unmounting all partitions)\n");
      printf("\nYour choice?  ");
      gets(s);
      printf("\n\n");
      cmd = toupper(s[0]);
      rc = 0;
      switch (cmd) {
      case 'P':
      	rc = fatwtc_flushall(WTC_PURGE | WTC_WAIT);
     		printf("Cache purge was %s\n", rc ? "unsuccessful!" : "successful.\n");
      	if (!rc) {
      		printf("Restart fmt_device.c to format devices (exiting now).\n");
      		exit(0);
      	}
         break;
      case 'Q':
	      // Unmount all of the mounted FAT partitions & devices before exiting
	      for (i = 0; i < num_fat_devices * FAT_MAX_PARTITIONS; ++i) {
	         if (fat_part_mounted[i]) {
	            // unmount the entire device associated with this FAT partition
	            rc = fat_UnmountDevice(fat_part_mounted[i]->dev);
	            if (!rc) {
	               // ignore further (already unmounted) partitions on this device
	               i += FAT_MAX_PARTITIONS - i % FAT_MAX_PARTITIONS;
	            }
	         }
	      }
	      exit(0);
      case 'A': case 'B': case 'C': case 'D':
      case 'E': case 'F': case 'G': case 'H':
         rc = format((cmd - 'A') / 4, (cmd - 'A') % 4);
         break;
      case '0': case '1':
         // -1 parameter triggers format of ALL partitions
         rc = format(cmd - '0', -1);
         break;
      default:
      	printf("Unsupported or unrecognized command.\n");
      	printf("Note: Presently, only FAT devices 0 and 1 are supported.\n\n");
         continue;
      }

      if (rc)
      	printf("\nOperation failed (%d) %ls.\n\n", rc, error_message(rc));
      else
      	printf("\nOperation successful.\n\n");
   }
}

