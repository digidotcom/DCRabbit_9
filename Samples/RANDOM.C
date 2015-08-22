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
/***************************************************************************

	 Samples\Random.c

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
