/***********************************************************************************
	board_id.c

   Z-World, 2001
	This sample program is for the OP7200 series controllers.
	
	This program will detect the controller type that you have (via product
	ID) and will display that information in the STDIO window. Here's the
	controller product information. 

	Product ID     Product Description
	---------- 		-------------------
	 0x1100			OP7200...22MHz ethernet, DI/O, ADC, Rabbit Net, Touchscreen 
	 0x1101        OP7210...22MHz ethernet  DI/O, ADC, Rabbit Net, no Touchscreen
	 0x1102			OP7200B..22MHz RCM2250 ethernet, DI/O, ADC, Rabbit Net, Touchscreen
	 0x1103			OP7210B..22MHz RCM2250 ethernet, DI/O, ADC, Rabbit Net

************************************************************************************/
#class auto

/* Return the number of elements in an array. */
#define NELEMENTS(arr)	(sizeof(arr)/sizeof(arr[0]))


///////////////////////////////////////////////////////////////////////////

void main()
{
	auto char * names[8];
	auto int i;

	names[0] = "OP7200...22MHz ethernet, DI/O, ADC, Rabbit Net, Touchscreen" ;
	names[1] = "OP7210...22MHz ethernet  DI/O, ADC, Rabbit Net, no Touchscreen" ;
	names[2] = "OP7200B..22MHz RCM2250 ethernet, DI/O, ADC, Rabbit Net, Touchscreen" ;
	names[3] = "OP7210B..22MHz RCM2250 ethernet, DI/O, ADC, Rabbit Net" ;
	names[4] = "0x1104" ;
	names[5] = "0x1105" ;
	names[6] = "0x1106" ;
	names[7] = "0x1107" ;

	if(_BOARD_TYPE_ < 0x1100 || _BOARD_TYPE_ > 0x1100+NELEMENTS(names))
	{
		printf("\n\rERROR: Invalid controller BOARD_ID detected!!!\n");
	}
	else
	{
		i = _BOARD_TYPE_ & 0x000f;
		// display controller information in the STDIO window	
		printf("\n\rController is a %s\n", names[i]);
	}
}
