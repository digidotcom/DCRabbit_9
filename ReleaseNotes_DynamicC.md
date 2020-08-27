![Digi logo](https://www.digi.com/digi/media/nav/logo-digi.png "Digi International Inc.")  
**DIGI INTERNATIONAL**  
9350 Excelsior Blvd, Suite 700  
Hopkins, MN 55343, USA  
+1 (952) 912-3444 | +1 (877) 912-3444  
[www.digi.com](https://www.digi.com)

Dynamic C 9 Release Notes
=========================
### Dynamic C 9
### Version 9.62A (August 2020)

---

INTRODUCTION
------------
These are the release notes for Dynamic C 9.

[Dynamic C 9](https://www.digi.com/support/productdetail?pid=5053) is the
IDE (Integrated Development Environment) for Digi International's core
modules and single-board computers based on Rabbit 2000 and 3000 processors.

The IDE includes a graphical editor and debugger, a command-line compiler,
and a graphical tool (Rabbit Field Utility) for installing compiled binary
firmware to a serially-connected device.

SUPPORTED PRODUCTS
------------------

  * BL1800, BL1810, BL1820
  * BL2000, BL2010, BL2020, BL2030, BL2040
  * BL2100, BL2101, BL2110, BL2111, BL2115, BL2120, BL2121, BL2130
  * BL2500, BL2510
  * BL2600, BL2610
  * EG2100, EG2110
  * LP3500, LP3510
  * OP6600, OP6700, OP6800, OP6810, OP7200, OP7210
  * PowerCore FLEX Board Series, RabbitFLEX SBC40 Series
  * RCM2000, RCM2010, RCM2020
  * RCM2100, RCM2110, RCM2120, RCM2130
  * RCM2200, RCM2250, RCM2260
  * RCM2300
  * RCM3000, RCM3010
  * RCM3100, RCM3110
  * RCM3200, RCM3209, RCM3210, RCM3220, RCM3229
  * RCM3300, RCM3305, RCM3309, RCM3310, RCM3315, RCM3319, RCM3360, RCM3365,
    RCM3370, RCM3375
  * RCM3400, RCM3410
  * RCM3600, RCM3610
  * RCM3700, RCM3710, RCM3720, RCM3750, RCM3760
  * RCM3900, RCM3910
  * SR9100, SR9150, SR9160

KNOWN ISSUES
------------

1. Unsupported page number formats may crash DC.  For example, `%t`
   is not supported in the Page Number Text field of the Page Setup dialog.

2. A terminating program can have debugging problems if a breakpoint is set
   on the first line when the program starts or restarts. With no breakpoint
   at the first line all debugging behaves normally.

3. A boolean comparison, e.g. `lhs < rhs`, will generate incorrect code if
   `lhs` is an (unsigned) long array by type or by cast, and `rhs` is a multi-
   dimensional array of any type in which the lowest dimension index is an
   expression of variable(s) and constant(s), e.g.
   `LongArray[0] >= Array[0][i + 1]`. The workaround is to enclose `rhs` in
   parentheses. Correct code will be generated for
   `LongArray[0] >= (Array[0][i + 1])`. Note that correct code is generated
   for `LongArray[0] >= Array[0][i + j]` because the lowest index expression
   does not involve any constants.

4. Source code lines in program files (.c extension) can be of any length
   beginning with version 9.60, but the 250 character maximum line length
   remains for library files.

5. The `sizeof(nonExistentStruct)`, where `nonExistentStruct` has no
   definition, evalutes to 0 instead of producing a compile error as it
   should.

6. The data type for `xstring` or `xdata` entries in an `xdata` statement can
   be cast to `long` without compiler warning or error, but the type will
   always be `long *`, and will occupy 2 bytes, not 4, for a root address
   which contains the xmem address of the entry. If one creates a struct into
   which the `xdata` entries are read, fields for `xdata` or `xstring`
   entries need to be `long *`. See sample program `xmem\xdata.c` for an
   example.

7. Stack relative addressing in an assembly statement, such as

       ld    iy, (SP+@SP+VarName)

   is limited to 255 bytes. If `VarName` is more than 255 bytes deep in the
   stack this will compile without warning or error but will retrieve the
   data that is the actual depth modulo 256. The following workaround is
   always safe:

       ld    hl, @sp+VarName
       add   hl, sp
       ld    hl, (hl)
       ld    iy, hl

8. Memory leaks lose about 4Kbytes per compile. Close and re-open Dynamic C
   periodically to avoid problems.

9. A function returning a struct does not work correctly from within a
   cofunction. Values copied into the local structure will be set incorrectly.

10. A C statement with only a semicolon within a `#asm` block does not compile
   if the block is within a global `#asm` defined function. The "c ;" in
   section 13.1.3 of the Dynamic C manual does compile and is within a `#asm`
   defined function but that function is within a C defined function. Using
   `"rst 0x28"` is a suitable substitute for "c ;" within any `#asm` block.

11. An array can be used to initialize a type compatible constant pointer,
    unless the array is a struct member. A workaround for this case is to
    initialize the pointer with the address of the struct member array which
    will assign the expected value to the pointer and the program will run as
    expected despite a warning about a type mismatch.

        struct {
            int memberArray[];
        } s;

        int globalArray[];

        int * const pInt = globalArray;    // ok
        int * const pInt = s.memberArray;  // results in an error
        int * const pInt = &s.memberArray; // workaround

12. A debug timeout target crash may occur after hot-swapping a smaller xD
    card for a larger xD card using the presently documented method. For a
    workaround to this problem, please refer to sample program
    `FAT\fat_hot_swap_3365_75.c`. Note the added comments and call to the
    low level, undocumented `_fat_config_init()` function.

13. In debug run mode (as opposed to stand-alone run mode) following a program
    termination caused by a run time exception, the Dynamic C debug kernel
    does not always correctly restart an application subsequent to a normal
    program termination. Symptoms can include an apparent application hang,
    non-clearing of STDIO window content before adding new content, unhandled
    RST38, etc.

    To work around the problem the application program should not be allowed
    to terminate normally in debug run mode. When testing small sections of
    code which include the possible generation of run time exceptions, adding
    a `while (1) ;` loop as necessary to prevent application termination can
    be helpful. Then, in order to restart the test code for another debug
    session, type `Ctrl+Q` to stop program execution in the debugger followed
    by `Ctrl+F2` to reset the execution cursor back to the start of `main()`.

    Note that a typical embedded application does not terminate in normal
    operation.

14. Sample program `Samples\RCM3300\Module_Integration\integration.c` does
    not run properly when run with Down Load Manager support.

15. The command-line compiler will not emit warnings and errors when compiling
    to a binary file.  The workaround is to compile to an attached target
    using the "-n" command-line option for a "Null compile/syntax check
    without running".  If the compiler does not emit any errors, re-compile
    with one of the binary output options (-b, -br).

16. On the BL2600 SBCs, there is some overlap of calibration constants stored
    in the UserBlock.  If you need to access both:

    - single-ended unipolar channels 2 to 7 with gaincode 7 and
    - single-ended  bipolar channels 0 to 5 with gaincode 0

    you will need to define macro `BL26XX_CAL_ADC_OVERLAP_FIX` in your
    programs and re-calibrate the affected unipolar channels with gaincode
    7 and ALL of the bipolar channels with all gaincodes.

UPDATE CONSIDERATIONS
---------------------
If updating an existing installation of 9.62 or earlier via the Dynamic C 9
[GitHub repository](https://github.com/digidotcom/DCRabbit_9), you should
manually apply the changes in `ucos2.patch`, found in the Dynamic C
installation directory.  The Dynamic C 9.62A (and later) installers include
the patched file.

UPDATE BEST PRACTICES
---------------------
Digi recommends testing the new release in a controlled environment with
your application before you deploy firmware to production devices.

TECHNICAL SUPPORT
-----------------
Get the help you need via our Technical Support team and online resources.
Digi offers multiple support levels and professional services to meet your
needs. All Digi customers have access to product documentation. firmware,
drivers, knowledge base and peer-to-peer support forums.

Visit us at [https://www.digi.com/support](https://www.digi.com/support)
to find out more.

---

CHANGE LOG
==========
## 9.62A (August 2020)
This is a recommended release.

The big change for this release is that Dynamic C is now an [Open
Source project on GitHub](https://github.com/digidotcom/DCRabbit_9).
It includes source code to the previously-encrypted SSL libraries,
and the license was changed to MPL 2.0 (Libraries) and ISC (Samples).

This release was made as 9.62A since it does not include changes to
the compiler and only contains modified libraries, samples and
documentation.  It uses shortcuts (`.lnk` files) to launch the compiler
with a command-line parameter of `-c A`, forcing the compiler to
identify itself as 9.62A and use registry entries for version 9.62A.

Note that the bugfix for DC-391 below is included in the Dynamic C
9.62A installer, but included as a patch in the GitHub repository
due to licensing issues with the uC/OS-II code.  View `ucos2.patch`
for instructions on how to apply it.

### NEW FEATURES
- Incorporates Serial Flash update (40002882_A) for revision E of
  45DB641 chips used in new hardware.  See `PatchNotes_SerialFlash.txt`
  for details.
- Add `Samples/extract_flash.c` to download firmware stored on device flash.
- Add `SFLASH_ERASE.C` sample for RCM33xx and RCM37xx to erase serial flash.

### BUG FIXES
- DC-73: fix calculation of `XMEMORYSIZE` in `StdBios.c` for
  `FAST_RAM_COMPILE` mode.
- DC-207: Correctly render SSI/RabbitWeb (shtml/zhtml) tags spanning
  multiple 256-byte blocks in files on FAT filesystem.
- DC-260: Improve ASIX Ethernet link recovery by allowing more time
  to establish link after turning PHY back on (8s vs. 2s).
- DC-283: Repeatedly setting a gateway then deleting all routers no
  longer fills APR cache.
- DC-358: Create `ZNETSUPPORT.LIB` from subset of `remoteuploaddefs.lib`
  to allow use of zconsole apps on non-RCM3300 hardware.
- DC-391: Usage of `OS_TASK_OPT_STK_CLR` with `OSTaskCreateExt()` would
  erase an extra byte after the stack.  For stacks ending at a 64KB
  boundary (e.g., 0x6FFFF), it would wrap to 0 (e.g., 0x60000).
- RCM-94: support 45DB642D serial flash devices with 1024-byte page size.
- RCM-126: add RCM3900/disable_ethernet.c to disable ASIX Ethernet when
  not using TCP/IP.  Chip generates excess heat if not initialized at
  startup.
- Fix `ASIX.LIB` error in patch2 that broke link recovery code.
- Fix `%g` formatting error for floats smaller than 0.1 (backported from
  Dynamic C 10).
- Fix "compile to RAM" on RCM3750 by updating `_Get_Stack_Size()` which
  failed for physical addresses larger than 1MB.
- Fix Timer A registers used in `pktEopen()` of `PACKET.LIB`.
- Don't allow expiration of existing ARP entry in `router_add()` (backported
  from Dynamic C 10).
- Backport changes from Dynamic C 10 to fix duplicate ACK and keepalive
  issues in `TCP.LIB`.

### OTHER FUNCTIONAL CHANGES
- Document known issue #15 with the command-line compiler not emitting
  warnings/errors for "compile to binary" mode.
- Document known issue #16 and provide work-around for overlapping
  calibration constants on BL26xx SBCs. (DC-276)
- Add Function Help (CTRL-H) for `_xalloc()`.

---

## 9.62 PATCH UPDATE 2 (February 2013)

### NEW FEATURES
- The `Lib\NandFlash\nflash.lib` driver library has been updated with support
  for Micron/Numonyx/ST NANDxxxW3A nand flash devices. Previously supported
  nand flash devices, used on e.g. RCM3365 and RCM3900 boards, have been
  discontinued and are no longer available. As of 01-Dec-2012, new production
  e.g. RCM3365 and RCM3900 boards will have a Micron/Numonyx/ST NAND256W3A
  (32MB) nand flash device installed.

  Custom applications which use nand flash, whether based on e.g. RCM3365,
  RCM3900 boards or a custom nand flash board design, should be recompiled
  using Dynamic C 9.62 with this PATCH UPDATE 2 installed in order to support
  the new, increased selection of small-block nand flash devices.

- Includes the content of an earlier patch which adds support to
  Dynamic C 9.62 for a new BL2600Q board type, product ID 0x170F, comprised
  of an RCM3900 installed into a BL2600 base board. Please refer to the
  `README_BL2600Q_PATCH.TXT` file for details.

### BUG FIXES
- Defect 20311 patched. On boards such as the OP6700 or TCP/IP Dev Kit, the
  MAC address was not being read from the serial EEPROM.
- Defect 20900 patched. When UDP send functionality's ARP resolution fails it
  now correctly releases its ARP table handle.
- Defect 31569 patched. When `ARP.LIB` functionality detects an IP conflict
  with a remote host, the response is now correct for either gratuitous or
  normal ARP requests.
- Defect 31780 patched. Multicast ARP table entries are no longer purged.
- Defect 32677 patched. Rabbit 3000A's SSL now works with Firefox and Safari
  web browsers.
- Defect 42241 patched. Rabbit now responds correctly to ARP probes (i.e. "who
  has IP w.x.y.z?" Ethernet broadcast) from source IP 0.0.0.0.
- Defect 42695 patched. A two-flash (i.e. 2*256K flash devices on board) DLP
  now has the full 256K secondary flash size available for program code.
- Defect DC-4 patched. `HTTPC.LIB`'s HTTPC post functionality, following a
  successful post operation, sets `HTTPC_STATE_HEADER` state.
- Defect DC-19 patched. The ASIX PHY link-fail bug work-around, broken in
  9.62 PATCH UPDATE 1, is fixed. Ethernet link is restored within a few
  seconds of cable reconnection after either a lengthy (approximately 20
  minutes or more) cable disconnection or a series of repeated quick cable
  disconnection, reconnection events.
- Defect RCM-7 patched. Prevent reset of a TCP connection's keep-alive timer
  in the case that a previously connected remote host attempts to establish a
  new connection which re-uses its previous TCP connection's source and
  destination IP addresses and port numbers.
- Defect 42746 patched. The `Samples\xalloc_stats.c` standard sample program
  no longer reports an error due to calling `xalloc_stats()` with a zero
  parameter.

### OTHER FUNCTIONAL CHANGES
- In `tcp_config.lib`, the `_PRIMARY_STATIC_IP` and `_PRIMARY_NETMASK` macro
  definitions are now conditional to prevent warnings when overridden by custom
  definition(s). The default definitions remain the same, "10.10.6.100" and
  "255.255.255.0" respectively.

- Defining `DO_NOT_WARN_ON_RT_FULLDUPLEX` before `#use dcrtcp.lib` prevents a
  warning in `REALTEK.LIB` when `RT_FULLDUPLEX` is also defined.

---

## 9.62 PATCH UPDATE 1 (June 2011)

### NEW FEATURES
- This patch includes a previous patch to add BL2600Q (an RCM3900 installed
  into a BL2600 base board) support to Dynamic C 9.62. See
  `README_BL2600Q_PATCH.TXT` in the main Dynamic C 9.62 installation folder
  for details.

- Two different new behaviors are available after a BIN file is loaded to the
  Rabbit target via the RFU:

    1. When `RFU_BIN_WAIT_FOR_RESET` is defined, after the BIN file is loaded
       via the RFU the application will not run until the programming cable is
       disconnected and followed by a hardware reset.
    2. When `RFU_BIN_WAIT_FOR_RUN_MODE` is defined and `RFU_BIN_WAIT_FOR_RESET`
       is not defined, after the BIN file is loaded via the RFU the
       application will not run until the programming cable is disconnected.

  An easy place to define the desired new macro to control BIN file behavior
  is in Dynamic C's Project Options' Defines box. Note that when neither
  `RFU_BIN_WAIT_FOR_RESET` nor `RFU_BIN_WAIT_FOR_RUN_MODE` are defined,
  Dynamic C 9.62 behavior is per its usual. Also note that compiling in debug
  mode when either of these macros is defined will result in a "helpful"
  error message.

- Added HTTP client support via the new `Lib\tcpip\httpc.lib` library. See
  also the new `Samples\tcpip\http\http_client.c` sample program.

### BUG FIXES
- Defect 21640 patched. Stack(s) are now allocated more space-efficiently,
  using `_xalloc()`'s alignment capability. Also, the correct value is
  assigned to the global `BaseOfStacks1` variable.
- Defect 24080 patched. `SERLINK.LIB`'s `ppplink_asymap()` function now
  respects the peer's asynchronous serial escape character table setting.
- Defect 24260 patched. Updated `WIEGAND.LIB`'s `wiegand_init()` function to
  always call the appropriate `SetVectExternX000()` and the `SetVectIntern()`
  functions.
- Defect 24421 patched. The `errmsg.ini` file and the `ERRNO.LIB` and
  `ERRORS.LIB` libraries are patched to add `ESHAREDBUSY` (240) and `EFUNCBLK`
  (243) error codes and to make the `EFATMUTEX` (300) error code consistent
  throughout.
- Defect 24430 patched. Corrected the `OP68xx.LIB` library's `brdInit()`
  function help descriptions of PC0,1 (serial port D, RS485) and PD4,5
  (serial port B, RS232).
- Defect 24474 patched. Improved the `COSTATE.LIB` library's `DelayTicks()`,
  `DelayMs()` and `DelaySec()` function help descriptions.
- Defect 24475 patched. Improved the `COSTATE.LIB` library's `IntervalTick()`,
  `IntervalMs()`, and `IntervalSec()` function help descriptions.
- Defect 24477 patched. The `COSTATE.LIB` library's `DelayTicks() function is
  patched to accept a `long` ticks delay parameter. This matches all other
  `Delay*()` and `Interval*()` firsttime functions.
- Defect 24743 patched. The `PowerCoreFLEX.LIB` library's `switchIn() function
  now causes a `-ERR_BADPARAMETER` run time error to occur if its `swin`
  parameter is invalid.
- Defect 25077 patched. A work around in `IP.LIB`'s internal
  `is_nonlocal_subnet()` function allows multiple PPP interfaces (e.g on
  serial ports B and D) to work correctly.
- Defect 25307 patched. `SERLINK.LIB`'s serial ISR now preserves XPC.
- Defect 25613 patched. The `REALTEK.LIB`'s internal `emu9346_clk()` function
  is updated to prevent the possibility of an asm jump out of range error.
- Defect 25727 patched. The `RNET.LIB` `rn_find function()` now correctly
  searches for Rabbitnet device(s) on the specified RabbitNet port(s).
- Defect 25750 patched. The `FAT.LIB` library's `fat_Truncate()` function's
  ordering of FAT code checks is updated to work around a truncation bug
  caused by a mixed-type comparison (`long` vs. `unsigned long`) of the
  negative FAT code vs. the non-negative current file position.
- Defect 25944 patched. The `WEIGAND.LIB` library's `weigand_init()` function
  now correctly supports either of the input capture 1 or 2 options on Rabbit
  3000 targets.
- Defect 26212 patched. The `RNET_KEYIF.LIB` library's `void rn_keyUnget()`
  function no longer attempts to return a value.
- Defect 27167 patched. The `BOOTP.LIB` library's `dhcp_init()` function now
  preserves DHCP options requests.
- Defect 27247 patched. Dynamic C's `LIB.DIR` file has been updated to remove
  the reference to a non-existent `RN_CFG_RCM39.LIB`.
- Defect 27384 patched. The Remote Application Update (RAU) now correctly
  supports customizing the number of DLP images that may be stored.
- Defect 27692 patched. The IX register's content is now preserved by
  `SYS.LIB`'s `updateTimers()` function.
- Defect 28314 patched. The `STDIO.LIB` library's `snprintf()` function now
  always null-terminates its output buffer.
- Defect 28403 patched. The RCM3300 Remote Application Update (RAU) DLM / DLP
  shared network information save / restore feature now works correctly.
- Defect 28846 patched. Rabbit boards equipped with an ASIX 10/100 Ethernet
  device and connected to some models of network switch (e.g. Foundry Fastedge
  X448, Linksys EF3124, Linksys EZXS55W) now reliably recover link after the
  Ethernet cable is disconnected then reconnected.
- Defect 29196 patched. `UCOS2.LIB`'s `ISIntExit()` is patched to prevent
  possible errant task switching if
  `OSLockNesting && --bios_intnesting == OSLockNesting`.
- Defect 30070 patched. `Mem.lib`'s `_rs_xalloc_init()` function is patched
  to work around a Dynamic C compiler ORG table problem with non-flagged
  battery backable RAM in "Compile to Flash, Run in RAM" compile mode.
- Defect 30489 patched. The `BL26xx.LIB`'s `brdInit()` function now handles
  PD2 correctly for the BL2600Q's SD card detect. (A previous patch to
  Dynamic C 9.62 was provided to support the BL2600Q - an RCM3900 core on a
  BL2600 base board.)
- Defect 31863 patched. The `GRAPHIC.LIB`'s `TextBorderInit()` function help
  is updated to reflect a `GRAPHIC.LIB` code patch to correctly handle either
  a `NULL` or an empty (i.e. `""`) string passed in as the title parameter.
- Defect 32281 patched. `FTP_CLIENT.LIB` is updated to work with the IIS FTP
  server by discarding any extra arguments in the response to Rabbit's QUIT.
- Defect 32434 patched. `SDFLASH.LIB`'s `sdspi_get_csd()` function now
  supports SD cards that use CRC7 instead of CRC16 in their CSD block.
- Defect 32470 patched. Serial data flash sizes in `SFLASH.LIB`'s
  `sf_devtable[]` information array now reflect the full number of pages on
  each of the supported devices.
- Defect 32937 patched. Corrupted FAT MBR information no longer causes a divide
  by zero run time exception.
- Defect 33853 patched. All serial ports' initialization in `SERLINK.LIB` now
  reads `SxDR` four times, which suffices to clear any possible pending data
  in any serial port.
- Defect 34017 patched. `ZSERVER.LIB`'s `sspec_changefv()` no longer truncates
  maximum-length string type form variables to be one less than the maximum
  specified length.
- Defect 34070 patched. In non-blocking FAT mode, a `fat_Truncate()` call to
  within a file's first cluster followed by a `fat_Close()` call no longer
  hangs with a permanent `-EBUSY` result.
- Defect 35373 patched. Rabbit applications which do not have a battery-backed
  RTC may see an incorrect result at start up from the BIOS's bios_divider19200
  and freq_divider calculations. To prevent this baud rate miscalculation,
  non-battery-backed RTC Rabbit applications should now be compiled with
  `ROBUST_NON_BB_BAUD_RATE_CALC` added into Dynamic C's Project Options'
  Defines box.
- Defect 38754 patched. `RS232.LIB`'s ISRs now perform ipres as the very last
  op-code before returning to pre-interrupt code. This prevents any
  possibility of contributing to gradual stack overflow.
- Defect 38777 patched. When uC/OS-II is used, `SERLINK.LIB`'s ISR now
  prevents task switching while interrupt processing continues with reenabled
  interrupts.
- Defect 39302 patched. In `modbus_master.lib`, custom `_initMBMpacket()`
  functionality is restored for TCP-only Modbus master boards. A library
  function stub is now provided, along with function help.
- Defect 39386 patched. Updated `HTTPC.LIB`'s `httpc_post_ext()` and
  `httpc_postx_ext()` functions to prevent errant compile-time errors.
- Defect 25978 patched. The `Samples\FileSystem\FAT\fat_write_mbr.c` standard
  sample program's instructions for erasing a serial flash's previous partition
  information now specify clearing pages 0 through 5, inclusive.
- Defect 26635 patched. The stock `RabbitLink\clear_param.c` (utility) and
  `RabbitLink\download.c` (firmware) programs now compile cleanly, without
  warnings or errors.
- Defect 26636 patched. The `RabbitLink\download.c` (firmware) program now
  compiles without error when its USE_DHCP macro is commented out.
- Defect 27230 patched. The Modbus (former module) `Samples\Modbus` standard
  sample programs and a Modbus-related executable utility program are
  supplied.
- Defect 27289 patched. `The Samples\FileSystem\FAT\fat_write_mbr.c` standard
  sample program now explicitly disallows partitioning of an SD card flash
  memory device.
- Defect 36077 patched. `The Samples\OP7200\Calib_Save_Retrieve\savecalib.c`
  standard sample program's serial cable connection instructions now refer to
  the appropriate header (J10).
- Defect 38736 patched. The `Samples\OP7200\LCD_TouchScreen\cal_touchscreen.c`
  standard sample program now enables the backlight and sets the screen
  contrast to avoid a blank appearance.

---

## 9.62 (May 2008)

### BUG FIXES

- Defect #25923 fixed. Corrected `.c` part of file mask for grep.
- Defect #25748 fixed. Corrected initialization of RCM3900 and
  RCM3910 boards' SD card select and SD card detect lines.

---

## 9.60 (February 2008)

### BUG FIXES

- Defect #24366 fixed.  TCP connections to peers with large
  receive windows no longer time out or reset.
- Defect #24941 fixed.  The built-in uCOS-II statistics variables
  are now correctly initialized when used, so the `OS_TaskStat()`
  function now always provides a reasonable OSCPUUsage value.
- Defect #24990 fixed.  Incorrect `#asm` block accesses of the
  `auto int statusbyte` variable in the `Lib\RabbitNet\RNET_KEYIF.LIB`
  library's `rn_keyProcess()` function have been fixed.  As a
  result of this fix, the `Samples\RabbitNet\RN1600\zmenu.c` sample
  now works correctly when making keypad menu selections.
- Defect #25085 fixed.  Partition begin / end tests in `PART.LIB`'s
  `mbr_ValidatePartitions()` function now work correctly.  A code
  generation problem work around is implemented for boolean
  conditional expressions which reference multi-dimensional
  arrays of long integer type where a non-constant expression is
  used to calculate at least one of the indices.  The work around
  is to encapsulate such references in parentheses and explicitly
  cast the reference to the appropriate long integer type within
  the encapsulating parentheses.
- Defect #25143 fixed.  In non-blocking mode, `FAT.LIB`'s
  `fat_ReadDir()` function no longer returns success (0) when zero
  bytes are read from the directory entry.  In this case,
  `fat_ReadDir()` now correctly returns `-EBUSY`.

---

## 9.52 (February 2007)

### BUG FIXES

- Defect #19939 fixed. Improper code generation with constant
  indicies in parenthesis used with multi-dimensional arrays now
  generates a warning. A workaround is detailed in the `ReadMe.txt`
  file.
- Defect #20472 fixed. PPP serial more reliable with bad link quality.
- Defect #81727 fixed. `OP72xx.LIB` `serMode()` function now always
  returns the appropriate result code.
- Defect #81695 fixed.  Added check for years entered as more than 2 digits.
  Added date range notification to stdio window.

---

## 9.50 (September 2006)

### NEW FEATURES

- Added the error_message function to `errors.lib`. This function
  returns a descriptive string in xmem corresponding to an error
  code.
- New sample `tcpip\http\cgi_concurrent.c` demonstrates how to
  manage concurrent CGI instances accessing a single shared
  resource, including proper behavior when the HTTP connection
  aborts.

### BUG FIXES

- Defect #81458 fixed. Missing right parens in statements
  generated bogus line numbers, and a 2nd compile could cause a
  fatal error.
- Defect #81474 fixed. Compiler reported bogus error message
  about an unclosed comment at end of file if there was a name
  collision between symbols.
- Defect #81482 fixed. Execution cursor could be incorrect when
  stepping over a nodebug function containing `_TRACE`.
- Defect #81485 fixed. DC now generates correct code for complex
  conditionals in cofunctions.
- Defect #81546 fixed. Constant structure assignment fixed.
- Defect #81556 fixed. `float` to `unsigned long` cast error fixed.
- Defect #81557 fixed. Constant multi-dimensional array of
  structs access fixed.
- Defect #81564 fixed. Forward jumps in `asm const` blocks now
  correctly fixed.
- Defect #81568 fixed. Setting `FAST_INTERRUPT` to 1 in separate
  I&D no longer generates compile-time errors.
- Defect #81575 fixed. Execution tracing no longer generates a
  Target communication error.
- Defect #81587 fixed. Many memory leaks fixed.
- Defect #81613 fixed. Error log exception values changed.
- Defect #81636 fixed. Nested structure and union member access
  and assignment fixed for multiple levels of structure nesting.
- Defect #81255 fixed. Dynamic C will now exit if the project file
  is read-only.
- Defect #81448 fixed. Dissassemble at cursor function was not
  working in libraries.
- Defect #81449 fixed. Warnings highlighted incorrect lines and
  the single step debug cursor placement was sometimes incorrect
  in library code.
- Defect #81450 fixed. Copy-and-paste containing TABs could cause
  strange behavior.
- Defect #81455 fixed. Watches for some constant constructs not
  working.
- Defect #81476 fixed. Dynamic C no longer can consume all
  available virtual memory and effectively lock up the PC.
- Defect #81483 fixed. Project open/close did not cancel if
  modified file close query was cancelled.
- Defect #81490 fixed. You can now right click for copy/paste in
  project options defines box.
- Defect #81547 fixed. The compiler can now use COM10 or higher
  for target communications.
- Defect #81566 fixed. Switching focus to DC no longer moves the
  cursor to the compiler message box.
- Defect #81572 fixed. Memory leak in compiler messages window
  corrected.
- Defect #81573 fixed. Debug mode problems when running BL1810
  (14 MHz) JackRabbit (did not stop on F4) corrected.
- Defect #81576 fixed. Switch to debug mode (shift + F5) now
  works in compile-to-RAM mode.
- Defect #81621 fixed. Flyover watch expressions are now correct
  for string constants in xmem code.
- Defect #81631 fixed. Watching string literals has been fixed.
- Defect #81480 fixed. `FS2.LIB`'s `fs_sync()` will no longer return
  an uninitialized  value as its result.
- Defect #81487 fixed. Constant `LZ_LOOK_AHEAD_SIZE` now defined
  in `LZSS.LIB`.
- Defect #81492 fixed. `STRING.LIB`'s `memcpy()`/`memmove()` functions
  returned `src` instead of `dst` if copy size was zero.
- Defect #81543 fixed. Timeout is no longer calculated during SYN
  handshake.
- Defect #81560 fixed.  `PCFR` and `PCFRShadow` were not set
  correctly in `spa_init()` in `RS232.LIB`.
- Defect #81570 fixed. Corrected `removeram` origin directive
  definition.
- Defect #81574 fixed. Dynamic C crash and exit due to incorrect
  origin directive definition fixed.
- Defect #81577 fixed. `removeram` origin now defined for flash
  compile mode.
- Defect #81580 fixed. Documentation: `WrPortI()` can be interrupted
  by ISRs.
- Defect #81581 fixed. `sf_enableCS()` and `sf_disableCS()` now
  protect shadow values when calling `WrPortI()`.
- Defect #81585 fixed. Setting a watch expression will no longer
  corrupt memory in non-separate I&D mode.
- Defect #81590 fixed.  RabbitFLEX keypads with other than 2
  outputs now work correctly.  Also, this change will cause the
  outputs on a 2-output keypad to swap, so reported keycodes on
  a 2-output keypad will change.
- Defect #81597 fixed. DHCP Fallback is working again.
- Defect #81604 fixed. `xalloc_stats(0)` is working again.
- Defect #81608	fixed. DHCP client now updates the IP address
  assigned when the server requires it.
- Defect #81657 fixed. On the ASIX AX88796 Ethernet chip, when
  the Ethernet cable is rapidly plugged and unplugged, the PHY
  could fail to detect a connection. The ASIX driver will now
  periodically check the link status, and will reset the PHY
  when it is down to work around the problem.
- Defect #81662 fixed. Analog and Ethernet related macros are now
  defined appropriately for the BL2000B.
- Defect #81663 fixed. The BL2010 and BL2030 specific `ADC_1OBIT`
  macro has been corrected to read `ADC_10BIT` (capital letter O
  typo changed to a numeral zero).
- Defect #81666 fixed. The TCP/IP stack will no longer respond on
  TCP port 0 when the reserve ports functionality is enabled.
- Defect #81667 fixed. Programs now initialize properly after
  reset. This bug had only affected non-looping programs.
- Defect #81675 fixed. BIOS's `UserBlockAddr` variable is now
  correct in Compile to flash, run in fast RAM compile mode when
  target board has a unique version 5 ID block installed.
- Defect #81678 fixed. Can now read all of User block when
  version 5 unique ID block and large size User block are
  installed.
- Defect #81507 fixed. `Userblock_clear.c` now correctly attempts
  to clear only the available User block area.
- Defect #81508 fixed. `Userblock_info.c` now reports correct
  information for unique v. 5 ID block.
- Defect #81551 fixed. `SerialToEthernet/FastSer2Eth.c` no longer
  causes timeout errors.
- Defect #81552 fixed. `MultiPort_S2E` - Errors no longer generated
  when compiling to a `.bin` file.
- Defect #81559 fixed. The `dlm_serial_2_flash.c` sample now works
  again.
- Defect #81561 fixed. Sample `zconsole\userblock_tcpipconsole.c`
  did not compile. `NetConfSave` is now defined.
- Defect #81578 fixed. The RFU will no longer issue an error if
  the file dropdown list exceeds 9 entries.
- Error logging is updated to version 2. Run time exceptions are
  now reported / recorded as negative int values and Dynamic C's
  traditional run time error codes have changed. See details in
  `Lib\errno.lib` and in `Lib\BiosLib\errors.lib`.

---

## 9.41 (February 2006)

### NEW FEATURES

- Samples, libraries and documentation added to support the new
  customizable RabbitFLEX boards.
- Dynamic C's `LIB.DIR` library files list mechanism now searches
  for library files within a specified directory tree. The option
  to specify a complete or relative pathlist to each individual
  library file remains. Note that each library file name must
  be unique, regardless of whether the library is explicitly
  specified or is found via searching a specified directory tree.
  Also note that this feature was actually implemented in
  Dynamic C 9.30.

---

## 9.40 (January 2006)

### NEW FEATURES

- Libraries and documentation updated for the OP7200 family,
  which has a new LCD controller.  The updated libraries work
  with both old and new versions of the OP72xx.  A BIN file
  created using the new libraries will also work with both old
  and new versions of the OP72xx.  (Note that the old libraries
  will not work with the new OP72xx.)

### BUG FIXES

- Defect #81131 fixed. New compiler fixes the "undefined but used
  global label main" problem.
- Defect #81159 fixed. Disassembler correctly prints signed
  values. Note that this bug was actually fixed in version 9.30.
- Defect #81198 fixed. New compiler fixes the "RCM3700 #rcodorg
  error - out of memory" problem.
- Defect #81199 fixed. New compiler fixes the "out of memory
  conditions can put Dynamic C in an unusable state" problem.
- Defect #81335 fixed. Assembler no longer generates incorrect
  binary for `rr a'` and related instructions. Note that this bug
  was actually fixed in version 9.30.
- Defect #81497 fixed. "bbram" variables are not allowed with the
  "static" qualifier.
- Defect #81505 fixed. Some RabbitWeb guard expressions
  incorrectly did not compile.
- Defect #81534 fixed. Writing to internal interrupt vector
  offsets higher than 0x0100 in separate I&D space no longer
  corrupts memory.
- Defect #81535 fixed. Lexer now properly recognizes '0h' and
  '0b'.
- Defect #81536 fixed. RabbitLink did not work in DC 9.30.
- Defect #81539 fixed. IO bug instructions in 2000 processors at
  end of asm blocks now properly emit nops following the
  instruction.
- Defect #81541 fixed. Command line compiler now compiles and
  runs programs on boards with small sector flash (BL1800/1810).
- Defect #81222 fixed. New `lib.dir` handling fixes this problem.
- Defect #81377 fixed. A scroll bar appears when neccessary.
- Defect #81468 fixed. The parameters of the targetless compile
  boards in the Board Selection list of the Project Options
  dialog can now be modified.
- Defect #81491 fixed. The board description had been missing
  from the compile dialogs if the Project Options dialog had
  never been opened since installing Dynamic C.
- Defect #81502 fixed. Rabbit 3000A and 4000 keywords added to
  syntax highlighting. This was fixed for Dynamic C 9.30.
- Defect #81462 fixed. Cursor placement is now correct when
  stepping over or into a nodebug function containing `_TRACE`.
- Defect #81500 fixed. Leaving the execution tracing window open
  while running for a long time no longer causes a problem.
- Defect #81477 fixed. For the PowerCoreFLEX ADC capability, it
  is now no longer possible for an ADC ISR to occur in the middle
  of reading the raw analog data.
- Defect #81514 fixed. SMTP library now works correctly in
  non-RabbitSys mode (DC 9.30-only defect).
- Defect #81525 fixed. `BBRAM_RESERVE_SIZE` now correctly
  calculated.
- Defect #81526 fixed. `BBRAM_RESERVEORG` now correctly calculated
  in non-RabbitSys mode.
- Defect #81529 fixed. Duplicate `USERDATA` definitions removed.
- Defect #81531 fixed. Duplicate cloning definitions removed.
- Defect #81532 fixed. Functionality moved to `RemoteUpload`
  libraries.
- Defect #81540 fixed. Realtek driver now works correctly for
  SmartStar.
- Defect #81515 fixed. PowerCoreFLEX version of `sflash_test.c`
  will no longer display uninitialized variables on failure.
- Defect #81537 fixed. RabbitLink did not work in version 3.01.
- Defect #81538 fixed. RabbitSys user program did not run after
  being loaded through RFU 3.01.

---

*Release Notes Part Number: 93001344*
