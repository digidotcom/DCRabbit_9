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
