/*******************************************************************************
   board_id.c

   Z-World, 2001
   This sample program is for BL21xx series controllers.

   This program will detect the controller type that you have (via board
   ID) and will display that information in the STDIO window. Here's the
   controller product information.

   Board ID   Product Description
   --------   -------------------
   0x0B00     BL2100 22MHz, Ethernet, Digital I/O, Analog
   0x0B01     BL2110 22MHz, Ethernet, Digital I/O, No Analog
   0x0B02     BL2120 22MHz, No Ethernet, Digital I/O, Analog
   0x0B03     BL2130 22MHz, No Ethernet, Digital I/O, No Analog
   0x0B04     BL2105 22MHz, Ethernet, Digital I/O, Analog,
              512K Flash & SRAM
   0x0B05     BL2115 22MHz, Ethernet, Digital I/O, No Analog,
              512K Flash & SRAM
   0x0B06     BL2101 22MHz, Ethernet, Digital I/O, 0-10V Analog
   0x0B07     BL2121 22MHz, No Ethernet, Digital I/O, 0-10V Analog
   0x0B08     BL2111 22MHz, Ethernet, Digital I/O, 0-10V Analog,
              512K Flash & SRAM

*******************************************************************************/

#class auto

void main()
{
   auto char *ptr[9];
   auto int i;

   ptr[0] = "00 22MHz, Ethernet, Digital I/O, Analog";
   ptr[1] = "10 22MHz, Ethernet, Digital I/O, No Analog";
   ptr[2] = "20 22MHz, No Ethernet, Digital I/O, Analog";
   ptr[3] = "30 22MHz, No Ethernet, Digital I/O, No Analog";
   ptr[4] = "05 22MHz, Ethernet, Digital I/O, Analog,\n512K Flash & SRAM";
   ptr[5] = "15 22MHz, Ethernet, Digital I/O, No Analog,\n512K Flash & SRAM";
   ptr[6] = "01 22MHz, Ethernet, Digital I/O, 0-10V Analog";
   ptr[7] = "21 22MHz, No Ethernet, Digital I/O, 0-10V Analog";
   ptr[8] = "11 22MHz, Ethernet, Digital I/O, 0-10V Analog,\n512K Flash & SRAM";

   if (_BOARD_TYPE_ < 0x0B00 || _BOARD_TYPE_ > 0x0B08) {
      printf("Non-BL21xx controller board ID (0x%04X) detected!!!\n",
             _BOARD_TYPE_);
   } else {
      i = _BOARD_TYPE_ & 0x000f;
      // display controller information in the STDIO window
      printf("Controller is a BL21%s.\n", ptr[i]);
   }
}

