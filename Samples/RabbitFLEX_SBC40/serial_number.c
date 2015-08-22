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
/*******************************************************************************
        Samples\RabbitFLEX_SBC40\serial_number.c

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

