/*
   Copyright (c) 2015 Digi International Inc.

   This Source Code Form is subject to the terms of the Mozilla Public
   License, v. 2.0. If a copy of the MPL was not distributed with this
   file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/* START LIBRARY DESCRIPTION *********************************************
default.h

DESCRIPTION:  Default libraries to be used for the boards. The use
              statements do not necessarily compile anything, it is just
              an indication that functions in the used libraries may be
              called and linked.

END DESCRIPTION **********************************************************/

/*** BeginHeader */

#ifndef _REMOTEAPPLICATIONUPDATE_LOADER

// NOTE: The board type macros are now defined in
//       BOARDTYPES.LIB (As of 8/13/02)

#ifndef BIOSCODE
  #makechain _GLOBAL_INIT
  extern void _GLOBAL_INIT();

  // Uncomment the following line to change the default storage class to static
  //#class static

   // if board type is undefined, assume it's a Jackrabbit
  #if _BOARD_TYPE_ == 0x0000
    #undef  _BOARD_TYPE_
    #define  _BOARD_TYPE_  BL1810
  #endif

  // Jackrabbit-specific libraries
  #if (_BOARD_TYPE_ & 0xFF00) == (BL1810 & 0xFF00)
    #use "jrio.lib"
    #use "jr485.lib"
  #endif

  // Intellicom-specific libraries and TCP/IP-specific libraries
  #if _BOARD_TYPE_ == OP6600 || _BOARD_TYPE_ == OP6700 || _BOARD_TYPE_ == RTDK
    #use "icom.lib"
  #endif

  // Ethernet Gateway-specific libraries
  // RABLINK and EG2100 do not require board library
  //#if _BOARD_TYPE_ == EG2000 || _BOARD_TYPE_ == EG2020
  //#use "egate.lib"
  //#endif

  // SmartStar series controller-specific libraries
  #if _BOARD_TYPE_ == SR9100 || _BOARD_TYPE_ == SR9110 || \
      _BOARD_TYPE_ == SR9150 || _BOARD_TYPE_ == SR9160
    #define ZWORLD_RESERVED_SIZE 0x0400
    #use "smrtstar.lib"
    // for 122x32 LCD and 1x7 keypad assembly option
    #use "keypad7.lib"
    #use "mscg12232.lib"
    #use "graphic.lib"
    #use "6x8l.lib"
    #use "8x10l.lib"
    #use "12x16l.lib"
    #use "courier12.lib"
  #endif

  // BL2000 series controllers-specific libraries
  #if (_BOARD_TYPE_ & 0xFF00) == (BL2000 & 0xFF00)
    #if _BOARD_TYPE_ == BL2000 || _BOARD_TYPE_ == BL2000B || \
        _BOARD_TYPE_ == BL2010B || _BOARD_TYPE_ == BL2020
      #define ADC_OPTION  // ADC option is installed
      #define ADC_12BIT    // ADC has 12 bits of resolution
    #elif _BOARD_TYPE_ == BL2010 || _BOARD_TYPE_ == BL2030
      #define ADC_OPTION  // ADC option is installed
      #define ADC_10BIT    // ADC has 10 bits of resolution
    #endif
    #if _BOARD_TYPE_ == BL2000 || _BOARD_TYPE_ == BL2000B || \
        _BOARD_TYPE_ == BL2020
      #define DAC_OPTION  // DAC option is installed
    #endif
    #if _BOARD_TYPE_ == BL2000 || _BOARD_TYPE_ == BL2000B || \
        _BOARD_TYPE_ == BL2010 || _BOARD_TYPE_ == BL2010B
      #define ETHERNET_OPTION
    #endif
    #if _BOARD_TYPE_ == BL2040
      #define ZWORLD_RESERVED_SIZE 0x0000  // no analog calibration constants
    #else
      #define ZWORLD_RESERVED_SIZE 0x0400  // reserved for analog calib constants
    #endif
    #use "bl20xx.lib"
  #endif

  // BL2100 series controllers-specific libraries
  #if (_BOARD_TYPE_ & 0xFF00) == (BL2100 & 0xFF00)
    #define ZWORLD_RESERVED_SIZE 0x0400 // Reserved 1k userblock for Z-World
    #use "bl21xx.lib"
    // for 122x32 LCD and 1x7 keypad assembly option
    #use "keypad7.lib"
    #use "mscg12232.lib"
    #use "graphic.lib"
    #use "6x8l.lib"
    #use "8x10l.lib"
    #use "12x16l.lib"
    #use "courier12.lib"
  #endif

  // OP6800 series controllers-specific libraries
  #if (_BOARD_TYPE_ & 0xFF00) == (OP6800 & 0xFF00)
    #use "op68xx.lib"
    // for 122x32 LCD and 1x7 keypad assembly option
    #use "keypad7.lib"
    #use "mscg12232.lib"
    #use "graphic.lib"
    #use "6x8l.lib"
    #use "8x10l.lib"
    #use "12x16l.lib"
    #use "courier12.lib"
  #endif

  // LP3500 series controllers-specific libraries
  #if (_BOARD_TYPE_ & 0xFF00) == (LP3500 & 0xFF00)
    #if _BOARD_TYPE_ == LP3500
      #define ADC_ONBOARD
      #define RLY_ONBOARD
      #define ZWORLD_RESERVED_SIZE 0x0400
    #endif
    #use "lp35xx.lib"
    #use "lcd122key7.lib"
  #endif

  // OP7200 series controllers-specific libraries
  #if (_BOARD_TYPE_ & 0xFF00) == (OP7200 & 0xFF00)
    #if _BOARD_TYPE_ == OP7200 || _BOARD_TYPE_ == OP7200B
      #define ZWORLD_RESERVED_SIZE 0x0400  // Reserved 1k userblock for Z-World
      #define ANALOG
      #define TOUCHSCREEN
    #endif
    #use "OP72xx.lib"
    #use "keypad9.lib"
    #use "SED1335F.lib"
    #use "graphic.lib"
    #use "6x8l.lib"
    #use "8x10l.lib"
    #use "10x16l.lib"
    #use "12x16l.lib"
    #use "17x35l.lib"
    #use "ZWLOGOS.LIB"
    #use "TERMINAL9.LIB"
    #use "TERMINAL12.LIB"
    #use "TERMINAL14.LIB"
    #use "glmenu.lib"
    #use "zwlogos.lib"
    #use "TS_R4096.lib"
    #use "glTouchScreen.lib"
    #use "rn_cfg_op72.lib"
    #use "rnet.lib"
    #use "rnet_dio.lib"
    #use "rnet_ain.lib"
    #use "rnet_aout.lib"
    #use "rnet_relay.lib"
    #use "rnet_keyif.lib"
    #use "rnet_lcdif.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (RCM3000 & 0xFF00)
    #define USE_PORTA_AUX_IO
    #use "rcm3000.lib"
    #use "lcd122key7.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (RCM3100 & 0xFF00)
    #define USE_PORTA_AUX_IO
    #use "rcm3100.lib"
    #use "lcd122key7.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (RCM3200 & 0xFF00) \
      _BOARD_TYPE_ == RCM3209 || _BOARD_TYPE_ == RCM3229
    #define USE_PORTA_AUX_IO
    #use "rcm3200.lib"
    #use "lcd122key7.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (RCM3400A & 0xFF00)
    #define USE_PORTA_AUX_IO
    #if _BOARD_TYPE_ == RCM3420B
      #define ZWORLD_RESERVED_SIZE 0 // No reserved userblock area for Z-World
    #else
      #define ADC_ONBOARD
      #define ZWORLD_RESERVED_SIZE 0x0800 // Reserved 2k userblock for Z-World
    #endif
    #use "rcm34xx.lib"
    #use "lcd122key7.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (BL2500A & 0xFF00)
    #define ADC_ONBOARD
    #define DAC_ONBOARD
    #define ZWORLD_RESERVED_SIZE 0x0400 // Reserved 1k userblock for Z-World
    #use "bl25xx.lib"
    #use "rn_cfg_bl25.lib"
    #use "rnet.lib"
    #use "rnet_dio.lib"
    #use "rnet_ain.lib"
    #use "rnet_aout.lib"
    #use "rnet_relay.lib"
    #use "rnet_keyif.lib"
    #use "rnet_lcdif.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (BL2600A & 0xFF00)
    #if _BOARD_TYPE_ == BL2600A
      #define BL26_ENET_OPT2
    #endif
    #if _BOARD_TYPE_ == BL2600B || _BOARD_TYPE_ == BL2600C
      #define BL26_ENET_OPT0
    #endif
    #if _BOARD_TYPE_ == BL2610A || _BOARD_TYPE_ == BL2610B
      #define BL26_ENET_OPT1
    #endif
    #if _BOARD_TYPE_ == BL2600K || _BOARD_TYPE_ == BL2600L
      #define BL26_ENET_OPT3
    #endif
    #if _BOARD_TYPE_ == BL2600M || _BOARD_TYPE_ == BL2600N
      #define BL26_ENET_OPT0
    #endif
    #if _BOARD_TYPE_ == BL2600Q
      #define BL26_ENET_OPT0
    #endif
    #use "bl26xx.lib"
    #use "rn_cfg_bl26.lib"
    #use "rnet.lib"
    #use "rnet_dio.lib"
    #use "rnet_ain.lib"
    #use "rnet_aout.lib"
    #use "rnet_relay.lib"
    #use "rnet_keyif.lib"
    #use "rnet_lcdif.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (RCM3600A & 0xFF00)
    #define USE_PORTA_AUX_IO
    #define ADC_ONBOARD
    #define ZWORLD_RESERVED_SIZE 0x0800 // Reserved 2k userblock for Z-World
    #use "rcm36xx.lib"
    #use "lcd122key7.lib"
  #endif

  #if (_BOARD_TYPE_ & 0xFF00) == (RCM3700A & 0xFF00)
    #define USE_PORTA_AUX_IO
    #define ADC_ONBOARD
    #define ZWORLD_RESERVED_SIZE 0x0800  // Reserved 2k userblock for Z-World
    #use "rcm37xx.lib"
    #use "lcd122key7.lib"
  #endif

  #if RCM3800_SERIES
    #define ZWORLD_RESERVED_SIZE 0x0800  // Reserved 2k userblock for Z-World
  #endif

  #if RABBITFLEX_SBC40_SERIES
    #define ZWORLD_RESERVED_SIZE 0x0800  // Reserved 2k userblock for Z-World
    #use "rabbitflex_sbc40.lib"
    #use "rn_cfg_rabbitflex_sbc40.lib"
    #use "rnet.lib"
    #use "rnet_dio.lib"
    #use "rnet_ain.lib"
    #use "rnet_aout.lib"
    #use "rnet_relay.lib"
    #use "rnet_keyif.lib"
    #use "rnet_lcdif.lib"
  #endif

  // Set macro to indicate SBC's or core modules not
  // reserving any User block area for Z-World's use.
  #ifndef ZWORLD_RESERVED_SIZE
    #define ZWORLD_RESERVED_SIZE 0
  #endif

  // CPU-specific libraries
  #if CPU_ID_MASK(_CPU_ID_) >= R3000
    #use "r3000.lib"
  #endif

  #if __RABBITSYS==0
    #define SETUSERMODE
    #define SETSYSMODE
    #define _system
    #define _SYS_CALL_VARS
    #define _NET_SYSCALL(x)
    #define _rk_printf printf
    #define _rk_sprintf sprintf
  #elif _USER
    #define SETUSERMODE
    #define SETSYSMODE
    #use tc_defs.lib
    #use syscommon.lib
  #elif _SYSTEM
    #define SETUSERMODE asm push su $ push su $ pop hl $ \
      ld (sp+@sp+sureg),hl $ ld hl,_rk_core+[_rk_core]+callback $ inc (hl) $\
      ld a, 1 $ ioi ld (WPCR), a $ setusr

    #define SETSYSMODE asm ld hl,(sp+@sp+sureg) $ push hl $ \
      pop su $ inc sp $ xor a $ ioi ld (WPCR), a $ \
      ld hl,_rk_core+[_rk_core]+callback $ dec (hl)

    #use rkauth.lib
    #use USERPGMINFO.LIB
    #use rkPrint.lib
    #use rkdebugkernel.lib
    #use RKIO.LIB
    #use SYSCOMMON.LIB
    #use RKCORE.LIB
    #use rkPtrCheck.lib
  #endif

  #if _USER
    #define PLD_ETH_COMPILING 0
    #define USE_ETH_PRELOADED_DRIVER 1
  #endif

  //***** Debug Kernel Information ******************************************
                                 // _DK_ENABLE_BREAKPOINTS_ is defined
                                 // internally by Dynamic C and is set to 1 to
                                 // enable breakpoint support in debug kernel, 0
                                 // to disable support and reclaim code space

  #define DK_ENABLE_BREAKPOINTS _DK_ENABLE_BREAKPOINTS_

                                 // _DK_ENABLE_ASMSINGLESTEP_ is defined
                                 // internally by Dynamic C and is set to 1
                                 // to enable assembly level single step
                                 // support in the debug kernel, 0 to
                                 // disable support and reclaim code space

  #define DK_ENABLE_ASMSINGLESTEP _DK_ENABLE_ASMSINGLESTEP_

                                 // _DK_ENABLE_WATCHEXPRESSIONS_ is defined
                                 // internally by Dynamic C and is set to 1
                                 // to enable watch expressions in the debug
                                 // kernel, 0 to disable support and reclaim
                                 // code space

  #define DK_ENABLE_WATCHEXPRESSIONS _DK_ENABLE_WATCHEXPRESSIONS_

                                 // _DK_ENABLE_TRACING_ is defined
                                 // internally by Dynamic C and is set to 1
                                 // to enable execution tracing in the debug
                                 // kernel, 0 to disable support and reclaim
                                 // code space

  #define DK_ENABLE_TRACING _DK_ENABLE_TRACING_

                                 // _DK_ENABLE_STACK_TRACING_ is defined
                                 // internally by Dynamic C and is set to 1
                                 // to enable stack tracing in the debug
                                 // kernel, 0 to disable support and reclaim
                                 // code space

  #define DK_ENABLE_STACK_TRACING _DK_ENABLE_STACK_TRACING_

                                 // _DK_SEND_STACK_LENGTH_ is defined
                                 // internally by Dynamic C and is set to 32
                                 // if Stack Tracing is disabled up to a max
                                 // value maintained in the registry, 4096
                                 // by default

  #define DK_SEND_STACK_LENGTH _DK_SEND_STACK_LENGTH_

  #define DK_ENABLE_DEBUGKERNEL _DK_ENABLE_DEBUGKERNEL_

  // If the debug kernel core is not enabled from the GUI, make sure all pieces
  // are disabled.
  #if DK_ENABLE_DEBUGKERNEL == 0

    #undef  DK_ENABLE_TRACING
    #define DK_ENABLE_TRACING        0

    #undef  DK_ENABLE_STACK_TRACING
    #define DK_ENABLE_STACK_TRACING    0

    #undef  DK_ENABLE_ASMSINGLESTEP
    #define DK_ENABLE_ASMSINGLESTEP    0

    #undef  DK_ENABLE_BREAKPOINTS
    #define DK_ENABLE_BREAKPOINTS      0

    #undef  DK_ENABLE_WATCHEXPRESSIONS
    #define DK_ENABLE_WATCHEXPRESSIONS  0
  #endif

  // libraries to compile for user programs
  // program.lib contains premain, and so must appear here
  #use "dkentry.lib"
  #use "dkcommon.lib"
  #use "dkcore.lib"
  #use "dkapp.lib"

  //#use "slice.lib"
  #if _SYSTEM
    #use "rkprogram.lib"
  #else
    #use "program.lib"
  #endif

  #if __RABBITSYS==0
    #use "mem.lib"
  #endif

#else  // BIOSCODE

  // libraries to parse during the BIOS compile
  // (i.e., functions in these libraries may be precompiled)
  #use "cbuf.lib"
  #use "dc.hh"
  #use "util.lib"
  #use "mutil.lib"
  #use "mutilfp.lib"
  #use "math.lib"
  #use "string.lib"
  #use "stdio.lib"
  #use "sys.lib"
  #use "cofunc.lib"
  #use "costate.lib"
  #use "slice.lib"
  #use "rs232.lib"
  #use "rtclock.lib"

  #if __RABBITSYS
    #use "rcm3365iomac.lib"
  #endif

  #if _SYSTEM
    #ifndef _RK_DEBUG_SYSTEM_
      #define _RK_DEBUG_SYSTEM_ _DK_ENABLE_DEBUGKERNEL_
    #endif
  #elif _USER
    #use "vdriver.lib"
    #use "sysvdriver.lib"
  #else
    #use "vdriver.lib"
    #use "stdvdriver.lib"
    #use "ERRNO.LIB"
  #endif

  #use "xmem.lib"

  #undef BIOSCODE
#endif  // BIOSCODE

#endif  // _REMOTEAPPLICATIONUPDATE_LOADER

/*** EndHeader */