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
/*************************************************************************
 	SAMPLES\FILESYSTEM\DUMPFILE.C

 	Dump out contents of a file.

 	Files are all numbered under the Z-World File Systems.  User enters
 	a dump and the contents are sent to STDOUT.  The file system config-
 	uration must match those of the program that wrote the file.

 	The fopen_rd() and fcreate() return 0 on success.  This is not a POSIX-
 	like behavior!

*************************************************************************/
#class auto


/**********************************/
/****  USER MODIFIED SETTINGS  ****/
/**********************************/

#define  MY_FORCE_FORMAT	0


/**
 * 	FS2_USER_PROGRAM_FLASH - Set aside this many kilobytes in the program
 * 		flash.  You will also need to edit .\lib\bioslib\memconfig.lib to
 *			set an appropriate value to XMEM_RESERVE_SIZE.  The definition here
 * 		may be larger or smaller - the minimum of both specifications will
 * 		be used.
 */
#define FS2_USE_PROGRAM_FLASH	16

/**
 * 	LX_2_USE - You can use fs_get_flash_lx() (for the 2nd flash);
 * 		fs_get_ram_lx() (for RAM, if configured in .\lib\bioslib\memconfig.lib);
 * 		or fs_get_other_lx() (for program flash if configured in
 *			.\lib\bioslib\memconfig.lib).
 */
#define LX_2_USE			fs_get_flash_lx()

#define FS_DEBUG
#define FS_DEBUG_FLASH

/**********************************/
/****  USER MODIFIED SETTINGS  ****/
/**********************************/

#memmap xmem

/*  Always pull in FS MkII. */
#use "fs2.lib"

FSLXnum	fs_ext;


/* -------------------------------------------------------------------- */


/* START FUNCTION DESCRIPTION ********************************************
mem_dump

SYNTAX:  void  mem_dump( const char * real_ptr, long addr, long d_size )

KEYWORDS:		debug

DESCRIPTION:	Formats bytes in root memory.  If addr != -1L, then bytes
					fetched from real_ptr but displayed as if from addr.

        0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
543210: xx xx xx xx xx xx xx xx_xx xx xx xx xx xx xx xx  |0123456789ABCDEF|

RETURN VALUE:	none.

SEE ALSO:		printf()

END DESCRIPTION **********************************************************/

	// Separator between byte 7 and 8.
#define CH_BETWIX	'\\'

nodebug void
mem_dump( char * real_ptr, long addr, long d_size )
{
	static const char 		hexdigs[] = "0123456789ABCDEF";
	auto char 		row[16];
	auto char 		str[80];
	auto char *		bp; 			/* hex byte ptr */
	auto char * 	ap;   		/* ASCII rendering pointer */
	auto char 		j;

	/*  If not faking address, set it... */
	if( -1L == addr ) {
		addr = (long) real_ptr;
	}

	while( d_size > 0 ) {
		/* The magic constants here define where the hex and ASCII areas are. */
		sprintf( str, "%05lx:  ", addr & 0xFFFFFFF0L );
		memset( str+7, ' ', sizeof(str)-1-7 );
		str[31] = CH_BETWIX;
		str[57] = '|';
		str[74] = '|';
		str[75] = '\0';

		j = ((char)addr) & 0x0F;
		bp = str + 8 + j * 3;
		ap = str + 58 + j;

		memcpy( row + j, real_ptr, 16 - j );

		/*ADVANCE*/
		real_ptr += (16 - j);
		addr += (16 - j);

		/*  Convert to printable ASCII, if applicable, default is '.' */
		for( ; d_size > 0 && j < 16 ; ++j, --d_size ) {
			*bp++ = hexdigs[ row[j] >> 4 ];
			*bp++ = hexdigs[ row[j] & 15 ];
			*ap = '.';
			if( ' ' <= row[j] && row[j] <= '~' ) {
				*ap = row[j];  		/* If printable ASCII, then display it. */
			}
			ap++;
			bp++;  		// skip space between hex numbers.
		}

		printf( "%s\n", str );
	}

}   /* end mem_dump() */


/**
 * 	Dump the file contents.
 */
void
do_dump(int fnum)
{
	char	contents[128];
	File 		f;
	int 		want, got;
	long 		addr;

	if( 0 != fopen_rd( & f, fnum ) ) {
		printf( "\nError: file #%d doesn't exist (errno %d)\n", fnum, errno );
		exit(2);
	}

	addr = 0L;
	while( 0 != (got=fread( & f, contents, sizeof(contents) )) ) {
		mem_dump( contents, addr, got );
		addr += got;
	}

	fclose( & f );
}   /* end do_dump() */


/**
 * 	Create a file with some contents.  Lots of error checking here.
 */
void
do_create(int fnum)
{
	auto char	contents[128];
	auto File 		f;
	auto int 		want, got;
	auto long 		addr;

	/*  fopen_wr() if file exists, or fcreate() if doesn't exist. */
	if( 0 != fopen_wr( & f, fnum ) && 0 != fcreate( & f, fnum ) ) {
		printf( "\nError: file #%d not writable (errno %d)\n", fnum, errno );
		exit(2);
	}

	sprintf( contents, "This is a test 1234 of FileSystem MkII\n" );

	if( 0 == fwrite( & f, contents, strlen(contents) ) ||
		 0 == fwrite( & f, contents, strlen(contents) ) ||
		 0 == fwrite( & f, contents, strlen(contents) ) ) {
		printf( "error: writing (errno %d)\n", errno );
	}

	fclose( & f );
}   /* end do_create() */


/**
 * 	Try opening (for reading) every file.  Print those we can do!
 */
void
scan_for_existing_files()
{
	auto File 	f;
	auto int 	fnum;
	auto int 	count;

	printf( "Found these existing files: " );
	for( fnum=1, count = 0 ; fnum < 128 ; ++ fnum ) {
		if( 0 == fopen_rd( & f, fnum ) ) {
			fclose( & f );
			printf( " #%d ", fnum );
			++count;
		}
	}

	if( 0 == count ) {
		printf( " ... none ..." );
	}
	printf( "\n" );
}   /* end scan_for_existing_files() */

/* -------------------------------------------------------------------- */

void
main()
{
	char  	contents[ 128 ];
	int 		rc;
	int 		fnum;

	errno = 0;
	fs_ext = LX_2_USE;
	if (!fs_ext) {
		printf("The specified device (LX# %d) does not exist.  Change LX_2_USE.\n",
				(int)fs_ext);
		exit(1);
	}
	else {
		printf("Using device LX# %d...\n", (int)fs_ext);
	}

	/*
	 * Step 2: format the filesystem if requested.  Note that formatting
	 * must be done _after_ the call to fs_init().
	 *
	 * This demo has compile-time constant of whether the flash should be
	 * formatted or not.
	 */

	rc = fs_init( 0, 0 );
	if( rc != 0 ) {
		printf( "Error: can't get filesystem initialized.\n" );
		exit(2);
	}

	if( MY_FORCE_FORMAT ) {
		printf( "Note: File System MkII not present.\n" );
		if( 0 != lx_format(fs_ext, 0) ) {
			printf( "ABORT: Can't Format MkII File System, errno=%d\n", errno );
			exit(2);
		}
	}

	/*  Show the user some possible solutions: */
	scan_for_existing_files();

	/*  Prompt for the file number to dump or create. */
	printf( "Which file number to affect? " );
	gets( contents );
	fnum = atoi( contents );
	if( fnum <= 0 ) {
		printf( "Error: bad input\n" );
		exit(2);
	}

	/*  Give user once chance to specify the operation to perform. */
	printf( "D)ump a file or C)reate file #%d? ", fnum );
	gets( contents );
	switch( contents[0] ) {
		case 'D' : case 'd' :
				do_dump(fnum);
				break;
		case 'C' : case 'c' :
				do_create(fnum);
				break;
		default :
				printf( "error: unknown command\n" );
	}

}   /* end main() */