Bootstrapping Rabbit 2000/3000 hardware, as implemented by the Rabbit Field
Utility (RFU).  This is just a rough description of the process based on
a serial capture and review of coldload, pilot and RFU source code.

You may wish to reference [How Dynamic C Cold-Boots a Rabbit Target][TN206],
or Chapter 4 of the [Rabbit 2000 Microprocessor Designer's Handbook][2] for
further documentation on this process.

[TN206]: https://ftp1.digi.com/support/documentation/0220049_d.pdf
[2]: https://www.digi.com/resources/documentation/digidocs/PDFs/0190070_l.pdf

This process loads an initial loader (`Bios/Coldload.bin`) at 2400bps via
triplets, then jumps to 57600 to load a secondary loader (`Bios/Pilot.bin`).
It then communicates with the Pilot BIOS to write firmware to the board's
flash memory.

First, use triplets at 2400bps to load `COLDLOAD.BIN`:

* Open serial port at 2400bps, no parity, 8 data bits, 1 stop bit.
* Clear RTS (loopback to CTS) and DTR (wired to /RESET).
* Set RTS and DTR (asserting DTR holds processor in reset).
* Clear DTR (bring processor out of reset).
* Send optional "Processor Verification" triplets before the cold loader.
  Disables the watchdog timer, and then sets GOCR to toggle the STATUS pin
  which we can read via DSR.
  * `80 09 51`  ; write 0x51 to WDTTR
  * `80 09 54`  ; write 0x54 to WDTTR
  * `80 0e 30`  ; write 0x30 to GOCR, wait 100ms and check for DSR high
  * `80 0e 20`  ; write 0x20 to GOCR, wait 100ms and check for DSR low
* Send `80 0e 20` to write 0x20 to GOCR, bringing STATUS (DSR) low.
* Send tripletted cold loader, inserting `80 0e 30` (write 0x30 to GOCR to
  bring DSR high) before the cold loader's `80 24 80` (write 0x80 to SPCR)
  which starts the program.

Now that the cold loader is running, use it to upload `PILOT.BIN`:

* Switch to 57600bps.
* Set RTS, clear DTR (RTS/DTR should already be in this state).
* Write `00 40 00 00 8d 15 e2`
  * Pilot BIOS physical address 0x00004000, 0x158D bytes, 0xE2 checksum of
    command.
* Read back `e2` (cold loader's calculated checksum)
* Send complete Pilot BIOS (cold loader waiting for starting byte of 0xCC)
  * Cold Loader will replace 0xCC with 0x00 (NOP) in RAM, but uses 0xCC in
    the 8-bit Fletcher checksum.
  * See premain/main of PILOT.C for `db 0CCh` marker byte.
* Read back `69 d4` (8-bit Fletcher checksum of `PILOT.BIN` = 0xD469; see
  `coldload.c` for algorithm).

Now we're running the Pilot BIOS. Use it to upload a firmware image to flash:

* Clear RTS and DTR.
* Increase baud rate to 460800. RTS and DTR still cleared.
  * This is likely the RFU identifying the top speed it can use to upload
    the firmware image.  If switching to 460800 failed for some reason,
    it would probably check 230400.
* Decrease baud rate to 57600. RTS and DTR still cleared.

We are now communicating with the Pilot BIOS.  It sends frames starting
with `7e 02 00 00` and then a "subtype" byte.  Responses start the
same way, ORing subtype with 0x80 for ACK and 0x40 for NAK .  If `7d` or `7e`
appear anywhere in the frame other than the 0x7E start of frame, they are
replaced by `7d 5d` or `7d 5e` respectively.

* Write "Set Baud Rate" command to increase baud rate to maximum
  (460800 = 0x00070800): `7e 02 00 00 06 04 00 26 0c 00 08 07 00 b5 4d`
* Read the ACK: `7e 02 00 00 86 00 00 9f 88 78 b0`
  * If target doesn't support 460800, it will send a NAK
    (`7e 02 00 00 46 ...`) and the host should send another request with
    a lower baudrate.
* Change to negotiated baud rate; RTS and DTR still cleared.

* Write "Info Probe" command: `7e 02 00 00 04 00 00 18 06 5a 24`
* Read the ACK header: `7e 02 00 00 84 af 00 f8 36`
  * Start of response `7c ff 0f 00 d7 bf 08 24 01 01 00 00`
    * `uint32_t location` (0x000FFF7C)
    * `uint16_t flash_id` (0xBFD7)
    * `uint8_t ramsize` (0x08)
    * `uint8_t divisor_19200` (0x24, frequency divider for 19200bps)
    * `uint32_t cpu_id` (0x00000101, Rabbit 3000r1; see TCData.ini)
  * Followed by System ID Block ending in `55 aa 55 aa 55 aa`
  * Finally a 2-byte checksum of the frame.
  * Reference Dynamic C libraries for format of System ID Block.

* Look up the flash ID in Dynamic C 9's `FLASH.INI` file to get sector size,
  number of sectors, flash size and write mode.
* Write "Flash Parameters" command with 16-bit sector size, number of sectors,
  flash size (shifted right 12 bits; a multiple of 4KB) and write mode: 
      `7e 02 00 00 0a 08 00 3a 14 00 10 80 00 80 00 01 00 75 74`
  In this example, an RCM3209:
  * 0x1000: 4KB
  * 0x0080: 128 sectors
  * 0x0080: 512KB
  * 0x0001: write mode 1
* Read the ACK: `7e 02 00 00 8a 00 00 ab 8c a8 c4`

* Write "Erase Flash" command with max address of 0x0008cf50.  In this case,
  firmware image is 0xCF50 bytes.  Flash mapped to base of 0x80000, hence the
  max address of 0x0008XXXX: `7e 02 00 00 09 04 00 2f 0f 50 cf 08 00 b0 75`
* Read the ACK: `7e 02 00 00 89 00 00 a8 8b 9c bf`

* Send 128-byte chunks of firmware image, wrapped in consistent header
  (`7e 02 00 00 03 87 00 24 8c CC BB AA ZZ YY XX WW`) with 2-byte footer.
  `CC` is the address type field:
  * 0x00 Physical (0xWWXXYYZZ is physical address)
  * 0x01 Logical (0xWWXX is XPC, 0xYYZZ is offset)
  * 0x03 No XPC (??)
  Pilot writes block of 0xAABB bytes to address 0xWWXXYYZZ.
* Read the ACK (always this sequence): `7e 02 00 00 83 00 00 96 85 54 a1`

* A smaller chunk at the end of the file (0x0050 bytes at offset 0x0008CF00)
  had this header:  `7e 02 00 00 03 57 00 c3 5c 00 50 00 00 cf 08 00`

* When upload complete, send "Start" command:
      `7e 02 00 00 05 01 00 1d 08 02 9e 2f`.
  Length is always 0x0001.  Data byte values:
  * 0x01 RAM Program, just jump to 0x0000
  * 0x02 Flash program, remap quadrant 0 to flash and jump to 0x0000

* Then set baud rate to 115200 (likely unnecessary), clear RTS and DTR
  (should already be cleared), and close the serial port.
  

Packet Header:
```
uint8_t  start_byte;        // 0x7e
uint8_t  protocol_version;  // 0x02
uint8_t  flags;             // 0x00 (unused)
uint8_t  type;              // 0x00
uint8_t  subtype;           // See list of subtypes below
uint16_t data_length;       // Length of data
uint16_t header_checksum;   // Standard Internet checksum of header bytes

/* Variable Length Data */
```

Packet Footer:
```
uint16_t checksum;          // Standard Internet checksum of header and data
```

Subtype:
* 0x06: Set Baud Rate
* 0x04: Info Probe
* 0x0a: Send Flash Parameters to Pilot based on Flash ID from Info Probe
* 0x09: Erase Program Flash
* 0x03: Write
* 0x05: Start BIOS

Checksums calculated as:
```
void TConnection::DoChecksum(uint8* data, int length, unsigned short& checksum)
{
        unsigned short c0, c1;
   c0 = checksum >> 8;
   c1 = checksum & 0x00ff;
        for(int i = 0; i < length; i++)
   {
        c0 = c0 + data[i];
      if(c0 & 0x100)
      {
        c0++;
         c0 &= 0x00ff;
      }
      c1 = c0 + c1;
      if(c1 & 0x100)
      {
        c1++;
         c1 &= 0x00ff;
      }
   }
   checksum = (c0 << 8) | c1;
}
```
