RabbitNet Firmware
==================

This directory contains resources for Digi's RabbitNet expansion cards.

  * RabbitNet Product Manual with detailed information
  * Hardware schematics 
  * Directories with source and compiled firmware for the ST72F264G microprocessor used on these products
  * Directory with verified shipped firmware

Contents of ST7microcode
------------------------

We have not confirmed how closely this code matches firmware from shipping
hardware.  It's provided as-is, to help with reverse engineering the
communication protocol for anyone attempting to create a replacement device.

| Directory | Product |   ID   | Part Number | Description                 |
| --------- | ------- | ------ | ----------- | --------------------------- |
| Common    |         |        |             | Code common to all products |
| rndio     | RN1100  | 0x1900 | 101-0612    | Digital I/O Expansion       |
| ADC       | RN1200  | 0x1A00 | 20-101-0616 | A/D Expansion               |
| DAC       | RN1300  | 0x1B00 | 20-101-0688 | D/A Expansion               |
| RNIF      | RN1400? | 0x2000 | 101-0617    | Relay Expansion             |
| Rndisp    | RN1600? | 0x1C00 | 101-0879    | Keyboard/Display Interface  |

Note that the RN1600 shipped with Product ID 0x2000, and the RN1400 shipped
as 0x2100.

The code in the Rndisp directory does not appear to match code pulled from
an RN1600 board built in 2015.  It's possible that it's code for an alternate
version of the display interface, developed as the RN1500 but never released.

Contents of RELEASE
-------------------
This directory holds copies of firmware extracted from shipped firmware,
along with instructions on installing firmware using "ST7 Visual Programmer".


License
-------

Copyright (c) 2022, Digi International, Inc.

Note that this license pertains to the contents of this directory 
(ST7microcode) and its subdirectories ONLY.  The top-level LICENSE.txt 
file covers the remaining repository contents.

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
