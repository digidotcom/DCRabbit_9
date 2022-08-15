;************************************************************************ 
;TITLE:		RNGPIF.INC
;AUTHOR:	
;DESCRIPTION:	 
;************************************************************************
;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+
 
; #define CS_DISABLED	1		; If enabled then code will verify the
 					; checksum and only execute the mainline
 					; microcode if the checksum verification
 					; process is correct.
 					; If not enabled then code will skip the
 					; checksum verification process and 
 					; always try and execute the mainline
 					; microcode even if its checksum is
 					; incorrect 

;+------------------------------------------------------------------------------+
;|	EQUATES INCLUSION							|
;+------------------------------------------------------------------------------+
 LONGS
SERIAL_NUMBER	EQU     $0	 	; Unique product serial number, on the 
                                        ; Master the order will be changed so the 
                                        ; value will be Zero, Rabbitnet commands
                                        ; c0r04 and c1r04 automatically order the
                                        ; bytes properly for the master.

 WORDS	
PRODUCT_ID	EQU	$2000		; Defines the product type as R-Net GPIF 
CODE_REV	EQU	$0101		; Defines the microcode revision level
 
 BYTES
PRODUCT_REV	EQU	'A'		; Defines the hardware revision level 	                                ;
SS2CMD		EQU	5		; Min.. Slave Select to Command (us)
CMD2DAT		EQU	30		; Min.. Command to 1st Data Byte (us)
DAT2DAT		EQU	30		; Min.. Inter data byte time (us)
DAT2SS		EQU	30		; Max.. Data to SS Inactive (us)
SS_ACTIVE	EQU	5		; Max.. Slave Select Active (ms)
SWDT2SWDT	EQU	$FF		; Max.. Initial SWDT timeout (25ms*255=6.375sec)
POLARITIES	EQU	$00		; GPIF Strobe and Reset set negative active
		
;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+	 
		
 #DEFINE	RESET		$0		; TARGET RESET 		(PC0)
 #DEFINE 	CHIP_SELECT	$1		; TARGET CHIP SELECT 	(PC1)
 #DEFINE	STROBE		$1		; TARGET STROBE		(PC1)
 #DEFINE	ACTIVITY	$2		; USER_1 YELLOW LED 	(PC2)
 #DEFINE 	USER_2		$3		; USER-2 YELLOW LED 	(PC3)
 #DEFINE 	ALARM		$4		; ALARM 		(PC4)
 #DEFINE	UP_BAD		$5		; UP BAD RED LED 	(PC5)
	
 #DEFINE 	CMD_REJECT	$2		; COMMAND REJECT 	(RN_STAT BIT 2)
		
 #DEFINE 	INPUT_MODE	$00		; PORT A DEFINED AS INPUTS
 #DEFINE 	OUTPUT_MODE	$FF		; PORT A DEFINED AS OUTPUTS
		
 #DEFINE 	ON		$FF		; USED IN SUBROUTINES TO ACTIVATE FUNCTION
 #DEFINE 	OFF		$00		; USED IN SUBROUTINES TO DEACTIVATE FUNCTION


 
