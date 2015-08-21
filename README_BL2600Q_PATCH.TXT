README_BL2600Q_PATCH.TXT

This patch adds support to Dynamic C 9.62 for a new BL2600Q board type, product
ID 0x170F, comprised of an RCM3900 installed into a BL2600 base board. This
patch is distributed as a ZIP file overlay. Simply extract all ZIP content into
the Dynamic C 9.62 main installation folder and the patched files will be placed
and / or replaced in the appropriate sub-folders.

Note that this patch has not passed Digi Rabbit brand Engineering Systems
Assurance's rigorous testing. See the BL2600Q_unit_test_log.txt file for
information on specific files' unit test results. Files not listed were not
explicitly unit tested. However, the unit testing performed provides basic
coverage of a substantial percentage of BL2600 functionality in particular and
Rabbit 3000 functionality in general.

Also note that the BL2600Q shares the RCM3900's SD card performance
characteristics. In particular, while the RCM3900 / BL2600Q SD card maximum 1GB
supported size is greater than the RCM3365 xD card maximum 128MB supported size,
the data throughput of the xD card can be expected to be 8 to 10 times that of
the SD card.

The included Samples\write_idblock_920_962.c program may be used to prepare a
stock RCM3900 board for use as a BL2600Q "brain" board. After patching Dynamic C
9.62, open Dynamic C 9.62, open the write_idblock_920_962.c program file,
uncomment the "//#define MYBOARD BL2600Q . . ." line, then compile and run the
program on a stock RCM3900 board.

Patch ZIP File Overlay Content and Hierarchy:
---------------------------------------------

BL2600_unit_test_log.txt
README_BL2600_PATCH.TXT
TCData.ini
Lib\
   BOARDTYPES.LIB
   DEFAULT.H
   BIOSLIB\
      CPUPARAM.LIB
      STDBIOS.C
   BL2600\
      BL26XX.LIB
   FileSystem\
      FAT_CONFIG.LIB
   SDflash\
      SDFLASH.LIB
   tcpip\
      BOARD_DEPS.LIB
Samples\
   write_idblock_920_962.c
   BL2600\
      BOARD_ID.C
      ADC\
         Adc_cal_se_bipolar.c
   FileSystem\
      FAT\
         FAT_HOT_SWAP_SD.C

