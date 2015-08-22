/*****************************************************************************
        Samples\FileSystem\FAT\fat_write_mbr.c
        Digi International, Copyright (c) 2007.  All rights reserved.

        Requires the FAT file system module to be installed.

        Requires that you run this on a core module with a FAT compatible
        flash memory device.  This sample works with the primary device
        defined by MBR_DRIVER_INIT as the default.  If you need to work
        with a different device, uncomment the appropriate MBR_DRIVER_INIT
        definition to override the core module's default primary setting.

        This sample creates multiple partitions on an UNFORMATTED flash
        device.  It only writes the partition table to the MBR, it does
        not format the individual partitions.  This can be done through
        the sample programs fmt_device.c or fat_shell.c.  Note that the
        FAT_MAXPARTITIONS macro definition in FATCONFIG.LIB may first need
        to be updated to suit a large number of partitions that are to be
        formatted and used simultaneously.

        If the device is not present, unresponsive or already formatted,
        this sample will exit with an error message.

        You can erase a serial flash device by running sflash_inspect.c
        and clearing the pages 0, 1 & 2.

        You can erase a NAND flash device by using nflash_inspect.c
        and clearing the pages 0 & 1.  Be aware that multiple
        partitions on an xD card are not supported by most PCs, so if
        you need your xD card to be PC compatible, use one partition.

        FAT does not support multiple partitions on miniSD flash cards,
        this sample will error if run on a miniSD card.

******************************************************************************/
#class auto
#define FAT_BLOCK

// To override the default primary device and force use of the serial flash,
// uncomment the following two macro definitions.
//#define MBR_DRIVER_INIT sf_InitDriver(root_driver, NULL)
//#define MBR_SIG "SFLASH-1"

// To override the default primary device and force use of the NAND flash,
// uncomment the following two macro definitions.
// For a removable device such as an xD card, the MBR_SIG macro definition
// should be edited to give each partitioned device a unique signature.
//#define MBR_DRIVER_INIT nf_InitDriver(root_driver, NULL)
//#define MBR_SIG "NFLASH-1"
// If the device to be partitioned is not enumerated as device 0, uncomment
// the following macro definition and edit if necessary to suit the device's
// enumeration.  (E.G. an RCM3360's or RCM3365's xD card is enumerated as 1).
//#define MY_ENUM_DEVICE 1

#ifndef MY_ENUM_DEVICE
#define MY_ENUM_DEVICE 0
#endif

// Call in the FAT filesystem support code.
#use "fat.lib"

mbr_drvr  my_driver;    // Driver structure
mbr_dev	 my_device;    // Device structure, this holds the partition table

void main(void)
{
	int rc;		// Return code store.  Always need to check return codes from
   				// FAT library functions.
   unsigned long sectors, partsize, start;
   float megs;
   char i, buf[15], *ptr;
	mbr_drvr  *root_driver;

 #ifdef FAT_SD_DEVICE
   // Primary SD card device check
   if (FAT_SD_DEVICE == FDDF_MOUNT_DEV_0) {
      printf("Multiple partitions are not supported on SD cards, exiting.\n");
      exit(-EPERM);
   }
 #endif

	// provide the root_driver reference required by the MBR_DRIVER_INIT macro
	root_driver = &my_driver;

   // initialize the FAT device
   rc = MBR_DRIVER_INIT;

   if (rc) {
      // failed device initialization, print error and exit
      printf("Device initialization failed with error %d.\n", rc);
      exit(rc);
   }

	// enumerate the FAT device
   do {
      rc = fat_EnumDevice(&my_driver, &my_device, MY_ENUM_DEVICE, MBR_SIG, 1);
   } while (rc == -EBUSY);

if (rc == -ENOMEDIUM) {
   	// no medium present, print error and exit
     	printf("No medium detected, insert card and run again.\n");
      exit(rc);
   }
	else if (rc != -EUNFORMAT) {
   	// already formatted, print error and exit
     	printf("Device is already formatted, please erase it and run again.\n");
      exit(rc);
   }

   sectors = my_device.seccount - 1;
   for (i = 0, start = 1; i < 4 && sectors > 32; ++i) {
      megs = (float) sectors / 2048.0;
      printf("\nAvailable space for partition %d:%6.1fMB\n", i+1, megs);
      printf("Size in MB for partition %d: ", i+1);
      gets(buf);
      megs = strtod(buf, &ptr);
      if (megs > 0.01) {
         partsize = (unsigned long) (megs * 2048.0);
         if (partsize > sectors) partsize = sectors;
         my_device.part[i].starthead = 254;
         my_device.part[i].parttype = 6;
         my_device.part[i].endhead = 254;
         my_device.part[i].startsector = start;
         my_device.part[i].partsecsize = partsize;
         start += partsize;
         sectors -= partsize;
      }
      else break;
   }

   printf("\nReady to create new partitions (y/n)? ");
   gets(buf);
   if (buf[0] == 'y' || buf[0] == 'Y') {
      // Format the erased device with the new partition table
      rc = fat_FormatDevice(&my_device,0);

      if (rc < 0) {
        	printf("\nPartitioning failed with error code %d.\n", rc);
      }
      else {
         printf("\nPartitions created.\n");
      }
   }
   else {
      printf("\nPartitioning aborted.\n");
   }

   rc = fatwtc_flushall(WTC_WAIT | WTC_PURGE);
   exit(rc);
}


