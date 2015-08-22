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
/**************************************************************************
	board_id.c

	This sample program is for the OP6800 series controllers.
	
	This program will detect the controller type that you have (via product
	ID) and will display that information in the STDIO window. Here's the
	controllers product information. 

	OP6800 Demo Board Setup:
	------------------------
	1. Connect a +12v power source to the OP6800 Demo Board connector
	   J3/J5 as follows:

	   PS     	OP6800 Demo Board
	   --			-----------------
	   +12v		+RAW Terminal  
	   GND      GND Terminal


	Product ID     Product Description
	---------- 		-------------------
	0x0D00			OP6800...22MHz, LCD/Keypad, Digital I/O, Ethernet
	0x0D01			OP6810...22MHz, LCD/Keypad, Digital I/O, No Ethernet
	0x0D02         OP6805...22MHz, LCD/Keypad, Digital I/O, Ethernet (512K SRAM & FLASH)
			 
**************************************************************************/
#class auto

///////////////////////////////////////////////////////////////////////////

void main()
{
	auto char *ptr[3];
	auto int i;

	ptr[0] = "OP6800...22MHz, LCD/Keypad, Digital I/O, w/Ethernet";
	ptr[1] = "OP6810...22MHz, LCD/Keypad, Digital I/O, No Ethernet";
	ptr[2] = "OP6805...22MHz, LCD/Keypad, Digital I/O, w/Ethernet\n(512K SRAM & FLASH)";

	
	if(_BOARD_TYPE_ < 0x0D00 || _BOARD_TYPE_ > 0x0D02)
	{
		printf("\n\n\rInvalid controller BOARD_ID 0x%04x detected!!!", _BOARD_TYPE_);
	}
	else
	{
		i = _BOARD_TYPE_ & 0x000f;
		// display controller information in the STDIO window	
		printf("\n\n\rController is a %s", ptr[i]);
	}
}


	
