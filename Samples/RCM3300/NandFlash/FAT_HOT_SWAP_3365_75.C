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
/**************************************************************
  FAT_HOT_SWAP_3365_75.c

  Requires the FAT filesystem module be installed.

  Also requires that you run this on a RCM3365 or RCM3375
  core module.

  Demonstrates hot swapping an XD card.

  Run program and hit any keyboard key while focus is on stdio
  window, or hit SW3 to signal a hot swap. Pull card and insert
  new or same one on prompt, or when green LED DS4 lights up.

  Don't pull card if not prompted or signaled by the LED!

  If hot swapping is to be performed, it must be done while the
  xD card is unmounted. This sample unmounts the device and waits
  for a new one when it detects a keyboard hit or SW3 push.

  Versions of the Dynamic C FAT prior to 2.10 did not use unique
  volume labels, so swapping two xD cards that were both formatted
  with  old FAT versions may cause cache recovery errors .

**************************************************************/

#memmap xmem

#define  GREEN    "\x1b[32m"    // Foreground colors for printf
#define  RED      "\x1b[31m"
#define  BLUE     "\x1b[34m"
#define  BLACK    "\x1b[30m"

#define  FILESIZE 256
#define  NFILES   30
#define  NTESTS   20

//**** Define these macros for library debugging info to display
//***  in the STDIO window
//#define FAT_VERBOSE
//#define FAT_DEBUG
#define FAT_HOTSWAP_VERBOSE

#define FAT_BLOCK          // Set FAT library to blocking mode

#use "fat.lib"
#use RCM33XX.LIB

#ifndef FAT_ALLOW_HOTSWAP
   	#error "Board type does not support hot swapping."
#endif

//*** Uncomment these for serial port E stdio output
#define	STDIO_DEBUG_SERIAL	SEDR
#define	STDIO_DEBUG_BAUD		57600
#define	STDIO_DEBUG_ADDCR

int main()
{
   FATfile my_file;
   static char fbuff[FILESIZE];
   auto int retries, rc, i, swapPending, j, nErrs, flags;
   auto char filename[13], buf[6];
   auto int xdcard, partition, ntests;

#ifndef FAT_ALLOW_HOTSWAP
   	#error "Board type does not support hot swapping."
#endif

   brdInit();
// boardInit() doesn't leave serial port E enabled
#ifdef STDIO_DEBUG_SERIAL
#if  (STDIO_DEBUG_SERIAL==SEDR)
   BitWrPortI(PGDR,&PGDRShadow,1,6);
   BitWrPortI(PGFR,&PGFRShadow,1,6);
   BitWrPortI(PGDDR,&PGDDRShadow,1,6);
   WrPortI(PGDCR,&PGDCRShadow,0);
   WrPortI(SECR,&SECRShadow,0);
#endif
#endif

   if(nf_XD_Detect(1)<0){
      printf("\n\n\n INSERT XD CARD");
   }

   swapPending = 2;
   ntests = nErrs = 0;

   while(ntests < NTESTS)
   {
      if(swapPending==2){

         // Busy wait while card not detected
         while(nf_XD_Detect(1)<0);

         // Mount XD card
         retries = 0;

         flags = FAT_XD_DEVICE | FDDF_MOUNT_DEV_0 | FDDF_MOUNT_PART_0 |
                 FDDF_COND_DEV_FORMAT | FDDF_COND_PART_FORMAT
                ;

tryAutomount:
         rc = fat_AutoMount(flags);

         if(rc)   // If failed to mount  XD card
         {
	          retries++;
	          printf("%sERROR: fat_AutoMount() returned  %d, Retrying\n%s",
	             RED, rc, BLACK);

	          if(retries==1)
             {
               flags |= FDDF_NO_RECOVERY;
               goto tryAutomount;
	          }
	          if(retries==2)
             {
               fatwtc_flushall(WTC_WAIT | WTC_PURGE);
               _fatwtc_init();
               goto tryAutomount;
	          }
	          else {
                nErrs++;
                break;
             }
         }
         else
         {
            printf("\n%sXD card mounted...\n", BLUE);
            printf("\nHit SW3 or any KB key when ready to swap cards\n%s", BLACK);
         	ledOut(4, 0);      // Turn on LED when mounted
            swapPending = 0;
         }
      }

      if(swapPending==1){

         //*** UnmountDevice will flush XD card with WTC_PURGE flag
         rc = fat_UnmountDevice(FAT_XD_PART->dev);
         if(rc)
         {  // Unmount failure
            printf("%sERROR: fat_UnmountDevice() returned %d.\n%s",
                     RED, rc, BLACK);
            nErrs++;
            continue;  // Abort while loop
         }
         else {
         	ledOut(4, 1);
            printf("\n%sCard unmounted, Switch XD cards now\n%s",
                    RED, BLACK);
         }

         // Busy wait while card detected
         while(nf_XD_Detect(1)>0);

         printf("\n%sCard Removed, put new or same card in%s\n\n",
                       RED, BLACK);
         swapPending = 2;   // Ready to auto-mount

         // Disconnect driver and device structures from tables.
         // This forces re-scanning of the device for a valid MBR
         // to work around a problem with swapping from a smaller
         // to a larger removable device.
         _fat_config_init();
      }
      else   // Test file operations
      {
         for(j=0; j< NFILES ; j++){ // use NFILES files per device

            // Key hit while focus on stdio window
            if(kbhit()){
               swapPending = 1;
               getchar();  // Clear key hit
               break;
            }
            else{
               //   SW3 hit signals hot swap too
               if (!switchIn(3)){
                  swapPending = 1;
                  break;
               }
            }

            strcpy(filename, "file");
            itoa(j, buf);
            strcat(filename,buf);
            strcat(filename,".txt");

            rc = fat_Open(FAT_XD_PART, filename,
                    FAT_FILE, FAT_CREATE, &my_file, NULL);
            if (rc){
               printf("%sERROR: fat_Open() returned result code %d.\n%s",
                       RED, rc, BLACK);
               nErrs++;
               break;  // Abort for loop
            }
              // WRITE TO THE FILE
            memset(fbuff, j, FILESIZE);
            if(rc=fat_Seek(&my_file, 0, SEEK_SET )){
               printf("%sERROR: fat_Seek() returned result code %d.\n%s",
                      RED, rc, BLACK);
               nErrs++;
               break;  // Abort for loop
            }
            rc = fat_Write(&my_file,fbuff,FILESIZE);
            if(rc<0){
               printf("%sERROR: fat_Write() %d \n%s",RED,rc,BLACK);
               nErrs++;
               break;  // Abort for loop
            }
            memset(fbuff,0,FILESIZE);

            if(rc=fat_Seek(&my_file, 0, SEEK_SET )){
               printf("%sERROR: fat_Seek() returned result code %d.\n%s",
                     RED, rc, BLACK);
               nErrs++;
               break;  // Abort for loop
            }

            // READ A FILE
            rc=fat_Read(&my_file,fbuff,FILESIZE);
            if(rc<0){
               printf("%sERROR: fat_Read() returned result code %d.\n%s",
                     RED, rc, BLACK);
               nErrs++;
               break;  // Abort for loop
            }

            for(i=0; i < FILESIZE; i++){
               if( fbuff[i] != j){
                  printf("%sERROR: fat_Read() bad value\n%s",RED,BLACK);
                  j = 0;
                  nErrs++;
                  break; // Abort for loop
               }
            }
            rc = fat_Close(&my_file);
            if(rc){
               printf("%sERROR: fat_Close() returned result code %d.\n%s",
                     RED, rc, BLACK);
               nErrs++;
               break;  // Abort for loop
            }
            rc = fat_Delete( FAT_XD_PART, FAT_FILE, filename);
            if(rc){
               printf("%sERROR: fat_Delete() returned result code %d.\n%s",
                     RED, rc, BLACK);
               nErrs++;
               break;  // Abort for loop
            }
         } // End for j

         ntests++;

         if(swapPending) continue; // Start next while loop iteration

         if(j !=NFILES){
            printf("%sFAT error \n%s", RED, BLACK);
         }
      } // End else
   }  // End while

   if(!nErrs){
      printf("\n%s SUCCESS \n%s", BLUE, BLACK);
   }
   else {
      printf("\n%s %d FAT Errors occurred \n%s", RED, nErrs, BLACK);
   }

   // Unmount and loop so we don't wear out flash if program left
   //  running in run mode
   printf("\n%sUnmounting the XD Card, please wait.\n%s", RED, BLACK);
   fat_UnmountDevice(FAT_XD_PART->dev);
   printf("\n%sXD Card unmounted, press any key to exit.\n%s", RED, BLACK);
   while(1) if(kbhit()) break;
}


