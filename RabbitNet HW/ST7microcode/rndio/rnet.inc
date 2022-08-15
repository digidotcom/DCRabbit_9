;************************************************************************ 
;TITLE:		RNET.INC
;AUTHOR:	
;DESCRIPTION:	 
;************************************************************************
;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+
	WORDS        
        EXTERN  c0r00 
        EXTERN  c0r01
	EXTERN  c0r02
	EXTERN  c0r03
	EXTERN  c0r04
	EXTERN  c0r05
	EXTERN  c0r06
	EXTERN  c0r07
	EXTERN  c1r00
	EXTERN  c1r01   
	EXTERN  c1r02
	EXTERN  c1r03
	EXTERN  c1r04
	EXTERN  c1r05
	EXTERN  c1r06
	EXTERN  c1r07   
	EXTERN 	dummy    
        EXTERN 	mcc_rt  
        EXTERN  spi_rt  
        EXTERN  spi_init 
;	EXTERN  safe_0_7                      	
;	EXTERN  safe_8_15  
	EXTERN  swdt_ival
        
;        BYTES
;	segment 'ram0'
	
;.scr_0:		ds.b 1			; 8 bit scratch reg 0
;.tmr_0:		ds.b 1			; 8 bit timer reg 0
;.swdt_reload_hi:	ds.b 1			; High byte of SWDT reload value
;.swdt_reload_lo:	ds.b 1			; Low byte of SWDT reload value
;.swdt_tmr_hi		ds.b 1			; High byte of SWDT timer
;.swdt_tmr_lo		ds.b 1			; Low byte of SWDT timer
;.rn_cmd:		ds.b 1			; Rabbit-Net Command Register
;.rn_stat:		ds.b 1			; Rabbit-Net Status Byte

