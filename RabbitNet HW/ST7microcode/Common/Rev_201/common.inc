;************************************************************************ 
;TITLE:		COMMON.INC
;AUTHOR:	
;DESCRIPTION:	 
;************************************************************************
 

;+------------------------------------------------------------------------------+
;|	 EQUATES INCLUSION							|
;+------------------------------------------------------------------------------+
 BYTES
SS		EQU	$7		; Slave Select		(PB7)
                                        ;
WDT		EQU	$0		; WDT TIMOUT OCCURED	(RN_STAT BIT 0)
REJECT		EQU	$1		; COMMAND REJECT 	(RN_STAT BIT 1)
					; 			(RN_STAT BIT 2)
					;			(RN_STAT BIT 3)
COMM_ERR	EQU	$4		; R-NET COMM ERROR	(RN_STAT BIT 4)
DEV		EQU	$5		; TYPE 0 FOR SLAVE	(RN_STAT BIT 5)
READY		EQU	$6		; SLAVE READY		(RN_STAT BIT 6)
BUSY		EQU	$7		; SLAVE BUSY		(RN_STAT BIT 7)
	
FLASH		EQU	$0		; FLASH WR ENAB		(CMD07_STAT BIT 0)
USER		EQU	$1		; USER Handler		(CMD07_STAT BIT 1
					; SPARE 2 & 3           (CMD07_STAT BITS 2-3)
SWDT		EQU	$4		; SOFTWARE WDT		(CMD07_STAT BIT 4)
HWDT		EQU	$5		; HARDWARE WDT		(CMD07_STAT BIT 5)
SRES		EQU	$6		; SLAVE SOFT RESET	(CMD07_STAT BIT 6)
HRES		EQU	$7		; SLAVE HARD RESET	(CMD07_STAT BIT 7)
	
SPIE		EQU	$7		; SPI INT ENAB		(SPICR REG BIT 7)
        
CMPR		EQU	$3		; SLAVE COMPARE ERR     (COMM_STAT BIT 3)
MODF		EQU	$4		; SPI MODE FAULT	(SPICSR	REG BIT 4)
OVR		EQU	$5		; SPI OVERRUN		(SPICSR REG BIT 5)
WCOL		EQU	$6		; SPI WRITE COLLISION	(SPICSR REG BIT 6)
SPIF		EQU	$7		; SPI INTERRUPT ENAB	(SPICSR REG BIT 7)
			
WDGRF		EQU	$00		; HWDT RESET FLAG	(SICSR BIT 0)
			
HWDT_TIMEOUT	EQU	$FF		; HWDT TIMEOUT PERIOD 128.552 ms (6 bit cntr 2.048ms/cnt)
 		

