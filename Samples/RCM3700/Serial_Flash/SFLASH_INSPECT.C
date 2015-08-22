/*****************************************************************************
sflash_inspect.c
Z-World, 2003

This program is a handy utility for inspecting the contents of a serial
flash chip. When it starts it attempts to initialize a serial flash
chip on serial port B. If one is found then the user can perform two different
commands:
'p' prints out the contents of a specified page in the serial flash

'c' clears (sets to zero) all of the bytes in a specified page.


*************************************************************8***************/

#define SFLASH_DEBUG

#define SF_SPI_CSPORT PEDR
#define SF_SPI_CSSHADOW PEDRShadow
#define SF_SPI_CSDD PEDDR
#define SF_SPI_CSDDSHADOW PEDDRShadow
#define SF_SPI_CSPIN 6

#use "sflash.lib"

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

   brdInit();
   sfspi_init();
	if(sf_init())
   {
   	printf("Flash init failed\n");
      exit(-1);
   }
   else
   {
   	printf("Flash init OK\n");
      printf("# of blocks: %d\n", sf_blocks);
      printf("size of block: %d\n", sf_blocksize);
   }
   printf("press c to clear a page, p to print out contents\n");
   while(1)
   {
   	inchar = getchar();

      if(inchar == 'c')
      {
         for(i = 0;i < sf_blocksize;i++)
         {
         	flash_buf[i] = 0;
         }
         sf_writeRAM(flash_buf, 0, sf_blocksize);
         printf("page number to clear?");
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
         printf("page number to print out?");
      	pagenum = input_number();
 			if(pagenum >= 0 && pagenum < sf_blocks)
         {
	     		printf("\nPage %d\n", pagenum);
	  	    	sf_pageToRAM(pagenum);
	         sf_readRAM(flash_buf, 0, sf_blocksize);
	      	for(j=0;j <= sf_blocksize/16;j++)
	         {
               printf("%04x: ", 16*j);
	         	for(k=0;k < 16;k++)
	            {
               	if(16*j + k < sf_blocksize)
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
	         }	// end of for j<= sf_blocksize/16
         }	// end of if(pagenum >= 0 && pagenum < sf_blocks)
         else
         {
         	printf("ERROR: invalid page number\n");
         }

 		}	// end of if(inchar == 'p')
   }	// end of while
}




