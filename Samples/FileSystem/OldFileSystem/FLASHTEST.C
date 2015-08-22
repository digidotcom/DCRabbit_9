/***************************************************************

	flashtest.c
   Zworld, 2000

	This program does a basic test on an attached second flash.  It
	assumes that the second flash is on CS2,/OE0,/WE0.  It first erases
	the chip.  Then sector by sector it verifies the sector is all 0xff,
	then it writes 0xa5 to the sector and verifies that was what was
	written, then it writes 0x5a to the sector and verifies that was
	written.  If the sector is a byte writable it erases the sector and
	verifies that the sector is 0xff.

	Valid output for a 512 byte sector 256KB flash.  The final sector
	should be one less than the number of sectors.  It should complete
	without any errors.

	number of sectors:  512, sector size:  512                                      
	sector 511                                                                      
	done (0 errors)
	
****************************************************************/

#use "flash.lib"

/***************************************************************

 		0xc2 is a typical setting for the second flash on the
 		TCP/IP Dev Kit, the Intellicom, the Advanced Ethernet
 		Core, and the RabbitLink.
 
****************************************************************/
 
#define FLASH_MB3CR 0xc2 // 0xc2 -> CS2, /OE0, /WE0, 0 WS.

main()
{
	int x, errors;

	static char buffer1[4096], buffer2[4096], buffer3[4096];

	FlashDescriptor fd;

	memset(buffer2,0,sizeof(buffer2));
	memset(buffer3,0xff,sizeof(buffer3));
	
	flash_init(&fd,FLASH_MB3CR,NULL);
	flash_erasechip(&fd);

	printf("number of sectors:  %d, sector size:  %d\n",
		fd.number_sectors, fd.sector_size);

	if(fd.sector_size>4096) {
		printf("sector size too big\n");
		return -1;
	}

	errors=0;

	for(x=0;x<fd.number_sectors;x++) {
		printf("sector %d     \r",x);

		/* check erase sector is 0xff */

		flash_read(&fd,x,0,paddr(buffer2),fd.sector_size);
		if(memcmp(buffer3,buffer2,fd.sector_size)!=0) {
			errors++;
			printf("\nsector:  %d checking 0xff\n",x);
		}

		/* write and verify 0xa5 */
		
		memset(buffer1,0xa5,sizeof(buffer1));
		flash_writesector(&fd,x,paddr(buffer1));
		flash_read(&fd,x,0,paddr(buffer2),fd.sector_size);
		if(memcmp(buffer1,buffer2,fd.sector_size)!=0) {
			errors++;
			printf("\nsector:  %d error writing 0xa5\n",x);
		}

		/* write and verify 0x5a */
				
		memset(buffer1,0x5a,sizeof(buffer1));
		flash_writesector(&fd,x,paddr(buffer1));
		flash_read(&fd,x,0,paddr(buffer2),fd.sector_size);
		if(memcmp(buffer1,buffer2,fd.sector_size)!=0) {
			errors++;
			printf("\nsector:  %d error writing 0x5a\n",x);
		}

		/* if byte writable, erase and verify 0xff */

		if(fd.byte_writable) {
			flash_erasesector(&fd,x);
			flash_read(&fd,x,0,paddr(buffer2),fd.sector_size);
			if(memcmp(buffer3,buffer2,fd.sector_size)!=0) {
				errors++;
				printf("\nsector:  %d error erasing sector\n",x);
			}
		}
	}

	printf("\ndone (%d errors)\n",errors);
}