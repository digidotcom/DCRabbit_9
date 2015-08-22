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
/**********************************************************************
    PowerCoreFLEX_Board_Options.c

  This program is designed to view board configuration
  and to setup targetless compile options
  for the PowerCore FLEX board series.  Since the board features vary
  on the PowerCore FLEX, this program must be used to setup targetless
  compile options for your board.  Use the following procedure to
  create a Dynamic C project to create binary files for your
  PowerCore FLEX series board(s):

  1. Run this program and then copy the output from the
  stdio window into your system clipboard (Ctrl-C) or turn on
  stdio logging to capture the output to a file (select the
  "Debug Windows" tab from the "Environment Options" dialog
  and check the "Log to File" checkbox for Stdio).
  2. Save a copy of your PowerCore FLEX project file by selecting from
  the menu bar File->Project->Save As.  Save the project as
  "PowerCoreFLEX_targetless.dcp".  You will use this project file
  whenever you want to create binary files.
  3. From the menu bar, select Options->Project Options.  The project
  options dialog will appear.
  4. Select the tab marked "Compiler" from the "Project Options" dialog.
  5. Under "Default Compile Mode", select the radio button marked
  "Compile defined target configuration to .bin file"
  6. Select the tab marked "Defines" (still in the "Project Options"
  dialog).
  7. Paste the macro definitions (the output of this program) into
  the "Global Macro Definitions" text box.
  8. Select the tab marked "Targetless" (still in the "Project Options"
  dialog)
  9. Select the "Board Selection" sub-tab, select board from the list
  labeled:
  "PowerCore FLEX Board Series, options vary, see PowerCoreFLEX_Board_Options.c"
  10. Click the "OK" button to commit the changes.
  11. Save the project file again (File->Project->Save).
  12. You are now done setting up the project file for targetless
  compiles for your PowerCore FLEX series board.

  NOTE!!:  Compiling using this new project file will generate
  warning messages for invalid macro redefinitions.  This behavior
  is expected (ignore these warnings).

**********************************************************************/

#if !POWERCOREFLEX_SERIES
	#error "This program is intended only for PowerCore FLEX series boards."
#endif

int _GetDevSpecIndex(int n, MemDevSpec *devspec);

main() {
	auto int i;
   auto unsigned long value;
   auto char buf[SYS_MACRO_LENGTH];
	auto MemDevSpec devspec;

   // Define board specific macros necessary for targetless compile
   // as defined by the system id block.
	printf("_FLASH_SIZE_=0x%x;\n", _FLASH_SIZE_);
   printf("_RAM_SIZE_=0x%x;\n", _RAM_SIZE_);
   printf("_CRYSTAL_SPEED_=%ld;\n", (unsigned long)_CRYSTAL_SPEED_);
   printf("_FREQ_DIV_19200_=%ld;\n", (unsigned long)_FREQ_DIV_19200_);
   printf("_BIOSBAUD_=%ld;\n", (unsigned long)_BIOSBAUD_);

   printf("_DC_MD0_=%d;",             _DC_MD0_);
   printf("_DC_MD0_ID_=0x%08LX;", 	  _DC_MD0_ID_);
   printf("_DC_MD0_TYPE_=0x%04X;\n", 	  _DC_MD0_TYPE_);
   printf("_DC_MD0_SIZE_=0x%04X;", 	  _DC_MD0_SIZE_);
   printf("_DC_MD0_SECSIZE_=0x%04X;", _DC_MD0_SECSIZE_);
   printf("_DC_MD0_SECNUM_=0x%04X;\n",  _DC_MD0_SECNUM_);
   printf("_DC_MD0_SPEED_=0x%04X;",   _DC_MD0_SPEED_);
   printf("_DC_MD0_MBC_=0x%04X;\n",   _DC_MD0_MBC_);

   printf("_DC_MD1_=%d;",             _DC_MD1_);
   printf("_DC_MD1_ID_=0x%08LX;", 	  _DC_MD1_ID_);
   printf("_DC_MD1_TYPE_=0x%04X;\n", 	  _DC_MD1_TYPE_);
   printf("_DC_MD1_SIZE_=0x%04X;", 	  _DC_MD1_SIZE_);
   printf("_DC_MD1_SECSIZE_=0x%04X;", _DC_MD1_SECSIZE_);
   printf("_DC_MD1_SECNUM_=0x%04X;\n",  _DC_MD1_SECNUM_);
   printf("_DC_MD1_SPEED_=0x%04X;",   _DC_MD1_SPEED_);
   printf("_DC_MD1_MBC_=0x%02X;\n",   _DC_MD1_MBC_);

   printf("_DC_MD2_=%d;",             _DC_MD2_);
   printf("_DC_MD2_ID_=0x%08LX;", 	  _DC_MD2_ID_);
   printf("_DC_MD2_TYPE_=0x%04X;\n", 	  _DC_MD2_TYPE_);
   printf("_DC_MD2_SIZE_=0x%04X;", 	  _DC_MD2_SIZE_);
   printf("_DC_MD2_SECSIZE_=0x%04X;", _DC_MD2_SECSIZE_);
   printf("_DC_MD2_SECNUM_=0x%04X;\n",  _DC_MD2_SECNUM_);
   printf("_DC_MD2_SPEED_=0x%04X;",   _DC_MD2_SPEED_);
   printf("_DC_MD2_MBC_=0x%02X;\n",   _DC_MD2_MBC_);

  	if(SysIDBlock.idBlock2.macrosLoc) {
  		i = 0;
     	while(_GetSysMacroIndex(i, buf, &value) == 0) {
         printf("%09s=0x%08LX;\n", buf, value);
         i++;
      }
   }
  	if(SysIDBlock.idBlock2.devSpecLoc) {
  		i = 0;
     	while(_GetDevSpecIndex(i, &devspec) == 0) {
         printf("_DC_MD%x_=%d;",             i+3, 	 1);
         printf("_DC_MD%x_ID_=0x%08LX;",     i+3,   devspec.ID);
         printf("_DC_MD%x_TYPE_=0x%04X;\n",    i+3,   devspec.type);
         printf("_DC_MD%x_SIZE_=0x%04X;",    i+3,   devspec.devSize);
         printf("_DC_MD%x_SECSIZE_=0x%04X;", i+3,   devspec.sectorSize);
         printf("_DC_MD%x_SECNUM_=0x%04X;\n",  i+3,   devspec.numSectors);
         printf("_DC_MD%x_SPEED_=0x%04X;",   i+3,   devspec.devSpeed);
         printf("_DC_MD%x_MBC_=0x%02X;\n",   i+3,   devspec.MBC);
         i++;
      }
   }

}

int _GetDevSpecIndex(int n, MemDevSpec *devspec) {
   auto long devspecloc;
   auto uint16 len;
   auto int i;

   // Calculate location of macro table
	devspecloc = IDBlockAddr - SysIDBlock.idBlock2.devSpecLoc;
   // Get the length of the macro table
   len = xgetint(devspecloc);
   // End of macro table is used to terminate loop below
   devspecloc += 2;

   // Check that id block is defined, that macro table is version 0
   // and that the length of the table is appropriate
   if(IDBlockAddr == 0 || (char)xgetint(devspecloc) != 0 || len > 256) {
   	return -2;
   }
   devspecloc++;

	// Calculate number of dev spec entries in table (-3 for is header of table)
   len = (len - 3)/sizeof(MemDevSpec);

   n = n * sizeof(MemDevSpec);
   if(n > len) {
   	return -1;
   }

   devspecloc += n;
   xmem2root(devspec, devspecloc, sizeof(MemDevSpec));

   return 0;
}

