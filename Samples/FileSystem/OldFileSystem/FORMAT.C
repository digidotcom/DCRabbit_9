/*******************************************************************************
        format.c
        Z-World, 2000

        This program can be used for boards that have a second flash.

        This program is an example of the code to format a flash.
        
*******************************************************************************/

/*
 *	File system in flash.
 *
 */

#define FS_FLASH
#use "filesystem.lib"

/*
 *	These are the correct options for the second flash
 *	on the TCP/IP Dev Kit, Intellicom, Advanced Rabbit Core,
 * and Rabbit Link boards for a 256K second flash and
 * that stretches the whole flash.
 *
 *	The WEAR attribute should be 1 on a new flash, and some
 *	higher value on an unformatted used flash.  If you are
 * reformatting a flash you can set WEAR to 0 to keep the
 * old values.
 *
 */

#define RESERVE	0
#define BLOCKS		64
#define WEAR		1

main()
{
	if(fs_format(RESERVE,BLOCKS,WEAR)) {
		printf("error formating flash\n");
	} else {
		printf("flash successfully formatted\n");
	}
}