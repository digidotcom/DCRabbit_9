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
/*** IMPORTANT NOTICE ***

   This ID block utility program has not been verified nor validated by passing
   through Rabbit's Engineering Systems Assurance test process.  It is provided
   as is and without any warranty.  The end User assumes all responsibility
   for running this utility on a Rabbit board.

   In particular, writing an incorrect ID block onto a Rabbit board can make
   the board virtually inoperable until a correct ID block is restored.  In
   this case, a knowledgeable end User who diligently follows the detailed
   instructions herein should be able to restore a correct ID block to the
   given Rabbit board.

 *** IMPORTANT NOTICE ***/

#if CC_VER < 0x0920 || CC_VER > 0x0962
	#error "Unsupported Dynamic C version!"
	#error "This write_idblock_920_962.c utility program works only with Dynamic"
	#error " C versions 9.20 through 9.62, inclusive."
 #if CC_VER < 0x0653
	#fatal "This version of Dynamic C does not recognize ID or User blocks."
 #elif CC_VER < 0x0920
	#fatal "Use the write_idblock_653_910.c utility instead."
 #else
	#fatal "Use the appropriate later version of the write_idblock_X_Y.c utility."
 #endif
#endif

/*************************************************************************
   write_idblock_920_962.c - version 1.03


   Writes product System ID block(s) to top of flash and verifies.  This
   version is intended only for Dynamic C 9.20 through 9.62, inclusive.

   This program is intended to write a System ID block on boards that
   don't have a System ID block or to update (replace) an existing System
   ID block with a different version ID block.

 ************************************************************************


 Instructions:


 0. Do not just skip past the rest of these instructions after
    encountering some optional steps.  In particular, if your target
    board is an RCM3200 or other board that is capable of "Code and
    BIOS in Flash, Run in RAM" compile mode, pay special attention to
    the last few steps in this list of instructions.


 1. Add or edit or uncomment your own MYBOARD macro for a custom board,
    or uncomment the appropriate Z-World board MYBOARD macro below, for
    the board that you are writing a new System ID block onto.  If your
    board is a FLEXSBC type, uncomment the MYBOARDISFLEXSBC macro in
    addition to uncommenting the appropriate one of the POWERCOREFLEX
    (RCM3800-based) or POWERCOREFLEXA (RCM3810-based) MYBOARD macro.


 2. (Only required if the target is both ethernet capable and its MAC
    address is stored in its ID block.  Note that Z-World's OP6700 and
    TCP/IP Development Kit boards store their MAC address in an onboard
    EEPROM.  When HASEEPROM is set to 1 --- the default value for these
    two boards --- this utility will not write a MAC address into the ID
    block.)  If the target is ethernet capable, enter its MAC address.
    For Z-World boards, the first three digits are fixed and the last
    three digits should be on a sticker on the board.


 3. (Optional on most boards but required for FLEXSBC.)  Uncomment and set
    the SERIALNUMBER macro to an appropriate string value.  For FLEXSBC
    boards this should be the specific design / serial number.  If the
    length of the SERIALNUMBER macro string including the zero terminator
    character is longer than sizeof(SysIDBlock.serialNumber) then
    SERIALNUMBER will be truncated without termination when it is copied
    into the ID block.


 4. (Optional.)  Uncomment and set the PRODUCTNAME macro to an appropriate
    string value.  If the length of the PRODUCTNAME macro string including
    the zero terminator character is longer than
    sizeof(SysIDBlock.productName) then PRODUCTNAME will be truncated
    without termination when it is copied into the ID block.


 5. (Optional.)  Set the NUMUSER4KBLOCKS macro to 0 for no User block, or
    to the number of 4K blocks in the desired User block size.  EG:  For
    an 8 KB User block, set NUMUSER4KBLOCKS to 2.  If this macro is not
    defined, a default value will be used based on the definition of
    IDBLOCK_VERSION.


 6. (Optional.)  Requires setting the IDBLOCK_VERSION macro to 5 or
    greater.  Set the NUMRESERVED4KBLOCKS macro to 0 for an ID block
    with no user-defined reserved persistent data area (the sys ID block
    will still be written), or to the number of 4K blocks in the desired
    ID block plus reserved area.  Note that if DRIVERFILE, IODESCFILE, or
    IOPERMFILE is defined, NUMRESERVED4KBLOCKS must be defined to be
    greater than 0.  EG:  For a 16 KB ID block plus reserved area, set
    NUMRESERVED4KBLOCKS to 4.  If this macro is not defined, a default
    value will be used based on the definition of IDBLOCK_VERSION.

    IDBLOCK_VERSION >= 5 && NUMRESERVED4KBLOCKS > 0:
                                                            Top of FLASH
      ------------------------------------------------------------------
         | User Block B | User Block A | Persistent Data + Sys ID Block |
      ------------------------------------------------------------------
         ^sector        ^sector        ^sector
          boundary       boundary       boundary


    IDBLOCK_VERSION == [3,5] && (NUMRESERVED4KBLOCKS == 0):
                                                            Top of FLASH
      ------------------------------------------------------------------
        | User Block B + Sys ID Block B | User Block A + Sys ID Block A |
      ------------------------------------------------------------------
         ^sector                        ^sector
          boundary                       boundary


 7. (Optional.)  Uncomment and set the IDBLOCK_VERSION macro to a
    number from 1 through 5, inclusive.  0 is not allowed; 1 is an ID
    block with at most a simulated separate User block; 2 is
    unmirrored combined ID plus User blocks; 3 and 4 are mirrored
    combined ID plus User blocks; 5 requires Dynamic C version 9.20 or
    greater and is required if using a macro table or a memory device
    specification table or pre-loaded drivers or an I/O description
    table or an I/O permissions table; 6 and greater are not allowed.


 8. (Optional.  Requires setting the IDBLOCK_VERSION macro to 5 or
    greater, and also requires the User to provide a suitable pre-
    loaded drivers binary file.)  Uncomment exactly one of the provided
    DRIVERFILE macros, or leave the provided examples all commented-out
    and add or edit or uncomment your own DRIVERFILE macro definition
    to suit your board.


 9. (Optional.  Requires setting the IDBLOCK_VERSION macro to 5 or
    greater, and also requires the User to provide a suitable I/O
    descriptions binary file.)  Uncomment exactly one of the provided
    IODESCFILE macros, or leave the provided examples all commented-out
    and add or edit or uncomment your own IODESCFILE macro definition
    to suit your board.


10. (Optional.  Requires setting the IDBLOCK_VERSION macro to 5 or
    greater, and also requires the User to provide a suitable I/O
    permissions binary file.)  Uncomment exactly one of the provided
    IOPERMFILE macros, or leave the provided examples all commented-out
    and add or edit or uncomment your own IOPERMFILE macro definition
    to suit your board.


11. (Optional.)  If a custom board, uncomment and set the HAS2FLASHES
    macro to either 0 (one flash installed) or 1 (two flashes
    installed).


12. (Optional.)  If a custom board, uncomment and set the HASEEPROM
    macro to either 0 (no EEPROM installed and used for ethernet MAC
    address storage) or 1 (EEPROM installed and used for ethernet MAC
    address storage).


13. (Optional.)  If a custom board, uncomment and set the HASETHERNET
    macro to either 0 (no ethernet capability) or 1 (ethernet
    capability).


14. (Only required if the target is an RCM3200 or other "Code and BIOS
    in Flash, Run in RAM" compile mode capable board that does not
    already have a correct ID Block containing an appropriate product
    ID code.)  Add "_BOARD_TYPE_=RCM3200" (or other appropriate product
    ID code, without the quotes, and noting that there are no spaces
    within the quotes) to the Project Options' Defines box.

15. Run the program and follow the prompts in the STDIO window.  The
    contents of the ID block will be printed, or any run-time errors
    reported, to the STDIO window.


16. (Only required if the target is an RCM3200 or other "Code and BIOS
    in Flash, Run in RAM" compile mode capable board that does not
    already have a correct ID Block containing an appropriate product
    ID code.)  Ignore the warning about the redefined _BOARD_TYPE_
    macro, and remove the "_BOARD_TYPE_=RCM3200" (or other appropriate
    product ID code) from the Project Options' Defines box.

 ************************************************************************


	History:


	1.03
		- Added CPCE "BL2600Q patch" (BL2600 + RCM3900) support.
	1.02
		- Added support for RCM32x9, RCM33x9, RCM39x0 boards.
		- Renamed from write_idblock_920_952.c to write_idblock_920_962.c.
		- Restricted supported Dynamic C compiler version range from 9.20
		  through 9.62, inclusive.
	1.01
		- Renamed from write_idblock_920_up.c to write_idblock_920_952.c.
		- Restricted supported Dynamic C compiler version range from 9.20
		  through 9.52, inclusive.
		- Removed Rabbit 4000, SMCxxxx references.
		- Removed previous history specific to write_idblock.c.
	1.00
		- Roughly corresponds to write_idblock.c version 3.26.  Removed
		  RCM44xxx, RCM45xxx references.


 ************************************************************************/

#define VERSION "1.03"
#define MY_PROGRAM_NAME "write_idblock_920_962.c"

/*************************************************************************
** STEP 00:  Do not just skip past the rest of these instructions after **
** encountering some optional steps.  In particular, if your target     **
** board is an RCM3200 or other board that is capable of "Code and BIOS **
** in Flash, Run in RAM" compile mode, pay special attention to the     **
** last few steps in this list of instructions.                         **
*************************************************************************/


/*************************************************************************
** STEP 01: Add or edit or uncomment your own MYBOARD macro for a       **
** custom board, or uncomment the appropriate Z-World board MYBOARD     **
** macro below, for the board that you are writing a new System ID      **
** block onto.  If your board is a FLEXSBC type, uncomment the          **
** MYBOARDISFLEXSBC macro in addition to uncommenting the appropriate   **
** one of the POWERCOREFLEX (RCM3800-based) or POWERCOREFLEXA           **
** (RCM3810-based) MYBOARD macro.                                       **
*************************************************************************/


//*** Custom Board Product IDs ***//

//#define	MYBOARD 0xFF00		// add custom board 0xFF00's description here
//#define	MYBOARD 0xFF01		// add custom board 0xFF01's description here
// . . .
//#define	MYBOARD 0xFFFF		// add custom board 0xFFFF's description here


//*** RabbitCore Modules ***//

//#define	MYBOARD RCM2000	// 25MHz R2000, 256K flash, 512k SRAM
//#define	MYBOARD RCM2010	// 25MHz R2000, 256K flash, 128k SRAM
//#define	MYBOARD RCM2020	// 18MHz R2000, 256K flash, 128K SRAM

//#define	MYBOARD RCM2100	// 22MHz R2000, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD RCM2110	// 22MHz R2000, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD RCM2115	// 22MHz R2000, 256K flash, 128K SRAM, ethernet (no RJ45)
//#define	MYBOARD RCM2120	// 22MHz R2000, 2x256K flash, 512K SRAM
//#define	MYBOARD RCM2130	// 22MHz R2000, 256K flash, 128K SRAM

//#define	MYBOARD RCM2200	// 22MHz R2000, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD RCM2210	// 22MHz R2000, 256K flash, 128K SRAM, ethernet (no RJ45)
//#define	MYBOARD RCM2250	// 22MHz R2000, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD RCM2260A	// 22MHz R2000, 2x256K flash, 512K SRAM, ethernet (no RJ45)

//#define	MYBOARD RCM2300	// 22MHz R2000, 256K flash, 128K SRAM

//#define	MYBOARD RCM3000	// 29MHz R3000, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD RCM3010	// 29MHz R3000, 256K flash, 128K SRAM, ethernet

//#define	MYBOARD RCM3100	// 29MHz R3000, 2x256K flash, 512K SRAM
//#define	MYBOARD RCM3110	// 29MHz R3000, 256K flash, 128K SRAM

//#define	MYBOARD RCM3200	// 44MHz R3000, 512K flash, 256K+512K SRAM, ethernet
//#define	MYBOARD RCM3209	// 44MHz R3000, 512K flash, 256K+512K SRAM, ethernet
//#define	MYBOARD RCM3210A	// 29MHz R3000, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD RCM3220A	// 44MHz R3000, 512K flash, 256K+512K SRAM
//#define	MYBOARD RCM3229	// 44MHz R3000, 512K flash, 256K+512K SRAM, ethernet

//#define	MYBOARD RCM3300A	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 8M serial flash
//#define	MYBOARD RCM3305	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 8M serial flash
//#define	MYBOARD RCM3309	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 8MB serial flash
//#define	MYBOARD RCM3310A	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 4M serial flash
//#define	MYBOARD RCM3315	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 4M serial flash
//#define	MYBOARD RCM3319	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 4MB serial flash
//#define	MYBOARD RCM3350A
//#define	MYBOARD RCM3360A	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 16M nand flash, xD Card
//#define	MYBOARD RCM3360B	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 16M nand flash
//#define	MYBOARD RCM3370A	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, xD Card
//#define	MYBOARD RCM3365	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 16M nand flash, xD Card
//#define	MYBOARD RCM3375	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, xD Card

//#define	MYBOARD RCM3400A	// 29MHz R3000, 512K flash, 512K SRAM, ethernet on PB, analog
//#define	MYBOARD RCM3410A	// 29MHz R3000, 256K flash, 256K SRAM, ethernet on PB, analog
//#define	MYBOARD RCM3420A	// Reserved
//#define	MYBOARD RCM3450	// Reserved

//TinyCore
//#define	MYBOARD RCM3600A	// 22MHz R3000, 512K flash, 512K SRAM
//#define	MYBOARD RCM3600LP	// Reserved
//#define	MYBOARD RCM3610A	// 22MHz R3000, 256K flash, 128K SRAM
//#define	MYBOARD RCM3660	// Reserved
//Hobby
//#define	MYBOARD RCM3600H	// 22MHz R3000, 512K flash, 512K SRAM
//#define	MYBOARD RCM3610H	// 22MHz R3000, 256K flash, 128K SRAM

//Ethernet TinyCore
//#define	MYBOARD RCM3700A	// 22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash
//#define	MYBOARD RCM3700LP	// Reserved
//#define	MYBOARD RCM3710A	// 22MHz R3000, 256K flash, 128K SRAM, ethernet, 1M serial flash
//#define	MYBOARD RCM3720A	// 22MHz R3000, 512K flash, 256K SRAM, ethernet, 1M serial flash
//#define	MYBOARD RCM3750	// 22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash
//#define	MYBOARD RCM3760	// 22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash, 3.3V
//Hobby Ethernet
//#define	MYBOARD RCM3700H	// 22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash
//#define	MYBOARD RCM3710H	// 22MHz R3000, 256K flash, 128K SRAM, ethernet, 1M serial flash
//#define	MYBOARD RCM3720H	// 22MHz R3000, 512K flash, 256K SRAM, ethernet, 1M serial flash

//PowerCore FLEX
//#define	MYBOARD POWERCOREFLEX	// Varies (RCM3800 predefined model)
//#define	MYBOARD POWERCOREFLEXA	// Varies (RCM3810 predefined model)
//#define	MYBOARDISFLEXSBC	// Varies, based on POWERCOREFLEX/A base type

//R3000 Replacement Cores:
//#define	MYBOARD RCM3900	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 32MB nand flash, SD socket
//#define	MYBOARD RCM3910	// 44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, SD socket


//*** Single Board Computers ***//

//Jackrabbit
//#define	MYBOARD BL1800		// 29MHz R2000, 256K flash, 128K SRAM, RS485, battery
//#define	MYBOARD BL1805		// 29MHz R2000, 256K flash, 512K SRAM, RS485, battery
//#define	MYBOARD BL1810		// 14MHz R2000, 128K flash, 128K SRAM, RS485, battery
//#define	MYBOARD BL1820		// 14MHz R2000, 128K flash, 128k SRAM

//Wildcat
//#define	MYBOARD BL2000		// 22MHz R2000, 256K flash, 128K SRAM, ethernet, high-quality analog
//#define	MYBOARD BL2000B	// 22MHz R2000, 512K flash, 512K SRAM, ethernet, high-quality analog
//#define	MYBOARD BL2010		// 22MHz R2000, 256K flash, 128K SRAM, ethernet, analog
//#define	MYBOARD BL2010B	// 22MHz R2000, 256K flash, 512K SRAM, ethernet, analog
//#define	MYBOARD BL2020		// 22MHz R2000, 256K flash, 128K SRAM, high-quality analog
//#define	MYBOARD BL2030		// 22MHz R2000, 256K flash, 128K SRAM, analog
//#define	MYBOARD BL2040		// 22MHz R2000, 256K flash, 128K SRAM

//Smart Cat
//#define	MYBOARD BL2100		// 22MHz RCM2200, 256K flash, 128K SRAM, ethernet, analog
//#define	MYBOARD BL2101		// 22MHz RCM2200, 256K flash, 128K SRAM, ethernet, 10V analog
//#define	MYBOARD BL2105		// 22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet, analog
//#define	MYBOARD BL2110		// 22MHz RCM2200, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD BL2111		// 22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet, 10V analog
//#define	MYBOARD BL2115		// 22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD BL2120		// 22MHz RCM2300, 256K flash, 128K SRAM, analog
//#define	MYBOARD BL2121		// 22MHz RCM2300, 256K flash, 128K SRAM, 10V analog
//#define	MYBOARD BL2130		// 22MHz RCM2300, 256K flash, 128K SRAM

//RabbitNet
//#define	MYBOARD BL2500A	// 29MHz RCM3010, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD BL2500B	// 29MHz RCM3000, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD BL2500C	// 44MHz RCM3200, 512K flash, 256K+512K SRAM, ethernet
//#define	MYBOARD BL2510A	// 29MHz RCM3110, 256K flash, 128K SRAM
//#define	MYBOARD BL2510B	// 29MHz RCM3100, 2x256K flash, 512K SRAM

//#define	MYBOARD BL2600A	// 44MHz RCM3200, 512K flash, 256K+512K SRAM, ethernet
//#define	MYBOARD BL2600B	// 29MHz RCM3000, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD BL2600C	// 29MHz RCM3010, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD BL2600D	// Reserved
//#define	MYBOARD BL2600K	// Reserved
//#define	MYBOARD BL2600L	// Reserved
//#define	MYBOARD BL2600M //   44MHz RCM3365, 512K flash, 512K+512K SRAM, ethernet, // 16 Mb Nand Flash, xD Connector
//#define	MYBOARD BL2600N	// 44MHz RCM3375, 512K flash, 512K+512K SRAM, ethernet, xD Connector
//#define	MYBOARD BL2600Q	// 44MHz RCM3900, 512K flash, 512K+512K SRAM, ethernet, 32MB nand flash, SD socket
//#define	MYBOARD BL2610A	// 29MHz RCM3100, 2x256K flash, 512K SRAM
//#define	MYBOARD BL2610B	// 29MHz RCM3110, 256K flash, 128K SRAM

//Fox Low-Power Controller
//#define	MYBOARD LP3500		// 7MHz R3000, 2x256K flash, 512K SRAM, analog, relay
//#define	MYBOARD LP3510		// 7MHz R3000, 256K flash, 128K SRAM


//*** Embedded Control Systems ***//

//SmartStar
//#define	MYBOARD SR9100		// 29MHz R2000, 2x256K flash, 128K SRAM

//MiniStar
//#define	MYBOARD SR9150		// 22MHz R2000, 2x256K flash, 128K SRAM, ethernet
//#define	MYBOARD SR9160		// 22MHz R2000, 2x256K flash, 128K SRAM


//*** Operator Interfaces ***//

//Intellicom
//#define	MYBOARD OP6600		// 18MHz R2000, 256K flash, 128K SRAM
//#define	MYBOARD OP6700		// 18MHz R2000, 2x256K flash, 256K SRAM, ethernet

//Minicom
//#define	MYBOARD OP6800		// 22MHz RCM2200, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD OP6805		// 22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet
//#define	MYBOARD OP6810		// 22MHz RCM2300, 256K flash, 128K SRAM

//eDisplay
//#define	MYBOARD OP7200		// 22MHz RCM2200, 256K flash, 128K SRAM, ethernet, analog, touchscreen
//#define	MYBOARD OP7200B	// 22MHZ RCM2250, 2x256K flash, 512K SRAM, ethernet, analog, touchscreen
//#define	MYBOARD OP7210		// 22MHz RCM2200, 256K flash, 128K SRAM, ethernet
//#define	MYBOARD OP7210B	// 22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet


//*** Other Products ***//

//Ethernet Modem
//#define	MYBOARD EM1500A	// 44MHz RCM3200, 512K flash, 256K+512K SRAM, ethernet

//RabbitLink
//#define	MYBOARD EG2110A	// 22MHz R2000, 2x256K flash, 128K SRAM, ethernet, 3V
//#define	MYBOARD RABLINK	// 22MHz R2000, 2x256K flash, 128K SRAM, ethernet

//TCP/IP Development Kit
//#define	MYBOARD RTDK		// 18MHz R2000, 2x256K flash, 128K SRAM, ethernet


/*************************************************************************
** STEP 02:  (Only required if the target is both ethernet capable and  **
** its MAC address is stored in its ID block.  Note that Z-World's      **
** OP6700 and TCP/IP Development Kit boards store their MAC address in  **
** an onboard EEPROM.  When HASEEPROM is set to 1 --- the default value **
** for these two boards --- this utility will not write a MAC address   **
** into the ID block.)  If the target is ethernet capable, enter its    **
** MAC address.  For Z-World boards, the first three digits are fixed   **
** and the last three digits should be on a sticker on the board.       **
*************************************************************************/


const char newMAC[6] = { 0x00, 0x90, 0xC2,
                         0x00, 0x00, 0x00 };


/*************************************************************************
** STEP 03:  (Optional on most boards but required for FLEXSBC.)        **
** Uncomment and set the SERIALNUMBER macro to an appropriate string    **
** value.  For FLEXSBC boards this should be the specific design /      **
** serial number.  If the length of the SERIALNUMBER macro string       **
** including the zero terminator character is longer than               **
** sizeof(SysIDBlock.serialNumber) then SERIALNUMBER will be truncated  **
** without termination when it is copied into the ID block.             **
*************************************************************************/


//#define SERIALNUMBER "Serial / Design Number"


/*************************************************************************
** STEP 04: (Optional.)  Uncomment and set the PRODUCTNAME macro to an  **
** appropriate string value.  If the length of the PRODUCTNAME macro    **
** string including the zero terminator character is longer than        **
** sizeof(SysIDBlock.productName) then PRODUCTNAME will be truncated    **
** without termination when it is copied into the ID block.             **
*************************************************************************/


//#define PRODUCTNAME "Product Name"


/*************************************************************************
** STEP 05:  (Optional.)  Set the NUMUSER4KBLOCKS macro to 0 for no     **
** User block, or to the number of 4K blocks in the desired User block  **
** size.  EG:  For an 8 KB User block, set NUMUSER4KBLOCKS to 2.        **
*************************************************************************/


//#define	NUMUSER4KBLOCKS	4


/*************************************************************************
** STEP 06:  (Optional.)  Set the NUMRESERVED4KBLOCKS macro to 0 for an **
** ID block with no reserved persistent data area, or to the number of  **
** 4K blocks in the desired ID block plus reserved area. EG:  For a 16  **
** KB ID block plus reserved area, set NUMRESERVED4KBLOCKS to 4.        **
*************************************************************************/


//#define	NUMRESERVED4KBLOCKS	0


/*************************************************************************
** STEP 07:  (Optional.)  Uncomment and set the IDBLOCK_VERSION macro   **
** to a number from 1 through 5, inclusive.  0 is not allowed; 1 is an  **
** ID block with at most a simulated separate User block; 2 is          **
** unmirrored combined ID plus User blocks; 3 and 4 are mirrored        **
** combined ID plus User blocks; 5 requires Dynamic C version 9.20 or   **
** greater and is required if using a macro table or a memory device    **
** specification table or preloaded drivers or an I/O description table **
** or an I/O permissions table; 6 and greater are not allowed.          **
*************************************************************************/


//#define	IDBLOCK_VERSION	5


/*************************************************************************
** STEP 08:  (Optional.  Requires setting the IDBLOCK_VERSION macro to  **
** 5 or greater, and also requires the User to provide a suitable       **
** preloaded drivers binary file.)  Uncomment exactly one of the        **
** provided DRIVERFILE macros, or leave the provided examples all       **
** commented-out and add or edit or uncomment your own DRIVERFILE macro **
** definition to suit your board.                                       **
*************************************************************************/


//#define DRIVERFILE "rcm3365_asix_sst39vf040.bin"


/*************************************************************************
** STEP 09:  (Optional.  Requires setting the IDBLOCK_VERSION macro to  **
** 5 or greater, and also requires the User to provide a suitable I/O   **
** descriptions binary file.)  Uncomment exactly one of the provided    **
** IODESCFILE macros, or leave the provided examples all commented-out  **
** and add or edit or uncomment your own IODESCFILE macro definition to **
** suit your board.                                                     **
*************************************************************************/


//#define IODESCFILE "RCM3365IODesc.bin"


/*************************************************************************
** STEP 10:  (Optional.  Requires setting the IDBLOCK_VERSION macro to  **
** 5 or greater, and also requires the User to provide a suitable I/O   **
** permissions binary file.)  Uncomment exactly one of the provided     **
** IOPERMFILE macros, or leave the provided examples all commented-out  **
** and add or edit or uncomment your own IOPERMFILE macro definition to **
** suit your board.                                                     **
*************************************************************************/


//#define IOPERMFILE  "RCM3365IOPerm.bin"


/*************************************************************************
** STEP 11:  (Optional.)  If a custom board, uncomment and set the      **
** HAS2FLASHES macro to either 0 (one flash installed) or 1 (two        **
** flashes installed).                                                  **
*************************************************************************/


//#define	HAS2FLASHES	1


/*************************************************************************
** STEP 12:  (Optional.)  If a custom board, uncomment and set the      **
** HASEEPROM macro to either 0 (no EEPROM installed and used for        **
** ethernet MAC address storage) or 1 (EEPROM installed and used for    **
** ethernet MAC address storage).                                       **
*************************************************************************/


//#define	HASEEPROM	0


/*************************************************************************
** STEP 13:  (Optional.)  If a custom board, uncomment and set the      **
** HASETHERNET macro to either 0 (no ethernet capability) or 1          **
** (ethernet capability).                                               **
*************************************************************************/


//#define	HASETHERNET	1


/*************************************************************************
** STEP 14:  (Only required if the target is an RCM3200 or other "Code  **
** and BIOS in Flash, Run in RAM" compile mode capable board that does  **
** not already have a correct ID Block containing an appropriate        **
** product ID code.)  Add "_BOARD_TYPE_=RCM3200" (or other appropriate  **
** product ID code, without the quotes, and noting that there are no    **
** spaces within the quotes) to the Project Options' Defines box.       **
*************************************************************************/


/*************************************************************************
** STEP 15:  Run the program and follow the prompts in the STDIO        **
** window.  The contents of the ID block will be printed, or any run-   **
** time errors reported, to the STDIO window.                           **
*************************************************************************/


/*************************************************************************
** STEP 16:  (Only required if the target is an RCM3200 or other "Code  **
** and BIOS in Flash, Run in RAM" compile mode capable board that does  **
** not already have a correct ID Block containing an appropriate        **
** product ID code.)  Ignore the warning about the redefined            **
** _BOARD_TYPE_ macro, and remove the "_BOARD_TYPE_=RCM3200" (or other  **
** appropriate product ID code) from the Project Options' Defines box.  **
*************************************************************************/


/*************************************************************************
** END OF USER-CUSTOMIZABLE SECTION                                     **
*************************************************************************/

// Define this macro if compiling this program to a .bin file.  Defining this
// macro will force the MAC ID already present in the system ID block to be
// used.


//#define RETAIN_MAC_ID


// Defining this macro causes the MAC address to be the only output from this
// program.


//#define FACTORY_INSTALL


// check for some potential problems


#ifdef COMPILE_SECONDARY_PROG
#error "This utility is not intended for use as a secondary (DLP) program."
#error "Must remove COMPILE_SECONDARY_PROG macro from Compiler Options Defines box."
#fatal "Must also type Ctrl-Y to recompile the BIOS."
#endif


#ifdef COMPILE_PRIMARY_PROG
#error "This utility is not intended for use as a primary (DLM) program."
#error "Must remove COMPILE_PRIMARY_PROG macro from Compiler Options' Defines box."
#fatal "Must also type Ctrl-Y to recompile the BIOS."
#endif


// check for undefined register macros
//  (R2000C and up has these, prior R2000 versions always return 0s)


#ifndef GCPU
#define GCPU 0x2E
#endif


#ifndef GREV
#define GREV 0x2F
#endif


// check for undefined board types


#ifndef BL1800
	#define BL1800		0x0101
#endif
#ifndef BL1805
	#define BL1805		0x0103
#endif
#ifndef BL1810
	#define BL1810		0x0100
#endif
#ifndef BL1820
	#define BL1820		0x0102
#endif
#ifndef BL2000
	#define BL2000		0x0800
#endif
#ifndef BL2000B
	#define BL2000B	0x0806
#endif
#ifndef BL2010
	#define BL2010		0x0801
#endif
#ifndef BL2010B
	#define BL2010B	0x0805
#endif
#ifndef BL2020
	#define BL2020		0x0802
#endif
#ifndef BL2030
	#define BL2030		0x0803
#endif
#ifndef BL2040
	#define BL2040		0x0804
#endif
#ifndef BL2100
	#define BL2100		0x0B00
#endif
#ifndef BL2101
	#define BL2101		0x0B06
#endif
#ifndef BL2105
	#define BL2105		0x0B04
#endif
#ifndef BL2110
	#define BL2110		0x0B01
#endif
#ifndef BL2111
	#define BL2111		0x0B08
#endif
#ifndef BL2115
	#define BL2115		0x0B05
#endif
#ifndef BL2120
	#define BL2120		0x0B02
#endif
#ifndef BL2121
	#define BL2121		0x0B07
#endif
#ifndef BL2130
	#define BL2130		0x0B03
#endif
#ifndef BL2500A
	#define BL2500A	0x1500
#endif
#ifndef BL2500B
	#define BL2500B	0x1502
#endif
#ifndef BL2500C
	#define BL2500C	0x1504
#endif
#ifndef BL2510A
	#define BL2510A	0x1501
#endif
#ifndef BL2510B
	#define BL2510B	0x1503
#endif
#ifndef BL2600A
	#define BL2600A	0x1700
#endif
#ifndef BL2600B
	#define BL2600B	0x1701
#endif
#ifndef BL2600C
	#define BL2600C	0x1704
#endif
#ifndef BL2600D
	#define BL2600D	0x1703
#endif
#ifndef BL2600K
	#define BL2600K	0x170A
#endif
#ifndef BL2600L
	#define BL2600L	0x170B
#endif
#ifndef BL2600M
	#define BL2600M	0x170C
#endif
#ifndef BL2600N
	#define BL2600N	0x170D
#endif
#ifndef BL2600Q
	#define BL2600Q	0x170F
#endif
#ifndef BL2610A
	#define BL2610A	0x1702
#endif
#ifndef BL2610B
	#define BL2610B	0x1705
#endif
#ifndef EG2110A
	#define EG2110A	0x0603
#endif
#ifndef EM1500A
	#define EM1500A	0x1800
#endif
#ifndef FLEXSBC
	#define FLEXSBC	0x2400
#endif
#ifndef LP3500
	#define LP3500		0x1200
#endif
#ifndef LP3510
	#define LP3510		0x1201
#endif
#ifndef OP6600
	#define OP6600		0x0300
#endif
#ifndef OP6700
	#define OP6700		0x0302
#endif
#ifndef OP6800
	#define OP6800		0x0D00
#endif
#ifndef OP6805
	#define OP6805		0x0D02
#endif
#ifndef OP6810
	#define OP6810		0x0D01
#endif
#ifndef OP7200
	#define OP7200		0x1100
#endif
#ifndef OP7200B
	#define OP7200B	0x1102
#endif
#ifndef OP7210
	#define OP7210		0x1101
#endif
#ifndef OP7210B
	#define OP7210B	0x1103
#endif
#ifndef POWERCOREFLEX
	#define POWERCOREFLEX 0x2300
#endif
#ifndef POWERCOREFLEXA
	#define POWERCOREFLEXA 0x2301
#endif
#ifndef RABLINK
	#define RABLINK	0x0601
#endif
#ifndef RCM2000
	#define RCM2000	0x0200
#endif
#ifndef RCM2010
	#define RCM2010	0x0201
#endif
#ifndef RCM2020
	#define RCM2020	0x0202
#endif
#ifndef RCM2100
	#define RCM2100	0x0700
#endif
#ifndef RCM2110
	#define RCM2110	0x0701
#endif
#ifndef RCM2115
	#define RCM2115	0x0704
#endif
#ifndef RCM2120
	#define RCM2120	0x0702
#endif
#ifndef RCM2130
	#define RCM2130	0x0703
#endif
#ifndef RCM2200
	#define RCM2200	0x0900
#endif
#ifndef RCM2210
	#define RCM2210	RCM2200
#endif
#ifndef RCM2250
	#define RCM2250	0x0E00
#endif
#ifndef RCM2260A
	#define RCM2260A	0x0901
#endif
#ifndef RCM2300
	#define RCM2300	0x0A00
#endif
#ifndef RCM3000
	#define RCM3000	0x0F00
#endif
#ifndef RCM3010
	#define RCM3010	0x0F01
#endif
#ifndef RCM3100
	#define RCM3100	0x1000
#endif
#ifndef RCM3110
	#define RCM3110	0x1001
#endif
#ifndef RCM3200
	#define RCM3200	0x1300
#endif
#ifndef RCM3209
	#define RCM3209	0x2D20
#endif
#ifndef RCM3210A
	#define RCM3210A	0x1301
#endif
#ifndef RCM3220A
	#define RCM3220A	0x1302
#endif
#ifndef RCM3229
	#define RCM3229	0x2D21
#endif
#ifndef RCM3300A
	#define RCM3300A	0x1400
#endif
#ifndef RCM3305
	#define RCM3305	0x1408
#endif
#ifndef RCM3310A
	#define RCM3310A	0x1402
#endif
#ifndef RCM3309
	#define RCM3309	0x2D30
#endif
#ifndef RCM3315
	#define RCM3315	0x1409
#endif
#ifndef RCM3319
	#define RCM3319	0x2D31
#endif
#ifndef RCM3350A
	#define RCM3350A	0x1401
#endif
#ifndef RCM3360A
	#define RCM3360A	0x1403
#endif
#ifndef RCM3360B
	#define RCM3360B	0x1405
#endif
#ifndef RCM3365
	#define RCM3365	0x1406
#endif
#ifndef RCM3370A
	#define RCM3370A	0x1404
#endif
#ifndef RCM3375
	#define RCM3375	0x1407
#endif
#ifndef RCM3400A
	#define RCM3400A	0x1600
#endif
#ifndef RCM3410A
	#define RCM3410A	0x1610
#endif
#ifndef RCM3420A
	#define RCM3420A	0x1602
#endif
#ifndef RCM3450
	#define RCM3450	0x1604
#endif
#ifndef RCM3600A
	#define RCM3600A	0x1E00
#endif
#ifndef RCM3600H
	#define RCM3600H	0x1E20
#endif
#ifndef RCM3600LP
	#define RCM3600LP	0x1E02
#endif
#ifndef RCM3610A
	#define RCM3610A	0x1E01
#endif
#ifndef RCM3610H
	#define RCM3610H	0x1E21
#endif
#ifndef RCM3660
	#define RCM3660	0x1E03
#endif
#ifndef RCM3700A
	#define RCM3700A	0x1F00
#endif
#ifndef RCM3700H
	#define RCM3700H	0x1F20
#endif
#ifndef RCM3700LP
	#define RCM3700LP	0x1F03
#endif
#ifndef RCM3710A
	#define RCM3710A	0x1F01
#endif
#ifndef RCM3710H
	#define RCM3710H	0x1F21
#endif
#ifndef RCM3720A
	#define RCM3720A	0x1F02
#endif
#ifndef RCM3720H
	#define RCM3720H	0x1F22
#endif
#ifndef RCM3750
	#define RCM3750	0x1F04
#endif
#ifndef RCM3760
	#define RCM3760	0x1F05
#endif
#ifndef RCM3900
	#define RCM3900	0x2D00
#endif
#ifndef RCM3910
	#define RCM3910	0x2D01
#endif
#ifndef RTDK
	#define RTDK		0x0301
#endif
#ifndef SR9100
	#define SR9100		0x0502
#endif
#ifndef SR9150
	#define SR9150		0x0C00
#endif
#ifndef SR9160
	#define SR9160		0x0C01
#endif


// ensure that some particular macros are defined, or are defined appropriately


#ifndef MYBOARD
	#fatal "MYBOARD must be set to the appropriate board type."
#endif


#ifdef MYBOARDISFLEXSBC
 #if MYBOARD != POWERCOREFLEX && MYBOARD != POWERCOREFLEXA
	#error "For a FLEXSBC board the MYBOARD macro must be defined as either the"
	#fatal " POWERCOREFLEX (RCM3800) or the POWERCOREFLEXA (RCM3810) base type."
 #endif
 #ifndef SERIALNUMBER
	#fatal "Must define SERIALNUMBER to FLEXSBC's  serial / design number!"
 #endif
#endif


#ifndef IDBLOCK_VERSION
	// these choices are generalizations; your board's mileage may vary
 #if CPU_ID_MASK(_CPU_ID_) == R2000
	#define IDBLOCK_VERSION 2
 #elif (MYBOARD & 0xFF00) == (POWERCOREFLEX & 0xFF00)
	#define IDBLOCK_VERSION 5
 #else
	#define IDBLOCK_VERSION 4
 #endif
#endif


#ifndef NUMUSER4KBLOCKS
 #if IDBLOCK_VERSION < 5 || (MYBOARD & 0xFF00) == (POWERCOREFLEX & 0xFF00)
	#define NUMUSER4KBLOCKS 4
 #else
	#define NUMUSER4KBLOCKS 2
 #endif
#endif


#ifndef NUMRESERVED4KBLOCKS
 #if IDBLOCK_VERSION < 5 || (MYBOARD & 0xFF00) == (POWERCOREFLEX & 0xFF00)
	#define NUMRESERVED4KBLOCKS 0
 #else
	#define NUMRESERVED4KBLOCKS 4
 #endif
#endif


#if NUMUSER4KBLOCKS < 0 || NUMUSER4KBLOCKS > 16
	#fatal "NUMUSER4KBLOCKS must be set from 0 to 16 (0K to 64K), inclusive."
#endif


#if IDBLOCK_VERSION < 1 || IDBLOCK_VERSION > 5
	#fatal "If used, IDBLOCK_VERSION must be set from 1 to 5, inclusive."
#endif


#if IDBLOCK_VERSION < 5 || NUMRESERVED4KBLOCKS <= 0
 #ifdef DRIVERFILE
	#fatal "DRIVERFILE requires IDBLOCK_VERSION >= 5, NUMRESERVED4KBLOCKS > 0."
 #endif
 #ifdef IODESCFILE
	#fatal "IODESCFILE requires IDBLOCK_VERSION >= 5, NUMRESERVED4KBLOCKS > 0."
 #endif
 #ifdef IOPERMFILE
	#fatal "IOPERMFILE requires IDBLOCK_VERSION >= 5, NUMRESERVED4KBLOCKS > 0."
 #endif
#endif


#ifndef _DFN_SIZE
	#undef _DF_SIZE_MASK
	#define _DF_SIZE_MASK 0xFF000000
	#define _DFN_SIZE(dcdfn) (0xFF & ((dcdfn & _DF_SIZE_MASK) >> 24))
#endif
#ifndef _DFN_SUBTYPE
	#undef _DF_SUB_MASK
	#define _DF_SUB_MASK 0x0000FFFF
	#define _DFN_SUBTYPE(dcdfn) (dcdfn & _DF_SUB_MASK)
#endif
#ifndef _DFN_TYPE
	#define _DFN_TYPE(dcdfn) (0xFF & ((dcdfn & _DF_TYPE_MASK) >> 16))
#endif
#ifndef _DFN_TYPE_NAND
	#undef _DF_TYPE_NAND
	#define _DF_TYPE_NAND _DFN_TYPE_NAND(_DC_DFLASH0_)
	#define _DFN_TYPE_NAND(dcdfn) (_DFN_TYPE(dcdfn) == 0x02)
#endif
#ifndef _DFN_TYPE_SER
	#undef _DF_TYPE_SER
	#define _DF_TYPE_SER _DFN_TYPE_SER(_DC_DFLASH0_)
	#define _DFN_TYPE_SER(dcdfn) (_DFN_TYPE(dcdfn) == 0x01)
#endif


#if CC_VER >= 0x0930
	#define dkDebugTag dkcDebugTag
#endif


// some useful macros


#ifndef HAS2FLASHES
 #if ((MYBOARD == BL2105) || (MYBOARD == BL2111) || (MYBOARD == BL2115))
	#define HAS2FLASHES 1
 #elif ((MYBOARD == BL2500B) || (MYBOARD == BL2510B))
	#define HAS2FLASHES 1
 #elif ((MYBOARD == BL2600B) || (MYBOARD == BL2610A))
	#define HAS2FLASHES 1
 #elif (MYBOARD == EG2110A)
	#define HAS2FLASHES 1
 #elif (MYBOARD == LP3500)
	#define HAS2FLASHES 1
 #elif (MYBOARD == OP6600)
	#define HAS2FLASHES 1
 #elif (MYBOARD == OP6700)
	#define HAS2FLASHES 1
 #elif (MYBOARD == OP6805)
	#define HAS2FLASHES 1
 #elif ((MYBOARD == OP7200B) || (MYBOARD == OP7210B))
	#define HAS2FLASHES 1
 #elif (MYBOARD == RABLINK)
	#define HAS2FLASHES 1
 #elif ((MYBOARD == RCM2100) || (MYBOARD == RCM2120))
	#define HAS2FLASHES 1
 #elif ((MYBOARD == RCM2250) || (MYBOARD == RCM2260A))
	#define HAS2FLASHES 1
 #elif (MYBOARD == RCM3000)
	#define HAS2FLASHES 1
 #elif (MYBOARD == RCM3100)
	#define HAS2FLASHES 1
 #elif (MYBOARD == RTDK)
	#define HAS2FLASHES 1
 #elif ((MYBOARD == SR9100) || (MYBOARD == SR9150) || (MYBOARD == SR9160))
	#define HAS2FLASHES 1
 #else
	#define HAS2FLASHES 0
 #endif
#endif


#ifndef HASEEPROM
 #if (MYBOARD == OP6700)
	#define HASEEPROM 1
 #elif (MYBOARD == RTDK)
	#define HASEEPROM 1
 #else
	#define HASEEPROM 0
 #endif
#endif


#ifndef HASETHERNET
 #if ((MYBOARD == BL2000) || (MYBOARD == BL2000B))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2010) || (MYBOARD == BL2010B))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2100) || (MYBOARD == BL2101) || (MYBOARD == BL2105))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2110) || (MYBOARD == BL2111) || (MYBOARD == BL2115))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2500A) || (MYBOARD == BL2500B) || (MYBOARD == BL2500C))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2600A) || (MYBOARD == BL2600B) || (MYBOARD == BL2600C))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2600D) || (MYBOARD == BL2600K) || (MYBOARD == BL2600L))
	#define HASETHERNET 1
 #elif ((MYBOARD == BL2600M) || (MYBOARD == BL2600N) || (MYBOARD == BL2600Q))
	#define HASETHERNET 1
 #elif (MYBOARD == EG2110A)
	#define HASETHERNET 1
 #elif (MYBOARD == EM1500A)
	#define HASETHERNET 1
 #elif (MYBOARD == OP6700)
	#define HASETHERNET 1
 #elif ((MYBOARD == OP6800) || (MYBOARD == OP6805))
	#define HASETHERNET 1
 #elif ((MYBOARD == OP7200) || (MYBOARD == OP7200B))
	#define HASETHERNET 1
 #elif ((MYBOARD == OP7210) || (MYBOARD == OP7210B))
	#define HASETHERNET 1
 #elif (MYBOARD == POWERCOREFLEX)
	#define HASETHERNET 1
 #elif (MYBOARD == RABLINK)
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM2100) || (MYBOARD == RCM2110) || (MYBOARD == RCM2115))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM2200) || (MYBOARD == RCM2210) || (MYBOARD == RCM2250))
	#define HASETHERNET 1
 #elif (MYBOARD == RCM2260A)
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3000) || (MYBOARD == RCM3010))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3200) || (MYBOARD == RCM3210A))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3209) || (MYBOARD == RCM3229))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3300A) || (MYBOARD == RCM3310A) || (MYBOARD == RCM3350A))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3305) || (MYBOARD == RCM3315))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3309) || (MYBOARD == RCM3319))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3360A) || (MYBOARD == RCM3360B) || (MYBOARD == RCM3370A))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3365) || (MYBOARD == RCM3375))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3400A) || (MYBOARD == RCM3410A) || (MYBOARD == RCM3420A))
	#define HASETHERNET 1
 #elif (MYBOARD == RCM3450)
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3700A) || (MYBOARD == RCM3700H))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3710A) || (MYBOARD == RCM3710H))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3720A) || (MYBOARD == RCM3720H))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3700LP) || (MYBOARD == RCM3750) || (MYBOARD == RCM3760))
	#define HASETHERNET 1
 #elif ((MYBOARD == RCM3900) || (MYBOARD == RCM3910))
	#define HASETHERNET 1
 #elif (MYBOARD == RTDK)
	#define HASETHERNET 1
 #elif (MYBOARD == SR9150)
	#define HASETHERNET 1
 #else
	#define HASETHERNET 0
 #endif
#endif


#define VALIDMARKER "\x55\xAA\x55\xAA\x55\xAA"


// possible #ximport of preloaded drivers, I/O descriptions and I/O permissions


#ifdef DRIVERFILE
// #ximport <file path and name> pl_drivers
   #ximport DRIVERFILE pl_drivers
#else
   #define pl_drivers 0L
#endif

#ifdef IODESCFILE
// #ximport <file path and name> io_description
   #ximport IODESCFILE io_description
#else
   #define io_description 0L
#endif

#ifdef IOPERMFILE
// #ximport <file path and name> io_permissions
   #ximport IOPERMFILE io_permissions
#else
   #define io_permissions 0L
#endif


// definitions and data structures for creating the macro table


#define MACRO_NAME_SIZE 9
typedef struct {
	char macro[MACRO_NAME_SIZE];
   long value;
} mac_tab_entry_t;

#define MAC_ENTRY_SIZE sizeof(mac_tab_entry_t)
#define MAC_TABLE_SIZE (256/MAC_ENTRY_SIZE)
#define MACENCODE(VAL, POS) ((VAL) << (POS))

mac_tab_entry_t macroTable[MAC_TABLE_SIZE];
int macroTableCount;


// definitions and data structures for creating the memory device table


#define MEM_SPEC_TABLE_SIZE 256/sizeof(MemDevSpec)

MemDevSpec memDevSpecTable[MEM_SPEC_TABLE_SIZE];
int memSpecTableCount;

typedef struct {
   uint8 *pos;					// current position in buffer (pos-buffer+1) is number of bytes to write)
  	uint8 buffer[0x4000];
} blockbuffer_t;


// version 5 ID block board-specific definitions and data structures


typedef struct {
	unsigned int boardseries;		// Product series
	unsigned int ethernet;        // 0=None, 1=Realtek, 2=SMSC, 3=ASIX, 4=R4K Ethernet

	unsigned int fastram;         // FastRam size in 4K blocks
	unsigned int fastramSpeed;    // Access time in ns

	unsigned int sram;            // Battery SRAM size in 4K blocks
   unsigned int sramSpeed;			// Access time in ns

	unsigned int pflash1;         // Parallel Flash 1 size in 4K blocks
   unsigned int pflash1Speed;		// Access time in ns

	unsigned int pflash2;         // Parallel Flash 2 size in 4K blocks
   unsigned int pflash2Speed;		// Access time in ns

	unsigned int sflash;          // Serial Flash size in 1M blocks
	unsigned long cpuclk;   		// CLK speed in hertz
	unsigned int clkdoubler;      // 0 = CLK not doubled 1 = Double CLK
	unsigned int adcramp;         // 0 = Not installed, 1 = installed
	unsigned int zeroxing;        // 0 = Not installed, 1 =installed
	unsigned int pwrtype;       	// 0 = Full-Wave or DC, 1 = CT Full-Wave,
											// 2 = Half-Wave, 3 = External regulated +5v
                           		// 4 = Unregulated DC
   unsigned int pwrconnector;		// 0 = Molex, 1 = 2 x 25 connector
	unsigned int interposer;      // 0 = not installed 1 = installed
					    					// (circuit can be used for Wi-Fi)
	unsigned int psrating;        // Current in milli-amps ( a value of 1000 means 1A)
	unsigned int battery;   		// 0 = Not installed 1 = installed

/* start of (so far) RCM4xxx-specific section */
   unsigned int flash16;			// 16-Bit Flash: 0 = Not installed 1 = installed
   unsigned int ram16;				// 16-Bit RAM: 0 = Not installed 1 = installed
   unsigned int adc;					// Onboard ADC IC: number of ADC bits 12 or 14
   unsigned int nand;				// Onboard NAND Flash: size in 1M blocks
   unsigned int pb1unavail;		// prog port PB1 (CLKA) is not available:
											//  0 = available (for backwards compatibility)
											//  1 = not available
/* end of (so far) RCM4xxx-specific section */

   unsigned int crc;					// Checksum, must be last entry in structure.
} rcm38xxinfo;

// Create instance of structure
rcm38xxinfo testinfo;


#define _NO_ENET  	0
#define _REALTEK_ETH 1
#define _SMSC_ETH		2
#define _ASIX_ETH		3
#define _R4K_ETH  	4
#define _WIFIB_ETH	5


blockbuffer_t SysIDBuffer;


// use xmem as much as possible
#memmap xmem

// function prototypes
void add_dev_spec(MemDevSpec* entry);
void add_macro_entry(char* macname, unsigned long value);
uint32 build_dev_spec(uint8 version, uint8* dest, MemDevSpec *src);
uint32 build_macro_table(uint8 version, uint8* dest, mac_tab_entry_t *src);
uint32 build_sysid_block(uint8* dest);
int calcCRC(int, void *, int);
int getCPUID(void);
int GetFlashID(void);
int IDBlockPLDIOCheck(void);
void PrintFlashDescription(char *, int, int);
void PrintProductDescription(int);
void PrintSysIDBlockInfo(void);
void RAM_GetUserBlockInfo(struct userBlockInfo *uBI);
void setTimeStamp(unsigned long);
int set_bd_configuration(char * packet);
void set_board_info(rcm38xxinfo *testinfo);
void showFlashWriteResults(int i, char *msg);
int writeIDBlock(void);
int writeToFlash(long what, long where, long xlen);
int _GetDevSpecIndex(int n, MemDevSpec *devspec);
char *_GetDFlashType(unsigned dfType);
void _updateBIOSIDUserBlocksInfo(void);


// global variables


// global temporary storage for either the new or the retained MAC address
char mac2write[6];


/********************************************************************/


int set_bd_configuration(char * packet)
{
	// initialize the testinfo structure to all zero bytes
	memset(&testinfo, 0, sizeof(testinfo));

	#if (MYBOARD == POWERCOREFLEX)
	   // Here's the configuration for PowerCore3800 predefined model:
	   #ifdef MYBOARDISFLEXSBC
	   testinfo.boardseries = FLEXSBC;  // Product series
	   #else
	   testinfo.boardseries = POWERCOREFLEX;  // Product series
	   #endif
	   testinfo.ethernet = _REALTEK_ETH;            // 0=None, 1=Realtek, 2=SMSC, 3=ASIX
	   testinfo.fastram = 0x80;               // Size in 4K blocks
	   testinfo.fastramSpeed = 15;            // Access speed in ns

	   testinfo.sram = 0x80;                  // Size in 4K blocks
	   testinfo.sramSpeed = 55;               // Access speed in ns

	   testinfo.pflash1 = 0x80;               // Size in 4K blocks
	   testinfo.pflash1Speed = 45;            // Access speed in ns

	   testinfo.pflash2 = 0;                  // Size in 4K blocks
	   testinfo.pflash2Speed = 0;             // Access speed in ns

	   testinfo.sflash = 1;                   // Size in 1M blocks
	   testinfo.cpuclk = 51600000;            // CLK speed in hertz
	   testinfo.clkdoubler = 1;               // 0 = CLK not doubled 1 = Double CLK
	   testinfo.adcramp = 1;                  // 0 = Not installed, 1 = installed
	   testinfo.zeroxing = 1;                 // 0 = Not installed, 1 =installed
	   testinfo.pwrtype = 4;                  // 1 = External regulated +5v 2 = Unregulated DC
	                                          // 3 = FW bridge, 4 = FW CT  5 = HW
	   testinfo.pwrconnector = 0;             // 0 = Molex, 1 = 2 x 25 connector
	   testinfo.interposer = 0;               // 0 = not installed 1 = installed
	                                          // (circuit can be used for Wi-Fi)
	   testinfo.psrating = 2000;              // Current in milli-amps ( a value of 1000 means 1A)
	   testinfo.battery = 1;                  // 0 = Battery not installed 1 = battery installed

	#elif (MYBOARD == POWERCOREFLEXA)
	   //Here's the configuration for PowerCore3810 predefined model:
	   #ifdef MYBOARDISFLEXSBC
	   testinfo.boardseries = FLEXSBC;  // Product series
	   #else
	   testinfo.boardseries = POWERCOREFLEXA; // Product series
	   #endif
	   testinfo.ethernet = _NO_ENET;          // 0=None, 1=Realtek, 2=SMSC, 3=ASIX
	   testinfo.fastram = 0;                  // Size in 4K blocks
	   testinfo.fastramSpeed = 0;             // Access speed in ns

	   testinfo.sram  = 0x40;                 // Size in 4K blocks
	   testinfo.sramSpeed = 55;               // Access speed in ns

	   testinfo.pflash1 = 0x80;               // Size in 4K blocks
	   testinfo.pflash1Speed = 45;            // Access speed in ns

	   testinfo.pflash2 = 0;                  // Size in 4K blocks
	   testinfo.pflash2Speed = 0;             // Access speed in ns

	   testinfo.sflash = 0;                   // Size in 1M blocks
	   testinfo.cpuclk = 25800000;            // CLK speed in hertz
	   testinfo.clkdoubler = 0;               // 0 = CLK not doubled 1 = Double CLK
	   testinfo.adcramp = 1;                  // 0 = Not installed, 1 = installed
	   testinfo.zeroxing = 1;                 // 0 = Not installed, 1 =installed
	   testinfo.pwrtype = 4;                  // 1 = External regulated +5v 2 = Unregulated DC
	                                          // 3 = FW bridge, 4 = FW CT  5 = HW
	   testinfo.pwrconnector = 0;             // 0 = Molex, 1 = 2 x 25 connector
	   testinfo.interposer = 0;               // 0 = not installed 1 = installed
	                                          // (circuit can be used for Wi-Fi)
	   testinfo.psrating = 1000;              // Current in milli-amps ( a value of 1000 means 1A)
	   testinfo.battery = 1;                  // 0 = Battery not installed 1 = battery installed

	#endif

	// calculate the CRC for the structure, including CRC intialized to zero
	testinfo.crc = calcCRC(0x0000, &testinfo, sizeof(testinfo));
	return 1;
}


#define CS2_RAM			 	0x06	// CS2/OE1/WE1
#define CS1_RAM				0x05	// CS1/OE1/WE1


#ifdef WSTATES0
 #if 0xc0 == WSTATES0
	/*
		A lot of trouble to ultra-safely get rid of a warning.
		Some versions of Dynamic C libraries don't have these definitions,
		 other versions have WSTATES0 defined to 0xc0 instead of to 0xC0.
	*/
	#undef WSTATES0
 #endif
#endif
	#define WSTATES0 0xC0
	#define WSTATES1 0x80
	#define WSTATES2 0x40
	#define WSTATES4 0x00


void set_board_info(rcm38xxinfo *testinfo)
{
	uint16 dflashsize;
   uint16 temp;
   uint32 acopts;
   uint32 dcopts;
   uint32 psrating;
	MemDevSpec memspec;
   char	div19200;

  	unsigned long brd_optx;

   // Set board series
	SysIDBlock.productID =  testinfo->boardseries;

   SysIDBlock.cpuID = getCPUID();
	div19200 = getdivider19200();
	SysIDBlock.crystalFreq = div19200 * 614400ul;

   // possibly fill the MAC address
	if (testinfo->ethernet) {
		memcpy(SysIDBlock.macAddr, mac2write, sizeof(SysIDBlock.macAddr));
	}

   // Read and set 1st flash parameters at runtime
	SysIDBlock.flashID = (unsigned int)GetFlashID();
	SysIDBlock.flashSize = 	testinfo->pflash1;
	SysIDBlock.flashType = _FlashInfo.writeMode;
	SysIDBlock.sectorSize = _FlashInfo.sectorSize;
	SysIDBlock.numSectors = _FlashInfo.numSectors;
	SysIDBlock.flashSpeed = testinfo->pflash1Speed;

	// Read and set 2nd flash parameters at runtime
	if (testinfo->pflash2) {
		// we assume that the 2nd flash (if installed)
		// is identical to the first one
		SysIDBlock.flash2ID = SysIDBlock.flashID;
		SysIDBlock.flash2Size = testinfo->pflash2;
		SysIDBlock.flash2Type = SysIDBlock.flashType;
		SysIDBlock.sector2Size = SysIDBlock.sectorSize;
		SysIDBlock.num2Sectors = SysIDBlock.numSectors;
		SysIDBlock.flash2Speed = testinfo->pflash2Speed;
	}

	SysIDBlock.ramID = 0x00;  // Memory device ID is SRAM

  	// primary ram is fast ram if present
   if(testinfo->fastram) {
   	// Each of the RCM3800 (PowerCore FLEX)
   	//  can have fastram
      // Their respective configurations only differ by wait states
      SysIDBlock.ramSize = testinfo->fastram;
      SysIDBlock.ramSpeed = testinfo->fastramSpeed;
      SysIDBlock.idBlock2.ramMBC = WSTATES0 | CS2_RAM;
      // specify second RAM
      memspec.ID = 0;               // Z-World part #, 0 = SRAM, other = flash id
      memspec.type = 0;             // Write method
      memspec.devSize = testinfo->sram;         // in 1000h pages
      memspec.sectorSize = 0;                   // size of flash sector in bytes
      memspec.numSectors = 0;                   // number of sectors
      memspec.devSpeed = testinfo->sramSpeed;   // in nanoseconds
      memspec.MBC = WSTATES2 | CS1_RAM;

      add_dev_spec(&memspec);

      SysIDBlock.flashSize = testinfo->pflash1;
		SysIDBlock.flash2Size = testinfo->pflash2;

		// all previous FAST_RAM_COMPILE boards, including PowerCore FLEX
		SysIDBlock.idBlock2.flashMBC = WSTATES1 | CS_FLASH;
		SysIDBlock.idBlock2.flash2MBC = WSTATES1 | CS_FLASH2;
	}
   else {
		SysIDBlock.ramSize = testinfo->sram;
		SysIDBlock.ramSpeed = testinfo->sramSpeed;
		SysIDBlock.flashSize = testinfo->pflash1;
		SysIDBlock.flash2Size = testinfo->pflash2;

		// PowerCore
		SysIDBlock.ramID = 0x00;  // Memory device ID is SRAM
		SysIDBlock.idBlock2.ramMBC = WSTATES0 | CS1_RAM;
		SysIDBlock.idBlock2.flashMBC = WSTATES0 | CS_FLASH;
		SysIDBlock.idBlock2.flash2MBC = WSTATES0 | CS_FLASH2;
   }

   // Change testinfo->pwrtype value from:
   // 1 = External regulated +5v 2 = Unregulated DC,
   // 3 = FW bridge, 4 = FW CT  5 = HW
   //
   // Change to:
   // 0 = Full-Wave or DC, 1 = CT Full-Wave, 2 = Half-Wave,
   // 3 = External regulated +5v, 4 = Unregulated DC
   //
   // This re-mapping of values is due to difference between
   // DC macro implementation and customer PS option selection.

   switch(testinfo->pwrtype)
   {
      case 1:
         testinfo->pwrtype = 3;
         break;

      case 2:
         testinfo->pwrtype = 4;
         break;

      case 3:
         testinfo->pwrtype = 0;
         break;

      case 4:
         testinfo->pwrtype = 1;
         break;

      case 5:
         testinfo->pwrtype = 2;
         break;
   }

   // if acopts is < 3 then dcopts will already be correctly set to 0.
   // dcopts is only meaningful if acopts equals 3, otherwise ignore dcopts.
   acopts = testinfo->pwrtype;
   dcopts = 0;

   if(acopts >= 3) {
      if(acopts == 3)
         dcopts = 0;
      else
         dcopts = 1;
      acopts = 3;
   }

   // Divide by 1000 to get into 3 bits (supports 0-7 A).
   psrating = testinfo->psrating/1000;

   /*
        bits    0-2     Power supply options (0 None, 1 or 2 A)
        bits    3-4     0 = Full-Wave, 1 = CT Full-Wave,
                        2 = Half-Wave, 3 = DC (check bits 10-11)
        bit     5       Zero Crossing (0 not installed, 1 installed)
        bit     6       Ramp Generator (0 not installed, 1 installed)
        bit     7       Battery (0 not installed, 1 installed)
        bit     8       Clock double okay (0 don't double, 1 okay to double)
        bits    9       Interposer (0 = not installed 1 = installed)
                        (Interposer circuit can be used for Wi-Fi)
        bits    10-11   Power type (0 = External regulated +5v, 1 = Unregulated DC)
        bit     12      Power connector (0 = Molex, 1 = 2 x 25 connector)
        bits    13-31   Reserved for future use (set to 0 for now)
   */
   psrating               &= 7;
   acopts                 &= 3;
   testinfo->zeroxing     &= 1;
   testinfo->adcramp      &= 1;
   testinfo->battery      &= 1;
   testinfo->clkdoubler   &= 1;
   testinfo->interposer   &= 1;
   dcopts                 &= 3;
   testinfo->pwrconnector &= 1;
   add_macro_entry("3800OPT0",
                   MACENCODE((uint32) psrating,                 0)  |
                   MACENCODE((uint32) acopts,                   3)  |
                   MACENCODE((uint32) (testinfo->zeroxing),     5)  |
                   MACENCODE((uint32) (testinfo->adcramp),      6)  |
                   MACENCODE((uint32) (testinfo->battery),      7)  |
                   MACENCODE((uint32) (testinfo->clkdoubler),   8)  |
                   MACENCODE((uint32) (testinfo->interposer),   9)  |
                   MACENCODE((uint32) dcopts,                   10) |
                   MACENCODE((uint32) (testinfo->pwrconnector), 12)
                   );
   /*
        bits 0-15 -  Compile time configuration descriptor: This value is used
                     during compile time, by libraries and drivers, to determine
                     which driver(s) to include and to determine how those
                     drivers are configured.
        bits 16-26 - first bit describes whether or not drivers are pre-loaded,
                     subsequent bits are unused and reserved.
        bits 27-31 - Runtime information: These bits are used during runtime to
                     determine the actual ethernet hardware configuration of a
                     particular board for proper runtime initiliazation of
                     ethernet driver(s) on that particular board.
   */

   /*
	   0x00001000 for Realtek PowerCore
		0x08001000 for SMSC PowerCore
    */
	switch(testinfo->ethernet) {
   case _NO_ENET:
   	break;
	case _REALTEK_ETH:
	   add_macro_entry("ETH0", 0x00001000);
      break;
	case _SMSC_ETH:
	   add_macro_entry("ETH0", 0x08001000);
      break;
	case _ASIX_ETH:
		break;
	case _R4K_ETH:
		break;
   default:
   	break;
   }

	/*
   	_DC_DFLASH0_
        bits    0-15    Subtype of data flash
        bits    16-23   Type of flash
        bits    24-31   Size of flash as a power of 2, supports from 2^0 - 2^255
	 */
	temp = testinfo->sflash;
   // if size is zero then don't define DFLASH0
   if(temp) {
   	// 2^20 is 1 megabyte
	   dflashsize = 20;
   	while(temp >>= 1) {
      	dflashsize++;
      }
	   add_macro_entry("DFLASH0",	MACENCODE(1, 0L) |				// subtype
      									MACENCODE(1, 16L) |         // type
      									MACENCODE(dflashsize, 24L)  // size as a power of 2
											);
   }
}


void main(void)
{
	static int i, crc;
	static struct userBlockInfo uBI;
	int saveMB1or3CR;
	char ch, div19200val;
   uint32 length;
   uint32 flashOfst, flashTop;
   uint32 idblockaddr, ublockAaddr, ublockBaddr, markeraddr;
   int idblocksector, ublockAsector, ublockBsector;

   length = 0;
#ifndef FACTORY_INSTALL
   printf("%s, version %s.\n", MY_PROGRAM_NAME, VERSION);
#else
	if(IDBlockPLDIOCheck()) {
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
		       SysIDBlock.macAddr[0], SysIDBlock.macAddr[1], SysIDBlock.macAddr[2],
		       SysIDBlock.macAddr[3], SysIDBlock.macAddr[4], SysIDBlock.macAddr[5]);
      exit(0);
   }
	memcpy(mac2write, SysIDBlock.macAddr, sizeof(mac2write));
#endif

#if (HASETHERNET && !HASEEPROM)
	memcpy(mac2write, newMAC, sizeof(mac2write));	// default to the new MAC
	if (SysIDBlock.productID) {
 #ifdef RETAIN_MAC_ID
		memcpy(mac2write, SysIDBlock.macAddr, sizeof(mac2write));	// use old MAC
 #else
		printf("\nRetain the %02X:%02X:%02X:%02X:%02X:%02X MAC address (Y/N)? ",
		       SysIDBlock.macAddr[0], SysIDBlock.macAddr[1], SysIDBlock.macAddr[2],
		       SysIDBlock.macAddr[3], SysIDBlock.macAddr[4], SysIDBlock.macAddr[5]);
		ch = toupper(getchar());
		if (ch != 'N') {
			ch = 'Y';	// force any other character to 'Y'
			memcpy(mac2write, SysIDBlock.macAddr, sizeof(mac2write));	// old MAC
		}
		printf("%c\n", ch);
 #endif
	}
#endif


	//////////////////////////////////////////////////
	// map MB1CR or MB3CR quadrant to flash on /CS0
	// make sure A18 NOT inverted in MB1CR or MB3CR (handles bank select)


#if FLASH_COMPILE
	saveMB1or3CR = MB1CRShadow;
	WrPortI(MB1CR, &MB1CRShadow, FLASH_WSTATES | 0x00);
	_InitFlashDriver(0x01 | 0x02);
#else
	saveMB1or3CR = MB3CRShadow;
	WrPortI(MB3CR, &MB3CRShadow, FLASH_WSTATES | 0x00);
	_InitFlashDriver(0x08);
#endif

	// Add macro table entries here
   SysIDBuffer.pos = SysIDBuffer.buffer;

	//////////////////////////////////////////////////
	// set up the ID block fields
	memset(&SysIDBlock, 0x00, sizeof(SysIDBlock));

	SysIDBlock.tableVersion = IDBLOCK_VERSION;

	SysIDBlock.vendorID = 1;

	setTimeStamp(dc_timestamp);

	if(SysIDBlock.tableVersion < 5) {
    	SysIDBlock.idBlockSize = sizeof(SysIDBlock)-sizeof(SysIDBlockType2);
   }
   else {
   	SysIDBlock.idBlockSize = sizeof(SysIDBlock);
   }

	// mark the ID block "valid"
	memcpy(SysIDBlock.marker, VALIDMARKER, sizeof(SysIDBlock.marker));

	//////////////////////////////////////////////////
	// board-specific data


	SysIDBlock.productID = MYBOARD;
	SysIDBlock.flashID = (unsigned int)GetFlashID();

#ifdef _FLASH_SIZE_
	SysIDBlock.flashSize = _FlashInfo.flashSize = _FLASH_SIZE_;
#else
	SysIDBlock.flashSize = _FlashInfo.flashSize;
#endif
#ifdef _SECTOR_SIZE_
	SysIDBlock.sectorSize = _FlashInfo.sectorSize = _SECTOR_SIZE_;
#else
	SysIDBlock.sectorSize = _FlashInfo.sectorSize;
#endif
#ifdef _NUM_SECTORS_
	SysIDBlock.numSectors = _FlashInfo.numSectors = _NUM_SECTORS_;
#else
	SysIDBlock.numSectors = _FlashInfo.numSectors;
#endif
	SysIDBlock.flashType = _FlashInfo.writeMode;

	SysIDBlock.flashSpeed = 0x00;   // special case, means "unknown"

#if HAS2FLASHES
	// we assume that the 2nd flash (if installed) is
	//  identical to the first one
	SysIDBlock.flash2ID = SysIDBlock.flashID;
	SysIDBlock.flash2Size = _FLASH_SIZE_;
	SysIDBlock.flash2Type = _FlashInfo.writeMode;
	SysIDBlock.sector2Size = _FlashInfo.sectorSize;
	SysIDBlock.num2Sectors = _FlashInfo.numSectors;
	SysIDBlock.flash2Speed = 0x00;  // special case, means "unknown"
#endif

	SysIDBlock.ramID = 0x01;
	SysIDBlock.ramSize = _RAM_SIZE_;
	SysIDBlock.ramSpeed = 0x00;	// special case, means "unknown"

   SysIDBlock.idBlock2.flashMBC = FLASH_WSTATES | CS_FLASH;
   SysIDBlock.idBlock2.flash2MBC = FLASH_WSTATES | CS_FLASH2;
   SysIDBlock.idBlock2.ramMBC = RAM_WSTATES | CS_RAM;

	SysIDBlock.cpuID = getCPUID();	  // Rabbit CPU + Version
	div19200val = getdivider19200();
	SysIDBlock.crystalFreq = div19200val * 614400ul;

	// may fill serial number and product name here

#ifdef SERIALNUMBER
	if (strlen(SERIALNUMBER) >= sizeof(SysIDBlock.serialNumber)) {
		memcpy(SysIDBlock.serialNumber, SERIALNUMBER,
		       sizeof(SysIDBlock.serialNumber));
	} else {
		strcpy(SysIDBlock.serialNumber, SERIALNUMBER);
	}
#endif

#ifdef PRODUCTNAME
	if (strlen(PRODUCTNAME) >= sizeof(SysIDBlock.productName)) {
		memcpy(SysIDBlock.productName, PRODUCTNAME,
		       sizeof(SysIDBlock.productName));
	} else {
		strcpy(SysIDBlock.productName, PRODUCTNAME);
	}
#endif

#if NUMUSER4KBLOCKS
	if (SysIDBlock.tableVersion >= 5) {

 #if NUMRESERVED4KBLOCKS
   	// Version 5 unique ID block mirrored user blocks.
		SysIDBlock.idBlock2.persBlockLoc  = NUMRESERVED4KBLOCKS * 4096ul -
                                          sizeof(SysIDBlock);
		SysIDBlock.idBlock2.userBlockSiz2 = NUMUSER4KBLOCKS * 4096ul -
                                          sizeof(SysIDBlock.marker);
		// to protect v5uID+mUser blocks data,
		//  tell older DC versions that we have no User block
		SysIDBlock.userBlockSize = 0;
		SysIDBlock.userBlockLoc  = 0;
 #else
   	// Version 5 combined and mirrored ID/User blocks
		SysIDBlock.idBlock2.persBlockLoc = 256ul * 2ul;
		SysIDBlock.userBlockSize = (unsigned)
		                           (NUMUSER4KBLOCKS * 4096ul -
		                            sizeof(SysIDBlockType) -
		                            SysIDBlock.idBlock2.persBlockLoc);
		SysIDBlock.userBlockLoc = (unsigned)
		                          (SysIDBlock.idBlock2.persBlockLoc +
		                           SysIDBlock.userBlockSize);
 #endif
	}
	else if (SysIDBlock.tableVersion > 1) {
		// reserve space for the User Block
		SysIDBlock.userBlockSize = NUMUSER4KBLOCKS * 4096 -
		                          (int) SysIDBlock.idBlockSize;
		SysIDBlock.userBlockLoc = SysIDBlock.userBlockSize;
	}
#endif


#if (HASETHERNET && !HASEEPROM)
	// fill MAC address
	memcpy(SysIDBlock.macAddr, mac2write, sizeof(SysIDBlock.macAddr));
#endif

	if (NUMUSER4KBLOCKS && SysIDBlock.tableVersion >= 5) {
		// Fill in version 5 fields in IDBlock
		if ( io_description )
		{
			xmem2root(&length, io_description, sizeof(length));
			SysIDBlock.idBlock2.ioDescLoc = length;
		}
      else SysIDBlock.idBlock2.ioDescLoc = 0;

		if ( io_permissions )
		{
			xmem2root(&length, io_permissions, sizeof(length));
			SysIDBlock.idBlock2.ioPermLoc = length + SysIDBlock.idBlock2.ioDescLoc;
		}
      else {
  			SysIDBlock.idBlock2.ioPermLoc = 0;
      }

		if ( pl_drivers )
		{
			SysIDBlock.idBlock2.driversLoc = SysIDBlock.idBlock2.persBlockLoc;
		}
      else {
      	SysIDBlock.idBlock2.driversLoc = 0;
      }

#if (MYBOARD & 0xFF00) == (POWERCOREFLEX & 0xFF00)
		set_bd_configuration(NULL);
		set_board_info(&testinfo);
#endif

      length = build_dev_spec(0, SysIDBuffer.pos, memDevSpecTable);
      SysIDBuffer.pos += (uint16)length;
      SysIDBlock.idBlock2.devSpecLoc = length;

      length = build_macro_table(0, SysIDBuffer.pos, macroTable);
      SysIDBuffer.pos += (uint16)length;
      SysIDBlock.idBlock2.macrosLoc = (uint16)length;
      if(SysIDBlock.idBlock2.devSpecLoc) {
	      SysIDBlock.idBlock2.devSpecLoc = SysIDBlock.idBlock2.devSpecLoc + length;
      }

#if IDBLOCK_VERSION >= 5 && NUMRESERVED4KBLOCKS > 0 && NUMUSER4KBLOCKS > 0
      // Version 5 unique ID block w/ mirrored User blocks are not allowed to
      //  share any flash sector(s).  The following calculations ensure that
      //  the start address of each block is "extended" to the start of the
      //  lowest numbered flash sector that is a part of the block.
      flashTop = (uint32) SysIDBlock.flashSize * 4096ul;
 #if FLASH_COMPILE
      flashOfst = 0ul;
 #else
      // We manually map the /CS0 flash into the top (MB3CR) quadrant for both
      //  RAM_COMPILE and FAST_RAM_COMPILE modes, so the flash offset depends
      //  on the MSBit of the Rabbit's physical memory addressing.
      flashOfst = (1ul << (MSB_BIT + 1)) - flashTop;
 #endif
      idblocksector = longToSector(flashTop - NUMRESERVED4KBLOCKS * 4096L);
      idblockaddr = sectorToLong(idblocksector);
      ublockAsector = longToSector(idblockaddr - NUMUSER4KBLOCKS * 4096L);
      ublockAaddr = sectorToLong(ublockAsector);
      ublockBsector = longToSector(ublockAaddr - NUMUSER4KBLOCKS * 4096L);
      ublockBaddr = sectorToLong(ublockBsector);

		// Mark the v5uID+mUser blocks valid.  We use WriteFlash() here for
		//  simplicity's sake.
      markeraddr = flashOfst + ublockAaddr + NUMUSER4KBLOCKS * 4096ul
                   - sizeof(SysIDBlock.marker);
      _overwrite_block_flag = 1;
		i = WriteFlash(markeraddr, SysIDBlock.marker, sizeof(SysIDBlock.marker));
      _overwrite_block_flag = 0;
		showFlashWriteResults(i, "User block image 'A' validation marker");

      markeraddr = flashOfst + ublockBaddr + NUMUSER4KBLOCKS * 4096ul
                   - sizeof(SysIDBlock.marker);
      _overwrite_block_flag = 1;
		i = WriteFlash(markeraddr, SysIDBlock.marker, sizeof(SysIDBlock.marker));
      _overwrite_block_flag = 0;
		showFlashWriteResults(i, "User block image 'B' validation marker");
#endif
	}
	//////////////////////////////////////////////////
	// calculate the ID block's CRC


	SysIDBlock.idBlockCRC = 0x0000;

	i = (int) (&SysIDBlock.idBlock2) - (int) (&SysIDBlock.tableVersion);
	crc = calcCRC(0x0000, &SysIDBlock, i);
	crc = calcCRC(crc, &SysIDBlock.idBlockSize, 16);

	SysIDBlock.idBlockCRC = crc;

	if (SysIDBlock.tableVersion >= 5) {
		SysIDBlock.idBlock2.idBlockCRC2 = 0;
		SysIDBlock.idBlock2.idBlockCRC2 = calcCRC(crc,&SysIDBlock.idBlock2, sizeof(SysIDBlockType2));
	}

	//////////////////////////////////////////////////
	// write the ID Block(s) to flash

	SysIDBuffer.pos += (uint16)build_sysid_block(SysIDBuffer.pos);
	showFlashWriteResults(writeIDBlock(), "ID block");

	if(SysIDBlock.idBlock2.ioDescLoc)
   {
		xmem2root(&length, io_description, sizeof(length));
	   i = writeToFlash(io_description + sizeof(long),
   						  SysIDBlock.idBlock2.ioDescLoc,
      	              length);
	   showFlashWriteResults(i, "I/O descriptions");
   }
   if(SysIDBlock.idBlock2.ioPermLoc)
   {
		xmem2root(&length, io_permissions, sizeof(length));
      i = writeToFlash(io_permissions + sizeof(long),
                       SysIDBlock.idBlock2.ioPermLoc,
                       length);
	   showFlashWriteResults(i, "I/O permissions");
   }
   if(SysIDBlock.idBlock2.driversLoc)
   {
		xmem2root(&length, pl_drivers, sizeof(length));
      i = writeToFlash(pl_drivers + sizeof(long),
                       SysIDBlock.idBlock2.driversLoc,
                       length);
      showFlashWriteResults(i, "Preloaded drivers");
   }

	// now, we need to update the BIOS's ID/User Blocks Address
	//  and Sector variables
	_updateBIOSIDUserBlocksInfo();

	//////////////////////////////////////////////////
	// now try and read the ID block back...


	memset(&SysIDBlock, 0x00, sizeof(SysIDBlock));
#if FLASH_COMPILE
	i = _readIDBlock(0x01 | 0x02);
#else
	i = _readIDBlock(0x08);
#endif


	//////////////////////////////////////////////////
	// ...and report the information we (hope we have) found


	if (i != 0) {
		printf("Error %d when reading ID block.\n", i);
		exit(1);
	} else {
#ifndef FACTORY_INSTALL
		PrintSysIDBlockInfo();
		PrintProductDescription(SysIDBlock.productID);
		PrintFlashDescription("Flash ID", (int) SysIDBlock.flashID,
		                      SysIDBlock.flashType);
		PrintFlashDescription("Flash2 ID", (int) SysIDBlock.flash2ID,
		                      SysIDBlock.flash2Type);
		PrintFlashDescription("GetFlashID() result", GetFlashID(),
		                      _FlashInfo.writeMode);

 #if RAM_COMPILE
		RAM_GetUserBlockInfo(&uBI);
 #else
		GetUserBlockInfo(&uBI);
 #endif
		if (!uBI.blockSize) {
			printf("\nNo valid User block found on this board!\n");
		} else {
			printf("\nID+User blocks area top address = 0x%08LX\n", uBI.topAddr);
			printf("   User block image A address   = 0x%08LX\n", uBI.addrA);
			printf("   User block image B address   = 0x%08LX\n", uBI.addrB);
			printf("   User block size              = 0x%08LX\n", uBI.blockSize);
			printf("   User block image %s is valid.\n", uBI.blockAvalid ? "A" : "B");
		}
#else
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
		       SysIDBlock.macAddr[0], SysIDBlock.macAddr[1], SysIDBlock.macAddr[2],
		       SysIDBlock.macAddr[3], SysIDBlock.macAddr[4], SysIDBlock.macAddr[5]);
#endif
	}


	//////////////////////////////////////////////////
	// restore altered MBxCR


#if FLASH_COMPILE
	WrPortI(MB1CR, &MB1CRShadow, saveMB1or3CR);
#else
	WrPortI(MB3CR, &MB3CRShadow, saveMB1or3CR);
#endif

   // Force BIOS recompile for next compilation by telling DC
   //  that this board is not "tagged" the same as it was.
   dkDebugTag = !dkDebugTag;

	//////////////////////////////////////////////////
	// Loop forever to prevent repetitive flash writes,
	//  in case this program is started in run mode!


#ifndef FACTORY_INSTALL
	printf("\n\nLooping forever to prevent repetitive flash writes.\n");
	while (1);
#endif

}

/********************************************************************/

void showFlashWriteResults (int i, char *msg)
{
	if (i != 0)	{
		printf("\nError writing %s (%d): \n", msg, i);
		if (i == -1)	printf("   attempt to write to non-flash area\n");
		else if (i == -2)	printf("   source not located in root\n");
		else if (i == -3)	printf("   timeout during flash write\n");
		else printf("   unknown error type\n");
		exit(1);
	} else {
#ifndef FACTORY_INSTALL
		printf("\n%s successfully written.\n", msg);
#endif
	}
} //showFlashWriteResults()


/********************************************************************/


int writeIDBlock(void)
{
	auto unsigned long physaddr;
	auto int err;
   auto int *pbuf;
	auto uint32 blocklength;

   blocklength = SysIDBuffer.pos - SysIDBuffer.buffer;
 #if FLASH_COMPILE
	physaddr = sectorToLong(_FlashInfo.numSectors) - blocklength;
 #else
	// flash is mapped at end of hard-coded 1 MByte physical memory space
	physaddr = 0x100000ul - blocklength;
 #endif

	_overwrite_block_flag = 1;
	err = WriteFlash(physaddr, SysIDBuffer.buffer, (uint16)blocklength);
	_overwrite_block_flag = 0;

#if NUMUSER4KBLOCKS
	if ((SysIDBlock.tableVersion >= 3) && (SysIDBlock.tableVersion < 5)  ||
   	 (SysIDBlock.tableVersion == 5) && (NUMRESERVED4KBLOCKS == 0))
   {
		// Mirrored ID/User Block version, might as well do it now!
		physaddr -= NUMUSER4KBLOCKS * 4096ul;
		_overwrite_block_flag = 1;
		SysIDBlock.marker[5] = '\x00';
		err = WriteFlash(physaddr, SysIDBuffer.buffer, (uint16)blocklength);
		SysIDBlock.marker[5] = '\xAA';
		_overwrite_block_flag = 0;
	}
#endif

	return err;
}//writeIDBlock()


/********************************************************************/

char xmembuf[4096];		//buffer for copying from XMEM to flash (user block)

int writeToFlash (long what, long where, long xlen)
{
	auto int offs, len;
	auto unsigned long physaddr;
	auto long savelen;
	auto int err;
	auto int crc,flashcrc;
	auto uint32 blocklength;

   blocklength = SysIDBuffer.pos - SysIDBuffer.buffer;
	/*-----------------10/5/2004 5:09PM-----------------
	 physaddr points to a flash address above the
	 System ID Block.  This address provides enough
	 space to place the data, ending at the beginning of
	 the System ID Block.
	 --------------------------------------------------*/
#if FLASH_COMPILE
	physaddr = sectorToLong(_FlashInfo.numSectors) - where - blocklength;
#else
	// flash is mapped at end of hard-coded 1 MByte physical memory space
	physaddr = 0x100000ul - where - blocklength;
#endif

	offs = 0;
	crc = 0;
	savelen = xlen;
	do
	{
		_overwrite_block_flag = 1;
		len = (int)xlen;
		if ( len > sizeof(xmembuf) )		//only read enough to fill the buffer
		{
			len = sizeof(xmembuf);
		}
		xlen -= len;
		xmem2root(xmembuf, what + offs, len);
		crc = getcrc(xmembuf, len, crc);

		err = WriteFlash(physaddr+offs, &xmembuf, len);

		offs += len;
	} while ( xlen && !err );		//repeat until done or an error.
	_overwrite_block_flag = 0;

	if (err)
		return err;

	memset(xmembuf,0,sizeof(xmembuf));

	//Read back flash data and compare CRC values.
	xlen = savelen;
	offs = 0;
	flashcrc = 0;
	do
	{
		len = (int)xlen;
		if ( len > sizeof(xmembuf) )		//only read enough to fill the buffer
		{
			len = sizeof(xmembuf);
		}
		xlen -= len;
		xmem2root(xmembuf, what + offs, len);
		flashcrc = getcrc(xmembuf, len, flashcrc);
		offs += len;
	} while ( xlen && !err );		//repeat until done or an error.
	return (crc == flashcrc) ? 0: -4;
}//writeToFlash()


/********************************************************************/
#ifndef _RK_FIXED_VECTORS
#define _RK_FIXED_VECTORS __SEPARATE_INST_DATA__
#endif

int GetFlashID(void)
{
	static int id;

#asm
#ifdef __SEPARATE_INST_DATA__
	#if (_RK_FIXED_VECTORS && FLASH_COMPILE)
	ld		hl, _GetFlashID
	lcall	_CallRAMFunction
	#else
	call	_GetFlashID
	#endif
#else
	call	_GetFlashID
#endif
	ld		(id), hl
#endasm

	return id;
}


/********************************************************************/


int getCPUID(void)
{
	static int id;

#asm
	ioi	ld a, (GCPU)
	and	0x1F
	ld		(id+1), a
	ioi	ld a, (GREV)
	and	0x1F
	ld		(id), a
#endasm

	return id;
}


/********************************************************************/


void setTimeStamp(unsigned long timeStamp)
{
	static struct tm t;

	mktm(&t, timeStamp);
	SysIDBlock.timestamp[0] = (t.tm_year + 1900) / 100;
	SysIDBlock.timestamp[1] = t.tm_year % 100;
	SysIDBlock.timestamp[2] = t.tm_mon;
	SysIDBlock.timestamp[3] = t.tm_mday;
	SysIDBlock.timestamp[4] = t.tm_hour;
	SysIDBlock.timestamp[5] = t.tm_min;
	SysIDBlock.timestamp[6] = t.tm_sec;
}


/********************************************************************/


#define CRC_POLY		0x1021	  // 16-bit CRC polynomial
											//		(recommended by CCITT X25 standard)


/*
 *	CRC calculation functions
 *		- can be called from C (calcCRC) or assembly (_calcCRC)
 *		- reference:  "A painless guide to CRC error detection algorithms",
 *							8/19/1993, Ross N. Williams
 */


#asm
//	int calcCRC(int initCRC, void *data, int blocksize);
calcCRC::
	ld		hl, 2
	add	hl, sp
	ld		hl, (hl)
	ex		de, hl			  ; initial CRC value

	ld		hl, 6				   ; the "blocksize" variable
	add	hl, sp
	ld		hl, (hl)
	ld		b, h
	ld		c, l					; get data size off stack


	ld		hl, 4				   ; the "data" variable
	add	hl, sp
	ld		hl, (hl)				; hl now contains pointer to data


;; assembly entry point
_calcCRC::
;		de contains initial CRC value
;		hl contains pointer to data
;		bc contains number of bytes
dataloop:
	push	hl
	ld		h, (hl)			 ; get next byte into hl (and shift it left 8 bits)
	ld		l, 0x00

	push	bc						; save byte counter
	call	_byteCRC				; call CRC function from BIOS
	pop	bc						; restore byte counter

	pop	hl
	inc	hl
	dec	bc

	xor	a
	cp		b
	jr		nz, dataloop
	cp		c
	jr		nz, dataloop

	ex		de, hl			  ; CRC returned in hl
	ret
#endasm


//////////////////////////////////////////////////////////////////////


#asm
;;
;;	Calculate the CRC value for a single byte.  Can be called multiple
;;	times (without resetting the CRC value passed to it) to do a CRC
;;	check on a larger block of data.
;;
;;		data byte passed in h (l should be 0x00)
;;		crc value passed, returned in de
;;
_byteCRC::
	push	bc
	push	af

	ld		b, 0x08			 ; bit counter
crcloop:
	push	bc						; save bit counter
	ld		a, h
	xor	d
	scf
	ccf							; clear carry bit
	rl		de						; roll crc left one bit
	ex		de, hl
	scf
	ccf							; clear carry bit
	rl		de						; roll data left one bit
	ex		de, hl
	bit	7, a					; result from earlier XOR
	jr		z, bit7iszero
bit7isone:
	ld		bc, CRC_POLY		; recommended 16-bit polynomial (X.25)
	ld		a, e
	xor	c
	ld		e, a
	ld		a, d					; XOR crc with polynomial
	xor	b
	ld		d, a
bit7iszero:
	pop	bc						; restore bit counter
	djnz	crcloop

	pop	af
	pop	bc
	ret
#endasm

struct _FlashDescription {
	int fType;
	char *fDesc;
};

const struct _FlashDescription FlashDesc[] = {
	{1, "small sector, sector erase (0x30) byte write"},
	{2, "small sector, sector write"},
	{3, "small page, page erase (0x50) byte write"},
	{4, "small sector, sector erase (0x20) byte write"},
   {5, "small sector, sector erase (0x30) word write"},
	{0x11, "large sector (128/96/8/8/16), sector erase (0x30) byte write"},
	{0x12, "large sector (16/8/8/96/128), sector erase (0x30) byte write"},
	{0x13, "large sector (16*7/4/4/8), sector erase (0x30) byte write"},
	{0x14, "large sector (8/4/4/16*7), sector erase (0x30) byte write"},
	{0x15, "large sector (64*3/32/8/8/16), sector erase (0x30) byte write"},
	{0x16, "large sector (16/8/8/32/64*3), sector erase (0x30) byte write"},
	{0x17, "large sector (64*7/32/8/8/16), sector erase (0x30) byte write"},
	{0x18, "large sector (16/8/8/32/64*7), sector erase (0x30) byte write"},
	{0x19, "large sector (64*8), sector erase (0x30) byte write"},
	{0x1A, "large sector (64*8), sector erase (0x30) byte write"},
	{0x1B, "large sector (64*4), sector erase (0x30) byte write"},
	{0x1C, "large sector (64*4), sector erase (0x30) byte write"},
	{0x1D, "large sector (128/96/8/8), sector erase (0x30) byte write"},
	{0x1E, "large sector (32*8), sector erase (0x30) byte write"},
	{0x1F, "large sector (8/4/4/16*7), sector erase (0x30) byte write"},
	{0x20, "large sector (16*8), sector erase (0x30) byte write"},
	{0x21, "large sector (16*8), sector erase (0x30) byte write"},
	{0, "unknown.\n   If valid, a description should be added herein"}
};

struct _Flash {
	int fID;
	char *name;
};

const struct _Flash Flash[] = {
	{0x0134, "n AMD AM29F002BB"},
	{0x0140, "n AMD AM29LV002BT"},
	{0x016D, "n AMD AM29LV001BB"},
	{0x0177, "n AMD AM29F004BT"},
	{0x017B, "n AMD AM29F004BB"},
	{0x01A4, "n AMD AM29F040B"},
	{0x01B0, "n AMD AM29F002BT"},
	{0x01B5, "n AMD AM29LV004BT"},
	{0x01B6, "n AMD AM29LV004BB"},
	{0x01C2, "n AMD AM29LV002BB"},
	{0x01ED, "n AMD AM29LV001BT"},

	{0x0434, " Fujitsu MBM29F002BC"},
	{0x04B0, " Fujitsu MBM29F002TC"},

	{0x1F07, "n Atmel AT49F002"},
	{0x1F08, "n Atmel AT49F002T"},
	{0x1F25, "n Atmel AT29C1024"},
	{0x1F26, "n Atmel AT29LV1024"},
	{0x1F35, "n Atmel AT29LV010 / AT29BV010A"},
	{0x1FA4, "n Atmel AT29C040"},
	{0x1FBA, "n Atmel AT29LV020 / AT29BV020"},
	{0x1FC4, "n Atmel AT29LV040 / AT29BV040"},
	{0x1FD5, "n Atmel AT29C010"},
	{0x1FDA, "n Atmel AT29C020"},

	{0x2023, "n STMicroelectronics M29W010B"},
	{0x20E2, "n STMicroelectronics M29F040B"},
	{0x20E3, "n STMicroelectronics M29W040B"},

	{0x4001, " Mosel/Vitelic V29C51001T"},
	{0x4002, " Mosel/Vitelic V29C51002T"},
	{0x4003, " Mosel/Vitelic V29C51004T"},
	{0x4060, " Mosel/Vitelic V29LC51001"},
	{0x4063, " Mosel/Vitelic V29C31004T"},
	{0x4073, " Mosel/Vitelic V29C31004B"},
	{0x4082, " Mosel/Vitelic V29LC51002"},
	{0x40A1, " Mosel/Vitelic V29C51001B"},
	{0x40A2, " Mosel/Vitelic V29C51002B"},
	{0x40A3, " Mosel/Vitelic V29C51004B"},

	{0xAD34, " Hyundai HY29F002B"},
	{0xADB0, " Hynix/Hyundai HY29F002T"},

	{0xBF07, "n SST SST29EE010"},
	{0xBF08, "n SST SST29LE010 / SST29VE010"},
	{0xBF10, "n SST SST29EE020"},
	{0xBF12, "n SST SST29LE020 / SST29VE020"},
	{0xBF13, "n SST SST29SF040"},
	{0xBF14, "n SST SST29VF040"},
	{0xBF20, "n SST SST29SF512"},
	{0xBF21, "n SST SST29VF512"},
	{0xBF22, "n SST SST29SF010"},
	{0xBF23, "n SST SST29VF010"},
	{0xBF24, "n SST SST29SF020"},
	{0xBF25, "n SST SST29VF020"},
	{0xBF3D, "n SST SST29LE512 / SST29VE512"},
	{0xBF5D, "n SST SST29EE512"},
	{0xBFB4, "n SST SST39SF512"},
	{0xBFB5, "n SST SST39SF010"},
	{0xBFB6, "n SST SST39SF020"},
	{0xBFB7, "n SST SST39SF040"},
	{0xBFD4, "n SST SST39LF512 / SST39VF512"},
	{0xBFD5, "n SST SST39LF010 / SST39VF010"},
	{0xBFD6, "n SST SST39LF020 / SST39VF020"},
	{0xBFD7, "n SST SST39LF040 / SST39VF040"},
   {0x2780, "n SST SST39LF400A / SST39VF400A"},
   {0x2781, "n SST SST39LF800A / SST39VF800A"},

	{0xC234, " Macronix MX29F002B"},
	{0xC2B0, " Macronix MX29F02T"},

	{0xDA45, " Winbond W29C020CT"},
	{0xDA46, " Winbond W29C040"},
	{0xDAB5, " Winbond W39L020"},
	{0xDAC1, " Winbond W29EE011"},

	{0, "n unlisted type.\n   If valid, a description should be added herein"}
};

void PrintFlashDescription(char *which, int FlashID, int FlashType)
{
	int i;

	i = 0;
	while (Flash[i].fID) {
		if (Flash[i].fID == FlashID) {
			break;
		}
		i++;
	}
	printf("\n%s 0x%04X is a%s.\n", which, FlashID, Flash[i].name);

	i = 0;
	while (FlashDesc[i].fType) {
		if (FlashDesc[i].fType == FlashType) {
			break;
		}
		i++;
	}
	printf("   Type 0x%04X is %s.\n", FlashType, FlashDesc[i].fDesc);
}

struct _Product {
	int pID;
	char *name;
	char *description;
};

const struct _Product Product[] = {
//*** RabbitCore Modules ***//

	{RCM2000, "n RCM2000", "25MHz R2000, 256K flash, 512k SRAM"},
	{RCM2010, "n RCM2010", "25MHz R2000, 256K flash, 128k SRAM"},
	{RCM2020, "n RCM2020", "18MHz R2000, 256K flash, 128K SRAM"},

	{RCM2100, "n RCM2100", "22MHz R2000, 2x256K flash, 512K SRAM, ethernet"},
	{RCM2110, "n RCM2110", "22MHz R2000, 256K flash, 128K SRAM, ethernet"},
	{RCM2115, "n RCM2115",
	 "22MHz R2000, 256K flash, 128K SRAM, ethernet (no RJ45)"},
	{RCM2120, "n RCM2120", "22MHz R2000, 2x256K flash, 512K SRAM"},
	{RCM2130, "n RCM2130", "22MHz R2000, 256K flash, 128K SRAM"},

	{RCM2200, "n RCM2200", "22MHz R2000, 256K flash, 128K SRAM, ethernet"},
	{RCM2210, "n RCM2210",
	 "22MHz R2000, 256K flash, 128K SRAM, ethernet (no RJ45)"},
	{RCM2250, "n RCM2250", "22MHz R2000, 2x256K flash, 512K SRAM, ethernet"},
	{RCM2260A, "n RCM2260",
	 "22MHz R2000, 2x256K flash, 512K SRAM, ethernet (no RJ45)"},

	{RCM2300, "n RCM2300", "22MHz R2000, 256K flash, 128K SRAM"},

	{RCM3000, "n RCM3000", "29MHz R3000, 2x256K flash, 512K SRAM, ethernet"},
	{RCM3010, "n RCM3010", "29MHz R3000, 256K flash, 128K SRAM, ethernet"},

	{RCM3100, "n RCM3100", "29MHz R3000, 2x256K flash, 512K SRAM"},
	{RCM3110, "n RCM3110", "29MHz R3000, 256K flash, 128K SRAM"},

	{RCM3200, "n RCM3200", "44MHz R3000, 512K flash, 256K+512K SRAM, ethernet"},
	{RCM3209, "n RCM3209", "44MHz R3000, 512K flash, 256K+512K SRAM, ethernet"},
	{RCM3210A, "n RCM3210", "29MHz R3000, 256K flash, 128K SRAM, ethernet"},
	{RCM3220A, "n RCM3220", "44MHz R3000, 512K flash, 256K+512K SRAM"},
	{RCM3229, "n RCM3229", "44MHz R3000, 512K flash, 256K+512K SRAM"},

	{RCM3300A, "n RCM3300",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 8M serial flash"},
	{RCM3305, "n RCM3305",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 8M serial flash"},
	{RCM3309, "n RCM3309",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 8M serial flash"},
	{RCM3310A, "n RCM3310",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 4M serial flash"},
	{RCM3315, "n RCM3315",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 4M serial flash"},
	{RCM3319, "n RCM3319",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 4M serial flash"},
	{RCM3360A, "n RCM3360",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 16M nand flash+socket"},
	{RCM3360B, "n RCM3360",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 16M nand flash"},
	{RCM3365, "n RCM3365",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 16M nand flash+socket"},
	{RCM3370A, "n RCM3370",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, nand flash socket"},
	{RCM3375, "n RCM3375",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, nand flash socket"},

	{RCM3400A, "n RCM3400",
	 "29MHz R3000, 512K flash, 512K SRAM, ethernet on PB, analog"},
	{RCM3410A, "n RCM3410",
	 "29MHz R3000, 256K flash, 256K SRAM, ethernet on PB, analog"},
	{RCM3420A, "n RCM3420", "Reserved"},
	{RCM3450, "n RCM3450", "Reserved"},

//TinyCore
	{RCM3600A, "n RCM3600", "22MHz R3000, 512K flash, 512K SRAM"},
	{RCM3610A, "n RCM3610", "22MHz R3000, 256K flash, 128K SRAM"},
	{RCM3600LP, "n RCM3600", "Reserved"},
	{RCM3660, "n RCM3660", "Reserved"},
// Hobby
	{RCM3600H, "n RCM3600", "22MHz R3000, 512K flash, 512K SRAM"},
	{RCM3610H, "n RCM3610", "22MHz R3000, 256K flash, 128K SRAM"},

//Ethernet TinyCore
	{RCM3700A, "n RCM3700",
	 "22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash"},
	{RCM3700LP, "n RCM3700", "Reserved"},
	{RCM3710A, "n RCM3710",
	 "22MHz R3000, 256K flash, 128K SRAM, ethernet, 1M serial flash"},
	{RCM3720A, "n RCM3720",
	 "22MHz R3000, 512K flash, 256K SRAM, ethernet, 1M serial flash"},
	{RCM3750, "n RCM3750",
	 "22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash"},
	{RCM3760, "n RCM3760",
	 "22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash, 3.3V"},
//Hobby Ethernet
	{RCM3700H, "n RCM3700",
	 "22MHz R3000, 512K flash, 512K SRAM, ethernet, 1M serial flash"},
	{RCM3710H, "n RCM3710",
	 "22MHz R3000, 256K flash, 128K SRAM, ethernet, 1M serial flash"},
	{RCM3720H, "n RCM3720",
	 "22MHz R3000, 512K flash, 256K SRAM, ethernet, 1M serial flash"},

//PowerCore
	{POWERCOREFLEX, " PowerCore FLEX", "Varies"},
	{POWERCOREFLEXA, " PowerCore FLEX", "Varies"},

//FLEXSBC
	{FLEXSBC, " FLEXSBC", "Varies"},

//Rabbit 3000 replacement cores
	{RCM3900, "n RCM3900",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, 32M nand flash, SD socket"},
	{RCM3910, "n RCM3910",
	 "44MHz R3000, 512K flash, 512K+512K SRAM, ethernet, SD socket"},

//*** Single Board Computers ***//

//Jackrabbit
	{BL1800, " BL1800", "29MHz R2000, 256K flash, 128K SRAM, RS485, battery"},
	{BL1805, " BL1805", "29MHz R2000, 256K flash, 512K SRAM, RS485, battery"},
	{BL1810, " BL1810", "14MHz R2000, 128K flash, 128K SRAM, RS485, battery"},
	{BL1820, " BL1820", "14MHz R2000, 128K flash, 128k SRAM"},

//Wildcat
	{BL2000, " BL2000",
	 "22MHz R2000, 256K flash, 128K SRAM, ethernet, high-quality analog"},
	{BL2000B, " BL2000",
	 "22MHz R2000, 512K flash, 512K SRAM, ethernet, high-quality analog"},
	{BL2010, " BL2010", "22MHz R2000, 256K flash, 128K SRAM, ethernet, analog"},
	{BL2010B, " BL2010", "22MHz R2000, 256K flash, 512K SRAM, ethernet, analog"},
	{BL2020, " BL2020",
	 "22MHz R2000, 256K flash, 128K SRAM, high-quality analog"},
	{BL2030, " BL2030", "22MHz R2000, 256K flash, 128K SRAM, analog"},
	{BL2040, " BL2040", "22MHz R2000, 256K flash, 128K SRAM"},

//Smart Cat
	{BL2100, " BL2100",
	 "22MHz RCM2200, 256K flash, 128K SRAM, ethernet, analog"},
	{BL2101, " BL2101",
	 "22MHz RCM2200, 256K flash, 128K SRAM, ethernet, 10V analog"},
	{BL2105, " BL2105",
	 "22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet, analog"},
	{BL2110, " BL2110", "22MHz RCM2200, 256K flash, 128K SRAM, ethernet"},
	{BL2111, " BL2111",
	 "22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet, 10V analog"},
	{BL2115, " BL2115", "22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet"},
	{BL2120, " BL2120", "22MHz RCM2300, 256K flash, 128K SRAM, analog"},
	{BL2121, " BL2121", "22MHz RCM2300, 256K flash, 128K SRAM, 10V analog"},
	{BL2130, " BL2130", "22MHz RCM2300, 256K flash, 128K SRAM"},

//RabbitNet
	{BL2500A, " BL2500", "29MHz RCM3010, 256K flash, 128K SRAM, ethernet"},
	{BL2500B, " BL2500", "29MHz RCM3000, 2x256K flash, 512K SRAM, ethernet"},
	{BL2500C, " BL2500", "44MHz RCM3200, 512K flash, 256K+512K SRAM, ethernet"},
	{BL2510A, " BL2510", "29MHz RCM3110, 256K flash, 128K SRAM"},
	{BL2510B, " BL2510", "29MHz RCM3100, 2x256K flash, 512K SRAM"},

	{BL2600A, " BL2600", "44MHz RCM3200, 512K flash, 256K+512K SRAM, ethernet"},
	{BL2600B, " BL2600", "29MHz RCM3000, 2x256K flash, 512K SRAM, ethernet"},
	{BL2600C, " BL2600", "29MHz RCM3010, 256K flash, 128K SRAM, ethernet"},
	{BL2600D, " BL2600", "Reserved"},
	{BL2600K, " BL2600", "Reserved"},
	{BL2600L, " BL2600", "Reserved"},
	{BL2600M, " BL2600", "44MHz RCM3365, 512K flash, 512K+512K SRAM, ethernet, 16 Mb Nand Flash, xD Connector"},
	{BL2600N, " BL2600", "44MHz RCM3375, 512K flash, 512K+512K SRAM, ethernet, xD Connector"},
	{BL2600Q, " BL2600", "44MHz RCM3900, 512K flash, 512K+512K SRAM, ethernet, 32MB nand flash, SD socket"},

	{BL2610A, " BL2610", "29MHz RCM3100, 2x256K flash, 512K SRAM"},
	{BL2610B, " BL2610", "29MHz RCM3110, 256K flash, 128K SRAM"},

//Fox Low-Power Controller
	{LP3500, "n LP3500", "7MHz R3000, 2x256K flash, 512K SRAM, analog, relay"},
	{LP3510, "n LP3510", "7MHz R3000, 256K flash, 128K SRAM"},


//*** Embedded Control Systems ***//

//SmartStar
	{SR9100, "n SR9100", "29MHz R2000, 2x256K flash, 128K SRAM"},

//MiniStar
	{SR9150, "n SR9150", "22MHz R2000, 2x256K flash, 128K SRAM, ethernet"},
	{SR9160, "n SR9160", "22MHz R2000, 2x256K flash, 128K SRAM"},


//*** Operator Interfaces ***//

//Intellicom
	{OP6600, "n OP6600", "18MHz R2000, 256K flash, 128K SRAM"},
	{OP6700, "n OP6700", "18MHz R2000, 2x256K flash, 256K SRAM, ethernet"},

//Minicom
	{OP6800, "n OP6800", "22MHz RCM2200, 256K flash, 128K SRAM, ethernet"},
	{OP6805, "n OP6805", "22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet"},
	{OP6810, "n OP6810", "22MHz RCM2300, 256K flash, 128K SRAM"},

//eDisplay
	{OP7200, "n OP7200",
	 "22MHz RCM2200, 256K flash, 128K SRAM, ethernet, analog, touchscreen"},
	{OP7200B, "n OP7200",
	 "22MHZ RCM2250, 2x256K flash, 512K SRAM, ethernet, analog, touchscreen"},
	{OP7210, "n OP7210", "22MHz RCM2200, 256K flash, 128K SRAM, ethernet"},
	{OP7210B, "n OP7210", "22MHz RCM2250, 2x256K flash, 512K SRAM, ethernet"},


//*** Other Products ***//

//Ethernet Modem
	{EM1500A, "n EM1500",
	 "44MHz RCM3200, 512K flash, 256K+512K SRAM, ethernet"},

//RabbitLink
	{EG2110A, "n EG2110", "22MHz R2000, 2x256K flash, 128K SRAM, ethernet, 3V"},
	{RABLINK, " RABLINK", "22MHz R2000, 2x256K flash, 128K SRAM, ethernet"},

//TCP/IP Development Kit
	{RTDK, "n RTDK", "18MHz R2000, 2x256K flash, 128K SRAM, ethernet"},


	{0, "n unlisted type", "If valid, a description should be added herein"}
};

void PrintProductDescription(int ProductID)
{
	int i;

	i = 0;
	while (Product[i].pID) {
		if (Product[i].pID == ProductID) {
			break;
		}
		i++;
	}
	printf("\nProduct ID 0x%04X is a%s.\n", ProductID, Product[i].name);
	printf("   %s.\n", Product[i].description);
}

void PrintSysIDBlockInfo(void)
{
	auto int i;
   auto char buf[SYS_MACRO_LENGTH];
   auto unsigned long value;
   auto MemDevSpec devspec;

	printf("\nSystem ID Block content:\n\n");
	printf("   tableVersion  = %d\n", SysIDBlock.tableVersion);
	printf("   productID     = 0x%04X\n", SysIDBlock.productID);
	printf("   vendorID      = %d\n", SysIDBlock.vendorID);
	printf("   timestamp     = %02d/%02d/%02d%02d  %02d:%02d:%02d\n\n",
		SysIDBlock.timestamp[2], SysIDBlock.timestamp[3],
		SysIDBlock.timestamp[0], SysIDBlock.timestamp[1],
			SysIDBlock.timestamp[4], SysIDBlock.timestamp[5],
			SysIDBlock.timestamp[6]);

	printf("   flashID       = 0x%08LX\n", SysIDBlock.flashID);
	printf("   flashType     = 0x%04X\n", SysIDBlock.flashType);
	printf("   flashSize     = %d Kbytes\n", SysIDBlock.flashSize * 4);
	printf("   sectorSize    = %d bytes\n", SysIDBlock.sectorSize);
	printf("   numSectors    = %d\n", SysIDBlock.numSectors);
	printf("   flashSpeed    = %d nS\n\n", SysIDBlock.flashSpeed);

	printf("   flash2ID      = 0x%08LX\n", SysIDBlock.flash2ID);
	printf("   flash2Type    = 0x%04X\n", SysIDBlock.flash2Type);
	printf("   flash2Size    = %d Kbytes\n", SysIDBlock.flash2Size * 4);
	printf("   sector2Size   = %d bytes\n", SysIDBlock.sector2Size);
	printf("   num2Sectors   = %d\n", SysIDBlock.num2Sectors);
	printf("   flash2Speed   = %d nS\n\n", SysIDBlock.flash2Speed);

	printf("   ramID         = 0x%08LX\n", SysIDBlock.ramID);
	printf("   ramSize       = %d Kbytes\n", SysIDBlock.ramSize * 4);
	printf("   ramSpeed      = %d nS\n\n", SysIDBlock.ramSpeed);

	printf("   cpuID         = Rabbit %u (rev. %u)\n",
	       ((SysIDBlock.cpuID >> 8) & 0xFF) * 1000 + 2000,
	       SysIDBlock.cpuID & 0xFF);
	printf("   crystalFreq   = %.4f MHz\n\n", SysIDBlock.crystalFreq * 1.0e-6);

	printf("   macAddr       = %02X:%02X:%02X:%02X:%02X:%02X\n",
	       SysIDBlock.macAddr[0], SysIDBlock.macAddr[1], SysIDBlock.macAddr[2],
	       SysIDBlock.macAddr[3], SysIDBlock.macAddr[4], SysIDBlock.macAddr[5]);
	printf("   serialNumber  = '%s'\n", SysIDBlock.serialNumber);
	printf("   productName   = '%s'\n\n", SysIDBlock.productName);

   if (SysIDBlock.tableVersion >= 5)
   {
  	   printf("   flashMBC      = 0x%02X\n", SysIDBlock.idBlock2.flashMBC);
  	   printf("   flash2MBC     = 0x%02X\n", SysIDBlock.idBlock2.flash2MBC);
  	   printf("   ramMBC        = 0x%02X\n", SysIDBlock.idBlock2.ramMBC);
  	   printf("   devSpecLoc    = 0x%08LX\n", SysIDBlock.idBlock2.devSpecLoc);
  	   printf("   macrosLoc     = 0x%08LX\n", SysIDBlock.idBlock2.macrosLoc);
	   printf("   driversLoc    = 0x%08LX\n", SysIDBlock.idBlock2.driversLoc);
	   printf("   ioDescLoc     = 0x%08LX\n", SysIDBlock.idBlock2.ioDescLoc);
	   printf("   ioPermLoc     = 0x%08LX\n", SysIDBlock.idBlock2.ioPermLoc);
	   printf("   persBlockLoc  = 0x%08LX\n", SysIDBlock.idBlock2.persBlockLoc);
	   printf("   userBlockSiz2 = 0x%04X\n", SysIDBlock.idBlock2.userBlockSiz2);
	   printf("   idBlockCRC2   = 0x%04X\n", SysIDBlock.idBlock2.idBlockCRC2);
   }
	printf("   reserved[0]   = 0x%02X\n\n", SysIDBlock.reserved[0]);
	printf("   idBlockSize   = 0x%08LX bytes\n", SysIDBlock.idBlockSize);
	printf("   userBlockSize = 0x%04X\n", SysIDBlock.userBlockSize);
	printf("   userBlockLoc  = 0x%04X\n\n", SysIDBlock.userBlockLoc);

  	printf("   idBlockCRC    = 0x%04X\n", SysIDBlock.idBlockCRC);
	printf("   marker        = %02X %02X %02X %02X %02X %02X\n",
	       SysIDBlock.marker[0], SysIDBlock.marker[1], SysIDBlock.marker[2],
	       SysIDBlock.marker[3], SysIDBlock.marker[4], SysIDBlock.marker[5]);

   if (SysIDBlock.tableVersion >= 5)
   {
     	printf("\nSystem Macro Table contents:\n");
   	if(SysIDBlock.idBlock2.macrosLoc) {
   		i = 0;
      	while(_GetSysMacroIndex(i, buf, &value) == 0) {
	         printf("   %-13s = 0x%08LX\n", buf, value);
            if(strncmp(buf, "_DC_BRD_OPT0_", 13) == 0) {
	            if(value & 1) {
	               printf("      16 Bit FLASH\n");
	            }
	            if(value & 2) {
	               printf("      16 Bit RAM\n");
	            }
	            if(value & 4) {
	               printf("      12 Bit ADC\n");
	            }
	            if(value & 8) {
	               printf("      14 Bit ADC\n");
	            }
	            if(value & 16) {
	               printf("      32MB NAND FLASH\n");
	            }
	            if(value & 32) {
	               printf("      Prog port PB1 is unavailable or not pulled up\n");
	            }
            }
            else if (strncmp(buf, "_DC_DFLASH0_", 13) == 0) {
            	if (20u > (uint16) _DFN_SIZE(value)) {
						printf("      %u KB %s data flash, subtype 0x%04X\n",
						       1u << (uint16) _DFN_SIZE(value) - 10u,
						       _GetDFlashType((uint16) _DFN_TYPE(value)),
						       (uint16) _DFN_SUBTYPE(value));
            	} else {
						printf("      %u MB %s data flash, subtype 0x%04X\n",
						       1u << (uint16) _DFN_SIZE(value) - 20u,
						       _GetDFlashType((uint16) _DFN_TYPE(value)),
						       (uint16) _DFN_SUBTYPE(value));
            	}
            }
            i++;
         }
      }
      else {
			printf("   No entries.\n");
      }
     	printf("\nDevice Specification Table contents:");
   	if(SysIDBlock.idBlock2.devSpecLoc) {
   		i = 0;
      	while(_GetDevSpecIndex(i, &devspec) == 0) {
	         printf("\n   Dev ID        = 0x%08LX\n", devspec.ID);
	         printf("   Dev Type      = 0x%04X\n", devspec.type);
	         printf("   Dev Size      = %d Kbytes\n", devspec.devSize * 4);
	         printf("   sectorSize    = %d bytes\n", devspec.sectorSize);
	         printf("   numSectors    = %d\n", devspec.numSectors);
	         printf("   Dev Speed     = %d nS\n", devspec.devSpeed);
	         printf("   Dev MBC       = 0x%02X\n", devspec.MBC);
	         i++;
         }
      }
      else {
			printf("\n   No entries.\n");
      }
   }
}

struct _DFlash {
	unsigned dfID;
	char *name;
};

const struct _DFlash DFlash[] = {
	{0x0001, "serial"},
	{0x0002, "nand"},
	{0, "unknown"},
};

char *_GetDFlashType(unsigned dfType)
{
	int i;

	i = 0;
	while (DFlash[i].dfID) {
		if (DFlash[i].dfID == dfType) {
			break;
		}
		++i;
	}
	return DFlash[i].name;
}

int _GetDevSpecIndex(int n, MemDevSpec *devspec)
{
   auto long devspecloc;
   auto uint16 len;
   auto int chr, i;

   // Calculate location of macro table
	devspecloc = IDBlockAddr - SysIDBlock.idBlock2.devSpecLoc;
   // Get the length of the macro table
   len = xgetint(devspecloc);
   // End of macro table is used to terminate loop below
   devspecloc += 2;

   // Check that id block is defined, that macro table is version 0
   // and that the length of the table is appropriate
   chr = (char)xgetint(devspecloc);
   if(IDBlockAddr == 0 || (char) chr != 0 || len > 256) {
   	return -2;
   }
   devspecloc++;

	// Calculate number of dev spec entries in table (-3 for is header of table)
   len = (len - 3)/sizeof(MemDevSpec);

   if(n >= len) {
   	return -1;
   }

   n *= sizeof(MemDevSpec);
   devspecloc += n;
   xmem2root(devspec, devspecloc, sizeof(MemDevSpec));

   return 0;
}

void add_macro_entry(char* macname, unsigned long value)
{
#GLOBAL_INIT { macroTableCount = 0; }

   if(macroTableCount < MAC_TABLE_SIZE) {
		strncpy(macroTable[macroTableCount].macro, macname, MACRO_NAME_SIZE);
		macroTable[macroTableCount].macro[MACRO_NAME_SIZE-1] = '\0';
   	if(strlen(macname) >= MACRO_NAME_SIZE) {
      	printf("ERROR: Macro names longer than 8 characters are not permitted.\n");
         printf("       Truncating macro name \"%s\" to \"%s\"", macname,
         		 macroTable[macroTableCount].macro);
      }
      macroTable[macroTableCount].value = value;
      macroTableCount++;
   }
   else {
   	printf("ERROR: Too many macro table entries--256 byte limit\n");
      exit(-1);
   }
}

void add_dev_spec(MemDevSpec *memspec)
{
#GLOBAL_INIT { memSpecTableCount = 0; }

	memDevSpecTable[memSpecTableCount] = *memspec;
   memSpecTableCount++;
}

// build_macro_table
// Description: Encodes the macroTable array into the format specified by
// the version paramter. The macroTable array is used to build the
// information, and this function
// encodes the macroTable as follows:
// All versions:  Structure Header
//						uint16 size_of_table_in_bytes;
//  					uint8	 macro_table_version;
//	Version 0:		char macro_name[2-9],  null terminated variable sized macro
// 												  from 2 to 9 characters
// 					uint32 value           value of macro
//						<repeat macro_name and value as appropriate>
// Future versions may support compressed encoding.
//	Return value:	number of bytes written to buffer
//						negative indicates function failed due to unknown version or
//						other error.
// Paramaters
//				version-encoding of macro table as specified above
//				dest-buffer to write encoded table
//				macro_table_t- source structure in a common format.
uint32 build_macro_table(uint8 version, uint8* dest, mac_tab_entry_t *src)
{
	auto uint16 mactab_size;
   auto uint8 *pbuf;
   auto uint16 count;
   auto uint16 macnamesize;

   if(!macroTableCount) return 0; // there is no table

   pbuf = dest;
   pbuf += sizeof(uint16); // skip past size (recalculate below)
   *pbuf++ = version;		// set the version
   switch(version) {
   case 0:
   	count = 0;
   	while(count < macroTableCount) {
      	macnamesize = strlen(src[count].macro)+1;	// add one for null char
      	if(macnamesize <= MACRO_NAME_SIZE) {
         	memcpy((void*)pbuf, (void*)src[count].macro, macnamesize);
            pbuf += macnamesize;
         	memcpy((void*)pbuf, (void*)&(src[count].value), sizeof(src[count].value));
            pbuf += sizeof(src[count].value);
            count++;
         }
         else {
         	return -1;
         }
      }
      break;
   default:
   	return -1;
      break;
   }
	mactab_size = pbuf - dest;
   * ((uint16*) dest) = mactab_size;
   return mactab_size;
}

uint32 build_dev_spec(uint8 version, uint8 *dest, MemDevSpec *src)
{
   auto uint8 *pbuf;
   auto uint16 count;
   auto uint16 devspecsize;

   if(!memSpecTableCount) return 0; // there is no table

   pbuf = dest;
   pbuf += sizeof(uint16); // skip past size (recalculate below)
   *pbuf++ = version;		// set the version
   switch(version) {
   case 0:
   	count = 0;
   	while(count < memSpecTableCount) {
      	memcpy(pbuf, src+count, sizeof(MemDevSpec));
			pbuf += sizeof(MemDevSpec);
         count++;
      }
      break;
   default:
   	return -1;
      break;
   }
	devspecsize = pbuf - dest;
   * ((uint16*) dest) = devspecsize;
   return devspecsize;

	return 0;
}

uint32 build_sysid_block(uint8 *dest)
{
   uint8* pbuf;
   uint16 length;

   pbuf = dest;
   if(SysIDBlock.tableVersion < 5) {
   	// Write head of block
		length = ((char*)(&SysIDBlock.idBlock2)-(char*)&SysIDBlock);
   	memcpy(pbuf, &SysIDBlock, length);
      pbuf += length;
   	// Write foot of block
		length = ((char*)&SysIDBlock+sizeof(SysIDBlock))-(char*)&SysIDBlock.reserved;
   	memcpy(pbuf, &SysIDBlock.reserved, length);
      pbuf += length;
   }
   else {
   	// Write head of block
   	memcpy(pbuf, &SysIDBlock, sizeof(SysIDBlock));
      pbuf += sizeof(SysIDBlock);
   }
	return pbuf-dest;
}

//
// mid-level routine that updates the BIOS's ID/User Blocks
//  Address and Sector variables
//
nodebug
void _updateBIOSIDUserBlocksInfo(void)
{
	int sectorNumber;
	uint32 flashOfst, flashTop, totalBlocksSize;

	flashTop = (uint32) SysIDBlock.flashSize * 4096ul;
#if FLASH_COMPILE
	flashOfst = 0ul;
#else
	// We manually map the /CS0 flash into the top (MB3CR) quadrant for both
	//  RAM_COMPILE and FAST_RAM_COMPILE modes, so the flash offset is relative
	//  to the end of the Rabbit's 1 MByte physical memory space.
	flashOfst = 0x100000ul - flashTop;
#endif

	// Use a top-of-flash relative ID block address so we can calculate the
	//  related ID+User blocks information regardless of compile mode.
	IDBlockAddr = flashTop - SysIDBlock.idBlockSize;
	IDBlockSector = longToSector(IDBlockAddr);

	if ((SysIDBlock.tableVersion >= 5) && !SysIDBlock.userBlockSize &&
	    SysIDBlock.idBlock2.userBlockSiz2)
	{
		// Calculate the starting address of the User block "B" (lowest-addressed)
		//  image.
		// Note that the User block images are not permitted to share a flash
		//  sector with each other or with the ID block plus optional persistent
		//  data storage area.
		// First, find the starting address of the lowest-addressed sector
		//  containing the ID block's and / or the optional persistent data's
		//  sector.
		sectorNumber = longToSector(IDBlockAddr -
		                            SysIDBlock.idBlock2.persBlockLoc);
		UserBlockAddr = sectorToLong(sectorNumber);	// force to sector start
		// Next, find the starting address of the lowest-addressed sector
		//  containing the User block "A" image and validity marker.
		sectorNumber = longToSector(UserBlockAddr -
		                            (SysIDBlock.idBlock2.userBlockSiz2 + 6ul));
		UserBlockAddr = sectorToLong(sectorNumber);	// force to sector start
		// Finally, find the starting address of the lowest-addressed sector
		//  containing the User block "B" image and validity marker.
		sectorNumber = longToSector(UserBlockAddr -
		                            (SysIDBlock.idBlock2.userBlockSiz2 + 6ul));
		UserBlockAddr = sectorToLong(sectorNumber);	// force to sector start
	} else {
		totalBlocksSize = (uint32) SysIDBlock.idBlockSize +
		                  (uint32) SysIDBlock.userBlockLoc;
		if ((SysIDBlock.tableVersion >= 3) && (SysIDBlock.userBlockSize != 0)) {
			totalBlocksSize += totalBlocksSize;
		}
		UserBlockAddr = flashTop - totalBlocksSize;
	}
	UserBlockSector = longToSector(UserBlockAddr);

	// Now calculate the actual ID+User blocks' addresses, accounting for the
	//  possible flash-mapping offset depending on compile mode.
	IDBlockAddr += flashOfst;
	UserBlockAddr += flashOfst;
}

/****************************
	Return true if drivers
   and io permissions match
   what is currently loaded
/***************************/
int IDBlockPLDIOCheck(void)
{
	auto long length;
   if(SysIDBlock.tableVersion <= 5) {
   	return 0;
   }
	if(SysIDBlock.idBlock2.driversLoc && pl_drivers) {
      xmem2root(&length, pl_drivers, sizeof(length));
      xmem2root(SysIDBuffer.buffer, pl_drivers + sizeof(long), (unsigned)length);
      if(xmemcmp(IDBlockAddr - SysIDBlock.idBlock2.driversLoc, SysIDBuffer.buffer, (unsigned)length) != 0) {
         return 0;
      }
	}
   else if (pl_drivers) {
   	return 0;
   }
	if(SysIDBlock.idBlock2.ioPermLoc && io_permissions) {
      xmem2root(&length, io_permissions, sizeof(length));
      xmem2root(SysIDBuffer.buffer, io_permissions + sizeof(long), (unsigned)length);
      if(xmemcmp(IDBlockAddr - SysIDBlock.idBlock2.ioPermLoc, SysIDBuffer.buffer, (unsigned)length) != 0) {
         return 0;
      }
   }
   else if (io_permissions) {
   	return 0;
	}
   return 1;
}

#if RAM_COMPILE
 #ifndef V2_USE_USERBLOCKADDR
	#define V2_USE_USERBLOCKADDR 0
 #endif
nodebug
void RAM_GetUserBlockInfo(struct userBlockInfo *uBI)
{
	auto char validTest[6];
	auto int sectorNumber;
	auto uint32 flashOfst, offset;

	// initially zero all struct members
	memset(uBI, 0, sizeof(struct userBlockInfo));

	uBI->topAddr = sectorToLong(_FlashInfo.numSectors);
	// We manually map the /CS0 flash into the top (MB3CR) quadrant for both
	//  RAM_COMPILE and FAST_RAM_COMPILE modes, so the flash offset is relative
	//  to the end of the Rabbit's 1 MByte physical memory space.
	flashOfst = 0x100000ul - uBI->topAddr;

	// get the as-just-written size of the User block
	uBI->blockSize =  SysIDBlock.userBlockSize;
	if (!uBI->blockSize && 5 <= SysIDBlock.tableVersion) {
		uBI->blockSize =  SysIDBlock.idBlock2.userBlockSiz2;
	}

	if (uBI->blockSize) {
		switch (SysIDBlock.tableVersion) {
		default:	// cases 5 and up
			if (SysIDBlock.userBlockSize) {
				// "old style" mirrored combined ID/User block images, located
				//  immediately below the top of flash, possibly with a storage area
				//  gap between the ID and User blocks within an image
				uBI->addrA = uBI->topAddr -
				             (SysIDBlock.idBlockSize + SysIDBlock.userBlockLoc);
				uBI->addrB = uBI->addrA -
				             (SysIDBlock.idBlockSize + SysIDBlock.userBlockLoc);
				//*** block A valid? ***
				xmem2root(validTest, flashOfst + uBI->topAddr - 6ul, 6u);
				if (!memcmp(VALIDMARKER, validTest, 6u)) {
					uBI->blockAvalid = 1;
				} else {
					//*** block B valid? ***
					xmem2root(validTest, flashOfst + uBI->addrA - 6ul, 6u);
					if (memcmp(VALIDMARKER, validTest, 6u)) {
						uBI->blockSize = 0ul;	// no valid block
					}
				}
			} else if (SysIDBlock.idBlock2.userBlockSiz2) {
				// "New style" unique ID block plus mirrored User block images,
				//  located immediately below the top of flash, possibly with a
				//  persistent storage area between the ID block and the top-most
				//  User block image.  None of the ID block and the User block
				//  images are permitted to share a flash sector.
				// NB:  Each "new style" User block image is followed by a six byte
				//      validity marker which is generally included within the
				//      flash sector (and typically also 4 KB) boundary.
				offset = SysIDBlock.idBlock2.persBlockLoc;
				sectorNumber = longToSector(uBI->topAddr - offset);
				uBI->addrA = sectorToLong(sectorNumber);
				offset = SysIDBlock.idBlock2.userBlockSiz2;
				sectorNumber = longToSector(uBI->addrA - 6ul - offset);
				uBI->addrA = sectorToLong(sectorNumber);
				sectorNumber = longToSector(uBI->addrA - 6ul - offset);
				uBI->addrB = sectorToLong(sectorNumber);
				//*** block A valid? ***
				xmem2root(validTest, flashOfst + uBI->addrA + uBI->blockSize, 6u);
				if (!memcmp(VALIDMARKER, validTest, 6u)) {
					uBI->blockAvalid = 1;
				} else {
					//*** block B valid? ***
					xmem2root(validTest, flashOfst + uBI->addrB + uBI->blockSize, 6u);
					if (memcmp(VALIDMARKER, validTest, 6u)) {
						uBI->blockSize = 0ul;	// no valid block
					}
				}
			}
			break;
		case 4:
		case 3:
			// mirrored combined ID/User block images, located immediately below
			//  the top of flash
			uBI->addrA = uBI->topAddr -
			             (SysIDBlock.idBlockSize + SysIDBlock.userBlockLoc);
			uBI->addrB = uBI->addrA -
			             (SysIDBlock.idBlockSize + SysIDBlock.userBlockLoc);
			//*** block A valid? ***
			xmem2root(validTest, flashOfst + uBI->topAddr - 6ul, 6u);
			if (!memcmp(VALIDMARKER, validTest, 6u)) {
				uBI->blockAvalid = 1;
			} else {
				//*** block B valid? ***
				xmem2root(validTest, flashOfst + uBI->addrA - 6ul, 6u);
				if (memcmp(VALIDMARKER, validTest, 6u)) {
					uBI->blockSize = 0ul;	// no valid block
				}
			}
			break;
		case 2:
 #if (_BOARD_TYPE_ & 0xFF00) == (BL2000 & 0xFF00) || \
     (_BOARD_TYPE_ & 0xFF00) == (BL2100 & 0xFF00) || \
     V2_USE_USERBLOCKADDR
			// single User Block as located by the BIOS, offset by our manual
			//  RAM_COMPILE mode flash-mapping
			uBI->addrA = UserBlockAddr - flashOfst;
 #else
			// single combined ID/User Blocks immediately below top
			uBI->addrA = uBI->topAddr -
			             (SysIDBlock.idBlockSize + SysIDBlock.userBlockLoc);
 #endif
			uBI->blockAvalid = 1;
			break;
		case 1:	// real ID Block
		case 0:	// no ID Block
			// single simulated 8K User Block starting at top minus 16K
			uBI->addrA = uBI->topAddr - 0x4000ul;
			uBI->blockAvalid = 1;
			break;
		}
	}
	// update the uBI information to suit the flash-mapping
	uBI->topAddr += flashOfst;
	if (uBI->addrA) uBI->addrA += flashOfst;
	if (uBI->addrB) uBI->addrB += flashOfst;
}
#endif

