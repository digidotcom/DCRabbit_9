/********************************************************************

	sflash_test.c
 	Rabbit Semiconductor, 2006

	This program is for the RabbitFLEX SBC40's that have a serial flash
	device installed.

	Instructions
	============
	1. Compile and run this program.

	2. You should see in STDIO Window:

		     "This serial flash test passed"

      This indicates a write and read were successful.

      OR

		     "This serial flash test failed"

      This indicates a write and read were not successful.


*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// The following MACRO's are preset for PowerCoreFLEX series core modules
#define SF_SPI_CSPORT PDDR
#define SF_SPI_CSSHADOW PDDRShadow
#define SF_SPI_CSDD PDDDR
#define SF_SPI_CSDDSHADOW PDDDRShadow
#define SF_SPI_CSPIN 6

#use "sflash.lib"

int sf_pagesize;


main()
{
   auto int i, key;
	auto char buf1[64];
	auto char buf2[64];

	// Initialize I/O to use RabbitFLEX SBC40
	brdInit();

   sfspi_init();
	if(sf_init())
 	{
		printf("Serial flash initialize error!");
      exit(1);
   }
   else
   {
   	printf("Flash init OK\n");
      printf("# of blocks: %d\n", sf_blocks);
      printf("size of block: %d\n", sf_blocksize);
      printf("prefix size: %d\n", sf_prefixsize);
   	printf("page size: %d\n", (sf_pagesize = sf_blocksize - sf_prefixsize));
   }

   while(1)
	{
   	printf("\n\n");
   	printf("!!!This program will automatically overwrite data in your flash device!!!\n");
		printf("Do you want to continue(Y/N)? ");

		while(!kbhit());
		key = getchar();
		if(key == 'Y' || key == 'y')
		{
			break;
		}
		else if(key == 'N' || key == 'n')
		{
			exit(0);
		}

	}
	while(kbhit()) getchar();
   printf("\n\n\n");

	memset(buf1, 0x00, sizeof(buf1));
	memset(buf2, 0x00, sizeof(buf2));

   for(i=0; i <10; i++)
   {
  		strcpy(buf1, "This serial flash test");
  		sf_writeRAM(buf1, 0, strlen(buf1));
  		sf_RAMToPage(0);

      memset(buf2, 0x00, sizeof(buf2));
	  	sf_pageToRAM(0);
	   sf_readRAM(buf2, 0, sizeof(buf2));

      strcpy(buf1, "This serial flash test");
      buf2[strlen(buf1)] = '\0'; // Terminate string read from flash

      if(strncmp(buf1, buf2, strlen(buf1)))
   	{
			printf("%s#%02d FAILED!\n", buf1, i);
   	}
   	else
   	{
			printf("%s#%02d passed\n", buf2, i);
   	}
   }
}