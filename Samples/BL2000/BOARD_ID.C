/**************************************************************************
	board_id.c

   Z-World, 2001
	This sample program is for the BL2000 series controllers.
	
	This program will detect the controller type that you have and
	will display that information in the STDIO window. The following
	are the various controller versions.

	BL2000	// 22MHz 12 bit A/D, DAC,    Digital I/O, Ethernet
	BL2010	// 22MHz 10 bit A/D, no DAC, Digital I/O, Ethernet
	BL2020	// 22MHz 12 bit A/D, DAC,    Digital I/O, no Ethernet
	BL2030	// 22MHz 10 bit A/D, no DAC, Digital I/O, no Ethernet
 
**************************************************************************/
#class auto

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto char *ptr[4];
	auto int i;
	
	ptr[0] = "BL2000...22MHz 12 bit A/D, DAC, Digital I/O, Ethernet";
	ptr[1] = "BL2010...22MHz 10 bit A/D, no DAC, Digital I/O, Ethernet";
	ptr[2] = "BL2020...22MHz 12 bit A/D, DAC, Digital I/O, no Ethernet";
	ptr[3] = "BL2030...22MHz 10 bit A/D, no DAC, Digital I/O, no Ethernet";
	
	if(_BOARD_TYPE_ < 0x0800 || _BOARD_TYPE_ > 0x0803)
	{
		printf("\n\n\rInvalid controller BOARD_ID detected");
	}
	else
	{
		i = _BOARD_TYPE_& 0x000f;
		printf("\n\n\rYour controller is a %s", ptr[i]);
	}
}
