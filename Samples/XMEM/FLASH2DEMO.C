/*****************************************************

	flash2demo.c
	Z-World, 2001

	This program demonstrates writing to the 2nd flash
	(up to 256kb in size) on products with two flash devices.

   The WriteFlash2() function from XMEM.LIB requires
	that both flash devices on the board must be the
	same type; this should not be a problem with
	existing Z-World products.

	As of Dynamic C 7.03, the second flash (if installed)
	is mapped to the second memory quadrant (physical
	addresses 0x00040000-0x0007FFFF).  This flash can
	be written to with the WriteFlash2() function and
	read from with xmem2root().
	   		
*****************************************************/
#class auto


#ifdef _RAM_
#error "This program must be compiled to flash."
#endif
 
#define ADDR	0x00040100ul

int main()
{
	char	buf1[80], buf2[80];
	int	len, err;
	

	// fill a root buffer with data
	sprintf(buf1, "Oh wonder, how many goodly creatures are there here!");
	len = strlen(buf1);
	
	// write the data to the 2nd flash	
	err = WriteFlash2(ADDR, buf1, len);
	if (err != 0) {
		printf("WriteFlash2() returned error:  ");
		if (err == -1)
			printf("attempt to write outside of 0x00040000-0x0007FFFF.\n");
		else if (err == -2)
			printf("source pointer not in root.\n");
		else if (err == -3)
			printf("timeout occurred while writing to flash.\n");
		else
			printf("unknown error (%d).\n", err);
		exit(1);
	}

	// copy data from 2nd flash to root buffer		
	xmem2root(buf2, ADDR, len);

	// check if buffers match!		
	if (strncmp(buf1, buf2, len))
		printf("Error -- buffers don't match, data not written correctly?\n");
	else
		printf("Buffers match, data written to 2nd flash.\n");

	return(0);
}
