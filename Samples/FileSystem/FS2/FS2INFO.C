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
/*************************************************************************
 FS2INFO.C - Filesystem Mk II sample program.

 This detects the type of flash memory attached to OE0/WE0/CS2.
 It prints some information about the device and suggestions
 for fs2 compile/runtime configuration.

 If you modify the appropriate library, you can also access
 RAM and program flash.

 Supported devices are obtained from a BIOS table.  Currently,
 only devices with a uniform sector size are supported.

 The filesystem itself is not initialized in this demonstration.
 Some of the internal routines are accessed to extract data which
 is not normally relevant to applications.
*************************************************************************/
#class auto


// Set aside this many kilobytes in the program flash.
// You will also need to edit .\lib\bioslib\memconfig.lib to set an appropriate
// value to XMEM_RESERVE_SIZE.  The definition here may be larger or
// smaller - the minimum of both specifications will be used.
#define FS2_USE_PROGRAM_FLASH	512	// Largest possible amount

// In addition, you can edit .\lib\bioslib\memconfig.lib to set a value for
// FS2_RAM_RESERVE to reserve some RAM for use by the filesystem.

#use "fs2.lib"

int main()
{
	FS_lxd * lxd;
	FSLXnum lxn;
	word flash_id;
	byte mfg;
	byte device;
	int ls_shift;
	int ls_size;
	int d_size;
	long num_ls;
	long space;
	long avail;
	long usable;
	int eff;
	int lxnum;

	errno = 0;
	if (_fs.num_lx < 1) {
		printf("Could not find any devices for filesystem.\n");
		exit(1);
	}

	lxn = fs_get_flash_lx();
	if (lxn)
		printf("Detected preferred flash device: LX# %d\n", (int)lxn);
	else
		printf("No preferred flash device detected.\n");

	lxn = fs_get_ram_lx();
	if (lxn)
		printf("Detected RAM device: LX# %d\n", (int)lxn);
	else
		printf("No RAM device detected.\n");

	lxn = fs_get_other_lx();
	if (lxn)
		printf("Detected secondary flash device: LX# %d\n", (int)lxn);
	else
		printf("No secondary flash device detected.\n");

	printf("BIOS settings:\n");
	printf("XMEM_RESERVE_SIZE %ld (bytes)\n", (long)XMEM_RESERVE_SIZE);
	printf("FS2_USE_PROGRAM_FLASH %ld (*1024=%ld bytes)\n", (long)FS2_USE_PROGRAM_FLASH, (long)FS2_USE_PROGRAM_FLASH * 1024);
	printf("FS2_RAM_RESERVE %ld (*4096=%ld bytes)\n", (long)FS2_RAM_RESERVE, (long)FS2_RAM_RESERVE * 4096);
	printf("RAM_START %ld\n", (long)RAM_START);
	printf("RAM_SIZE %ld\n", (long)RAM_SIZE);
	printf("DATAORG %ld\n", (long)DATAORG);
	printf("STACKORG %ld\n", (long)STACKORG);
	printf("ERRLOG_NBLOCKS %ld\n", (long)ERRLOG_NBLOCKS);
	printf("TC_SYSBUF_BLOCK4K %ld\n", (long)TC_SYSBUF_BLOCK4K);
	printf("DATASEGVAL %ld\n", (long)DATASEGVAL);
	printf("XMEMSEGVAL %ld\n", (long)XMEMSEGVAL);
	printf("XMEMORYSIZE %ld\n", (long)XMEMORYSIZE);
	printf("MAX_USERBLOCK_SIZE %ld\n", (long)MAX_USERBLOCK_SIZE);

	printf("\n");

	for (lxnum = 1; lxnum <= _fs.num_lx; lxnum++) {
	lxd = _fs.lx + lxnum;
	flash_id = lxd->id;
	device = flash_id & 0x00FF;
	mfg = (flash_id >> 8) & 0x00FF;

	printf("\nDevice number %d\n", lxnum);
	printf("Device detected:\n");
	switch (mfg) {
		case 0xBF:
			printf("  Silicon Storage Technology\n");
			break;
		case 0x40:
			printf("  Mosel Vitelic\n");
			break;
		case 0x1F:
			printf("  Atmel\n");
			break;
		case 0xDA:
			printf("  Winbond\n");
			break;
		default:
			printf("  Manufacturer code 0x%x\n", (unsigned)mfg);
			break;
	}
	space = lxd->ps_size * lxd->num_ps;
	printf("  Device code 0x%x\n", (unsigned)device);
	printf("  Sector size %ld\n", lxd->ps_size);
	printf("  Number of sectors %u\n", lxd->num_ps);
	printf("  Total bytes %ld\n", space);
	printf("  Typical usable bytes %ld\n", fs_get_lx_size(lxnum, 1, 0));
	printf("  Device offset %ld\n", lxd->dev_offs);
	printf("  Device class %s\n\n",
	  lxd->dev_class == FSDC_SSW ? "sector-writable" :
	  lxd->dev_class == FSDC_BW ? "byte-writable" :
	  "RAM");

	if (lxd->dev_class == FSDC_SSW) {
		if (lxd->ps_size > 8192) {
			printf("Device not suitable for FS2; sector size too large.\n");
			exit(2);
		}
		if ((lxd->ps_size & -lxd->ps_size) != lxd->ps_size) {
			printf("Device not suitable for FS2; sector size not a power of 2.\n");
			exit(3);
		}
	}
	else if (lxd->dev_class == FSDC_BW) {
		if (lxd->num_ps < 4) {
			printf("Warning: not enough sectors to allow full function.\n");
			printf("Usage may be limited to append-only, followed by\n");
			printf("re-format when filesystem becomes full.\n");
		}
	}

	// Now that we have the good oil, calculate suitable range of filesystem parameters.
	printf("Capacity estimates, assuming %d files maximum:\n\n", FS_MAX_FILES);
	printf("ls_size    num_ls    available   usable      Eff. (%%)\n");
	printf("--------   --------  ---------   ---------   ---------\n");

	for (ls_shift = 6; ls_shift <= 13; ls_shift++) {
		ls_size = 1 << ls_shift;
		num_ls = space >> ls_shift;
		if (num_ls > 32768)
			continue;
		d_size = ls_size - sizeof(FS_w) - sizeof(FS_b) - sizeof(FSchecksum);
		if (d_size < sizeof(FS_h) + sizeof(FS_l) - sizeof(FS_b))
			continue;
		avail = d_size * num_ls;
		usable = d_size * (num_ls - FS_MAX_FILES - 1);
		if (lxd->dev_class == FSDC_SSW) {
			if (ls_size < lxd->ps_size)
				continue;
		}
		else {
			if (ls_size < lxd->ps_size)
				usable -= d_size * ((lxd->ps_size / ls_size) - 1);
		}
		if (usable < 1)
			continue;
		eff = (int)(100 * usable / space);
		printf("%8u   %8u  %9ld   %9ld   %9d\n", ls_size, (int)num_ls, avail, usable, eff);
	}

	} // for

	if (0) {
		fs_init(0, 0);		// Dummy reference to fs_init, so its pre-main initialization works.
	}
	return 0;
}