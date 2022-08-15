;************************************************************************ 
;TITLE:		RNDIO.INC
;AUTHOR:	
;DESCRIPTION:	 
;************************************************************************

;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+
 
 ;#define CS_DISABLED	1		; If enabled then code will verify the
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
SERIAL_NUMBER	EQU     $0	 	; Unique product serial number


 WORDS	
PRODUCT_ID	EQU	$1B00		; Defines the product type as R-Net DIO
CODE_REV	EQU	$0101		; Defines the microcode revision level

 BYTES
PRODUCT_REV	EQU	'A'		; Defines the hardware revision level 	                                ;
SS2CMD		EQU	1		; Min.. Slave Select to Command (us)
CMD2DAT		EQU	15		; Min.. Command to 1st Data Byte (us)
DAT2DAT		EQU	15		; Min.. Inter data byte time (us)
DAT2SS		EQU	15		; Max.. Data to SS Inactive (us)
SS_ACTIVE	EQU	1		; Max.. Slave Select Active (ms)
SWDT2SWDT	EQU	$FF		; Max.. Initial SWDT timeout (25ms*255=6.375sec)
SAFE_STATE_0_7	EQU	$00		; Initial Save State Value
SAFE_STATE_8_15 EQU	$00		; Initial Save State Value

LED		EQU	4		; LED in controlled by PC4
		
 WORDS	 
; Single-Ended Calibration Table for 8 A/D Channels    
cal_mode0	EQU	$F800		               
cal_mode1	EQU	{cal_mode0 + 48}		               
cal_mode2	EQU	{cal_mode1 + 48}		               
cal_mode3	EQU	{cal_mode2 + 48}		               
cal_mode4	EQU	{cal_mode3 + 48}		               		               
cal_mode5	EQU	{cal_mode4 + 48}		               
cal_mode6	EQU	{cal_mode5 + 48}		               
cal_mode7	EQU	{cal_mode6 + 48}
            
		               


                

               

 
