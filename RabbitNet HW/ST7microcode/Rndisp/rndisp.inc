;************************************************************************ 
;TITLE:		RNDISP.INC
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
PRODUCT_ID	EQU	$1C00		; Defines the product type as R-Net Display 
CODE_REV	EQU	$0101		; Defines the microcode revision level
 
 BYTES
PRODUCT_REV	EQU	'B'		; Defines the hardware revision level 	                                ;
SS2CMD		EQU	1		; Min.. Slave Select to Command (us)
CMD2DAT		EQU	15		; Min.. Command to 1st Data Byte (us)
DAT2DAT		EQU	12		; Min.. Inter data byte time (us)
DAT2SS		EQU	12		; Max.. Data to SS Inactive (us)
SS_ACTIVE	EQU	1		; Max.. Slave Select Active (ms)
SWDT2SWDT	EQU	$FF		; Max.. Initial SWDT timeout (25ms*255=6.375sec)
		
;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+	 

 #DEFINE	LCDMWRINST	$0		; 0 - LCDM MASTER WR INSTRUCTION
 #DEFINE	LCDMWRDATA	$1		; 1 - LCDM MASTER WR DATA
 #DEFINE	LCDMRDSTAT	$2		; 2 - LCDM MASTER RD STATUS
 #DEFINE	LCDMRDDATA	$3		; 3 - LCDM MASTER RD INSTRUCTION

 #DEFINE	LCDSWRINST	$4		; 4 - LCDM SLAVE WR INSTRUCTION
 #DEFINE	LCDSWRDATA	$5		; 5 - LCDM SLAVE WR DATA 
 #DEFINE	LCDSRDSTAT	$6		; 6 - LCDM SLAVE RD STATUS
 #DEFINE	LCDSRDDATA	$7		; 7 - LCDM SLAVE RD DATA
	
 #DEFINE	ENABLEDADR	$8		; 8 - ENABLE FRONT PANEL LEDS ADDRESS
 #DEFINE 	KPRDADR		$A		; A - KEYPAD READ DATA ADDRESS
 #DEFINE	LEDREGADR	$B		; B - LED REGISTER ADDRESS
	
 #DEFINE	FPRESET		$0		; FRONT PANEL RESET 	(PC0)
 #DEFINE 	FPSELECT	$1		; FRONT PANEL STROBE 	(PC1)
 #DEFINE	ACTIVITY	$2		; USER_1 YELLOW LED 	(PC2)
 #DEFINE 	USER_2		$3		; USER-2 YELLOW LED 	(PC3)
 #DEFINE 	ALARM		$4		; ALARM 		(PC4)
 #DEFINE	UP_BAD		$5		; UP BAD RED LED 	(PC5)
	
 #DEFINE 	KP_EVENT	$3		; KEY PAD STATUS CHANGE (RN_STAT BIT 3)
		
 #DEFINE 	INPUT_MODE	$00		; PORT A DEFINED AS INPUTS
 #DEFINE 	OUTPUT_MODE	$FF		; PORT A DEFINED AS OUTPUTS
	
 #DEFINE 	BACK_LIGHT	$7		; BACK LIGHT ENABLE/DISABLE BIT
	
 #DEFINE 	ON		$FF		; USED IN SUBROUTINES TO ACTIVATE FUNCTION
 #DEFINE 	OFF		$00		; USED IN SUBROUTINES TO DEACTIVATE FUNCTION
 
 #DEFINE	CLICK_TIME	$30		; DETERMINES THE LENGHT OF KP CLICK 


 
