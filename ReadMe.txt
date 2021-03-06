***************************************************************** 
README for Dynamic C Version 9.62
***************************************************************** 
-- Supported CPU Types

Dynamic C version 9.62 supports Rabbit 2000 and 3000 CPU types.
Rabbit 4000 and later CPUs are supported in Dynamic C 10.

-- LIB.DIR changes 

Dynamic C's LIB.DIR library files list mechanism now searches 
for library files within a specified directory tree. The 
option to specify a complete or relative pathlist to each 
individual library file remains. Note that each library file 
name must be unique, regardless of whether the library is 
explicitly specified or is found via searching a specified 
directory tree. Also note that this feature was actually 
implemented in Dynamic C v. 9.30. For more information on 
the changes to lib.dir, see section "4.22 Libraries" of the 
"Dynamic C User's Manual" for more details. 

-- BIOS and library reorganization 

The BIOS and libraries were reorganized to support RabbitSys. 
The code in RabbitBIOS.C is now split into various files 
in LIB\BIOSLIB as follows: 

STDBIOS.C -- most of the code from rabbitbios.c was 
moved here. 
CLONECONFIG.LIB -- Macros for configuring cloning. 
DKCONFIG.LIB -- Macros for configuring debug kernel. 
ERRLOGCONFIG.LIB -- Macros for configuring (non-RabbitSys) 
error logging. RabbitSys has its own 
form of error logging. 
MEMCONFIG.LIB -- Macros for configuring memory 
organization. 
SYSCONFIG.LIB -- Additional system level configuration 
(misc). 
TWOPROGRAMCONFIG.LIB -- Macros for configuring split memory 
for the old-style DLM/DLP. 
FATCONFIG.LIB -- Macros for configuring the FAT files 
system. 

For customers that are using an alternative BIOS file, we 
recommend starting from a copy of STDBIOS.C and adding your 
previous modifications to this copy to create a new BIOS. The 
RABBITBIOS.C file now wraps a #use of STDBIOS.C (shown below). 
You can hook your new BIOS into your project in the same way 
by adding a new #if section to RABBITBIOS.C to #use your new 
bios as follows: 

#if MYBIOS == 1 
#use "mybios.c" 
#elif __RABBITSYS == 0 
#use "stdBIOS.c" 
#elif __RABBITSYS == 1 
#use "sysBIOS.c" 
#else 
#use "rkBIOS.c" 
#endif 

To select your new bios, define "MYBIOS=1" to the project 
"Defines" tab in the "Project Options". 

Alternatively, you may copy the wrapper code from RabbitBios.c 
into your newly modified bios (i.e., the one based on 
STDBIOS.C), and use the "User Defined BIOS File" from the 
"Advanced Compiler Options Dialog" to select this file. 

There were many other organization changes in the libraries 
to support RabbitSys. 

-- Targetless Compile 

The targetless compile list is now more flexible, allowing 
easy additions or corrections via the TCData.ini file without 
requiring a DC compiler re-build. TCData.ini is located in 
the root install directory for Dynamic C. 

-- Debugger 

The new �Enable debug kernel� option in Project Options� 
Debugger tab allows complete debug kernel disable / code 
removal. This new option makes it easier to exclude the debug 
kernel from your application once you are ready to deploy it. 

-- Run-time Error Customization 

You can now add descriptions for run-time errors and have 
Dynamic C display a message box indicating what the error 
was just by adding a new entry to errors.ini. Dynamic C 
will look in errors.ini any time a negative exit value 
is encountered. For example, you can add the 
following entry in \DCRabbit_9.50\errmsg.ini 

// My custom errors 
800=My own run-time error message 

Calling "exit(-800)" in your program or library will cause 
Dynamic C to report "My own run-time error message" in a 
message box. 

-- xalloc and Memory Organization Changes 

The function xalloc and other related functions were modified 
so that they are now driven by the origin directives from the 
compiler. The reserve_Store and xalloc_Store structures in 
STACK.LIB have been completely replaced with the new xalloc 
initialization and allocation functions in mem.lib (located 
in lib\RabbitSys\mem.lib). 

The only API change for xalloc is that xalloc_stats must now 
be called with the parameter _rk_xubreak to display the 
current regions of memory reserved for xalloc. 

For advanced users, the compiler now generates an origin 
table that contains the blocks that are reserved for code and 
data origin or other non-xalloc use. With this change, the 
method of reserving a block of memory so that xalloc does 
*NOT* use it has also changed. To reserve a block of memory 
in DC 9.30 and later, the #resvorg should be used. All other 
orgs (e.g. #rcodorg, #rvarorg, etc) are also tracked by the 
compiler and those blocks are entered into the origin table 
generated by the compiler so it is not used by xalloc. 

The #resvorg is used as follows: 

#resvorg <NAME> segment offset size [reserve] 

For example, the following code would reserve the entire flash 
memory in flash compile mode 

#resvorg flashmem 0x0 0x0 0x80000 reserve 

The reserve keyword must be added to the end to reserve the 
entire block of memory. 

Some applications may require that fixed regions of RAM be 
reserved for their own use. For example, you may want to 
reserve the upper half of a 512K RAM in Flash compile mode. 
To reserve this you need to add the following line of code 
to DCRABBIT_9.50\LIB\BIOSLIB\STDBIOS.C at line 686 just 
below the "#resvorg removeflash 0x0 0x0 0x80000 reserve" 

#ifdef RESERVE_UPPER_RAM 
#resvorg reserveupperram 0xc0 0x0 RESERVE_UPPER_RAM batterybacked reserve 
#endif 

This tells the compiler to reserve RESERVE_UPPER_RAM bytes 
from physical address 0xC0000 by adding it to the 
origin table. This removes this memory block from the 
available xalloc memory. 

In your Project Options|Defines enter the amount of memory 
you want to reserve. For example, 

RESERVE_UPPER_RAM=0x40000 

would reserve physical memory from 0xc0000-0xfffff and make 
it unavailable for xalloc. You can then access this memory 
directly from your program as follows: 

main() { 
   long addr; 

   addr = 0xC0000; // point to block reserved for my use. 
} 

-- TCP/IP 

The IFS_RESTORE and IFS_SAVE functionality have been removed 
from ifconfig. They are now available by using the ifs_restore() 
and ifs_save() functions in 
lib\RCM3300\RemoteApplicationUpdate\remoteuploaddefs.lib. 

-- RabbitSys 

Use of the new RabbitSys related options on the Project 
Options� Compiler tab requires installation of the RabbitSys 
module, as well as a RabbitSys-capable target. When RabbitSys 
is selected in the Compiler Options, Dynamic C will attempt to 
detect and load RabbitSys to the attached board. A message 
box with the error message "Could not load the RabbitSys 
binary through the command line RFU..." will be displayed. 
This message indicates that RabbitSys has not been 
installed and/or purchased. 

-- RabbitFLEX(TM) Support added 

DC 9.50 includes libraries to support the RabbitFLEX(tm) 
product. 

-- #useix 

Library support for the #useix directive has been deprecated. 
The useix function qualifier is unaffected. 

-- Note about Battery backed SRAM

On boards such as the RCM3200 and RCM33xx family, the battery-
backed SRAM is mapped by default in the "Compile to flash, run in RAM"
compile mode, and this is the optimal and factory recommended compile 
mode for such boards.  

In "Compile to flash, run in flash" compile mode (not recommended for
RCM3200), only the flash and fast SRAM fit into the physical address 
space, and the battery-backed SRAM is not mapped into the physical 
address space at all.  Xalloc will return non-battery backed SRAM 
regardless of the type of memory requested when using this non-optimal 
compile mode on these boards.  More information about mapping BBRAM
in "Compile to flash, run in flash" mode for fast RAM boards can be 
found in Technical Note 248 in the Documentation section of
our website.
