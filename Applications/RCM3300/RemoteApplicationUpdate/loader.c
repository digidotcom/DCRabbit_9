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
/***********************************************************
	Applications\RemoteApplicationUpdate\loader.c

   This file implements a parallel flash resident loader that occupies the first
   8k of program flash.  The loader ximports the DLM image and a DLP image into
   parallel flash.  The loader's main responsibility is to load either the DLM
   from parallel flash or a DLP from parallel or serial flash, and start the
   loaded image running.  Parameters and information are passed to and from the
   loader via the flash transfer buffer at the top of RAM.

************************************************************/

root void exception(void);

// When set to 0, the following macro causes the DLM to be loaded
// unconditionally every time the board is restarted due to a hard reset or
// watchdog timeout.  If this macro is set to 1, and if the board is restarted
// due to a hard reset or watchdog timeout, then the loader will attempt to load
// the DLP if DLP size and address information is available in the parameter
// area in RAM.  The loader will not make decisions regarding the number of
// times a target has been restarted due to hard reset or watchdog timeout as
// that would require the loader to be capable of updating the user block.
// Setting this macro to 1 is potentially dangerous but left as an option to the
// user.
#define LDR_RESTART_DLP_AFTER_HARDRESET 	0

// ximport the DLM and DLP into xmem
#xcodorg DLMArea 				apply
#ximport DLM_IMP_PATH	dlmimage
#xcodorg DLPArea 				apply
#ximport DLP_IMP_PATH 	dlpimage
#xcodorg ldrxcode				apply

// Hard coded address of DLM/DLP BIOS's "special" FastRAM_InRAM variable.
#define LDR_FASTRAM_INRAM 	0x0003

// Size of loader stack - allocated as a static buffer in root data area.
#define LOADER_STACK_SIZE	0x1000

// The shadows needed by the loader.
char pdddrshadow, pddrshadow, sbcrshadow,  spcrshadow, pbddrshadow, pcfrshadow,
     tacsrshadow, tacrshadow, gocrshadow, tat5rshadow, sbershadow;

// The following macros are configuration macros for the serial flash library.
#define SF_RCM3300

#define SF_SPI_CSPORT 		PDDR
#define SF_SPI_CSSHADOW 	pddrshadow
#define SF_SPI_CSDD 			PDDDR
#define SF_SPI_CSDDSHADOW 	pdddrshadow
#define SF_SPI_CSPIN 1

#define SF_SERPORT_TR_SHADOW tat5rshadow
#define SF_SERPORT_CR_SHADOW sbcrshadow
#define SF_SERPORT_ER_SHADOW sbershadow

#define SF_SPI_TACRSHADOW  tacrshadow
#define SF_SPI_PCFRSHADOW  pcfrshadow
#define SF_SPI_PBDDRSHADOW pbddrshadow

#define LDR_SFLASH_BUFSIZE	1056
static char sflashbuffer[LDR_SFLASH_BUFSIZE];
#use "sflash.lib"
#use "remoteuploaddefs.lib"
#use "errno.lib"
#use "idblock.lib"
#use "mutil.lib"
#use "string.lib"
#use "sys.lib"
#use "sysio.lib"
#use "tc.lib"
#use "util.lib"
#use "xmem.lib"

nodebug void FatalLoaderError(void);

#memmap root
#asm
premain::
#endasm

nodebug
root void main()
{
   static ldrStartDLP startDLP;
	static ldrStartDLM startDLM;
   static ldrDLMStartParms dlmStartParms;
   static char imgverified;
   static char loaderstack[LOADER_STACK_SIZE];
	static  ldrRAMLoadInfo loadinfo;

#asm nodebug
	ipset	3
	jp		__ldr_init_loader

RunImageInFastRAM::
;; This code is copied to the FAST_RAM_COMPILE mode flash write buffer at the
;;  top of the MB2CR quadrant, and is run from there.
;;
;; Set up the rest of the FAST_RAM_COMPILE physical memory mapping.
;; This must be done before the BIOS is restarted by the "jp 0"
;;  instruction below, since we now want to run the program in RAM and
;;  not in flash.

		;; map bank 3 to the Flash on /CS0
		ld		a, FLASH_WSTATES | CS_FLASH
ioi	ld		(MB3CR), a

		;; map bank 2 to the RAM on /CS1
		ld		a, RAM2_WSTATES | CS_RAM2
ioi	ld		(MB2CR), a

		;; map bank 0 to bottom half of RAM on /CS2
		ld		a, RAM_WSTATES | CS_RAM
ioi	ld		(MB0CR), a

		;; Jump to address 0 to restart BIOS (In RAM now)
		jp		0

EndRunImageInFastRAM::
__ldr_init_loader:
// *** Set the MMIDR, turn on Separate ID (if enabled) ***
	ld		a, MMIDR_VALUE
	ioi	ld (MMIDR), a

	; setup the memory mapping registers
	ld		a, MB0CR_SETTING
	ioi	ld	(MB0CR), a

   ld    a, MB1CR_SETTING
   ioi    ld (MB1CR),a

   ld    a, MB2CR_SETTING
   ioi    ld (MB2CR),a

   ld    a, MB3CR_SETTING
   ioi    ld (MB3CR),a

	; setup the stack for the loader
	ld		hl, loaderstack
   ld		de, LOADER_STACK_SIZE - 1
   add	hl, de
	ld		sp, hl

	ld		a, 00001000b		; normal oscilator, processor and peri.
									;  from main clock, no periodic interrupt
	ioi	ld (GCSR), a

   ld		a,10100000b			; set clock and status pins low
   ld		(gocrshadow),a
   ioi	ld (GOCR),a

   ld    a,MEMBREAK
   ioi    ld (SEGSIZE), a
   ld    a, RAM_START
   ioi    ld (DATASEG), a

	ld		a, 0x80				; Ignore SMODE pins.  Dynamic C does this,
	ioi	ld (SPCR), a		; but must do it running stand alone too.
	ld		(spcrshadow), a

   ld      a, 0xC0
   ld      (pbddrshadow), a

   ld      a, 0c0h
   ioi   ld (PCFR), a   	; as initialized in pilot
   ld    hl, pcfrshadow
   ld    (hl), a

   ; make timers tick
   ld    a, 00000001b
   ioi    ld (TACSR),a
   ld    hl,tacsrshadow
   ld    (hl),a

   ; make timer A4 clocked by main clock
   ; disable interrupt
   ld    a, 00000000b
   ioi    ld (TACR),a
   ld    hl,tacrshadow
   ld    (hl),a

   ; setup interrupt vector tables
   ld      a,0xff & (INTVEC_BASE >> 8)
   ld      iir,a
   ld      a,0xff & (XINTVEC_BASE >> 8)
   ld      eir,a
#endasm

	memset((void*)&startDLP,      0xdd, sizeof(startDLP));
   memset((void*)&startDLM,      0, sizeof(startDLM));
	memset((void*)&dlmStartParms, 0, sizeof(dlmStartParms));

   // get value of gcsr register to pass to DLM
   dlmStartParms.type = LDR_TYPE_DLMSTARTPARMS;
   #asm
   ioi	ld	a,(GCSR)
   ld		(dlmStartParms+gcsr_value),a
   #endasm
   // get dlp parallel flash address and length to pass to DLM
   dlmStartParms.dlp_parallel_addr = dlpimage + sizeof(long);
   dlmStartParms.dlp_parallel_len  = xgetlong(dlpimage) - sizeof(ldrImageInfo);
   dlmStartParms.checksum = rupl_dochecksum((void*)&dlmStartParms,
                                            sizeof(startDLM) -
                                            sizeof(startDLM.checksum));

	// put start parameters for DLM in parameter area
   root2xmem(LDR_TYPE_DLMSTARTPARMS_OFS, (void*)&dlmStartParms,
             sizeof(dlmStartParms));

	// check parameter area for DLM address information
   xmem2root((void*)&startDLM, LDR_TYPE_LDRSTARTDLM_OFS, sizeof(startDLM));

   imgverified = 0;

	#asm
   ld a,0x51            ; disable watchdog
   ioi ld (WDTTR),a
   ld a,0x54
   ioi ld (WDTTR),a
	#endasm
   asm ipset 0

   sf_init();

	// setup information struct to be passed to rupl_valid_image
   loadinfo.physdatastart  = ((unsigned long)RAM_START << 12) + DATAORG;
	loadinfo.physdataend    = ((unsigned long)RAM_START << 12) + LDR_ROOTDATAORG;
	loadinfo.imgdestination = ((unsigned long)RAM_START << 12);
	loadinfo.loadfromsflash = 0;
   loadinfo.sflashbuffer   = sflashbuffer;
   loadinfo.sfbufsize      = sf_blocksize;

	// check to see if DLP is attempting to start a DLM image stored in a
   // location other than the default location in parallel flash
   if(startDLM.checksum == rupl_dochecksum((void*)&startDLM,
                                           sizeof(startDLM) -
                                           sizeof(startDLM.checksum)))
   {
      // if bit 30 is set in address, the image is stored in serial flash
      loadinfo.loadfromsflash =
                               (startDLM.length & IMAGE_IN_SFLASH_MASK) ? 1 : 0;
      // unset the bit from the length
      startDLM.length &= ~IMAGE_IN_SFLASH_MASK;
		// valid data is stored in the parameter area, do a crc check of the image
      // to make sure that the image is valid
		if(rupl_valid_image(startDLM.address, startDLM.length, 1, loadinfo, NULL))
      {
         imgverified = 1;
      }
   }
   if(!imgverified)
   {
	   // no DLM found, check for DLP
      xmem2root((void*)&startDLP, LDR_TYPE_LDRSTARTDLP_OFS, sizeof(startDLP));
      if(startDLP.checksum == rupl_dochecksum((void*)&startDLP,
                                              sizeof(startDLP) -
                                              sizeof(startDLP.checksum)))
      {
         // if bit 30 is set in address, the image is stored in serial flash
         loadinfo.loadfromsflash =
                               (startDLP.length & IMAGE_IN_SFLASH_MASK) ? 1 : 0;
         // unset the bit from the length
         startDLP.length &= ~IMAGE_IN_SFLASH_MASK;

#if LDR_RESTART_DLP_AFTER_HARDRESET == 1
			// if this macro is set to 1, then we let the loader load and start
         // the DLP without checking the GCSR value.
         if(rupl_valid_image(startDLP.address, startDLP.length, 1, loadinfo,
                             NULL))
         {
            imgverified = 1;
         }
#else
      	// check gcsr value to see if the loader is being restarted due to a
         // watchdog timeout or power loss.  If so, do not allow DLP to start.
         // Otherwise, force DLM to start.

         // if either of the two high bits were set in the GCSR register on
         // reset, the board went through a hard reset or watchdog timeout.
         if(!(dlmStartParms.gcsr_value & 0xc0))
         {
				if(rupl_valid_image(startDLP.address, startDLP.length, 1, loadinfo,
                                NULL))
            {
               imgverified = 1;
            }
			}
#endif
      }
	}
   if(!imgverified)
   {
      // no DLM/DLP found, start DLM
      if(!rupl_valid_image(dlmimage + sizeof(unsigned long),
                           xgetlong(dlmimage) - sizeof(ldrImageInfo),
      				         1, loadinfo, NULL))
		{
      	// no valid image has been found, set status pin high and go into a
         // tight loop - this should not happen.
			FatalLoaderError();
      }
   }
   // invalidate startDLP and startDLM parameters in RAM parameter area
	memset((void*)&startDLP, 0, sizeof(startDLP));
   memset((void*)&startDLM, 0, sizeof(startDLM));

   startDLP.type = LDR_TYPE_LDRSTARTDLP;
   startDLM.type = LDR_TYPE_LDRSTARTDLM;

   root2xmem(LDR_TYPE_LDRSTARTDLM_OFS, (void*)&startDLM, sizeof(startDLM));
	root2xmem(LDR_TYPE_LDRSTARTDLP_OFS, (void*)&startDLP, sizeof(startDLP));

   // At this point, rupl_valid_image has verified that the image to load
   // through a crc check over the entire executable image, and has loaded the
   // image into fast ram.  The following code copies the bank switching code to
   // ram and runs it, resulting in the loaded image being run.
#asm nodebug
		ipset	3
		;; Set the new flag value (in RAM, now that the copy is complete)
		ld		a, 0x08								; bottom (MB2CR) quadrant of RAM
		ld		hl, 0x0001							; mark as will be running in RAM
		ldp	(LDR_FASTRAM_INRAM), hl			; update the DLP's FastRAM_InRAM flag

		;; Map bank 1 to the upper half of the RAM on /CS2 in preparation for
		;; copying the RunImageInFastRAM function into the fast RAM mode's flash
		;; write buffer.
		;; (This assumes we're running in flash in the MB0CR quadrant!)
		ld		a, RAM_WSTATES | CS_RAM
ioi	ld		(MB1CR), a

      ;; The following code copies a piece of code from flash into RAM so that
      ;; memory banking can be changed without affecting the code that is
      ;; running.  The ldp instructions that are used are safe in that the
      ;; addresses that are being read and written should never cross a 64k
      ;; boundry (reading or writing an odd address just before a 64k boundry
      ;; will result in the second byte of the read or write coming from the
      ;; start of the current 64k block - ldp does not automatically increment
      ;; the value in the a register).  If RunImageInFastRAM was located near
      ;; the end of a 64k block, or if it were particulary large, or if the size
      ;; of the function plus the destination address was greater than a 64k
      ;; boundry the following code would no longer work correctly.
		ld		b, EndRunImageInFastRAM - RunImageInFastRAM + 1	;get function's size
		srl	b										; LDP moves two bytes at a time
		ld		a, 0x07								; init destination address bits 16-19
		ex		af, af'								; swap destination bits 16-19 into A'
		ld		ix, 0xF000							; init destination address bits 00-15
		xor	a										; init source address bits 16-19
		ld		iy, RunImageInFastRAM			; init destination address bits 00-15

.smallloop::
		ldp	hl, (iy)								; get word from memory at A:IY into HL
		inc	iy										; increment IY to next byte address
		inc	iy										; increment IY to next word address
		ex		af, af'								; swap destination bits 16-19 into A
		ldp	(ix), hl								; put word from HL into memory at A:IX
		ex		af, af'								; swap source bits 16-19 back into A
		inc	ix										; increment IX to next byte address
		inc	ix										; increment IX to next word address
		djnz	.smallloop							; decrement count, loop if nonzero

		ljp	0x71E000								; run switch routine at 0x7F000
#endasm
}

nodebug void FatalLoaderError(void)
{
#asm nodebug
exception::
   ld    a,10110000b    ; set clk pin low, status pin high
   ioi   ld (GOCR),a

   ld a,0x51            ; disable watchdog
   ioi ld (WDTTR),a
   ld a,0x54
   ioi ld (WDTTR),a

   ipset 3
__tightloop:
   jr    __tightloop
#endasm
}