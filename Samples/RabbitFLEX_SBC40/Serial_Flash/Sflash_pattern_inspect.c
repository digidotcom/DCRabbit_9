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
/********************************************************************

	sflash_pattern_inspect.c

	This program is for the RabbitFLEX SBC40's that have a serial flash
	device installed.

   Description
	===========
	This program writes a pattern to the first 100 pages which then
   can be inspected or cleared by the user.

	Instructions
	============
	1. Compile and run this program.
   2. Here's the options the user can use inspect or clear a page of flash.

   'p' prints out the contents of a specified page in the serial flash

	'c' clears (sets to zero) all of the bytes in a specified page.


*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

#define SF_SPI_CSPORT PDDR
#define SF_SPI_CSSHADOW PDDRShadow
#define SF_SPI_CSDD PDDDR
#define SF_SPI_CSDDSHADOW PDDDRShadow
#define SF_SPI_CSPIN 6

#use "sflash.lib"

int sf_pagesize;
char flash_buf[1056];

// Gets positive numeric input from keyboard and returns int value when enter key
// is pressed.  Returns -1 if non-numeric keys are pressed. (Allows backspace)
int input_number()
{
	int number;
   char inchar;

   number = 0;
   while(1)
   {
   	inchar = getchar();
      putchar(inchar); //echo input
      if(inchar == '\n' || inchar == '\r')
      {
      	return number;
      }
      else if(inchar == '\b')
      {
         //backspace
      	number = number / 10;
      }
      else if(inchar >= '0' && inchar <= '9')
      {
      	number = number*10 + (inchar - '0');
      }
      else
      {
      	//bad input
         return -1;
      }
   }	// end of while
}


int main()
{
	char ascii_buffer[17];
   char fbyte, inchar;
   int i, j, k, pagenum;
   auto int key;

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


   printf("Writing incremental pattern to page 0\n");
   // Load first page of flash with an incremental pattern
   for(i = 0;i < sf_pagesize;i++)
   {
   	flash_buf[i] = i;
   }
   sf_writeRAM(flash_buf, 0, sf_pagesize);
  	sf_RAMToPage(0);

   printf("Writing the page number to pages 1 - 99\n\n\n");
   // Load next 99 pages of flash with its page number
   for(pagenum=1; pagenum < 100; pagenum++)
   {
		for(i = 0;i < sf_pagesize;i++)
   	{
     		flash_buf[i] = pagenum;
   	}
   	sf_writeRAM(flash_buf, 0, sf_pagesize);
  		sf_RAMToPage(pagenum);
   }

   while(1)
   {
   	printf("Press c to clear a page, p to print out contents the page\n");
   	inchar = getchar();

      if(inchar == 'c')
      {
         for(i = 0;i < sf_pagesize;i++)
         {
         	flash_buf[i] = 0;
         }
         sf_writeRAM(flash_buf, 0, sf_pagesize);
         printf("Page number to clear? ");

         pagenum = input_number();
 			if(pagenum >= 0 && pagenum < sf_blocks)
         {
         	printf("\nClearing page %d\n", pagenum);
         	sf_RAMToPage(pagenum);
         }
         else
         {
         	printf("ERROR: invalid page number\n");
         }
      }
      else if(inchar == 'p')
      {
         printf("Page number to print out? ");
      	pagenum = input_number();
 			if(pagenum >= 0 && pagenum < sf_blocks)
         {
	     		printf("\nPage %d\n", pagenum);
	  	    	sf_pageToRAM(pagenum);
	         sf_readRAM(flash_buf, 0, sf_pagesize);
	      	for(j=0;j <= sf_pagesize/16;j++)
	         {
               printf("%04x: ", 16*j);
	         	for(k=0;k < 16;k++)
	            {
               	if(16*j + k < sf_pagesize)
                  {
							fbyte = flash_buf[16*j + k];
	               	printf("%02x ", fbyte);
	               	if(fbyte >= 16 && fbyte <= 127)
	               	{
	               		ascii_buffer[k] = fbyte;
	               	}
	               	else
	               	{
	               		ascii_buffer[k] = '.';
	               	}
                  }
                  else
                  {
                  	printf("   ");
                  	ascii_buffer[k] = 0;
                  }
	            }
	            ascii_buffer[16] = 0;
	            printf("  %s\n", ascii_buffer);
	         }
         }
         else
         {
         	printf("ERROR: invalid page number\n");
         }

 		}
   }
}




