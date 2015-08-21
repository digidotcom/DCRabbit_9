/*******************************************************************************
	board_id.c

	Z-World, 2004
	This sample program is for the BL2600 series controllers.

	This program will detect the controller type that you have (via product
	ID) and will display that information in the STDIO window. Here's the
	controller product information.

	Product ID       Product Description
	--------------   -------------------
	0x1700, BL2600   (RCM3200) 44MHz, 512K Flash, 256K SRAM, 512K FSRAM,
	                           Ethernet.
	0x1701, BL2600   (RCM3000) 29MHz, 512K Flash, 512K SRAM, Ethernet.
	0x1702, BL2610   (RCM3100) 29MHz, 512K Flash, 512K SRAM.
	0x1703, BL26xx   Reserved for future use.
	0x1704, BL2600   (RCM3010) 29MHz, 256K Flash, 128K SRAM, Ethernet.
	0x1705, BL2610   (RCM3110) 29MHz, 256K Flash, 128K SRAM.
	0x1706, BL26xx   Reserved for future use.
	0x1707, BL26xx   Reserved for future use.
	0x1708, BL26xx   Reserved for future use.
	0x1709, BL26xx   Reserved for future use.
	0x170A, BL26xx   Reserved for future use.
	0x170B, BL26xx   Reserved for future use.
	0x170C, BL2600   (RCM3365) 44MHz, 512K Flash, 256K SRAM, 512K FSRAM, 16 Mb
	                           Nand Flash, xD Connector.
	0x170D  BL2600   (RCM3375) 44MHz  512K Flash, 256K SRAM, 512K FSRAM, xD
	                           Connector.
	0x170E, BL26xx   Reserved for future use.
	0x170F, BL2600   (RCM3900) 44MHz, 512K Flash, 256K SRAM, 512K FSRAM, 32 MB
	                           Nand Flash, SD Socket.
*******************************************************************************/
#class auto

/* Return the number of elements in an array. */
#define NELEMENTS(arr) (sizeof arr / sizeof arr[0])

///////////////////////////////////////////////////////////////////////////

const char * const names[] = {
	"BL2600   (RCM3200) 44MHz, 512K Flash, 256K SRAM, 512K FSRAM,Ethernet.",
	"BL2600   (RCM3000) 29MHz, 512K Flash, 512K SRAM, Ethernet.",
	"BL2610   (RCM3100) 29MHz, 512K Flash, 512K SRAM.",
	"ERROR: Invalid controller BOARD_ID detected!!!",
	"BL2600   (RCM3010) 29MHz, 256K Flash, 128K SRAM, Ethernet.",
	"BL2610   (RCM3110) 29MHz, 256K Flash, 128K SRAM.",
	"ERROR: Invalid controller BOARD_ID detected!!!",
	"ERROR: Invalid controller BOARD_ID detected!!!",
	"ERROR: Invalid controller BOARD_ID detected!!!",
	"ERROR: Invalid controller BOARD_ID detected!!!",
	"BL2600   Reserved.",
	"BL2600   Reserved.",
	"BL2600   (RCM3365) 44MHz, 512K Flash/SRAM/FSRAM, NFlash, xD Connector.",
	"BL2600   (RCM3375) 44MHz, 512K Flash/SRAM/FSRAM, xD Connector.",
	"ERROR: Invalid controller BOARD_ID detected!!!",
	"BL2600   (RCM3900) 44MHz, 512K Flash/SRAM/FSRAM, NFlash, SD Socket."
};

void main(void)
{
	if (_BOARD_TYPE_ < 0x1700 || _BOARD_TYPE_ >= 0x1700 + NELEMENTS(names)) {
		printf("\nERROR: Unsupported controller BOARD_ID detected!!!\n");
	} else {
		// display controller information in the STDIO window
		printf("\nController is a:\n   %s\n", names[_BOARD_TYPE_ & 0x000F]);
	}
}