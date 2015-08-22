/********************************************************************

	sflash_test.c
 	Z-World, 2003

	This program is for BL2600 series controllers using the SF1000
   serial flash card.

   Description
	===========
	This simple sample program demonstrates how to read and write
   from the SF1000 Serial Flash. Please refer to SF1000 User Manual
   for the complete set of API functions for this product.

	Instructions
	============
   1. Plug SF1000 Serial Flash card into connector J9 on the BL2600,
      matching pin 1 on each connector.

	2. Compile and run this program.

	3. You should see in STDIO Window:

		     "This serial flash test passed"

      This indicates a write and read were successful.

      OR

		     "This serial flash test failed"

      This indicates a write and read were not successful.


*********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Use SF1000 serial library
#use sf1000.lib


main()
{
	auto int msg1, msg2;
   auto int i;
	auto char buf1[64];
	auto char buf2[64];

   // Initialize the controller
	brdInit();

	memset(buf1, 0x00, sizeof(buf1));
	memset(buf2, 0x00, sizeof(buf2));

 	if (msg1 = SF1000Init())
   {
		printf("Serial flash initialize: error msg %d\n", msg1);
   }

   printf("\n\n");
   printf("SF1000 Block size    = %d\n", SF1000_Block_size);
	printf("SF1000 number of Blocks = %d\n\n\n", SF1000_Nbr_of_Blocks);

   for(i=0; i <10; i++)
   {
  		strcpy(buf1, "This serial flash test");
		msg1 = SF1000Write(0x1000, buf1, strlen(buf1)+1);
		msg2 = SF1000Read(0x1000, buf2, strlen(buf1)+1);

   	if (strncmp(buf1, buf2, strlen(buf1)))
   	{
			printf("%s#%02d failed: error msg write %d, read %d\n", buf1, i, msg1, msg2);
   	}
   	else
   	{
			printf("%s#%02d passed\n", buf2, i);
   	}
   }
}