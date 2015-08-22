/***********************************************************

      zimport_fs2.c
      Z-World, 2002

      Demonstration of the zimport compression library

		This program uses the ZIMPORT.LIB compression
		library to demonstrate compression and decompression
		of FS2 files. The ZFILE structure is used to define
		both #zimport/#ximport and FS2 compressed and
		uncompressed files.

		The sample starts by using #ximport to import a file
		from the local disk at compile time. The resulting
		xmem "file" is compressed into an FS2 file using
		CompressFile. It then closes the xmem file and the
		FS2 file, and opens the FS2 file for input. The
		compressed file is then read from the FS2 file and
		the uncompressed data is sent to stdio.

		The LZ compression algorithm used by zimport (based
		upon the freely available LZ77 algorithm) utilizes
		a 4KB internal buffer for decompression, and a 24K
		buffer for compression (compression also requires a
		4K input buffer, which is allocated automatically for
		each output buffer that is defined). The zimport
		compression library handles memory management
		internally by pre-allocating these buffers. The
		default is a single 4KB input (decompression) buffer
		and no output (compression) buffer.

		The number of buffers to be used can be controlled
		through the use of two macros:
		OUTPUT_COMPRESSION_BUFFERS  (default = 0) and
		INPUT_COMPRESSION_BUFFERS   (default = 1)
		These numbers should coincide with the largest
		possible number of simultaneous open ZFILE's for
		output and input, respectively.

		If CompressFile is to be used, then at least one
		output compression buffer must be defined.

		See the ZIMPORT.C sample file for more information

************************************************************/
#class auto
#memmap xmem

// We need to use an output compression buffer if we are going
// to use CompressFile. This macro allows us to control the
// number of pre-allocated buffers. In this sample we use
// the default number of 1 input buffer, and since we are doing
// output (compression) we need an output compression buffer.
#define OUTPUT_COMPRESSION_BUFFERS 1

// Set aside this many kilobytes in the program flash.
// You will also need to edit BIOS\RABBITBIOS.C to set an appropriate
// value to XMEM_RESERVE_SIZE.  The definition here may be larger or
// smaller - the minimum of both specifications will be used.
#define FS2_USE_PROGRAM_FLASH	16

// You can use fs_get_flash_lx (for the 2nd flash); fs_get_ram_lx (for
// RAM, if configured in BIOS\RABBITBIOS.C); or fs_get_other_lx
// (for program flash if configured in \BIOS\RABBITBIOS.C).
#define LX_2_USE			fs_get_flash_lx()

#use "fs2.lib"			// Use FS2 filesystem
#use "zimport.lib"	// Use zimport compression

// Ximport the uncompressed sample input file
// Note that this is only for this example, FS2 files
// can be compressed to save filesystem space
#ximport "/samples/zimport/in.txt" non_compressed_loc

// Define the compressed output file "name"
#define OUT_COMPRESSED_FN       101
#define UNCOMPRESSED_FN			  102

// The number of bytes to read at a time.
// This can be adjusted to get a little more
// throughput or free up variable space.
// Increase size for speed, decrease for space.
// (This sample only)
#define BYTES2READ      30

int main() {
	static unsigned char outbuf[BYTES2READ+1];
	auto ZFILE input;
	auto ZFILE output;
	auto File fs2_file;
	auto int bytes_read, rc;
	auto long length;
	auto long t0,t1;

	// Initialize the FS2 filesystem
   rc = fs_init(0,0);
	if(rc) {
		printf("Could not initialize filesystem, error number %d\n", errno);
		exit(2);
	}

   // Format the FS2 filesystem
   rc = lx_format(LX_2_USE, 0);
	if (rc) {
		printf("Format failed, error code %d\n", errno);
		exit(3);
	}

	// Clear files before use
	fdelete(OUT_COMPRESSED_FN);
	fdelete(UNCOMPRESSED_FN);

  	// Open the input file for reading (This will open a non-compressed
  	// input file for compression)
   if(!OpenInputCompressedFile(&input, non_compressed_loc)) {
     	printf("Could not open imported file.\n");
     	exit(1);
   }

   // Open the output file, where the compressed output of
   // CompressFile will be placed
   if(!OpenOutputCompressedFile(&output, OUT_COMPRESSED_FN)) {
     	printf( "Error opening OUT_COMPRESSED_FN for output.\n" );
     	exit(1);
   }

	printf("Compressing...\n");

   t0 = MS_TIMER;

	// Compress the file input into output
   CompressFile(&input, &output);

   t1 = MS_TIMER;

   // Close the files, this should always be done to assure
   // the file is properly closed and all internal buffers
   // are flushed.
   CloseOutputCompressedFile(&output);
   CloseInputCompressedFile(&input);

   printf("Compression time = %ld milliseconds\n", t1 - t0);

	printf("Opening file for input...\n");
	// Open the newly-compressed FS2 file for reading
	if(!OpenInputCompressedFile(&input, OUT_COMPRESSED_FN)) {
		printf("Error opening compressed file.\n");
		exit(1);
	}

	printf("Compressed file contents:\n");

	// Now we verify that the compression worked by reading from the
	// newly-compressed file
	// This is the read loop, do decompression on-the-fly
	while(bytes_read = ReadCompressedFile(&input, outbuf, BYTES2READ)) {
		outbuf[bytes_read] = 0;  // Null terminate for printf
		printf("%s", outbuf);    // print to stdio
  	}

  	// Close the input file
	CloseInputCompressedFile(&input);

// *************************************
// As another example, we can do an FS2 to FS2 decompression
// using DecompressFile

	// Open the newly-compressed FS2 file for reading
	if(!OpenInputCompressedFile(&input, OUT_COMPRESSED_FN)) {
		printf("Error opening compressed file.\n");
		exit(1);
	}

   // Open the "normal" FS2 file for writing (we need
   // to create it since we deleted it above)
	fcreate(&fs2_file, UNCOMPRESSED_FN);

	// Decompress into output file. Unlike ReadCompressedFile,
	// this decompresses the entire file at once.
	DecompressFile(&input, &fs2_file);

	// Close the files
   fclose(&fs2_file);
   CloseInputCompressedFile(&input);

   // Open the "normal" FS2 file and dump its contents to stdio
	fopen_rd(&fs2_file, UNCOMPRESSED_FN);

	printf("\nDecompressed file contents:\n");

 	// Read from the uncompressed file
	while(bytes_read = fread(&fs2_file, outbuf, BYTES2READ)) {
		outbuf[bytes_read] = 0;  // Null terminate for printf
		printf("%s", outbuf);    // print to stdio
  	}

  	// Close the file
  	fclose(&fs2_file);

	printf("\nDone.\n");
}