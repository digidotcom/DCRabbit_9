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

  Samples\memory_usage.c

  Useful little program to show memory usage.

  For best table format results use "Terminal" for
  the Stdio window font.

  This program only detects the primary flash if the
  board has more than one flash!

************************************************************/
#class auto

#if __RABBITSYS
#error "MEMORY_USAGE.C is not currently supported under RabbitSys."
#endif

int table_entry( unsigned logaddr, long physaddr, long Size, char * use);
char readStackSeg();
void separator();

main()
{
	unsigned long mb,mt,ms,sz;
	static char datasegval;
   int i;
	const _sys_mem_origin_t * org_entry;
   long next;
   _rk_xbreak_t xsbreak;

	//********** Display Hexadecimal Statement ***************
	printf(
	 "All numbers are in hexadecimal unless noted\n\n"
	 );

	//********** Show Flash Present ***************
	printf(
		"     Dynamic C detects a flash of %08lx bytes.\n",
			(long)(_FLASH_SIZE_)<<12ul);
	if(_FLASH_SIZE_ != FLASH_SIZE)
		printf(
			"       However, the BIOS set FLASH_SIZE to %08lx bytes\n",
				(long)(FLASH_SIZE)<<12ul);
#if RAM_COMPILE
		printf(
			"     Flash is not being used. This program is compiled to RAM \n");
#elif FLASH_COMPILE
	printf(
		"     Flash starts at physical address 00000\n");
#else
		printf(
			"     This program is compiled to flash and copied to run in RAM \n");
		printf(
			"     The top of the flash is mapped to 0xC0000 \n");
#endif
	printf("\n");


#if __SEPARATE_INST_DATA__
		printf(
			"     Separate instruction and data space is enabled \n");
	printf("\n");
#endif

	//********** Show RAM Present ***************
	printf(
	   "     Dynamic C detects a RAM of %08lx bytes\n",(long)(_RAM_SIZE_)<<12ul);
	printf("     Ram starts at physical address %08lx\n",(long)(RAM_START)<<12ul);


	//********** Display MMU/MIU Registers ***************
	printf("\nMMU Registers:\n");
	printf("     SEGSIZE=%02x\n",MEMBREAK);
	#asm
		ioi ld a, (DATASEG)
		ld	(datasegval), a
	#endasm
	printf("     DATASEGVAL=%02x\n",datasegval);
	printf("     STACKSEGVAL=%02x\n",readStackSeg());
	printf("     XPC=%02x\n",XMEMSEGVAL);
	printf("\nMIU Bank Registers:\n");
	printf("     MB0CR=%02x\n",MB0CRShadow);
	printf("     MB1CR=%02x\n",MB1CRShadow);
	printf("     MB2CR=%02x\n",MB2CRShadow);
	printf("     MB3CR=%02x\n",MB3CRShadow);
	printf("\n");


	//********** Begin Usage Table ***************
	printf(
	 " \xda-----------------------------------------------------------------------------\xbf"
	 );
	printf(
	 " \xB3 PHYSADR LOGADR    SIZE  SIZE(Dec)  USAGE                                    \xB3"
	 );

	separator();
	printf(
#ifdef _FLASH_
	 " \xB3 FLASH                                                                       \xB3"
#else
	 " \xB3 RAM                                                                         \xB3"
#endif
	 );
	separator();

	//********** Root Code  ***************
	table_entry(0,(long)0,(long)ROOTCODESIZE,"Reserved by Compiler for Root Code");
	table_entry(0,(long)0,(long)StartUserCode-1,"used by BIOS");
	table_entry((unsigned)StartUserCode,
	  (long)StartUserCode,
	  (long)prog_param.RCE.aaa.a.addr-(long)StartUserCode,
	  "used by this program for root code");

#if __SEPARATE_INST_DATA__
	table_entry(0,(ROOTCONSTSEGVAL<<12l)+(long)ROOTCONSTORG,(long)ROOTCONSTSIZE,"Reserved for Root Constants");
#endif

	separator();

	//********** XMem Code  ***************
#if __SEPARATE_INST_DATA__
	table_entry(
		0xffffu,
	  (long)SID_XMEMORYORG,
	  (long)SID_XMEMORYSIZE,
	  "Reserved for Xmem Code");
#endif

	table_entry(
		0xffffu,
	  (long)XMEMORYORG,
	  (long)XMEMORYSIZE,
	  "Reserved for Xmem Code");

	mb = 0x0ffffful &
		((unsigned long)prog_param.XCB.aaa.a.addr +
		((unsigned long)prog_param.XCB.aaa.a.base<<12));
	mt = 0x0ffffful &
		((unsigned long)prog_param.XCE.aaa.a.addr +
		((unsigned long)prog_param.XCE.aaa.a.base<<12));
	ms = mt-mb;

	table_entry(
	   0xffffu,
	   mb,
	   ms,
	   "used by this program for xmem code");

#if (XMEM_RESERVE_SIZE>0)
	table_entry(
	   0xffffu,
	   FLASH_SIZE*4096L - DATAORG - XMEM_RESERVE_SIZE,
	   (long)XMEM_RESERVE_SIZE,
	   "used for the File System");
#endif
	separator();

	printf(
#ifdef _FLASH_
	 " \xB3 RAM                                                                         \xB3"
#else
	 " \xB3 MORE RAM (Code and BIOS in RAM option set in compiler options)              \xB3"
#endif
	 );
	separator();

	//********** Root RAM ***************
	table_entry(
	   (ROOTDATAORG) - (ROOTDATASIZE) + 1,
      paddr((void *) ((ROOTDATAORG) - (ROOTDATASIZE) + 1)),
	   (long) ROOTDATASIZE,
	   "Reserved for Root Data");

#if BBROOTDATASIZE > 0
	table_entry(
	   (BBROOTDATAORG) - (BBROOTDATASIZE) + 1,
      paddr((void *) ((BBROOTDATAORG) - (BBROOTDATASIZE) + 1)),
	   (long) BBROOTDATASIZE,
	   "Reserved for Battery Backed Root Data");
#endif

	table_entry(
	   prog_param.RDE.aaa.a.addr,
	   paddr((void *)prog_param.RDE.aaa.a.addr),
	   (long)(prog_param.RDB.aaa.a.addr - prog_param.RDE.aaa.a.addr)+1,
	   "used by this program for root data");

	table_entry(
	   (unsigned int)INTVEC_BASE,
	   paddr((void*)(INTVEC_BASE)),0x100ul,
	   "used for the internal vector table");

   table_entry(
      (unsigned int)XINTVEC_BASE,
      paddr((void*)(XINTVEC_BASE)),0x100ul,
      "used for the external vector table");

	separator();

	//********** Display xalloc used areas ***************
	// includes RAM space reserved for items already listed
   for (i = 0; i < _orgtablesize; ++i) {
   	org_entry = &_orgtable[i];
   	if (org_entry->type == RESVORG)
      	table_entry(org_entry->laddr, org_entry->paddr, org_entry->totalbytes,
                     "Reserved RAM space");
   }

	separator();

	//********** Stacks ***************
	table_entry(
	   0xffffu,
   	BaseOfStacks1,
	   SizeOfStacks1,
	   "Stack space");

	separator();

	//********** Flash Buffer ***************
	table_entry(
	   0xffffu,
	   flash_buf_phys,
	   (long)FLASH_BUF_SIZE,
	   "Used for the flash buffer");

#if ENABLE_ERROR_LOGGING
	//********** Error Log Buffer ***************
	table_entry(
	   0xffffu,
	   ERRLOG_PHYSICAL_ADDR&0xffff000ul,
	   (long)(ERRLOG_NBLOCKS*4096),
	   "Reserved for the Error log buffer");
	table_entry(
	   0xffffu,
	   ERRLOG_PHYSICAL_ADDR,
	   (long)ERRLOG_LOG_SIZE,
	   "Used for the Error log buffer");
#endif

#if (FS2_RAM_RESERVE!=0)
	table_entry(
	   0xffffu,
	   (long)FS2_RAM_PHYS,
	   FS2_RAM_RESERVE*4096L,
	   "Reserved for File System buffer");

#endif
	separator();

	//********** Display xalloc free areas ***************
   next = _rk_xubreak;
   while (next) {
   	xmem2root(&xsbreak, next, sizeof(_rk_xbreak_t));
      table_entry(0xe000u, next + sizeof(_rk_xbreak_t),
                  xsbreak.sbreak - xsbreak.limit, "Free XRAM space for xalloc");
      next = xsbreak.next;
   }

	// bottom of table
	printf(
	 " \xC0-----------------------------------------------------------------------------\xd9\n"
	 );

}


table_entry( unsigned logaddr,
             long physaddr,
             long Size,
             char * use
           )
{
	auto unsigned xAddr;
	auto unsigned long addr32;
   auto char     xBase;

	// *** base seg entry ***
	if(logaddr<ROOTDATAORG)
	{
		printf(" \xB3 %05lx   00:%04x   %05lx %6ld     %-40.38s \xB3",physaddr,logaddr,Size,Size,use);
	}
	// *** data seg entry ***
	else if(logaddr<(STACKORG))
	{
		printf(" \xB3 %05lx   %02lx:%04x   %05lx %6ld     %-40.38s \xB3",physaddr,(long)DATASEGVAL,logaddr,Size,Size,use);
	}
	// *** stack seg entry ***
	else if(logaddr<0xE000u)
	{
		printf(" \xB3 %05lx   %02x:%04x   %05lx %6ld     %-40.38s \xB3",physaddr,readStackSeg(),logaddr,Size,Size,use);
	}
	// *** xmem seg entry ***
	// if log address is in E000-FFFF range ignore logical entry parameter
	// and convert physical address to a an E000-FFFF logical address
	else
	{
		addr32 = physaddr-0xe000ul;
		xBase = (char)((addr32&0x000ff000L)>>12);
		xAddr = (unsigned)((addr32&0x00000fffL)|0xe000L);

		printf(" \xB3 %05lx   %02x:%04x   %05lx %6ld     %-40.38s \xB3",physaddr,xBase,xAddr,Size,Size,use);
	}
}


char readStackSeg(){
	static char retval;
   // **** read STACKSEG register ****
#asm
	ioi ld a,(STACKSEG)
	ld (retval),a
#endasm
	return retval;
}

void separator()
{
	printf(
	 " \xC3-----------------------------------------------------------------------------\xB4"
	 );
}