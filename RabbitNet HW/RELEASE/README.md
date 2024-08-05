Programming Notes
=================

These are notes from Tom's experience with programming RN1600 hardware in
July 2024.

Equipment
---------
Using a [Raisonance RLINK STD][1] for programming, connected to a [PCB
Probe Clip][2].  This device is a spring-loaded clip with POGO pins that make
contact with the unpopulated J3 (ICP PROGRAMMING) connector on the RN1600.

[1]: https://www.raisonance.com/RLink-STD__product__rlink.html
[2]: https://www.tindie.com/products/johnnywu/254mm-pcb-probe-clip/

Installed ST Visual Programmer ST7[3] on an Windows 10 (Intel) VM.

[3]: https://www.st.com/en/development-tools/stvp-st7.html

Separately ran the RLinkUSBInstall that the STVP installer added to the start
menu.  (RLinkDrv_1210_WinDriver_WinUSB_Win8_signed).

Ran the ST Visual Programmer app after installation.  Configured for RLINK,
USB, and "ICP OPT Enable" for an ST72F62x4 (to match the ST72F264G2M6 used).

Set jumpers on the RLINK-STD:
* First jumper set for ICC.
* Second jumper set for power.
* The other two jumpers are disconnected.

Plugged the RLINK into a USB port.  It provides power to the RN1600 over the
10-pin programming header.  Windows automatically installed the drivers
(available from the previous ST install).

At this point, it should be possible to read the chip and verify that the
interface is working.

Programming
-----------
1. Select the Program Memory TAB, then load file the appropriate S19 file.
2. Select the Option Byte TAB, then load file "Option Bytes.S19".
3. Select Program ALL TABS option for the device to be fully programmed.

The firmware image stores the 8-character serial number as BCD (binary-coded
decimal) at address 0xF700, if you want to write the correct serial number
to the board.  The RN1600 file has a dummy serial number of 12345678.
