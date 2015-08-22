/***********************************************************
	Applications\RemoteFirmwareUpdate\dlm_web.c
	Z-World, 2004

   This file implements a DLM which is capable of receiving DLP images via
   HTTP upload and storing the uploaded images into serial flash.  The DLM can
   start any one of several DLPs that are stored in serial flash or the DLP
   stored in parallel flash via an easy to use web interface.
************************************************************/
#memmap xmem

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/

// Pick the predefined TCP/IP configuration for this sample.  See
// LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
// configuration.
#define TCPCONFIG 1

// Controls whether the DLP will be automatically started after a successful
// load.
#ifndef DLM_START_DLP_AFTER_LOAD
#define DLM_START_DLP_AFTER_LOAD		0
#endif

// If set to one, the DLM will reset the restart limits when a new DLP is
// uploaded
#ifndef DLM_NEW_DLP_RESETS_RESTARTLIMIT
#define DLM_NEW_DLP_RESETS_RESTARTLIMIT		1
#endif

// Enable the web interface for uploading image files to the download mananger.
#define DLM_USE_HTTP_UPLOAD

//////////////////////////////////
// End of configuration section //
//////////////////////////////////

// By default, debug information is turned off in this file.  To enable
// debugging, undefine the following macro.
//#define DLM_APP_DEBUG

//	Uncomment to turn on some printf() output.
//#define DLM_APP_VERBOSE
// Uncomment to enable printf strings longer than 127 bytes to stdio window
//#define STDIO_ENABLE_LONG_STRINGS
// Uncomment to redirect printf output to serial port A when in run mode
//#define	STDIO_DEBUG_SERIAL	SADR
//#define	STDIO_DEBUG_BAUD		57600
//#define	STDIO_DEBUG_ADDCR

#ifdef DLM_APP_DEBUG
	#define _dlm_app_nodebug
#else
	#define _dlm_app_nodebug nodebug
#endif

#define MAXSECTORSIZE   4096

#ifdef DLM_USE_HTTP_UPLOAD
	#define USE_HTTP_UPLOAD
	#define HTTP_MAXBUFFER				512
#endif

#define SSPEC_MAXSPEC 					15

#use "dcrtcp.lib"
#use "http.lib"
#use "remoteuploaddefs.lib"
#use "downloadmanager.lib"

/* -------------------------------------------------------------------- */

typedef struct
{
	unsigned				CRC;
	unsigned long		Size;
   unsigned long     ImageSize;
	unsigned long		ProgramStartAddr;
	unsigned long		lpStore;
	unsigned int		sectorOffset;
	char  				error;
	char  				nDataBytes;
   char					LoadToSerialFlash;
   char 					sectorBuff[MAXSECTORSIZE];
   int 					SectorSize;
	int					FirstSector;
}DLMDownloadState;

DLMDownloadState gDownloadState;


/* -------------------------------------------------------------------- */

// SSPEC index where information about image located (see IMAGE_FILENAME).
int 	gImageIndex;

// User ID of user who can download a new application image.
int 	gUpdaterID;

// Status messages from last download.  String always ends with CR-NL-NUL.
// As message changes, the ROOTFILE is resized.  There is no overflow
// checking.
#define  STATUS_FILENAME	"status"
char	gStatusFile[120];
int 	gStatusID;

int gStartDLPAfterLoad;
int gShutDownWhenIdle;
int gLoadImageFromParallelFlash;

/* -------------------------------------------------------------------- */

// A message was copied into the status file buffer.  Make sure it ends
// with a CR-NL-NUL byte trio.  Change size of "file" to match content size.
_dlm_app_nodebug
void status_update()
{
	gStatusFile[sizeof(gStatusFile) - 3] = '\0';
	strcat(gStatusFile, "\r\n");
	sspec_resizerootfile(gStatusID, strlen(gStatusFile));
}

// Put a new message into the "status" file.  A CR-NL-NUL will be
// appended and the "file size" adjusted.
_dlm_app_nodebug
void status_set(char* mesg)
{
	strncpy(gStatusFile, mesg, sizeof(gStatusFile) - 3);
	status_update();
}

_dlm_app_nodebug
void status_clear(void)
{
	status_set("idle");
}

// This is called during the upload process to change the status file to show
// the current progress of the image file being received.
_dlm_app_nodebug
void status_setprogress()
{
   auto char stat[64];
   sprintf(stat, "Receiving image (%ld of %ld bytes received)",
           gDownloadState.Size - sizeof(ldrImageInfo) - sizeof(long),
           gDownloadState.ImageSize - sizeof(ldrImageInfo));
   status_set(stat);
}

// This is called at the end of the upload process to show if the upload was
// successful or not
_dlm_app_nodebug
void status_setcrc(char* s, unsigned int transmitted, unsigned int calculated)
{
	auto char stat[128];
	sprintf(stat, "%s transmitted CRC (%04x) %s calculated CRC (%04x)", s,
         		  transmitted,
                 transmitted == calculated ? "matches" : "does not match",
                 calculated);
   status_set(stat);
}

/* -------------------------------------------------------------------- */

/**********************************************************************
void prepWriteFlash(void);

Description:
   This function sets the sector size, starting DLP destination address, and
   load to serial flash flag when a DLP upload is beginning.

Assumption:
   If serial flash is present on the board, the serial flash driver has already
   been initialized.

Input:
	None.

Return value:
	None.

**********************************************************************/
_dlm_app_nodebug
void prepWriteFlash(void)
{
   if(dlm_sflashpresent)
	{
		// If serial flash is present, set gDownloadState.SectorSize equal to the
      // page size of the serial flash, and set the starting address for image
      // storage to the next available DLP image slot on the serial flash.
   	gDownloadState.SectorSize = sf_blocksize > MAXSECTORSIZE ?
                             MAXSECTORSIZE : sf_blocksize;
		gDownloadState.lpStore = dlm_getnextimgaddress(RLDR_DLM_SIMAGES_TYPE_DLP);
      gDownloadState.LoadToSerialFlash = 1;
   }
   else
   {
   	// If serial flash is not present, set gDownloadState.SectorSize equal to
      // the sector size of the parallel flash, and set the starting address for
      // image storage to the starting address of the DLP image stored on
      // parallel flash (the address of the DLP stored in parallel flash is
      // passed from the loader to the download manager as a startup parameter).
      gDownloadState.SectorSize  = _FlashInfo.sectorSize > MAXSECTORSIZE ?
                                   MAXSECTORSIZE : _FlashInfo.sectorSize;
      gDownloadState.lpStore     = (DLMStartParms.dlp_parallel_addr -
                                    sizeof(unsigned long)) &
                                    ~(gDownloadState.SectorSize-1L);
      gDownloadState.LoadToSerialFlash = 0;
   }
}

/**********************************************************************
unsigned long flash2physaddr(unsigned long addr, char quadrant, char* MBxCR);

Description:
	This functions takes an address in a 512kb flash (0x00000 - 0x80000), along
   with the quadrant in which the flash is mapped, and returns the physical
   address to read from and the MBxCR register setting to make the read happen.
   For instance, to read from adress 0x43000 in a 512kb parallel flash that is
   mapped to quadrant 3, this function will return a physical address of
   0xc3000 and clear bit 4 in MBxCR so that address line A18 is not
   asserted for the access to the flash chip, thereby accessing the upper
   half of the chip.

Assumption:
	This function assumes that a 512kb parallel flash chip is being used, and
   that MBxCR is the current value of the memory bank control register for the
   bank in which the flash chip is mapped.
Input:
	addr     - the physical address (0-0xFFFFF) in the 512kb flash chip
   quadrant - the quadrant (0-3) in which the flash is mapped
   MBxCR    - the memory bank control register value for the bank in which the
              flash chip is mapped.

Return value:
	Physical address which can be passed to xmem2root or root2xmem after setting
   the memory bank control register for the quadrant.

**********************************************************************/
#define DLM_QUADRANT_SIZE	0x40000
_dlm_app_nodebug
unsigned long flash2physaddr(unsigned long addr, char quadrant, char* MBxCR)
{
	auto unsigned long retaddr;
   retaddr = 0;
   if(addr >= DLM_QUADRANT_SIZE)
   {
   	retaddr = addr + (quadrant * DLM_QUADRANT_SIZE) - DLM_QUADRANT_SIZE;
      *MBxCR &= ~0x10;
   }
   else
   {
   	retaddr = addr + (quadrant * DLM_QUADRANT_SIZE);
      *MBxCR |= 0x10;
   }
   return retaddr;
}

/**********************************************************************
void ReadParallelFlash(void* dest, unsigned long source, int length);

Description:
   This function reads length bytes into the buffer pointed to by dest
   from source address in parallel flash.

Assumptions:
   There is a 512kb parallel flash on CS0 and it is mapped to quadrant 3.

Input:
   dest   - pointer to a buffer to receive data read from the parallel flash
   source - the address within the parallel flash device from which to read
            data
	length - the length in bytes of data to read (buffer pointed to by dest must
   			be at least as large as length).

Return value:
	None

**********************************************************************/
_dlm_app_nodebug
void ReadParallelFlash(void* dest, unsigned long source, int length)
{
	auto unsigned long addr;
	auto char MB3CRState;

	// MB3CRState retains the value of MB3CRShadow as we may have to change
   // the memory bank control register depending on which half of the flash is
   // being read.
   MB3CRState = MB3CRShadow;
   addr = flash2physaddr(source, 3, &MB3CRShadow);
   #asm
       ld   a,(MB3CRShadow)
       ioi ld  (MB3CR),a
   #endasm
   xmem2root((void*)dest, addr, length);
 	MB3CRShadow = MB3CRState;
   #asm
       ld   a,(MB3CRShadow)
       ioi ld  (MB3CR),a
   #endasm
}

/**********************************************************************
int WriteParallelFlash(int length);

Description:
   This function writes a sector's worth of accumulated DLP image to the
   parallel flash.

Assumptions:
   gDownloadState.lpStore contains the correct destination address in
   parallel flash for the data stored in gDownloadState.sectorBuff.
   There is a 512kb parallel flash on CS0 and it is mapped to quadrant 3.

Input:
	length - the length of data in gDownloadState.sectorBuff to be written to
   parallel flash.

Return value:
	0   Write was successful
	< 0 Write to parallel flash failed in which case error code from parallel
       flash driver is returned.

**********************************************************************/
_dlm_app_nodebug
int WriteParallelFlash(int length)
{
	auto int 	retval;
   auto char   MB3CRState;
	auto char   _overwrite_block_flag_state;
   auto unsigned long physaddr;

   // Save the state of the flash driver flag used to check/bypass overwriting
   // of the System ID block.  This download manager is written specifically for
   // compile to flash, run in ram mode in which case the first half of parallel
   // flash on CS0 is mapped into the third quadrant.  The flash driver checks
   // incoming addresses to see if a write is going to overwrite the System ID
   // block (which normally exists in the second half of flash mapped into
   // the third quadrant) and disallows the write unless the
   // _overwrite_block_flag_state flag is set.  It should not be the case that
   // the System ID block will be overwritten if writing a DLP to parallel
   // flash.
   _overwrite_block_flag_state = _overwrite_block_flag;
   MB3CRState = MB3CRShadow;
   physaddr = flash2physaddr(gDownloadState.lpStore, 3, &MB3CRShadow);
	// set the flag if A18 is being inverted
   _overwrite_block_flag = (MB3CRShadow & 0x10);
   #asm
   		ld	a,(MB3CRShadow)
   ioi	ld	(MB3CR),a
   #endasm

   retval = WriteFlash(physaddr, gDownloadState.sectorBuff, length);
   if(gDownloadState.FirstSector)
   {
      // gDownloadState.ProgramStartAddr is the start of the program - the first
      // four bytes transferred are the length of the image not including the
      // ldrImageInfo struct that follows the image.
      gDownloadState.FirstSector = FALSE;
      gDownloadState.ProgramStartAddr = gDownloadState.lpStore;
      gDownloadState.ProgramStartAddr += sizeof(long);
   }

   gDownloadState.sectorOffset = 0;
   gDownloadState.lpStore += length;
   MB3CRShadow = MB3CRState;
   _overwrite_block_flag = _overwrite_block_flag_state;
   #asm
      	ld	a,(MB3CRShadow)
   ioi	ld	(MB3CR),a
   #endasm

	return retval;
}

/**********************************************************************
int WriteSerialFlash(int length);

Description:
   This function writes a sector's worth of accumulated DLP image to the
   serial flash.

Assumption:
   gDownloadState.lpStore contains the correct destination address in
   serial flash for the data stored in gDownloadState.sectorBuff.

Input:
	length - the length of data in gDownloadState.sectorBuff to be written to
   serial flash

Return value:
	0   Write was successful
	< 0 Write to serial flash failed in which case error code from serial
       flash driver is returned.

**********************************************************************/
_dlm_app_nodebug
int WriteSerialFlash(int length)
{
	auto int rc;
   auto unsigned long physaddr;

   // Write this portion of the DLP to serial flash.  Return any errors reported
   // by the serial flash driver.
   if(rc = rupl_serial_flash_write(gDownloadState.lpStore,
                                   gDownloadState.sectorBuff, length) < 0)
   {
   	return rc;
   }
   if(gDownloadState.FirstSector)
   {
      // gDownloadState.ProgramStartAddr is the start of the program - the first
      // four bytes transferred are the length of the image not including the
      // ldrImageInfo struct that follows the image.
      gDownloadState.FirstSector = FALSE;
      gDownloadState.ProgramStartAddr = gDownloadState.lpStore;
      gDownloadState.ProgramStartAddr += sizeof(long);
   }

   gDownloadState.sectorOffset = 0;
   gDownloadState.lpStore += length;

	return 0;
}

/**********************************************************************
int ProcessProgramByte(char ch);

Description:
	This function processes received DLP image bytes writes them to
   either the parallel or serial flash.  It waits until a full sector
   has been received before writing to the flash.

Assumption:
	The program will be received in one contiguous block and will be
   stored starting on a sector boundary.

Input:
	ch - the byte to be processed

Return value:
	0  everthing good so far.
 < 0  a write to flash failed in which case the return code from either
      the serial flash or parallel flash driver is returned.
 > 0  ran out of storage at current image index in serial flash

**********************************************************************/
_dlm_app_nodebug
int ProcessProgramByte(char ch)
{
	static char *sPtr;
	auto int 	rc;

	// If we're starting a sector, reset ptr into temp buffer.
	if(gDownloadState.sectorOffset == 0)
	{
		sPtr = gDownloadState.sectorBuff;
	}

	rc = 0;

   // The first four bytes passed in to this function will be the size of the
   // image being downloaded.  The image transfered will also include a struct
   // at the end of the image that is not included in the size sent in the first
   // four bytes.
	if(gDownloadState.Size == sizeof(long))
   {
		memcpy((void*)&gDownloadState.ImageSize, sPtr - sizeof(long),
             sizeof(long));
		prepWriteFlash();
	}

   // Only compute crc of actual executable image - skip the first four bytes
   // and the last six bytes.
   if(gDownloadState.Size >= sizeof(long) &&
      (gDownloadState.Size < (gDownloadState.ImageSize + sizeof(long) -
                              sizeof(ldrImageInfo))))
   {
		gDownloadState.CRC = getcrc(&ch, 1, gDownloadState.CRC);
   }

	// keep a count of program size
	gDownloadState.Size++;
	gDownloadState.sectorOffset++;

	// load byte to sector buffer
	*sPtr++ = ch;

  	// if we have a sector full of data, write it
	if(gDownloadState.sectorOffset == gDownloadState.SectorSize)
	{
		if(gDownloadState.Size >= (DLM_IMAGE_SIZE))
      	rc = 1;
      else
      {
         if(gDownloadState.LoadToSerialFlash)
            rc = WriteSerialFlash(gDownloadState.SectorSize);
         else
            rc = WriteParallelFlash(gDownloadState.SectorSize);
      }
	}
	return rc;
}

/**********************************************************************
int WriteLastSector(char* fname);

Description:
	Called after entire program received to see if a partial sector was
	received last.

Assumptions:
	If serial flash is present, the serial flash driver has been
   initialized.
   Only called after a program is received.

Input:
	fname - the name of the image just received.  This parameter may be
   NULL.

Return value:
	0  Last sector written successfully
 < 0  a write to flash failed in which case the return code from either
      the serial flash or parallel flash driver is returned.
   1  The CRC calculated while receiving the image did not match the
      transmitted CRC.

**********************************************************************/
_dlm_app_nodebug
int WriteLastSector(char* fname)
{
	auto int rc;
   auto unsigned long physaddr;
   auto ldrImageInfo imginfo;

	if(gDownloadState.sectorOffset != 0)
	{
	   if(gDownloadState.LoadToSerialFlash)
      {
         if(rc = WriteSerialFlash(gDownloadState.sectorOffset) < 0)
         	return rc;
      }
      else
      {
         if(rc = WriteParallelFlash(gDownloadState.sectorOffset) < 0)
         	return rc;
      }
   }

   dlm_setcurdlpinfo(gDownloadState.Size, fname);
	// set status message to tell whether the computed crc matches the
   // transmitted crc which is now stored in flash
   if(dlm_getcurdlpimginfo(&imginfo))
   {
      if(imginfo.crc != gDownloadState.CRC)
   	{
         dlm_markcurdlpnotrunnable();
			return 1;
   	}
   }
   else
   {
   	// The file uploaded did not have an information block at the end of it,
      // or the block was corrupted.
      dlm_markcurdlpnotrunnable();
      return 1;
   }

#if DLM_NEW_DLP_RESETS_RESTARTLIMIT > 0
	dlm_initrestarts();
#endif

	return 0;
}

/**********************************************************************
int CheckImageCRC(int type);

Description:
	Called after entire program received to compute the CRC for an
   image that is about to be run.

Assumptions:
	If serial flash is present, the serial flash driver has been
   initialized.
   Only called after a program is received.
   This function only works for an image stored in serial flash.

Input:
	Type parameter to tell this function where the image is to check.  If this
   parameter is 0, then the image is in serial flash.  If type is 1, then the
   image is in parallel flash.

Return value:
	0  CRC check failed - either the stored CRC did not match the computed CRC,
   	or	 image
   1  CRC check successful, image received and stored correctly and ready
      to be loaded for execution.

**********************************************************************/
_dlm_app_nodebug
int CheckImageCRC(int type)
{
   auto unsigned long dlplength;
   auto unsigned long dlpaddr;
   auto unsigned int  calcCRC;
   auto ldrImageInfo  imginfo;
   auto ldrRAMLoadInfo ldinfo;
   auto char MB2CRState;
   auto char MB3CRState;
   auto int retval;

   retval = 0;
   switch(type)
   {
   // Image is in serial flash
   case 0:
      // Final CRC check prior to starting loader to start DLP.  This checks
      // that the CRC stored at the end of the image matches the CRC of the
      // image.
      if(dlm_getcurdlpimgaddress(&dlpaddr, &dlplength))
      {
         // set physdatastart, physdataend, and imgdestination members all to
         // zero as rupl_valid_image is being used only to do a CRC check on the
         // image and is not actually loading code into RAM.
         memset((void*)&ldinfo, 0, sizeof(ldinfo));
         ldinfo.loadfromsflash = 1;
         ldinfo.sflashbuffer   = gDownloadState.sectorBuff;
         ldinfo.sfbufsize      = sf_blocksize;

         // dlpaddr and dlplength returned from dlm_getcurdlpimgaddress above,
         // give the starting address and length of the entire image, including
         // the length at the start of the image and the ldrImageInfo struct
         // stored at the end of the image.
         dlpaddr  += sizeof(long);
         dlplength = dlplength - sizeof(ldrImageInfo) - sizeof(long);

         // read the ldrImageInfo struct stored at the end of the DLP image
         memset((void*)&ldinfo, 0, sizeof(ldinfo));
         ldinfo.loadfromsflash = 1;
         ldinfo.sflashbuffer   = gDownloadState.sectorBuff;
         ldinfo.sfbufsize      = sf_blocksize;
         rupl_serial_flash_read((void*)&imginfo, dlpaddr + dlplength,
                                sizeof(imginfo));

         // Run a CRC check on the image - the check does not include the length
         // or the ldrImageInfo struct.
         if(!(retval=rupl_valid_image(dlpaddr, dlplength, 0, ldinfo, &calcCRC)))
         {
            status_setcrc("Could not verify DLP -", imginfo.crc, calcCRC);
            dlm_markcurdlpnotrunnable();
         }
      }
      else
      {
         status_set("Could not start DLP, no valid image found");
         dlm_markcurdlpnotrunnable();
         retval = 0;
      }
      break;
   case 1:
      // set physdatastart, physdataend, and imgdestination members all to zero
      // as rupl_valid_image is being used only to do a CRC check on the image
      // and is not actually loading code into RAM.
      memset((void*)&ldinfo, 0, sizeof(ldinfo));
      ldinfo.loadfromsflash = 0;
      ldinfo.sflashbuffer   = gDownloadState.sectorBuff;
      ldinfo.sfbufsize      = MAXSECTORSIZE;

      dlpaddr   = DLMStartParms.dlp_parallel_addr;
      dlplength = DLMStartParms.dlp_parallel_len;

      // read the ldrImageInfo struct stored at the end of the DLP image
      ReadParallelFlash((void*)&imginfo, dlpaddr + dlplength, sizeof(imginfo));

      // MB2CRState and MB3CRState retain the value of MB2CRShadow and
      // MB2CRShadow.  We map the full 512k flash into quadrants 2 and 3 so
      // that rupl_valid_image has full access to the DLP.  In run in fast ram
      // mode, only the upper portion of the 512k flash is mapped into the
      // 3rd quadrant.  Here, we unmap battery backed ram and map the full 512k
      // in.

      MB2CRState = MB2CRShadow;
      MB3CRState = MB3CRShadow;
      MB2CRShadow = FLASH_WSTATES | CS_FLASH;
      MB3CRShadow = FLASH_WSTATES | CS_FLASH;
      #asm
         ld    a,(MB2CRShadow)
         ioi   ld (MB2CR),a
         ld    a,(MB3CRShadow)
         ioi   ld (MB3CR),a
      #endasm

      // Run a CRC check on the image - the check does not include the length
      // or the ldrImageInfo struct.  dlpaddr is the address of the dlp in
      // parallel flash when mapped into quadrants 0 and 1.  Flash is now mapped
      // into 2 and 3, so we add 0x80000 to the dlp address.
      dlpaddr += 0x80000;
      if(!rupl_valid_image(dlpaddr, dlplength, 0, ldinfo, &calcCRC))
      {
         status_setcrc("Could not verify DLP -", imginfo.crc, calcCRC);
         retval = 0;
      }
      MB3CRShadow = MB3CRState;
      MB2CRShadow = MB2CRState;
      #asm
         ld    a,(MB2CRShadow)
         ioi   ld (MB2CR),a
         ld    a,(MB3CRShadow)
         ioi   ld (MB3CR),a
      #endasm
      retval = 1;
      break;
   }
	return retval;
}

/**********************************************************************
int ShutdownAndRunDLP();

Description:
	This function shuts down the download manager, and starts a series
   of steps which restart the loader to load the DLP into RAM.

Assumptions:
	CRC check has already been done and was successful.
	If serial flash is present, the serial flash driver has been
   initialized.
   Only called after a program is received.
   This function only works for an image stored in serial flash.

Input:
	None

Return value:
	0  No image to run
   Does not return otherwise

**********************************************************************/
_dlm_app_nodebug
int ShutdownAndRunDLP()
{
	auto unsigned time;
   auto unsigned long dlplength;
   auto unsigned long dlpaddr;

   // Close all sockets first.  Allow IP stack time to response for TCP close.
   for(time = (int)(SEC_TIMER + 3) ; time != (int)SEC_TIMER ; )
   {
      tcp_tick(NULL);
   }
   ifdown(IF_ETH0);

	if(gLoadImageFromParallelFlash)
	{
		dlpaddr   = DLMStartParms.dlp_parallel_addr;
      dlplength = DLMStartParms.dlp_parallel_len;
      dlplength &= ~IMAGE_IN_SFLASH_MASK;
      dlm_startdlp(dlpaddr, dlplength);
   }
   else
   {
      if(dlm_getcurdlpimgaddress(&dlpaddr, &dlplength))
      {
         // dlpaddr and dlplength returned from dlm_getcurdlpimgaddress above,
         // give the starting address and length of the entire image, including
         // the length at the start of the image and the ldrImageInfo struct
         // stored at the end of the image.
         dlpaddr   += sizeof(long);
         dlplength  = dlplength - sizeof(ldrImageInfo) - sizeof(long);
         // Bitwise or in the IMAGE_IN_SFLASH_MASK which sets bit 30 in the
         // length.  The loader sees this and knows to load the image from
         // serial flash.
         dlplength |= IMAGE_IN_SFLASH_MASK;
         dlm_startdlp(dlpaddr, dlplength);
      }
   }
   return 0;
}

/******************************************************************************/
/********************** SECTION: HTTP UPLOAD **********************************/
/******************************************************************************/

/**********************************************************************
int show_stored_images(HttpState* state);

Description:
   This function is called by the shtml handler in the HTTP server and displays
   a table row for each stored image in serial flash.  Each row output contains
   the name of the image (if available) along with time and date of upload and
   the total file length (including the image length stored at the start of the
   image as well as the ldrImgInfo struct stored at the end).

Assumptions:
	If serial flash is present, the serial flash driver has been
   initialized.
   Only called by the HTTP server, not explicitly called by the download
   manager.
   This function only works for an image stored in serial flash.

Input:
	Pointer to HttpState structure (supplied by HTTP server).

Return value:
	0  If function is not done and needs the shtml handler to call it again.
   1  Function has displayed all information it needs to display, or has
      encountered a problem.  Returning 1 signals the HTTP server to
      no longer call this function.

**********************************************************************/

// Each of the following xdata declarations are for the pieces of text sent to
// the client by this cgi.  Strings are stored in xmem to save root data usage,
// and moved into a root buffer via sprintf and the %ls format specifier.

xdata S_HTTP_PFLASHIMG_HDR
{
	"<tr>" \
      "<td colspan=\"2\" style=\"background-color:#009933;color:white\">" \
         "Parallel flash image" \
      "</td>" \
   "</tr>"
};

xdata S_HTTP_PFLASHIMG_1
{
	"<tr>" \
	   "<form action=\"startimage.cgi\" method=\"post\"" \
        "enctype=\"multipart/form-data\">" \
	      "<input type=hidden value=0 name=pfimageindex>" \
	      "<td style=\"background-color:#CCCCCC;color:black\"> " \
	         "Parallel flash image - "
};

xdata S_HTTP_PFLASHIMG_2
{
												" bytes" \
         "</td>" \
         "<td style=\"background-color:#CCCCCC;color:black\"> " \
            "<input type=submit value=Start>" \
         "</td>" \
      "</form>" \
   "</tr>"
};

xdata S_HTTP_SFLASHIMG_HDR_1
{
	"<tr>" \
      "<td colspan=\"2\" style=\"background-color:#009933;color:white\">" \
         "Serial flash images (Maximum images: "
};

xdata S_HTTP_SFLASHIMG_HDR_2
{
  ",  Maximum image size: "
};

xdata S_HTTP_SFLASHIMG_HDR_3
{
	                        " bytes)" \
      "</td>" \
   "</tr>"
};

xdata S_HTTP_SFLASHIMG_START_1
{
	"<tr>" \
      "<form action=\"startimage.cgi\" method=\"post\"" \
        "enctype=\"multipart/form-data\">" \
         "<input type=hidden value="
};

xdata S_HTTP_SFLASHIMG_START_2
{
	                                  " name=sfimageindex>" \
         "<td style=\"background-color:#CCCCCC;color:black\"> "
};

xdata S_HTTP_SFLASHIMG_START_3
{
         "</td>" \
         "<td style=\"background-color:#CCCCCC;color:black\"> " \
            "<input type=submit value=Start>" \
         "</td>" \
      "</form>" \
   "</tr>"
};

#define DLM_SHOW_SIMAGES_STATE_INIT			0
#define DLM_SHOW_SIMAGES_STATE_PFLASH		1
#define DLM_SHOW_SIMAGES_STATE_SFLASH		2
_dlm_app_nodebug
int show_stored_images(HttpState* state)
{
	static DLMStoredImages simages;
   static int i, sflashimgcount, sflashimgheader, pflashimgheader;
   auto char tmbuf[32];
	auto struct tm time;

	// done if serial flash is not present
   if(!dlm_sflashpresent)
   {
   	return 1;
   }

   switch(state->substate)
   {
   case DLM_SHOW_SIMAGES_STATE_INIT:
      // Done if stored image information cannot be retreived from serial flash
      if(!dlm_getstoredimages(&simages))
      {
         return 1;
      }
      // Start showing information from the current active dlp image
      i = simages.activedlp;
      // Set the number of images shown from serial flash to 0
      sflashimgcount  = 0;
      // This flag is set once the serial flash image header is sent to the
      // client
      sflashimgheader = 0;
      // This flag is set once the parallel flash image header is sent to the
      // client
      pflashimgheader = 0;
      // Set the state and fall through to the next case
      state->substate = DLM_SHOW_SIMAGES_STATE_PFLASH;

   case DLM_SHOW_SIMAGES_STATE_PFLASH:
      if(DLMStartParms.checksum ==
         rupl_dochecksum((void*)&DLMStartParms,
                         sizeof(DLMStartParms) -
                         sizeof(DLMStartParms.checksum)))
      {
         // If parallel flash image header has not been displayed yet, show
         // it and return
         if(!pflashimgheader)
         {
            sprintf(state->buffer, "%ls", S_HTTP_PFLASHIMG_HDR);
            state->headerlen = strlen(state->buffer);
            state->headeroff = 0;
            pflashimgheader = 1;
            return 0;
         }
         sprintf(state->buffer, "%ls%ld%ls", S_HTTP_PFLASHIMG_1,
                 DLMStartParms.dlp_parallel_len, S_HTTP_PFLASHIMG_2);
         state->headerlen = strlen(state->buffer);
         state->headeroff = 0;

      }
      state->substate = DLM_SHOW_SIMAGES_STATE_SFLASH;
      break;

   case DLM_SHOW_SIMAGES_STATE_SFLASH:
      // Done if information for all images has been shown
      if(sflashimgcount == DLM_IMAGE_COUNT)
      {
         return 1;
      }

      // If serial flash images header has not been displayed yet, show
      // it and return
      if(!sflashimgheader)
      {
         sprintf(state->buffer, "%ls%d%ls%ld%ls", S_HTTP_SFLASHIMG_HDR_1,
                 DLM_IMAGE_COUNT, S_HTTP_SFLASHIMG_HDR_2, DLM_IMAGE_SIZE,
                 S_HTTP_SFLASHIMG_HDR_3);
         state->headerlen = strlen(state->buffer);
         state->headeroff = 0;
         sflashimgheader = 1;
         return 0;
      }

      // Format information into a single table row to be shown on the DLM index
      // page
      if((simages.images[i].type & RLDR_DLM_SIMAGES_TYPE_NOTRUNNABLE) !=
          RLDR_DLM_SIMAGES_TYPE_NOTRUNNABLE)
      {

         // Format timestamp for current image in serial flash.
         mktm(&time, simages.images[i].timestamp);
         sprintf(tmbuf, "%02d/%02d/%04d at %02d:%02d:%02d\n", time.tm_mon,
                 time.tm_mday, 1900 + time.tm_year, time.tm_hour, time.tm_min,
                 time.tm_sec);
         sprintf(state->buffer, "%ls%d%ls%s - %ld bytes, received on %s%ls",
                 S_HTTP_SFLASHIMG_START_1, i, S_HTTP_SFLASHIMG_START_2,
                 simages.images[i].name, simages.images[i].length, tmbuf,
                 S_HTTP_SFLASHIMG_START_3);
         state->headerlen = strlen(state->buffer);
         state->headeroff = 0;
      }
      // Wrap index around if necessary
      if(i == 0)
         i = DLM_IMAGE_COUNT;
      i = i - 1;
      sflashimgcount++;
      break;
   }
	return 0;
}

/**********************************************************************
int start_image_cgi(HttpState *s);

Description:
   This function is called by the HTTP server after one of the "Start" buttons
   is clicked from the download manager index page.  If an image in being
   started from serial flash, this function determines which index number the
   image is, runs a CRC check of the image, and shows a message on a new page
   indicating that the image is correct and will be running soon.  The same is
   true for a parallel flash image, except that the index does not need to be
   determined as there is only one parallel flash DLP image.  If the CRC fails
   for either a parallel or serial flash image, a message is shown on a new
   web page indicating that the CRC failed.

Assumptions:
	If serial flash is present, the serial flash driver has been
   initialized.
   Only called by the HTTP server, not explicitly called by the download
   manager.

Input:
	Pointer to HttpState structure (supplied by HTTP server).

Return values:
   0              Function has not yet completed, and needs the HTTP server to
                  call it again.
   CGI_SEND       Function has not yet completed, but needs the HTTP server to
                  send a string to the client.
   CGI_SEND_DONE  Function needs the HTTP server to send one last string and
                  does not need to be called anymore.

**********************************************************************/

// Each of the following xdata declarations are for the pieces of text sent to
// the client by this cgi.  Strings are stored in xmem to save root data usage,
// and moved into a root buffer via sprintf and the %ls format specifier.

xdata S_HTTP_STARTIMG_HDR_1
{
   "HTTP/1.0 200 OK\r\nDate: "
};

xdata S_HTTP_STARTIMG_HDR_2
{

										"\r\nContent-Type: text/html\r\n\r\n" \
      "<html>" \
         "<head>" \
            "<title>Upload status</title>" \
         "</head>" \
         "<body bgcolor=#FFFFFF fgcolor=#009900>"
};

xdata S_HTTP_STARTIMG_TBLHDR
{
            "<br><br><center>" \
            "<table border=0 width=65%% cellspacing=0 cellpadding=3>" \
               "<tr>" \
                  "<th colspan=2 style=background-color:#009933;color:white>" \
                     "Rabbit Semiconductor Download Manager" \
                  "</th>" \
               "</tr>" \
               "<tr>" \
                  "<td colspan=2 style=background-color:#CCCCCC;color:black>" \
                     "Loading DLP image..." \
                  "</td>" \
               "</tr>"
};

xdata S_HTTP_STARTIMG_SUCCESS
{
               "<tr>" \
                  "<td style=background-color:#CCCCCC;color:black>" \
                     "DLP image loaded successfully." \
                  "</td>" \
            "</table>" \
         "</body>" \
      "</html>"
};


xdata S_HTTP_STARTIMG_FAIL_1
{
               "<tr>" \
                  "<td style=background-color:#CCCCCC;color:black>"
};

xdata S_HTTP_STARTIMG_FAIL_2
{
						"</td>" \
                  "<td style=background-color:#CCCCCC;color:black>&nbsp</td>" \
               "</tr>" \
               "<tr>" \
               	"<td style=background-color:#CCCCCC;color:black>&nbsp</td>" \
                  "<td style=background-color:#CCCCCC;color:black>" \
                     "<a href=\"/\">Home</a>" \
                  "</td>" \
               "</tr>"
};

#define DLM_START_IMAGE_CGI_SFLASHIMG	0
#define DLM_START_IMAGE_CGI_PFLASHIMG	1
_dlm_app_nodebug
int start_image_cgi(HttpState *s)
{
	auto int len, i, rc;
	auto char *fname;
   auto char *ptr;
   auto char buffer[128];
   auto char date[30];				// Buffer for creating the HTTP "Date:" string.
   auto int imageindex;


	if(!http_getState(s))
   {
      // Note: it is safe to use the getData buffer, since the first call to
      // the CGI will not have any incoming data in that buffer.
      sprintf(http_getData(s), "%ls%s%ls", S_HTTP_STARTIMG_HDR_1,
         	  http_date_str(date), S_HTTP_STARTIMG_HDR_2);
      http_setState(s, 1);
      return CGI_SEND;  // Write string to client.
   }

	switch(http_getAction(s))
   {
   case CGI_START:
      fname = http_getField(s);
      if(strcmp(fname, "sfimageindex") == 0)
      {
         http_setCond(s, 1, DLM_START_IMAGE_CGI_SFLASHIMG);
      }
      else if(strcmp(fname, "pfimageindex") == 0)
      {
         http_setCond(s, 1, DLM_START_IMAGE_CGI_PFLASHIMG);
      }
      sprintf(http_getData(s), "%ls", S_HTTP_STARTIMG_TBLHDR);
      return CGI_SEND;  // Write string to client.
   case CGI_DATA:
      switch(http_getCond(s, 1))
      {
      case DLM_START_IMAGE_CGI_SFLASHIMG:
         // Get the the serial flash image index to load from the form's
         // data and mark the index as the current active.
         imageindex = atoi(http_getData(s));
         dlm_setactivedlp(imageindex);
         break;
      case DLM_START_IMAGE_CGI_PFLASHIMG:
         // Do nothing extra when loading from parallel flash.
         break;
      }

      break;
   case CGI_END:
      break;
   case CGI_EOF:
      if(CheckImageCRC(http_getCond(s, 1)))
      {
         sprintf(http_getData(s), "%ls", S_HTTP_STARTIMG_SUCCESS);
         gShutDownWhenIdle           = 1;
         gLoadImageFromParallelFlash = http_getCond(s, 1);
      }
      else
      {
         sprintf(http_getData(s), "%ls%s%ls", S_HTTP_STARTIMG_FAIL_1,
                 gStatusFile, S_HTTP_STARTIMG_FAIL_2);
      }
      return CGI_SEND_DONE;
   case CGI_ABORT:
      break;
   default:
      break;
   }
   return 0;
}

/**********************************************************************
int upload_cgi(HttpState * s);

Description:
	This function is called by the HTTP server to handle the uploading of a new
   DLP image.  If an error occurrs during the upload, an error message will
   be sent to the client indicating the problem.  While the image is being
   uploaded, a CRC of the image is calculated.  Once the entire image is
   received, a check is made of the calculated versus transmitted CRC.  If they
   differ, an error message will be sent to the client.

Assumptions:
	If serial flash is present, the serial flash driver has been
   initialized.
   Only called by the HTTP server, not explicitly called by the download
   manager.

Input:
	Pointer to HttpState structure (supplied by HTTP server).

Return values:
   0              Function has not yet completed, and needs the HTTP server to
                  call it again.
   CGI_SEND       Function has not yet completed, but needs the HTTP server to
                  send a string to the client.
   CGI_SEND_DONE  Function needs the HTTP server to send one last string and
                  does not need to be called anymore.

**********************************************************************/

// Each of the following xdata declarations are for the pieces of text sent to
// the client by this cgi.  Strings are stored in xmem to save root data usage,
// and moved into a root buffer via sprintf and the %ls format specifier.
xdata S_HTTP_UPLOADSTAT_HDR_1
{
	"HTTP/1.0 200 OK\r\nDate: "
};

xdata S_HTTP_UPLOADSTAT_HDR_2
{
	"\r\nContent-Type: text/html\r\n\r\n" \
   "<html>" \
      "<head>" \
         "<title>Upload status</title>" \
      "</head>"\
      "<body bgcolor=#FFFFFF fgcolor=#009900>"
};

xdata S_HTTP_UPLOADSTAT_TBLHDR
{
         "<br><br>" \
         "<center><table border=0 width=65%% cellspacing=0 cellpadding=3>" \
         "<tr>" \
            "<th colspan=2 style=background-color:#009933;color:white>"\
            	"Rabbit Semiconductor Download Manager" \
            "</th>" \
         "</tr>" \
         "<tr>" \
            "<td colspan=2 style=background-color:#CCCCCC;color:black>" \
               "Loading image..." \
            "</td>" \
         "</tr>"
};

xdata S_HTTP_UPLOADSTAT_SFLASHERROR
{
	"Serial flash error occurred while loading image."
};

xdata S_HTTP_UPLOADSTAT_SFLASHSPACE
{
	"Ran out of space while loading image. (Maximum size for each image is "
};

xdata S_HTTP_UPLOADSTAT_ERROR_1
{
         "<tr>" \
            "<td style=background-color:#CCCCCC;color:black>"
};

xdata S_HTTP_UPLOADSTAT_ERROR_2
{
				"</td> " \
            "<td style=background-color:#CCCCCC;color:black>" \
            	"<a href=\"/\"> Home </a>" \
            "</td>" \
         "</tr>" \
      "</table>" \
      "</body>" \
   "</html>"
};

xdata S_HTTP_UPLOADSTAT_FINAL_1
{
         "<tr>" \
            "<td style=background-color:#CCCCCC;color:black>"
};

xdata S_HTTP_UPLOADSTAT_FINAL_2
{
				".</td> " \
            "<td style=background-color:#CCCCCC;color:black>" \
            	"<a href=\"/\"> Home </a>" \
            "</td>" \
         "</tr>" \
      "</table>" \
      "</body>" \
   "</html>"
};

xdata S_HTTP_UPLOADSTAT_CRCFAIL_1
{
         "<tr>" \
            "<td style=background-color:#CCCCCC;color:black>"
};

xdata S_HTTP_UPLOADSTAT_CRCFAIL_2
{
				"</td> " \
            "<td style=background-color:#CCCCCC;color:black>" \
            	"<a href=\"/\"> Home </a>" \
            "</td>" \
         "</tr>" \
      "</table>" \
      "</body>" \
   "</html>"
};

_dlm_app_nodebug
int upload_cgi(HttpState * s)
{
	auto int len, i, rc;
	auto char *fname;
   auto char *ptr;
   auto char buffer[128];
   auto char date[30];				// Buffer for creating the HTTP "Date:" string.
   static unsigned long et;
   static char imagename[DLM_MAX_IMAGE_NAME];

   if (!http_getState(s))
   {
   	http_setState(s, 1);
      memset(&gDownloadState, 0, sizeof(gDownloadState));
      gDownloadState.FirstSector = TRUE;
		gStartDLPAfterLoad = DLM_START_DLP_AFTER_LOAD;
      gShutDownWhenIdle = 0;
      status_set("ready");
		// Note: it is safe to use the getData buffer, since the first call to
      // the CGI will not have any incoming data in that buffer.
      sprintf(http_getData(s), "%ls%s%ls", S_HTTP_UPLOADSTAT_HDR_1,
      		  http_date_str(date), S_HTTP_UPLOADSTAT_HDR_2);
      return CGI_SEND;	// Write string to client.
   }

	switch(http_getAction(s))
   {
   case CGI_START:
      et = MS_TIMER;
      fname = http_getField(s);
      memcpy(imagename, http_getFileName(s), sizeof(imagename));

      if(strcmp(fname, "dlpimage") == 0)
      {
         http_setCond(s, 1, 0);
         // Try writing a string to the client.  Most browsers will be able to
         // display this straight away.  This will give some confirmation that
         // something is happening.
         sprintf(http_getData(s), "%ls", S_HTTP_UPLOADSTAT_TBLHDR);
         return CGI_SEND;
      }
      else if (strcmp(fname, "rundlpstandalone") == 0)
      {
         http_setCond(s, 1, 1);
      }
      break;

   case CGI_DATA:
      switch(http_getCond(s, 1))
      {
      // case 0 is data for the DLP image being transferred
      case 0:
         status_setprogress();
         ptr = http_getData(s);
         for(i = 0; i < http_getDataLength(s); i++, ptr++)
         {
            if((rc = ProcessProgramByte(*ptr)) != 0)
            {
               if(rc < 0)
               {
                  // If ProcessProgramByte returns less than 0, there was an
                  // error while writing to serial flash.
                  sprintf(buffer, "%ls", S_HTTP_UPLOADSTAT_SFLASHERROR);
               }
               else
               {
						// If ProcessProgramByte returns greater than 0, the DLM ran
                  // out of serial flash space while trying to write the image.
                  sprintf(buffer, "%ls%ld bytes)",
                          S_HTTP_UPLOADSTAT_SFLASHSPACE, DLM_IMAGE_SIZE);
               }
               status_set(buffer);
               sprintf(http_getData(s), "%ls%s%ls", S_HTTP_UPLOADSTAT_ERROR_1,
                       buffer, S_HTTP_UPLOADSTAT_ERROR_2);
					// The CGI is done processesing due to error
               return CGI_SEND_DONE;
            }
         }

         break;
      // case 1 is data for the run DLP stand alone checkbox
      case 1:
         // this only happens if checkbox is checked, data should
         // be "on".
         strncpy(buffer, http_getData(s), http_getDataLength(s) > 127 ? 127 :
                http_getDataLength(s));
         buffer[http_getDataLength(s) > 127 ? 127 : http_getDataLength(s)] = 0;
         if(strcmpi(buffer, "on") == 0)
            gStartDLPAfterLoad = 1;
         break;
      }
      break;
   case CGI_EOF:
		// Write the last sector of the image, get the elapsed time for the
      // transfer, and update the DLM status to reflect the success/failure of
      // the upload.
      rc = WriteLastSector(imagename);
      et = MS_TIMER - et;
      if (et < 1) et = 1;
      sprintf(buffer, "Handled %ld bytes in %f seconds (%f bytes/sec). ",
              gDownloadState.Size, 0.001*et,
              (float)http_getContentLength(s)*1000.0/et);
      if(rc == 1)
      {
         strcat(buffer,
                "Upload failed - transmitted CRC did not match calculated CRC");
      }
      else if(rc < 0)
   	{
         strcat(buffer,
                "Upload failed - a flash error occurred while writing image");
      }
      else
      {
         strcat(buffer, "Upload successful");
      }
      status_set(buffer);
      sprintf(http_getData(s), "%ls%s%ls", S_HTTP_UPLOADSTAT_FINAL_1, buffer,
              S_HTTP_UPLOADSTAT_FINAL_2);

      // If the CRC check is ok so far and the user wants to run the image
      // after upload, do a CRC check of the image as it is stored in serial
      // flash and run if check is good.
      if(gStartDLPAfterLoad && (rc == 0))
      {
         if(CheckImageCRC(0))
         {
            if(gStartDLPAfterLoad)
               gShutDownWhenIdle = 1;
         }
         else
         {
            sprintf(http_getData(s), "%ls%s - upload failed. %s%ls",
                    S_HTTP_UPLOADSTAT_CRCFAIL_1, S_HTTP_UPLOADSTAT_CRCFAIL_2);
         }
      }
      return CGI_SEND_DONE;
   default:
      break;
   }
   return 0;
}

#ximport "pages/index.shtml"  index_shtml
#ximport "pages/status.shtml" status_shtml
#ximport "pages/upload.html"  upload_html

// The default for / must be first
// This table maps file extensions to the appropriate "MIME" type.  This is
// needed for the HTTP server.
SSPEC_MIMETABLE_START
   SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
   SSPEC_MIME(".html", "text/html"),
   SSPEC_MIME_FUNC(".cgi", "", NULL)
SSPEC_MIMETABLE_END

// The flash resource table is now initialized with these macros...
SSPEC_RESOURCETABLE_START
	// HTTP server resources
	// xmem files
   SSPEC_RESOURCE_P_XMEMFILE("/", index_shtml, "DLM", RUPL_ADMIN_GROUP,    \
                             0x0000, SERVER_HTTP, RUPL_HTTP_SERVER_AUTH),
   SSPEC_RESOURCE_P_XMEMFILE("/index.shtml", index_shtml, "DLM",           \
                             RUPL_ADMIN_GROUP, 0x0000, SERVER_HTTP,        \
                             RUPL_HTTP_SERVER_AUTH),
   SSPEC_RESOURCE_P_XMEMFILE("/upload.html", upload_html, "DLM",           \
                             RUPL_ADMIN_GROUP, 0x0000, SERVER_HTTP,        \
                             RUPL_HTTP_SERVER_AUTH),
   SSPEC_RESOURCE_P_XMEMFILE("/status.shtml", status_shtml, "DLM",         \
                             RUPL_ADMIN_GROUP, 0x0000, SERVER_HTTP,        \
                             RUPL_HTTP_SERVER_AUTH),
	// cgi
   SSPEC_RESOURCE_P_CGI("upload.cgi", upload_cgi, "DLM", RUPL_ADMIN_GROUP, \
                        0x0000, SERVER_HTTP, RUPL_HTTP_SERVER_AUTH),
	SSPEC_RESOURCE_P_CGI("startimage.cgi", start_image_cgi, "DLM",          \
                        RUPL_ADMIN_GROUP, 0x0000, SERVER_HTTP,             \
                        RUPL_HTTP_SERVER_AUTH),
	SSPEC_RESOURCE_P_FUNCTION("listsimages", show_stored_images, "DLM",     \
                             RUPL_ADMIN_GROUP, 0x0000, SERVER_HTTP,        \
                             RUPL_HTTP_SERVER_AUTH),
	// root variable
   SSPEC_RESOURCE_ROOTVAR("gStatusFile",  gStatusFile, PTR16, "%s"),
SSPEC_RESOURCETABLE_END


/******************************************************************************/
/********************** END HTTP UPLOAD SECTION *******************************/
/******************************************************************************/

/*********************************************************************
main()

Takes care of initializing the download manager and web server.  This
function is responsible for shutting down the download manager and
rebooting to the flash resident loader when gShutDownWhenIdle is set
to true.

Return Value: Does not return
*********************************************************************/
_dlm_app_nodebug
void main()
{
#ifdef DLM_APP_VERBOSE
	auto char 	ipstr[20];
#endif
	auto int 	retval;
   auto ruplNetworkPowerOnParameters networkparams;

#ifdef DLM_APP_VERBOSE
	printf("_______________________________________________________\n");
   printf("DLM APP >> Initializing....\n");
#endif

   memset(&gDownloadState, 0, sizeof(gDownloadState));

	gShutDownWhenIdle           = 0;
	gLoadImageFromParallelFlash = 0;
// add status file resource
	gStatusID = sspec_addrootfile(STATUS_FILENAME, (char *)&gStatusFile, 0,
                                 SERVER_HTTP);
   if(gStatusID < 0)
   {
#ifdef DLM_APP_VERBOSE
		printf("DLM APP >> ERROR: Can't add file \"" STATUS_FILENAME "\"!\n");
		exit(gStatusID);
#endif
   }
   // status file belongs to the DLM realm, is readable by the RUPL_ADMIN_GROUP,
   // not writeable, accesible to the HTTP server, and uses the authentication
   // method specified by DLP_HTTP_SERVER_AUTH (which defaults to digest
   // authentication.
	sspec_setpermissions(gStatusID, "DLM", RUPL_ADMIN_GROUP, 0x0000, SERVER_HTTP,
                        RUPL_HTTP_SERVER_AUTH, NULL);

   // Create an "authorized user" for the system.  To update the file,
   // external clients must use this name/password.
	gUpdaterID = sauth_adduser(RUPL_USER_NAME, RUPL_USER_PASSWORD,
                              SERVER_HTTP|SERVER_WRITABLE);
	if(gUpdaterID < 0)
   {
#ifdef DLM_APP_VERBOSE
		printf("DLM APP >> ERROR: Can't add user \"" RUPL_USER_NAME "\"!\n" );
		exit(gUpdaterID);
#endif
	}
   else
   {
	   // Ensure that that user has a group mask of RUPL_ADMIN_GROUP i.e. this
      // user is a member of "ADMIN".
	   sauth_setusermask(gUpdaterID, RUPL_ADMIN_GROUP, NULL);
		// Also need to assign individual write access.
      sauth_setwriteaccess(gUpdaterID, SERVER_HTTP);
   }

   if(retval = sock_init() > 0)
   {
#ifdef DLM_APP_VERBOSE
   		printf("DLM APP >> Could not initialize packet driver.\n");
   		exit(1);
#endif
   }

#if RUPL_SAVE_RESTORE_NETWORK_PARAMS > 0
	// Attempt to load network parameters saved to user block.  If network
   // parameters have been saved, this function will restore them via ifconfig.
   if(rupl_loadnetworkparameters(&networkparams))
   	ifconfig(IF_ANY, IFS_RESTORE, &networkparams.configuration, IFS_END);
#endif

	dlm_init();

#ifdef DLM_APP_VERBOSE
	printf("DLM APP >> IP: %s\n", inet_ntoa(ipstr, MY_ADDR(IF_DEFAULT)));
#endif

	status_set("Ready");

   http_init();
	tcp_reserveport(80);

	while(1)
   {
      http_handler();
      if(gShutDownWhenIdle)
      {
      	if(http_idle())
            ShutdownAndRunDLP();
      }
	}
}



