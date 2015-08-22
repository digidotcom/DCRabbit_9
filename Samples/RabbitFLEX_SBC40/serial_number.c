/*******************************************************************************
        Samples\RabbitFLEX_SBC40\serial_number.c
        Rabbit Semiconductor, 2006

        This program displays the design number and serial number of the
        RabbitFLEX SBC40 board on which it is running.  This can be used
        to identify an unknown board.

*******************************************************************************/

void main(void)
{
	char *serialnum;
   char *retval;

	// Get the design/serial number from the System ID Block
   serialnum = SysIDBlock.serialNumber;
	// Make sure that a serial number exists
   if (strlen(serialnum) > 0) {
   	// Split the design and serial numbers (they are separated by '/')
   	retval = strtok(serialnum, "/");
      if (retval) {
      	// Display the design and serial numbers
      	printf("Design #:  %s\n", serialnum);
         printf("Serial #:  %s\n", serialnum + strlen(serialnum) + 1);
      }
		else {
      	printf("Invalid serial/design number!\n");
      }
   }
   else {
   	printf("No serial number or design number found!  Is this a RabbitFLEX SBC40 board?\n");
   }
}

