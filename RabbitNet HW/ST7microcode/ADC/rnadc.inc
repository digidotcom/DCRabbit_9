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
PRODUCT_ID	EQU	$1A00		; Defines the product type as R-Net DIO
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

SE_MODE		EQU	$00             ; A/D modes of operation
DI_MODE		EQU	$01
MA_MODE		EQU	$02

LED		EQU	3		; LED in controlled by PC3
		
 WORDS	 
; Single-Ended Calibration Table for 8 A/D Channels    
segain0		EQU	$F800		               
segain1         EQU     {segain0 + 48}
segain2		EQU     {segain1 + 48}
segain3		EQU     {segain2 + 48}         
segain4         EQU	{segain3 + 48}               
segain5		EQU	{segain4 + 48}                
segain6         EQU	{segain5 + 48}  
segain7         EQU	{segain6 + 48} 
                
; Differential Calibration Table for 4 A/D Channels:	
digain0		EQU	{segain7 + 48} 
digain1		EQU	{digain0 + 48}
digain2		EQU	{digain1 + 48}		
digain3		EQU	{digain2 + 48}  
digain4		EQU	{digain3 + 48} 
digain5		EQU	{digain4 + 48}
digain6		EQU	{digain5 + 48}		
digain7		EQU	{digain6 + 48}

               
; 4-20ma Calibration Table for 8 Channels
ma_cal		EQU	{digain7 + 48}     
 
 