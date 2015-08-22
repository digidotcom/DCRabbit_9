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
/***********************************************************************
	FAT_SHELL.C

   Requires that you run this on a board with a compatible
   storage medium (serial flash, NAND flash or SD card).

   Gives a UNIX like shell for the FAT file system.  Requires that the
   devices have a formatted FAT partition as partition 0 before this
   program is run.  The shell has the ability to reformat this partition
   once it is running to give a fast way to delete all files during
   experimentation.

************************************************************************/

#define MAX_DEVICES	2	// must be >= actual number of configured FAT devices
#define MAX_FILES		20

// Uncomment following macro definition(s) to turn on Debug options
//#define FAT_DEBUG
//#define FAT_VERBOSE
//#define FATWTC_DEBUG
//#define FATWTC_DEBUG_INIT
//#define NFLASH_DEBUG
//#define NFLASH_VERBOSE
//#define NFLASH_FAT_DEBUG
//#define NFLASH_FAT_VERBOSE
//#define SFLASH_DEBUG
//#define SDFLASH_DEBUG
//#define SDFAT_DEBUG
//#define SDFLASH_VERBOSE
//#define PART_DEBUG

// Map program to xmem if not compiling to separate I&D space or debugging.
#if __SEPARATE_INST_DATA__
#ifdef FAT_DEBUG
#memmap xmem
#endif
#else
#memmap xmem
#endif

// Set FAT library to blocking mode
#define FAT_BLOCK

// Set file system to use forward slash as directory separator
#define FAT_USE_FORWARDSLASH

// These are the nand flash driver and FAT filesystem libraries
#use "fat.lib"

// Required control structures to operate the FAT filesystem
FATfile  file;
FATfile  files[MAX_FILES];

char buf[128];
char path[300];
char mpath[300];
char pwd[MAX_DEVICES * FAT_MAX_PARTITIONS][257];
int active_part;

// Prints a help listing of valid shell commands
void help()
{
	printf("FAT_Shell commands:\n");
   printf("p:                      Set partition where p is partition id\n");
   printf("ls                      List current directory\n");
   printf("cd [dirname]            Change directory [root]\n");
   printf("pwd                     Print current directory\n");
   printf("touch filename [alc]    Create file [1 cluster alloc]\n");
	printf("mtouch n filename [alc] Create n files [1 cluster each]\n");
   printf("wr filename [bytes]     Write to file [1k]\n");
	printf("mwr n filename [bytes]  Write to n files [1k each]\n");
   printf("ap filename [bytes]     Append to file [1k]\n");
	printf("map n filename [bytes]  Append to n files [1k each]\n");
   printf("mkdir dirname           Create directory\n");
   printf("mmkdir n dirname        Create n directories\n");
   printf("rd filename [bytes]     Read from file [first 1k max]\n");
   printf("split filename newfile  Split excess allocation to newfile\n");
   printf("trunc filename [bytes]  Truncate file [length] (Free Prealloc.)\n");
   printf("del filename            Delete the file\n");
   printf("rmdir dirname           Remove the directory (must be empty)\n");
   printf("tail filename [bytes]   Read last bytes from file [last 1k max]\n");
   printf("pdump                   Print partition info\n");
   printf("fat [startx [endx]]     Print FAT table [2 [64]]\n");
   printf("stat filename           Print file/directory info\n");
   printf("format [p]              Erase partition or device a,b,...,0,...\n");
   printf("h[elp]                  Print this help message\n");
   printf("exit                    Exit this program\n\n");
}

// Dumps partition and device information to the Stdio window
void pdump()
{
	int i;
   FATfile * f;

	printf("sec_clust = %u\n", fat_part_mounted[active_part]->sec_clust);
	printf("fat_cnt = %u\n", fat_part_mounted[active_part]->fat_cnt);
	printf("fat_len = %u\n", fat_part_mounted[active_part]->fat_len);
	printf("root_cnt = %u\n", fat_part_mounted[active_part]->root_cnt);
	printf("res_sec = %u\n", fat_part_mounted[active_part]->res_sec);
	printf("byte_sec = %u\n", fat_part_mounted[active_part]->byte_sec);
	printf("sec_fat = %lu\n", fat_part_mounted[active_part]->sec_fat);
	printf("serialnumber = %lu\n", fat_part_mounted[active_part]->serialnumber);
	printf("totcluster = %lu\n", fat_part_mounted[active_part]->totcluster);
	printf("badcluster = %lu\n", fat_part_mounted[active_part]->badcluster);
	printf("freecluster = %lu\n", fat_part_mounted[active_part]->freecluster);
	printf("nextcluster = %lu\n\n", fat_part_mounted[active_part]->nextcluster);
	printf("fatstart = %lu\n", fat_part_mounted[active_part]->fatstart);
	printf("rootstart = %lu\n", fat_part_mounted[active_part]->rootstart);
	printf("datastart = %lu\n", fat_part_mounted[active_part]->datastart);
	printf("clustlen = %lu\n", fat_part_mounted[active_part]->clustlen);
	printf("type = %d\n", fat_part_mounted[active_part]->type);
	printf("wtc_prt = %d\n", fat_part_mounted[active_part]->wtc_prt);
	printf("pnum = %d\n\n", fat_part_mounted[active_part]->pnum);
//	printf("opstate = %d\n", fat_part_mounted[active_part]->opstate);
//	printf("clust1 = %u\n", fat_part_mounted[active_part]->clust1);
//	printf("clust2 = %u\n", fat_part_mounted[active_part]->clust2);
//	printf("linkclust = %u\n", fat_part_mounted[active_part]->linkclust);
   for (f = fat_part_mounted[active_part]->first, i = 0; f; f = f->next, ++i);
   printf("open files = %d\n", i);

   printf("\nmpart.bootflag = %02X\n",
          fat_part_mounted[active_part]->mpart->bootflag);
   printf("mpart.starthead = %u\n",
          fat_part_mounted[active_part]->mpart->starthead);
   printf("mpart.startseccyl = %u\n",
          fat_part_mounted[active_part]->mpart->startseccyl);
   printf("mpart.parttype = %u\n",
          fat_part_mounted[active_part]->mpart->parttype);
   printf("mpart.endhead = %u\n",
          fat_part_mounted[active_part]->mpart->endhead);
   printf("mpart.endseccyl = %u\n",
          fat_part_mounted[active_part]->mpart->endseccyl);
   printf("mpart.startsector = %lu\n",
          fat_part_mounted[active_part]->mpart->startsector);
   printf("mpart.partsecsize = %lu\n",
          fat_part_mounted[active_part]->mpart->partsecsize);
   printf("mpart.status = %04X\n\n",
          fat_part_mounted[active_part]->mpart->status);

   printf("dev.seccount = %lu\n",fat_part_mounted[active_part]->dev->seccount);
   printf("dev.heads = %u\n", fat_part_mounted[active_part]->dev->heads);
   printf("dev.cylinder = %u\n", fat_part_mounted[active_part]->dev->cylinder);
   printf("dev.sec_track = %u\n",
                              fat_part_mounted[active_part]->dev->sec_track);
   printf("dev.byte_sec = %d\n", fat_part_mounted[active_part]->dev->byte_sec);
   printf("dev.byte_page = %d\n",
                              fat_part_mounted[active_part]->dev->byte_page);
   printf("dev.drv_num = %d\n", fat_part_mounted[active_part]->dev->dev_num);
   printf("dev.wtc_dev = %d\n", fat_part_mounted[active_part]->dev->wtc_dev);

   printf("\n");
}

// Prints out actual FAT table cluster entries in the range specified.
// Leave in for Beta, should be removed for release
void fat_print(long startx, long endx)
{
	long cluster;
   unsigned long nclust;
   int rc;

   for (cluster = startx; cluster <= endx; ++cluster) {
   	printf("%u (0x%04X)\t", (word)cluster, (word)cluster);
   	nclust = cluster;
		rc = _fat_next_clust(fat_part_mounted[active_part], &nclust, WTC_WAIT);
      if (rc == -EEOF)
      	printf("<EOC>\n");
      else if (rc == -ENODATA)
      	printf("<free>\n");
      else if (rc < 0)
      	printf("<BAD: %d>\n", rc);
      else
      	printf("%u (0x%04X)\n", (word)nclust, (word)nclust);
   }

}

// Prints out attributes of a file or directory.
int stat(char * filename)
{
   fat_dirent dent;
   fat_location loc;
	int rc;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   // Call fat_Status to fill in directory entry structure
   rc = fat_Status( fat_part_mounted[active_part], path, &dent );
   if (rc) {
   	if (rc == -ENOENT) {
      	printf("'%s' does not exist\n", path);
         return 0;
      }
   	printf("Status '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   printf("%s '%s' attributes are %c%c%c%c%c%c \n",
         	dent.attr & 0x10 ? "Directory" : "File",
            path,
            dent.attr & FATATTR_READ_ONLY ? 'R' : 'r',
            dent.attr & FATATTR_HIDDEN ? 'H' : 'h',
            dent.attr & FATATTR_SYSTEM ? 'S' : 's',
            dent.attr & FATATTR_VOLUME_ID ? 'V' : 'v',
            dent.attr & FATATTR_DIRECTORY ? 'D' : 'd',
            dent.attr & FATATTR_ARCHIVE ? 'A' : 'a');
   return 0;
}

// Touch create an empty but pre-allocated file of specified size.
int touch(char * filename, long alloc, int type)
{
	int rc;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   // Call fat_Open with MUST_CREATE mode and allocation size
   rc = fat_Open(fat_part_mounted[active_part], path, type, FAT_MUST_CREATE,
                 &file, &alloc);
   if (rc) {
   	if (rc == -EEXIST) {
      	printf("'%s' already exists\n", path);
         return 0;
      }
   	printf("Open '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   printf("%s '%s' created with %ld bytes\n",
		   				type == FAT_FILE ? "File" : "Directory", path, alloc);
	return fat_Close(&file);
}

// Deletes given file or directory
int del(char * filename, int type)
{
	int rc;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   rc = fat_Delete(fat_part_mounted[active_part], type, path);
   if (rc) {
   	if (rc == -EPERM) {
      	printf("'%s' is read-only, hidden etc. - NOT deleted\n", path);
         return 0;
      }
   	printf("Delete '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   printf("%s deleted.\n", path);
}

// Splits file 'filename' and re-assigns any unused pre-allocated clusters
// to newly created 'newfile'
int split(char * filename, char * newfile)
{
	int rc;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   // Open file to get a file handle
   rc = fat_Open(fat_part_mounted[active_part], path, FAT_FILE,0, &file, NULL);
   if (rc) {
   	if (rc == -ENOENT) {
      	printf("'%s' does not exist\n", path);
         return 0;
      }
   	printf("Open '%s' '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   // Put together working directory and newfile
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, newfile);
   // Call fat_Split with file handle, split position and new file's path/name
   rc = fat_Split(&file, file.de.fileSize, path);
   if (rc) {
   	fat_Close(&file);
   	if (rc == -EPERM) {
      	printf("'%s' is read-only, hidden etc. - NOT Split\n", filename);
         return 0;
      }
   	if (rc == -EEOF) {
      	printf("'%s' has no extra clusters - NOT Split\n", filename);
         return 0;
      }
   	printf("Split '%s' Error: %ls\n", filename, error_message(rc));
      return rc;
   }
   printf("%s split.\n", filename);
	return fat_Close(&file);
}

// Truncates the given file at position 'bytes' and frees any unused clusters.
int trunc(char * filename, long bytes)
{
	int rc;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   // Open file to get a file handle
   rc = fat_Open(fat_part_mounted[active_part], path, FAT_FILE,0, &file, NULL);
   if (rc) {
   	if (rc == -ENOENT) {
      	printf("'%s' does not exist\n", path);
         return 0;
      }
   	printf("Open '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   // Call fat_Truncate with file handle and desired size
   rc = fat_Truncate(&file, bytes);
   if (rc) {
      fat_Close(&file);
   	if (rc == -EPERM) {
      	printf("'%s' is read-only, hidden etc. - NOT Truncated\n", path);
         return 0;
      }
   	printf("Truncate '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   printf("%s truncated to %ld bytes.\n", path, file.de.fileSize);
	return fat_Close(&file);
}

// Writes or appends a file with 'bytes' characters from a repetetive fill
// string.
int wr(char * filename, long bytes, int append)
{
	auto char fox[128];
	int rc;
   long writ;
   long len;
   int ltw;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   len = 0;
   // Open file to get a file handle
   rc = fat_Open(fat_part_mounted[active_part], path, FAT_FILE, FAT_CREATE,
                 &file, &len);
   if (rc) {
   	printf("Open '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   // If appending, seek end of file before writing.
   if (append) {
   	rc = fat_Seek(&file, 0, SEEK_END);
      if (rc) {
	      printf("Seek '%s' Error: %ls\n", path, error_message(rc));
	      fat_Close(&file);
	      return rc;
      }
   }
   writ = 0;
   // Create the fill string by inserting filename and bytes
   sprintf(fox, "%s %ld The quick brown fox jumps over the lazy dog\n",
   					filename, bytes);
   len = strlen(fox);
   rc = 0;
   // Keep writing string to file until 'bytes' characters have been written
   while (writ < bytes) {
   	if (len < (bytes-writ))
      	ltw = (int)len;
      else {
			ltw = (int)(bytes-writ);
         fox[ltw-1] = '\n';
      }
      rc = fat_Write(&file, fox, ltw);
      if (rc > 0)
      	writ += rc;
      if (rc < ltw)
      	break;
   }
   if (rc < 0)
   	printf("Write Error: %ls\n", error_message(rc));
   printf("File '%s' %s with %ld bytes out of %ld\n", path,
			   						append ? "appended" : "written", writ, bytes);
	return fat_Close(&file);
}

// Read 'bytes' characters from either the beginning or end of file 'filename'
int rd(char * filename, long bytes, int tail)
{
	auto char b[81];
   char *ptr;
	int rc;
   int len;
   long red;
   int ltr;

   // Put together working directory and filename
   strcpy(path, pwd[active_part]);
   strcat(path, FAT_SLASH_STR);
   strcat(path, filename);
   // Open file to get a file handle
   rc = fat_Open(fat_part_mounted[active_part], path, FAT_FILE,0, &file, NULL);
   if (rc) {
   	printf("Open '%s' Error: %ls\n", path, error_message(rc));
      return rc;
   }
   // If reading from end, seek (end - bytes) before reading.
   if (tail) {
   	rc = fat_Seek(&file, -bytes, SEEK_END);
      if (rc) {
	      printf("Seek '%s' Error: %ls\n", path, error_message(rc));
	      fat_Close(&file);
	      return rc;
      }
   }
   red = 0;
   rc = 0;
   b[80] = 0;
   // Read in 80 characters at a time until 'bytes' characters read.
   while (red < bytes) {
   	if ((sizeof(b) - 1) < (bytes-red))
      	ltr = sizeof(b) - 1;
      else
			ltr = (int)(bytes-red);
      rc = fat_Read(&file, b, ltr);
      if (rc < 0)
      	break;
      b[rc] = 0;
  	   for (ptr = b; ptr < (b + rc); ptr += strlen(ptr) + 1)
     	   printf("%s", ptr);
     	red += rc;
   }
  	if (rc == -EEOF) {
      if (!red)
	     	printf("'%s' has no data.\n", filename);
   }
   else
	   if (rc < 0)
   		printf("Read Error: %ls\n", error_message(rc));
   printf("\nRead %ld bytes out of %ld\n", red, bytes);
	return fat_Close(&file);
}

// Print out directory listing of current working directory
int ls()
{
	int rc, nclust;
   fat_dirent dent;
   char fname[13];
   int del, lent;
   unsigned long clust;

   // Open directory to get a file handle
	rc = fat_Open(fat_part_mounted[active_part], pwd[active_part], FAT_DIR, 0,
	              &file, NULL);
   if (rc) {
      printf("Open Directory '%s' Error: %ls\n",
                           pwd[active_part], error_message(rc));
      return rc;
   }
   printf("Listing '%s' (dir length %lu)\n",pwd[active_part],file.de.fileSize);

   del = lent = 0;
	for (;;) {
   	// Use fat_ReadDir to read directory entries into structure 'dent'
   	rc = fat_ReadDir(&file, &dent, FAT_INC_DEF + FAT_INC_DEL );
      if (rc)
      	break;
		if (!dent.name[0])
      	break;
      else if (dent.name[0] == 0xE5)
      	++del;
      else if ((dent.attr & FATATTR_LONG_NAME) == FATATTR_LONG_NAME)
      	++lent;
      else {
      	if (del) {
         	printf("<%d deleted entries>\n", del);
            del = 0;
         }
      	if (lent) {
         	printf("<%d long-name entries>\n", lent);
            lent = 0;
         }
			// Looks OK
         _fat_Dir2Clust((char *)&dent, &clust);
         fat_GetName(&dent, fname, FAT_LOWERCASE);
         printf("%12.12s %c%c%c%c%c%c len=%lu\tclust=%lu",
         	fname,
            dent.attr & FATATTR_READ_ONLY ? 'R' : 'r',
            dent.attr & FATATTR_HIDDEN ? 'H' : 'h',
            dent.attr & FATATTR_SYSTEM ? 'S' : 's',
            dent.attr & FATATTR_VOLUME_ID ? 'V' : 'v',
            dent.attr & FATATTR_DIRECTORY ? 'D' : 'd',
            dent.attr & FATATTR_ARCHIVE ? 'A' : 'a',
            dent.fileSize,
            clust);
         for (nclust = 1;;++nclust) {
				rc = _fat_next_clust(file.part, &clust, WTC_WAIT);
            if (rc == -EEOF)
            	break;
            if (rc < 0) {
            	printf(" <%d!>", rc);
               break;
            }
            if (nclust == 6) {
            	printf(" ...");
               break;
            }
            printf(" %lu", clust);
         }
         printf("\n");

      }
   }
   if (del)
      printf("<%d deleted entries>\n", del);
   if (lent)
      printf("<%d long-name entries>\n", lent);
   fat_Close(&file);
   return rc;
}

// format mounted partition or device w/ one partition
int format(char option, word base_flags)
{
	int i, rc, selected_dev, selected_part;
	word flags_to_use;

	// initialize impossible device and partition numbers
	selected_dev = selected_part = -1;

   // check for option, as partition letter or device number
	if (!option) {
		selected_part = active_part;
	}
	else if ((selected_part = tolower(option) - 'a', 0 <= selected_part) &&
	         selected_part < num_fat_devices * FAT_MAX_PARTITIONS)
	{
		if (fat_part_mounted[selected_part]) {
			// set up format of a previously mounted partition
			option = 0;
		} else {
			// set up unconditional format of unmounted partition
			flags_to_use = FDDF_UNCOND_PART_FORMAT | FDDF_COND_PART_FORMAT;
			if (0 <= selected_part && FAT_MAX_PARTITIONS > selected_part) {
				flags_to_use |= FDDF_MOUNT_DEV_0;
				flags_to_use |= FDDF_MOUNT_PART_0 << selected_part;
			}
			else if (FAT_MAX_PARTITIONS <= selected_part &&
			         FAT_MAX_PARTITIONS * 2 > selected_part)
			{
				flags_to_use |= FDDF_MOUNT_DEV_1;
				flags_to_use |= FDDF_MOUNT_PART_0 <<
				                (selected_part - FAT_MAX_PARTITIONS);
			}
		}
		// Reset the working directory to root
		strcpy(pwd[selected_part], "");
	}
	else if ((selected_dev = tolower(option) - '0', 0 <= selected_dev) &&
	         selected_dev < num_fat_devices)
	{
		// set up unconditional format of device and its partition 0
		flags_to_use =  FDDF_UNCOND_DEV_FORMAT | FDDF_COND_DEV_FORMAT |
		                FDDF_UNCOND_PART_FORMAT | FDDF_COND_PART_FORMAT |
		                FDDF_MOUNT_PART_0;
		if (0 == selected_dev) {
			flags_to_use |= FDDF_MOUNT_DEV_0;
		}
		else if (1 == selected_dev) {
			flags_to_use |= FDDF_MOUNT_DEV_1;
		}
		for (i = 0; i < FAT_MAX_PARTITIONS; ++i) {
			// Reset the working directories to root
			strcpy(pwd[selected_dev - selected_dev % FAT_MAX_PARTITIONS + i], "");
		}
	} else {
		// out of range format option, report error!
		printf("Error, device or partition '%c' is not available to format.\n",
		       option);
		return -EINVAL;
	}

	if (!option) {
		// format the selected mounted partition
		printf("Unmounting the Partition . . . \n");
		rc = fat_UnmountPartition(fat_part_mounted[selected_part]);
		if (rc) {
			printf("Error: %ls\n", error_message(rc));
		}
		if (!rc) {
			printf("Formatting the Partition . . . \n");
			rc = fat_FormatPartition(
                &(_fat_device_table[selected_part/FAT_MAX_PARTITIONS]),
		           fat_part_mounted[selected_part], selected_part & 3,
                   6, NULL, NULL);
			if (rc) {
				printf("Error: %ls\n", error_message(rc));
			}
		}
		if (!rc) {
			printf("Mounting the Partition . . . \n");
			rc = fat_MountPartition(fat_part_mounted[selected_part]);
			if (rc) {
				printf("Error: %ls\n", error_message(rc));
			}
		}
		strcpy(pwd[selected_part], "");	// Reset the working directory to root
	} else {
		// format the selected device and its partition
		rc = fat_AutoMount(flags_to_use);
		if (rc) {
			printf("Format %c Error: %ls\n", option, error_message(rc));
		} else {
			printf("format %c succeeded\n", option);
		}
      // Turn off any formatting flags & turn on all mount partition flags
      flags_to_use &= FDDF_MOUNT_DEV_ALL;
		flags_to_use |= FDDF_MOUNT_PART_ALL;
      // Remount any available FAT partitions on given device
      rc = fat_AutoMount(flags_to_use);
	}

	printf("\n");
	// Scan the populated mounted partitions list to find all mounted partitions
	//  and if necessary, select the first mounted partition.
	for (i = 0; i < num_fat_devices * FAT_MAX_PARTITIONS; ++i) {
		if (fat_part_mounted[i] != NULL) {
			printf("Partition %c is mounted.\n", i + 'A');
			if (!fat_part_mounted[active_part]) {
				active_part = i;
			}
		}
	}
	printf("\n");

   return rc;
}

// Main program loop, does command parsing
int main()
{
	int i, rc;
	long prealloc;
	char * p, * q;
	char * cmd;
	int len;
	int type;
	long count, c, sx, ex;
	int max_part, temp_part;
	word flags_to_use;

#ifdef FATWTC_DEBUG_INIT
	_fatwtc_init();
#endif
	// ensure that we have sufficient pwd[] entries (if not, increase the value
	//  defined for the MAX_DEVICES macro at the top of this sample)
	assert(MAX_DEVICES >= num_fat_devices);

	// set the maximum possible partition number allowed
	max_part = num_fat_devices * FAT_MAX_PARTITIONS - 1;

	// initialize all possible partitions' pwd[] to empty string
	for (i = 0; i <= max_part; ++i) {
		strcpy(pwd[i], "");
	}

	// Auto-mount the FAT filesystem
	rc = fat_AutoMount(FDDF_USE_DEFAULT);
   if (rc == -EIO || rc == -ENOMEDIUM) {
		printf("Fatal device initialization error!  Exiting now.\n");
		exit(rc);
   }

	// Scan the populated mounted partitions list to find the first mounted
	// partition for the initial active selection.  The number of configured FAT
	// devices, as well as the mounted partition list, are provided for us in
	// FAT_CONFIG.LIB.
	for (active_part = 0; active_part < num_fat_devices * FAT_MAX_PARTITIONS;
	     ++active_part)
	{
		if (fat_part_mounted[active_part] != NULL) {
			// found a mounted partition, so use it
			break;
		}
	}

	// Check if a mounted partition was found
	if (active_part >= num_fat_devices * FAT_MAX_PARTITIONS) {
		printf("No partition(s) mounted by fat_AutoMount!\n");
		printf("Format partition 0 on available FAT devices? (y/N)  ");
		gets(buf);
		printf("\n");
		if (tolower(buf[0]) == 'y') {
			// Assumes that we have at most six devices: 0,1,2,3...
			//  And, we're formatting only partition 0 on each device: a,e,i,m,...
			for (i = 0; i < num_fat_devices; ++i) {
            rc = format('0' + i, 0);  // Format device with single partition
            // For backwards compatiblity, pre-2.13 FAT versions would not
            if (rc) { // auto-format partitions, instead would give -EUNFORMAT
              rc = format('A' + (i * FAT_MAX_PARTITIONS), 0);
              if (rc) {
                printf("Format Device Error: %ls.\nExiting now.\n",
                           error_message(rc));
                exit(rc);
              }
            }
			}
		} else {
			// No mounted partition(s) found and format all option refused,
			//  ensure rc is set to a FAT error code.
			rc = (rc < 0) ? rc : -ENOPART;
         printf("fat_AutoMount Error: %ls.\nExiting now.\n", error_message(rc));
			exit(rc);
		}
	} else {
		// It is possible that a non-fatal error was encountered and reported,
		// even though fat_AutoMount() succeeded in mounting at least one
		// FAT partition.
		printf("fat_AutoMount succeeded with return code %d.\n\n", rc);
	}

   help();

	// Scan the populated mounted partitions list to find all mounted partitions.
	for (i = 0; i < num_fat_devices * FAT_MAX_PARTITIONS; ++i)
	{
		if (fat_part_mounted[i] != NULL) {
			printf("Partition %c is mounted.\n", i + 'A');
			// select a new active partition, if necessary
			if (active_part >= num_fat_devices) {
				active_part = i;
			}
		}
	}
	printf("\n");

   // Command Parsing Loop - Calls routines above with parsed parameters.
   for (;;) {
   	if (fat_part_mounted[active_part]->first)
      	printf("--- warning: part[%d].first not null, it's 0x%04X\n",
      	       active_part, fat_part_mounted[active_part]->first);
   	printf("%c> ", active_part + 'A' );
      gets(buf);
      p = buf;
      while (isspace(*p)) ++p;
      cmd = p;
      while (isalnum(*p)) ++p;
      q = NULL;
      if (isspace(*p)) {
      	*p = 0;
         q = ++p;
         while (isspace(*q)) ++q;
         if (!*q)
         	q = NULL;
         else
         {
	         p = q + strlen(q) - 1;
   	      while (isspace(*p)) --p;
      	   ++p;
         	*p = 0;	// Zap trailing space
         }
      }
      else {
	      temp_part = tolower(cmd[0]) - 'a';
			if ((strlen(cmd) == 2) && (cmd[1] == ':') && (temp_part >= 0)
      	          && (temp_part <= max_part))
			{
				if (fat_part_mounted[temp_part]) {
					active_part = temp_part;
				}
            else {
					printf("? partition %c is not mounted\n", temp_part + 'A');
				}
            continue;
			}
         else if (*p) {
	      	printf("? unrecognized command\n");
   	      continue;
      	}
     	}
      if (!*cmd)
      	continue;

      // cmd is command string, q is parameter (or NULL).

		if (!strcmpi(cmd, "ls")) {
      	ls();
      }
      else if (!strcmpi(cmd, "cd")) {
      	if (!q)
         	strcpy(pwd[active_part], "");
         else if (*q != '.' || *(q+1) == '.')
         {
          	if (*q == FAT_SLASH_CH)
         		strcpy(pwd[active_part], q+1);
	         else if (*q == '.' || *(q+1) == '.') {
					q = strrchr(pwd[active_part], FAT_SLASH_CH);
      	      if (q)
         	   	*q = 0;
            	else
            		printf("No previous dir level\n");
	         }
				else {
      	   	len = strlen(q);
         	   if (strlen(pwd[active_part]) + len + 1 < sizeof(pwd[0])) {
            		strcat(pwd[active_part], FAT_SLASH_STR);
               	strcat(pwd[active_part], q);
	            }
   	         else
      	      	printf("Total dir name too long\n");
         	}
         }
         printf("PWD = '%s'\n", pwd[active_part]);
      }
      else if (!strcmpi(cmd, "pwd")) {
         printf("%s\n", pwd[active_part]);
      }
      else if (!strcmpi(cmd, "touch")) {
      	type = FAT_FILE;
         count = 1;
      __touch:
      	// parms "filename [alc]"
         if (!q || !*q) {
         	printf("? touch/mkdir expects filename [alc]\n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         prealloc = 0;
         if (*p) {
         	prealloc = strtol(p, NULL, 0);
            *p = 0;
         }
         if (count > 1) {
         	p = strchr(q, '.');

         	for (c = 1; c <= count; ++c) {
					if (p) {
               	memcpy(mpath, q, p-q);
                  sprintf(mpath + (p-q), "%ld", c);
                  strcat(mpath, p);
               }
               else {
	               strcpy(mpath, q);
	               sprintf(mpath+strlen(mpath), "%ld", c);
               }
               touch(mpath, prealloc, type);
            }
         }
         else
         	touch(q, prealloc, type);
      }
      else if (!strcmpi(cmd, "mtouch")) {
      	count = strtol(q, &q, 0);
			if (count <= 0) {
         	printf("? mtouch expects repetition count >= 1\n");
            continue;
         }
			while (isspace(*q)) ++q;
         type = FAT_FILE;
         goto __touch;
      }
      else if (!strcmpi(cmd, "wr")) {
         count = 1;
         type = 0;
      __wr:
      	// parms "filename [bytes]"
         if (!q || !*q) {
         	printf("? wr/ap expects filename [bytes]\n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         prealloc = 1024;
         if (*p) {
         	prealloc = strtol(p, NULL, 0);
            *p = 0;
         }
         if (count > 1) {
         	p = strchr(q, '.');

         	for (c = 1; c <= count; ++c) {
					if (p) {
               	memcpy(mpath, q, p-q);
                  sprintf(mpath + (p-q), "%ld", c);
                  strcat(mpath, p);
               }
               else {
	               strcpy(mpath, q);
	               sprintf(mpath+strlen(mpath), "%ld", c);
               }
               wr(mpath, prealloc, type);
            }
         }
         else
         	wr(q, prealloc, type);
      }
      else if (!strcmpi(cmd, "mwr")) {
         type = 0;
      __mwr:
      	count = strtol(q, &q, 0);
			if (count <= 0) {
         	printf("? mwr expects repetition count >= 1\n");
            continue;
         }
			while (isspace(*q)) ++q;
         goto __wr;
      }
      else if (!strcmpi(cmd, "ap")) {
         count = 1;
         type = 1;
         goto __wr;
      }
      else if (!strcmpi(cmd, "map")) {
         type = 1;
         goto __mwr;
      }
      else if (!strcmpi(cmd, "mkdir")) {
      	type = FAT_DIR;
         count = 1;
         goto __touch;
      }
      else if (!strcmpi(cmd, "mmkdir")) {
      	count = strtol(q, &q, 0);
			if (count <= 0) {
         	printf("? mmkdir expects repetition count >= 1\n");
            continue;
         }
			while (isspace(*q)) ++q;
         type = FAT_DIR;
         goto __touch;
      }
      else if (!strcmpi(cmd, "rd")) {
         count = 1;
         type = 0;
      __rd:
      	// parms "filename [bytes]"
         if (!q || !*q) {
         	printf("? rd expects filename [bytes]\n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         prealloc = 1024;
         if (*p) {
         	prealloc = strtol(p, NULL, 0);
            *p = 0;
         }
         if (count > 1) {
         	p = strchr(q, '.');

         	for (c = 1; c <= count; ++c) {
					if (p) {
               	memcpy(mpath, q, p-q);
                  sprintf(mpath + (p-q), "%ld", c);
                  strcat(mpath, p);
               }
               else {
	               strcpy(mpath, q);
	               sprintf(mpath+strlen(mpath), "%ld", c);
               }
               rd(mpath, prealloc, type);
            }
         }
         else
         	rd(q, prealloc, type);
      }
      else if (!strcmpi(cmd, "del")) {
         count = 1;
         type = FAT_FILE;
         if (!q || !*q) {
         	printf("? del expects filename \n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         *p = 0;
         del(q, type);
      }
      else if (!strcmpi(cmd, "rmdir")) {
         count = 1;
         type = FAT_DIR;
         if (!q || !*q) {
         	printf("? rmdir expects directory name \n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         *p = 0;
         del(q, type);
      }
      else if (!strcmpi(cmd, "split")) {
      	// parms "filename newfile"
         if (!q || !*q) {
         	printf("? split expects filename newfile\n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         if (*p) {
	         *p++ = 0;
   	      while (*p && isspace(*p)) ++p;
         }
         if (!*p) {
         	printf("? split expects filename newfile\n");
            continue;
         }
        	split(q, p);
      }
      else if (!strcmpi(cmd, "trunc")) {
      	// parms "filename newfile"
         if (!q || !*q) {
         	printf("? trunc expects filename [bytes]\n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         c = FAT_BRK_END;
         if (*p) {
            *p++ = 0;
         	while (isspace(*p)) ++p;
         	c = strtol(p, NULL, 0);
            if (!c && *p != '0')
            	c = FAT_BRK_END;
         }
        	trunc(q, c);
      }
      else if (!strcmpi(cmd, "tail")) {
         count = 1;
         type = 1;
         goto __rd;
      }
      else if (!strcmpi(cmd, "pdump")) {
      	pdump();
      }
      else if (!strcmpi(cmd, "fat")) {
			sx = 2;
         ex = 64;
         if (q) {
         	sx = strtol(q, &q, 0);
            while (isspace(*q)) ++q;
            if (*q)
            	ex = strtol(q, &q, 0);
         }
      	fat_print(sx, ex);
      }
      else if (!strcmpi(cmd, "stat")) {
         if (!q || !*q) {
         	printf("? stat expects filename \n");
            continue;
         }
         p = q;
         while (*p && !isspace(*p)) ++p;
         *p = 0;
         stat(q);
      }
      else if (!strcmpi(cmd, "format")) {
      	// format partition or device with no additional override flags
      	if (q) {
	      	format(*q, 0);
      	} else {
	      	format(0, 0);
      	}
      }
      else if ((!strcmpi(cmd, "h")) || (!strcmpi(cmd, "help"))){
      	help();
      }
      else if (!strcmpi(cmd, "exit")) {
      	// must unmount all of the mounted FAT partitions & devices
      	for (i = 0; i < num_fat_devices * FAT_MAX_PARTITIONS;
         											 i += FAT_MAX_PARTITIONS) {
	      	if (fat_part_mounted[i]) {
               rc = fat_UnmountDevice(fat_part_mounted[i]->dev);
               if (rc < 0)
                  printf("Unmount Error on %c: %ls\n", 'A' + i,
                                 error_message(rc));
	      	}
	      }
      	break;
      }
      else
      	printf("? unrecognized command\n");
   }

   return 0;
}