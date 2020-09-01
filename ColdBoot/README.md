This directory contains the source code for the binary files found in
`BIOS/`.  Use `build.bat` to compile a single file at a time.  The COLDLOAD
files require `makecold.exe`, which you can build with gcc:

    g++ makecold.cpp -o makecold.exe

Note that in the intial release of these files, `COLDLOAD.C` generates the
equivalent `Bios/COLDLOAD.BIN` file from the Dynamic C 9.62 release, but
`pilot.c` generates a different `Bios/pilot.bin`.

The differences in `Bios/pilot.bin` are due to an update to `_FlashWait()`
in `BIOSLIB/FLASHWR.LIB`.  Adding 11 `nop` opcodes after the `ret` in the
updated version results in differences limited to that section of the `.bin`
file, as confirmed by checking addresses in the `pilot.map` file (address
0x7106 corresponds to offset 0x1106 in the file).

The `Bios/RAMonly*.bin` files were for potential (non-Digi) hardware with a
single RAM chip (on /CS0, /OE0 and /WE0) and no flash memory.  It's possible
to build `Bios/RAMonlyColdload.bin` by changing `RAMCONTROLOPTION` to `0`
in `makecold.cpp`.  Defining `RAMONLYPILOT` and building the pilot BIOS
*might* create an equivalent `Bios/RAMonlypilot.bin`, but without knowing
the version of Dynamic C used to build that file, it's not possible to verify.

