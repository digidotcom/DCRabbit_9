/**********************************************************************
   xdata.c
   Rabbit, 2007

   Example of using xdata.

   This program demonstrates how to put various types of numeric and
   character data and xstring references into xdata constants.

   The compiler makes the names following either xdata or xstring
   keywords unsigned long variables which are initialized with the
   physical address of the specified xmem data.

   The data can viewed directly in the dump window by doing a PHYSICAL
   memory dump at the value contained in chars1 and output by the
   program.

   Something like this should be seen in the dump window:

00ca82 41 42 43 44 45 41 42 43 44 45 46 47 48 49 4A 20 ABCDEABCDEFGHIJ
00ca92 20 52 61 62 62 69 74 20 52 75 6C 65 73 21 20 20  Rabbit Rules!
00caa2 00 52 00 53 00 54 00 55 55 56 56 52 00 53 00 54  R S T UUVVR S T
00cab2 00 55 55 55 56 57 57 57 57 58 58 58 58 59 59 59  UUUVWWWWXXXXYYY
00cac2 59 5A 5A 5A 5A 00 00 80 3F 00 00 00 40 00 00 40 YZZZZ  Ä?   @  @
00cad2 40 00 00 80 40 00 00 A0 40 00 00 C0 40 48 65 6C @  Ä@  †@  ¿@Hel
00cae2 6C 6F 00 DF CA 00 00 6F 6E 65 00 74 77 6F 00 74 lo ﬂ   one two t
00caf2 68 72 65 65 00 66 6F 75 72 00 E9 CA 00 00 ED CA hree four È   Ì 
00cb02 00 00 F1 CA 00 00 F7 CA 00 00 88 90 1A 58 00 00   Ò   ˜   àêX
00cb12 00 94 1A 33 00                                   î3

   The program prints out the float values to demonstrate accessing the
   data, and because floats are hard to read in their raw hex format.

   Then the xstring addresses and strings are printed out via their
   information as stored in the xdata myData structure.
**********************************************************************/
#class auto

#define NFLOAT 6

// 1 byte length data
xdata chars1 {(char)65,(char)66,(char)67,(char)68,(char)69};
xdata chars2 {(char)0x41,(char)0x42,(char)0x43,(char)0x44,(char)0x45};
xdata chars3 {'\x46','\x47','\x48','\x49','\x4A','\x20','\x20'};
xdata chars4 {'R','a','b','b','i','t'};
xdata chars5 {" Rules!  "};

//**** 2 byte length data, if an integer fits into one or two
//****   bytes, then it is allocated two bytes
xdata ints1 {0x52,0x53,0x54,0x00005555,0x5656U};
xdata ints2 {82,83,84,21845,22101};

//**** 4 byte length data, if an integer requires 4 bytes,
//****  then it is allocated 4 bytes. Floats are treated as
//****  such and given 4 bytes.
xdata longs {0x57575757L,0x58585858UL,0x59595959,1515870810};
xdata floats {1.0,2.0,(float)3,40e-01,5e00,.6e1};

typedef struct myData_tag {
   char           c0;   // '\x88'
   long          *s1;   // myString
   unsigned long  u1;   // 88UL
   long          *s2;   // myStrings
   int            i1;   // 0x33
} myData_type;

xstring myString {"Hello"};
xstring myStrings {"one", "two", "three", "four"};

// Note that in xdata statements, xstring references (e.g. myString, myStrings)
//  have type long * even if explicitly cast to unsigned long type.
xdata myData {'\x88', myString, 88UL, myStrings, 0x33};


void main()
{
   static float farray[NFLOAT];
   static int i;
   static long physAddr, xStrAddr;
   static myData_type xData;

   printf("xdata starts at 0x%06lx (physical)\n\n", chars1);

   //**** Copy float data to root array.
   xmem2root(farray, floats, sizeof(float) * NFLOAT);

   for (i = 0; i < NFLOAT; ++i)
   {
      printf("farray[%d] = %f\n", i, farray[i]);
   }

   xmem2root(&xData, myData, sizeof(xData));  // read xdata into struct

   printf("\nRoot address of myString's xmem address is 0x%04x\n", xData.s1);
   xStrAddr = *xData.s1;                      // xmem address of myString
   printf("myString's xmem address is 0x%06lx\n", xStrAddr);
   xmem2root(&physAddr, xStrAddr, sizeof(physAddr));
   printf("myString[0] = \"%ls\" at 0x%06lx\n\n", physAddr, physAddr);

   printf("Root address of myStrings' xmem address is 0x%04x\n", xData.s2);
   xStrAddr = *xData.s2;                      // xmem array of string ptrs
   printf("myStrings' xmem address is 0x%06lx\n", xStrAddr);
   for (i = 0; i < 4; ++i)
   {
      xmem2root(&physAddr, xStrAddr + i*sizeof(xStrAddr), sizeof(physAddr));
      printf("myStrings[%d] = \"%ls\" at 0x%06lx\n", i, physAddr, physAddr);
   }
}

