/***************************************************************************

	 Samples\predefined.c

	 Z-World, Inc, 2001

    This program demonstrates shows how the contents
    of the predefined macros __DATE__, __TIME__, __FILE__, __LINE__ and
	 CC_VER can be displayed.
    
***************************************************************************/
#class auto


void main()
{
   printf("Compiling file: %s\n", __FILE__);   
	printf("Compiled on %s at %s\n", __DATE__, __TIME__);
   printf("Current line number: %d\n", __LINE__);
   printf("Current line number: %d\n", __LINE__);

   printf("Compiler version %d.%02x\n", CC_VER >> 8, CC_VER & 0x0FF);
}
