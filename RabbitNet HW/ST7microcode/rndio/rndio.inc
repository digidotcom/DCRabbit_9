;************************************************************************ 
;TITLE:		RNDIO.INC
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
SERIAL_NUMBER	EQU     0	 	; Unique product serial number, on the 
                                        ; Master the order will be changed so the 
                                        ; value will be zero, Rabbitnet commands
                                        ; c0r04 and c1r04 automatically order the
                                        ; bytes properly for the master.

 WORDS	
PRODUCT_ID	EQU	$1900		; Defines the product type as R-Net DIO
CODE_REV	EQU	$0102		; Defines the microcode revision level
    
    
 BYTES
PRODUCT_REV	EQU	'C'		; Defines the hardware revision level 	                                ;
SS2CMD		EQU	$00		; Min.. Slave Select to Command (us)
CMD2DAT		EQU	$0C		; Min.. Command to 1st Data Byte (us)
DAT2DAT		EQU	$0A		; Min.. Inter data byte time (us)
DAT2SS		EQU	$0A		; Max.. Data to SS Inactive (us)
SS_ACTIVE	EQU	$0A		; Max.. Slave Select Active (ms)
SWDT2SWDT	EQU	$FF		; Max.. Initial SWDT timeout (25ms*255=6.375sec)
SAFE_STATE_0_7	EQU	$00		; Initial Save State Value
SAFE_STATE_8_15 EQU	$00		; Initial Save State Value
		
IN0_7		EQU	$0		; Rd strobe 0-7 	(PB0)
IN8_15		EQU	$1		; Rd strobe 8-15 	(PB1)
IN16_23		EQU	$2		; Rd strobe 15-23  	(PB2)
OUT_EN		EQU	$3		; Enable Outputs 0-15	(PB3)
					; Slave Select (SS)	(PB7)
                                        ;
OUT0_7		EQU	$4		; Wr strobe 0-7 	(PC4)
OUT8_15		EQU	$5		; Wr strobe 8-15 	(PC5)
                                        ;
					; WDT			(RN_STAT BIT 0)
					; REJECT 		(RN_STAT BIT 1)
DIGIN		EQU	$2		; DIGIN CHANGE OCCURED	(RN_STAT BIT 2)
ANALOG		EQU	$3 		; ADC UPDATE OCCURED	(RN_STAT BIT 3)
					; COMM_ERR		(RN_STAT BIT 4)
					; DEV			(RN_STAT BIT 5)
					; READY			(RN_STAT BIT 6)
					; BUSY			(RN_STAT BIT 7)
	
ADC0_OK		EQU	$0		; ADC 0 UPDATED		(ADC CTRL/STATUS REG BIT 0)
ADC1_OK		EQU	$1		; ADC 1 UPDATED		(ADC CTRL/STATUS REG BIT 1)
ADC2_OK		EQU	$2		; ADC 2 UPDATED		(ADC CTRL/STATUS REG BIT 2)
ADC3_OK		EQU	$3		; ADC 3 UPDATED		(ADC CTRL/STATUS REG BIT 3)
ADC0_EN		EQU	$4		; ADC 0 ENABLED		(ADC CTRL/STATUS REG BIT 4)
ADC1_EN		EQU	$5		; ADC 1 ENABLED		(ADC CTRL/STATUS REG BIT 5)
ADC2_EN		EQU	$6		; ADC 2 ENABLED		(ADC CTRL/STATUS REG BIT 6)
ADC3_EN		EQU	$7		; ADC 2 ENABLED		(ADC CTRL/STATUS REG BIT 7)

R_LED		EQU	$7		; DATA BUS LEDS		(R_LED BIT 7)
	 
 WORDS 
 
ADSE_CHANNELS   EQU     4       ;Number of single-ended AD channels 
ADDI_CHANNELS   EQU	0       ;Number of differential AD channels
ADMA_CHANNELS   EQU     0       ;Number of 4-20ma channels

ADNUM_VGAINS   	EQU	1       ;Number of gains used for SE/DIFF
ADNUM_AGAINS   	EQU     0       ;Number of gains used for 4-20ma


CAL_ELEMENTS 	EQU	6   	;(Float) Gainfactor, (Int) Offset       
SIZE_ADSE	EQU	{CAL_ELEMENTS MULT ADSE_CHANNELS}
SIZE_ADDI       EQU     {CAL_ELEMENTS MULT ADDI_CHANNELS}
SIZE_ADMA       EQU     {CAL_ELEMENTS MULT ADMA_CHANNELS}
 
CALSIZE_ADSE    EQU     {CAL_ELEMENTS MULT ADSE_CHANNELS MULT ADNUM_VGAINS}
CALSIZE_ADDI    EQU	{CAL_ELEMENTS MULT ADDI_CHANNELS MULT ADNUM_VGAINS}
CALSIZE_ADMA    EQU     {CAL_ELEMENTS MULT ADMA_CHANNELS MULT ADNUM_VGAINS}


ADSE_FADDR    	EQU     $EC00  
ADDI_FADDR    	EQU    	{ADSE_FADDR + CALSIZE_ADSE}
ADMA_FADDR      EQU     {ADDI_FADDR + CALSIZE_ADDI}
NEXT_FADDR	EQU	{ADMA_FADDR + SIZE_ADMA}  

;----------------------------------------------------------------------------- 
;    Must set the following marcro's to specify the flash starting and 
;    ending address of where all calibration data and/or parameters will 
;    be stored.
;-----------------------------------------------------------------------------
CAL_START	EQU   	ADSE_FADDR   		
CAL_END         EQU	{NEXT_FADDR-1} 		

