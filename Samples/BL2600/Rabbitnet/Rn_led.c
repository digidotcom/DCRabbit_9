/********************************************************************

	rn_led.c
   Z-World Inc 2004

	This sample program is for any Rabbitnet board being used on
   the BL2600 controller.

	Description
	===========
	This program displays basic information for the given Rabbitnet
   board and will continously flash the LED on the attached Rabbitnet
   board(s) to indicate communication activity.

	Connections
	===========
   Connect RabbitNet and power supply cables to the controller
   and RN boards as described in the RabbitNet Manual.

	Instructions
	============
	1. Compile and run this program.
   2. View STDIO window to see information on the Rabbitnet board.

********************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto

nodebug
void msDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = MS_TIMER + delay;
   while( (long) (MS_TIMER - done_time) < 0 );
}


main()
{
	auto int device0, device1, tmpdev, portnum, i;
   auto char sendbyte;
   auto rn_devstruct *devaddr;

   // Initialize the controller
	brdInit();

   // Initialize controller RN ports
   rn_init(RN_PORTS, 1);

   //search on ports using physical node address
 	portnum=0000;
	if ((device0 = rn_device(portnum)) == -1)
	{
   	printf("\n\n*** No device found on port %d\n\n", 0);
   }
   else
   {
   	devaddr = (rn_devstruct *)device0;
   	printf("\n\n*** Device found on port 0\n");
      //rn_devtable is a global table
      printf("- Product ID 0x%04x\n", devaddr->productid);
      printf("- Serial number 0x%02x%02x%02x%02x\n",
      	    devaddr->signature[0], devaddr->signature[1],
             devaddr->signature[2], devaddr->signature[3]);
   }

   portnum=0100;
	if ((device1 = rn_device(portnum)) == -1)
	{
   	printf("\n\n*** No device found on port 1\n\n");
   }
   else
   {
   	devaddr = (rn_devstruct *)device1;
   	printf("\n\n*** Device found on port 1\n");
      //rn_devtable is a global table
      printf("- Product ID 0x%04x\n", devaddr->productid);
      printf("- Serial number 0x%02x%02x%02x%02x\n",
      	    devaddr->signature[0], devaddr->signature[1],
             devaddr->signature[2], devaddr->signature[3]);
   }


   if(device0 == -1 && device1 == -1)
   {
		printf("\nNo board connections!\n");
   	exit(0);
   }

   printf("\n\n");
   printf("Toggling Rabbitnet board communication activity LED");
	while(1)
  	{
   	if(device0 != -1)
      {
   		// Write to the echo command register
      	rn_write(device0, 0, &sendbyte, 1);
         msDelay(700);
		}
      if(device1 != -1)
      {
   		// Write to the echo command register
      	rn_write(device1, 0, &sendbyte, 1);
         msDelay(700);
		}
   }
}



