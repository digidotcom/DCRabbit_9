/***************************************************************************

	 Samples\Random.c

	 Z-World, Inc, 2000

    This program generates pseudo-random integers between 2000 and 2999
    The library rand() function returns a floating-point value (not POSIX).
    
***************************************************************************/
#class auto


unsigned int ranSeed;				// Current Random Seed

unsigned int irand ()
{
	if (ranSeed == 0x5555) ranSeed--;
	ranSeed = (ranSeed << 1) + (((ranSeed>>15)^(ranSeed>>1)^ranSeed^1) & 1);
	return ranSeed;
}


unsigned int random (unsigned int range, unsigned int minimum )
{		return irand() % range + minimum;
}


void main ()
{
	
	while (1)
	{
		printf ( "%u\n", random(1000,2000) );
	}
}
