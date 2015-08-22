/******************************************************************************
Sample demonstrates how the #pragma WARNIO can be used to help find ioe or ioi
accesses that may not work in system/user mode.
/*****************************************************************************/

main() {
#pragma WARNIO on
   #asm
   	ioi ld (hl), a
      ioe ld (0x44), a
   #endasm
#pragma WARNIO off
   #asm
   	ioi ld (hl), a
      ioe ld (0x44), a
   #endasm
}