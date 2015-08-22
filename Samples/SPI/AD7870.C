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
/*
	Samples\SPI\AD7870.c

	This program has been written by the Hardware Engineering Staff at Z-World in
	response to several customer requests.  As such, it has NOT had the testing and
	validation procedures which our "standard" software products have.  It is being
	made available as a sample.  There is no warranty, implied or otherwise.
*/
#class auto

// define the interface parameters before "use"ing the library
#define SPI_SER_B
#define SPI_CLK_DIVISOR			5
#define ADS7870_CS_PORT			PBDR
#define ADS7870_CS_PORTSHADOW	PBDRShadow
#define ADS7870_CS_BIT			7

/********************************
 * End of configuration section *
 ********************************/

#use "ADS7870.lib"

void main ()
{
	float Volts;
	int i;

	ADS7870Init ();
	while (1)
	{	Volts = ADS7870Read ( 0, 3 );
		printf ( "%6.2f\n\r", Volts );
	}
}
