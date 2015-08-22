/*
	Samples\SPI\AD7870.c

	Copyright Z-World Inc. Jan 2002

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
