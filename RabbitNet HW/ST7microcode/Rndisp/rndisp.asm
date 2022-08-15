st7/  	; This is required for specifying the instruction set for the targeted processor         

;****************************************************************************************
;|										
;|		     Rabbitnet Board Specific Microcode 
;|			Copyright (c), Z-World 2003				
;|			Copyright (c), Digi International 2022
;|										
;****************************************************************************************
;|  Program Name:	rndisp.asm	
;|  Date:   		7/10/03
;|  Revision History:	Rev 1.00 Initial Release (Internal Eval)   
;|                      Rev 1.01 Fixed buzzer initialization problem (To customers) 
;****************************************************************************************
        TITLE    "R-Net Display"               	; This title will appear on each
                                        	; page of the listing file
        MOTOROLA                        	; This directive forces the Motorola 
                                        	; format for the assembly (default)
                                                                                
;****************************************************************************************
;     Header files to include MACRO's for Rabbitnet Projects 
;****************************************************************************************
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_200\st72264.inc"
        #INCLUDE "rndisp.inc"         		
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_200\common.inc"
 

;****************************************************************************************
;     Extern references for accessing the common portion of the Rabbitnet microcode  
;****************************************************************************************
 BYTES
 	EXTERN	rn_cmd
 	EXTERN	rn_stat
 	EXTERN	act_tmr
 	EXTERN	swdt_reload
 	EXTERN	swdt_tmr
 	EXTERN	cmd07_stat
 	EXTERN	comm_stat
 	EXTERN	pend
 	EXTERN	flashkey1
 	EXTERN	flashkey2
 	EXTERN	nbytes
 	EXTERN	flashaddr
 	EXTERN	e2data
 	
 


 WORDS
       	EXTERN  mcc_rt         	;FFF0-FFF1h Master Clock Control Interrupt                 
	EXTERN	dummy          	;FFF2-FFF3h Timer A Interrupt                       
	EXTERN 	spi_rt         	;FFF4-FFF5h SPI Interrupt   
	EXTERN 	hard_reset      ;FFFE-FFFFh interrupt vector location  

	EXTERN  swdt_ival
 	EXTERN	rn_xfer
 	EXTERN	spi_pc
 	EXTERN	exitcmd
 	EXTERN	cmd_rej
 	EXTERN	c0r00
 	EXTERN	c0r02	        
	EXTERN	c0r03   
	EXTERN	c0r04
	EXTERN  c0r05
	EXTERN  c0r07
	EXTERN  c1r00
	EXTERN  c1r01   
	EXTERN  c1r02
	EXTERN  c1r03
	EXTERN  c1r04
	EXTERN  c1r05
	EXTERN  c1r06
	EXTERN  c1r07
	
;****************************************************************************************
;	Board Specific MACROs 
;****************************************************************************************
	
;	*************************************************************************
; 	MACRO:		wr_port_a
;	FUNCTION:	WRITE ONE BYTE TO FRONT PANEL
;	PARAMETERS:	REG A CONTAINS WRITE DATA
;	REGS USED:	A
;	REGS MODIFIED:	NONE
;	STACK BYTES:	0
;	*************************************************************************	
wr_port_a	MACRO 
	cpl 	PADDR			;Change to output open drain
	cpl 	PAOR			;Change to output push-pull
	ld 	PADR,A			;Load the write data
	bres	PCDR,#FPSELECT		;Enable Front Panel Select
	bset	PCDR,#FPSELECT		;Disable Front Panel Select
	clr	PAOR			;Change back to open drain 
	clr	PADDR			;Change back to input mode 	
		MEND
		
;
;	*************************************************************************
; 	MACRO:		rd_lcd_ms
;	FUNCTION:	READ LCD MASTER STATUS BYTE
;	PARAMETERS:	A REG CONTAINS RETURNED READ DATA
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************
rd_lcd_ms	MACRO
	ld	a,#LCDMRDSTAT		;
	ld	PBDR,a			;
	bres	PCDR,#FPSELECT		;
	ld	a,PADR			;
	bset	PCDR,#FPSELECT		;
		MEND
;
;	*************************************************************************
; 	MACRO:		rd_lcd_md
;	FUNCTION:	READ LCD MASTER DATA BYTE
;	PARAMETERS:	A REG CONTAINS RETURNED READ DATA
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************
rd_lcd_md	MACRO
	local	rdlcdmd0
rdlcdmd0:				;
	rd_lcd_ms			; Get master status
	and	a,#$80			; Check busy bit
	jrne	rdlcdmd0		; Wait if busy
	ld	a,#LCDMRDDATA		;
	ld	PBDR,a			;
	bres	PCDR,#FPSELECT		;
	ld	a,PADR			;
	bset	PCDR,#FPSELECT		;
		MEND
;
;	*************************************************************************
; 	MACRO		wr_lcd_mi
;	FUNCTION:	WRITE LCD MASTER INSTRUCTION
;	PARAMETERS:	A REG CONTAINS WRITE DATA BYTE
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	2 (1 CALL)
;	*************************************************************************
wr_lcd_mi	MACRO
	local	wrlcdmi0
	push	a			; Save write data
wrlcdmi0:
	rd_lcd_ms			; Get master status
	and	a,#$80			; Check busy bit
	jrne	wrlcdmi0		; Wait if busy
	ld	a,#LCDMWRINST		;
	ld	PBDR,a			;
	pop	a			; Restore write data
 	wr_port_a			;
		MEND
;
;	*************************************************************************
; 	MACRO		wr_lcd_md
;	FUNCTION:	WRITE LCD MASTER DATA
;	PARAMETERS:	A REG CONTAINS WRITE DATA BYTE
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	2 (1 CALL)
;	*************************************************************************
wr_lcd_md	MACRO
	local	wrlcdmd0
	push	a			; Save write data
wrlcdmd0:
	rd_lcd_ms			; Get master status
	and	a,#$80			; Check busy bit
	jrne	wrlcdmd0		; Wait if busy
	ld	a,#LCDMWRDATA		;
	ld	PBDR,a			;
	pop	a			; Restore write data
 	wr_port_a			;
		MEND
;
;	*************************************************************************
; 	MACRO		rd_lcd_ss
;	FUNCTION:	READ LCD SLAVE STATUS BYTE
;	PARAMETERS:	A REG CONTAINS RETURNED READ DATA
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************
rd_lcd_ss	MACRO
	ld	a,#LCDSRDSTAT		;
	ld	PBDR,a			;
	bres	PCDR,#FPSELECT		;
	ld	a,PADR			;
	bset	PCDR,#FPSELECT		;
		MEND
;
;	*************************************************************************
; 	MACRO:		rd_lcd_sd
;	FUNCTION:	READ LCD SLAVE DATA BYTE
;	PARAMETERS:	A REG CONTAINS RETURNED READ DATA
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************
rd_lcd_sd	MACRO
	local	rdlcdsd0
rdlcdsd0:
	rd_lcd_ss			; Get slave status
	and	a,#$80			; Check busy bit
	jrne	rdlcdsd0			; Wait if busy
	ld	a,#LCDSRDDATA		;
	ld	PBDR,a			;
	bres	PCDR,#FPSELECT		;
	ld	a,PADR			;
	bset	PCDR,#FPSELECT		;
		MEND
;
;	*************************************************************************
; 	MACRO:		wr_lcd_si
;	FUNCTION:	WRITE LCD SLAVE INSTRUCTION
;	PARAMETERS:	A REG CONTAINS WRITE DATA BYTE
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************
wr_lcd_si	MACRO
	local	wrlcdsi0
	push	a			; Save write data
wrlcdsi0:
	rd_lcd_ss			; Get slave status
	and	a,#$80			; Check busy bit
	jrne	wrlcdsi0		; Wait if busy
	ld	a,#LCDSWRINST		;
	ld	PBDR,a			;
	pop	a			; Restore write data
 	wr_port_a			;
		MEND
;
;	*************************************************************************
; 	MACRO:		wr_lcd_sd
;	FUNCTION:	WRITE LCD SLAVE DATA
;	PARAMETERS:	A REG CONTAINS WRITE DATA BYTE
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************
wr_lcd_sd	MACRO
	local	wrlcdsd0
	push	a			; Save write data
wrlcdsd0:
	rd_lcd_ss			; Get slave status
	and	a,#$80			; Check busy bit
	jrne	wrlcdsd0		; Wait if busy
	ld	a,#LCDSWRDATA		;
	ld	PBDR,a			;
	pop	a			; Restore write data
 	wr_port_a			;
		MEND
		            
;****************************************************************************************				
;     Application Ram segment, all accesses to this area uses the short addressing mode  
;****************************************************************************************   
 BYTES
	SEGMENT 'Application_ram'
	
led_shadow:	ds.b 1 			; LED shadow register
dly_tmr:	ds.b 1			; Delay timer
alm_tmr:	ds.b 1			; Alarm timer
kp_regs.b:
kp_dat:		ds.b 1			; Keypad data
kp_dwn:		ds.b 1			; Keypad down delta
kp_up:		ds.b 1			; Keypad up delta
nkp_dat:	ds.b 1			; New keypad data
act_buzzer:	ds.b 1			; Flag for buzzer control
msec_cnt:	ds.b 2	                ; Milli-sec counter for the buzzer
;****************************************************************************************				
;     Product Paramters which get overwritten when microcode is downloaded 
;     in the field.  
;****************************************************************************************   
 WORDS       
        SEGMENT  'Application_parms_1'
.sig_regs.w:	
	dc.b	SS2CMD				; Slave Select to Command (# us)
	dc.b	CMD2DAT				; Command to Data (# us)
	dc.b	DAT2DAT				; Data to Data (# us)
	dc.b	DAT2SS				; Max Data to Slave Select Inactive (# us)
	dc.b    SS_ACTIVE                       ; Max Slave Select Active (#ms)
	
.code_rev.w:
	dc.w	CODE_REV
	
.swdt_ival.w:
	dc.b	SWDT2SWDT			; Initial value of SWDT	
	
 
;****************************************************************************************				
;     Product Paramters which do not get overwritten when microcode is downloaded 
;     in the field.  
;****************************************************************************************    
 WORDS     
        SEGMENT  'Application_parms_2'
.prod_sn.w:
	dc.l	SERIAL_NUMBER			; 4 byte board unique serial number  		
.prod_id.w:	
	dc.w	PRODUCT_ID
.prod_rev.w:
	dc.b	PRODUCT_REV	
	
          												
								
;****************************************************************************************				
;	Mainline....used to do some initialization and background processing
;                   of board specific resources.   
;****************************************************************************************				
  WORDS         	
	SEGMENT  'Application_code'

.main:	clr	kp_dat			; Clear all
	clr	kp_dwn			;  keypad
	clr	kp_up			;  data
	call	init_timerA	
	rim				; Enable interrupts
						
	ld	a,#$40			; Setup initial RN status (Ready)
	ld	rn_stat,a		;  byte
	ld	SPIDR,a			; 
	
	clr	a			; Turn off the 
	call	up_bad			;  up_bad LED

m_loop:	call	rd_kp			; Get key pad data into A reg
	ld	nkp_dat,a		; Save it
	call	dly_100			; Wait to deglitch
	call	rd_kp			; Get key pad data again
	cp	a,nkp_dat		; Is it the same as before
	jrne	m_loop			; No...Start again
	cpl	a			; A 1 in bits 0-6 indicates key is pressed
	and	a,#$7f			; Mask off bit 7 which is unused
	ld	nkp_dat,a		; Save copy of new keypad data 
	xor	a,kp_dat		; Check if there is a change
	jreq	m_loop			; No change in kp data
	
	and	a,nkp_dat		; Start calculating value for
	jreq	update			;
update:	sim				; Disable interrupts so all 
					;  key pad status is consistant					
	ld	kp_dwn,a		;  keypad down status byte
	ld	a,nkp_dat		; Caculate value
	xor	a,kp_dat		; for keypad
	and	a,kp_dat		; up status
	ld	kp_up,a			; byte
					;
	ld	a,nkp_dat		; update key pad data byte
	ld	kp_dat,a		; with new key pad data 
	bset	rn_stat,#KP_EVENT	; Now update the R-Net status bytes both
	ld	a,rn_stat		; in memory and
	ld	SPIDR,a			; in the SPI data register
	rim				; enable interrupts again
	jra	m_loop			;			
	
;****************************************************************************************				
;	Board Specific initialization for when a hardware reset occurs 
;****************************************************************************************				

.brd_hard_init:	ret  				

;********************************************************************************
;* Board Initialization Routine for Software Reset 
;********************************************************************************
.brd_soft_init:	ret	
        					
;****************************************************************************************
;	Main Board initialization  
;****************************************************************************************
.brd_init: 	call	ST7_init		; Initialize ST7 I/O ports
		call	FP_init			; Initialize Front Panel	
		ret                                                                      
		
;****************************************************************************************				
;	User Handler that will get executed in the SPI interrupt routine if
;       USER bit is set in the pend control status byte. 
;****************************************************************************************
.user_handler: 	ret       

;****************************************************************************************
;	Board Specific 25ms timer functions 
;****************************************************************************************
.interval_tmr: 	ld	a,PCDR		; Blink the		
		xor	a,#$08		; yellow user_1 LED
		ld	PCDR,a		;
intvl_20:	tnz	act_tmr		; Check for R-Net activity (non-zero)
		jreq	intvl_30	; No..Go turn off Activity LED
		dec	act_tmr		; Yes..Decrement Activity timer
		bset	PCDR,#ACTIVITY	;  and turn on Activity LED
		jra	intvl_40	;
intvl_30:	bres	PCDR,#ACTIVITY	; 
intvl_40:	tnz	dly_tmr		; If delay timer is zero
		jreq	intvl_50	;  do nothing
		dec	dly_tmr		;  else decrement it	
intvl_50:	ret

;****************************************************************************************				
;	Board Specific RabbitNet command routines 
;****************************************************************************************				
; Insert Board specific Rabbitnet microcode					
					
			
;****************************************************************************************
;	Board Specific subroutines 
;****************************************************************************************
;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			ST7 INITIALIZATION					|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine initialises the used pins of the ST7				|
;|										|
;|INPUT PARAMETERS:								|
;| None										|
;|										|
;|INTERNAL PARAMETERS:								|
;| None										|
;|										|
;|OUTPUT PARAMETERS:								|
;| None										|
;+------------------------------------------------------------------------------+
ST7_init
	clr	A			; Load A reg with 0s
	ld	PADDR,A			; Initially set up PA as inputs
	ld	PAOR,A			; and make them floating
					; PA4 and PA6 are True Open Drain
	;
	ld	A,#$0F			; Set up for outputs on bits 3-0 
	ld	PBDDR,A			; PB bits 7-4 input(SPI), bits 3-0 outputs
	ld	PBOR,A			; PB bits 3-0 are push-pull
	;
	ld	A,#$23			; Set up initial value of port C, up_bad on
	ld	PCDR,A			;  and negate all other control signals
	ld	A,#$3F			; Load A reg with 
 	ld	PCDDR,A			; PC bits 5-0 are outputs
 	ld	PCOR,A			; PC bits 5-0 are push-pull 	
        ret 
;********************************************************************************
                              
;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			FRONT PANEL INITIALISATION				|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine initialises the FRONT PANEL electronics				|
;|										|
;|INPUT PARAMETERS: None							|
;| None										|
;|										|
;|INTERNAL PARAMETERS:								|
;| A (accumulator)								|
;|										|
;|OUTPUT PARAMETERS:								|
;| None										|
;+------------------------------------------------------------------------------+
;
fp_dat.w:
	dc.b	$AE			; Display OFF
	dc.b	$C0			; Display start line 0
	dc.b	$A4			; Static drive OFF
	dc.b	$00			; Column address 0
	dc.b	$B8			; Page address 0
	dc.b	$A9			; Duty cycle 1/32
	dc.b	$A0			; Rightward output
	dc.b	$EE			; Read modify write OFF
	dc.b	$AF			; Display ON

FP_init:
	ld	a,#$01			; Red LED ON all others OFF
	ld	led_shadow,a		; Initialize LED shadow register
	bres	PCDR,#FPRESET		; Lower then
	bset	PCDR,#FPRESET		; raise the front panel reset line 
	ld	A,#ENABLEDADR		; Set address to select
	ld	PBDR,A			; Enable front panel LEDs
	bres	PCDR,#FPSELECT		; Lower then raise
	bset	PCDR,#FPSELECT		; the chip select
	ld	A,#LEDREGADR		; Set address to select
	ld	PBDR,A			; FP LED register
	ld	a,led_shadow		; Turn off all LEDS except Red
	wr_port_a               	; Write led register
	
	clr	x			; Set index to zero
FP_init_10:                             ;
	ld	a,(fp_dat,x)		; Get initialization data from table
	wr_lcd_mi			; Send it to the master
	wr_lcd_si			; Send it to the slave
	inc	x			; Move to next entry in table
	cp	x,#$09			; End of table?
	jrne	FP_init_10		; No..Continue
					; Yes..Done with intialization
	ld	x,#$00			; LCD pattern data to clear display
	ld	a,#$00			; LCD page number 0
	call	wrlcdpg			; fill LCD page with pattern data
	ld	a,#$01			; LCD page number 1
	call	wrlcdpg			; fill LCD page with pattern data
	ld	a,#$02			; LCD page number 2
	call	wrlcdpg			; fill LCD page with pattern data
	ld	a,#$03			; LCD page number 3
	call	wrlcdpg			; fill LCD page with pattern data
	
	ld	a,#$B8			; Put small triangle in upper left corner
	wr_lcd_mi			; Select Page zero
	clr	a			;  and set
	wr_lcd_mi			;  column counter to zero
	ld	a,#$3F			; Initial column data for triange
FP_init_20: 				;
	wr_lcd_md			; Write data to LCDM
	srl	a			; Generate next column of triangle data
	jrne	FP_init_20		; Check of not done
	
	ret				; Done with initialization

;********************************************************************************

;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			100-1000MS TIMING LOOPs					|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine generates various wait periods in ms				|
;|										|
;|INPUT PARAMETERS:								|
;| None										|
;|										|
;|INTERNAL PARAMETERS:								|
;| A (accumulator)								|
;|										|
;|OUTPUT PARAMETERS:								|
;| None										|
;+------------------------------------------------------------------------------+
 

; timing routine for 25ms per tic
; ==================================================
 	WORDS
dly_100:
	push	a			; Save register A from calling routine
 	ld	a,#4			; Load wait period 4*25ms = 100ms
	ld	dly_tmr,a		; Put it in the count down timer 0
	jra	dly_A			; Now wait
dly_250:
	push	a			; Save register A from calling routine
 	ld	a,#10			; Load wait period 10*25ms = 250ms
	ld	dly_tmr,a		; Put it in the count down timer 0
	jra	dly_A			; Now wait

dly_500:
	push	a			; Save register A from calling routine
 	ld	a,#20			; Load wait period 20*25ms = 500ms
	ld	dly_tmr,a		; Put it in the count down timer 0
	jra	dly_A			; Now wait

dly_750:
	push	a			; Save register A from calling routine
 	ld	a,#30			; Load wait period 30*25ms = 750ms
	ld	dly_tmr,a		; Put it in the count down timer 0
	jra	dly_A			; Now wait

dly_1000:
	push	a			; Save register A from calling routine
 	ld	a,#40			; Load wait period 40*25ms = 1000ms
	ld	dly_tmr,a		; Put it in the count down timer 0
					;
dly_A:	tnz	dly_tmr			; Wait here 
	jrne	dly_A			; until it is zero
	pop	a 			; Done, so restore A register		
	ret     			; Return to caller
	
;********************************************************************************
;********************************************************************************
;+------------------------------------------------------------------------------+
;|	LOW LEVEL DRIVERS							|
;+------------------------------------------------------------------------------+
;
;	*************************************************************************
; 	ROUTINE;	up_bad(on/off)
;	PARAMETERS:	If REG A non-zero then UP_BAD LED ON
;			If REG A is zero then UP_BAD LED OFF
;	REGS USED:	A
;	REGS MODIFIED:	NONE
;	STACK BYTES:	0
;	*************************************************************************
up_bad:
	tnz a				;Check A reg for non-zero
	jreq upb_off			;If reg A is zero go turn off UP_BAD
	bset PCDR,#UP_BAD		;Reg A is non-zero so turn on UP_BAD
	ret				;and return
upb_off:
	bres PCDR,#UP_BAD		;Reg A was zero so turn off UP_BAD
	ret				;and return 
		
;
;	*************************************************************************
; 	ROUTINE;	bk_light
;	FUNCTION:	TURN LCDM LED BACK LIGHT ON/OFF
;	PARAMETERS:	IF A REG NON-ZERO TURN ON BACK LIGHT
;			IF A REG IS ZERO TURN OFF BACK LIGHT
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	2 (1 call)
;	*************************************************************************					
bk_light:
	tnz 	a			;Check A reg for non-zero
	jreq 	bklt_off		;If reg A is zero go turn off BACK LIGHT
	bset 	led_shadow,#BACK_LIGHT	;Reg A is non-zero so turn on BACK LIGHT
	ld	a,#LEDREGADR		;Set led register as destination
	ld	PBDR,a			;of the write
	ld	a,led_shadow		;restore the led write data byte
	wr_port_a			;write it
	ret				;and return
bklt_off:
	bres 	led_shadow,#BACK_LIGHT	;Reg A was zero so turn off BACK LIGHT
	ld	a,#LEDREGADR		;Set led register as destination
	ld	PBDR,a			;of the write
	ld	a,led_shadow		;restore the led write data byte
	wr_port_a			;write it
	ret				;and return
;
;	*************************************************************************
; 	ROUTINE;	wr_fp_leds
;	FUNCTION:	WRITE THE FP LED REG AND UPDATE SHADOW REG
;	PARAMETERS:	A REGISTER CONTAINS BYTE TO LOAD INTO LED REGISTER
;	REGS USED:	A
;	REGS MODIFIED:	NONE
;	STACK BYTES:	0
;	*************************************************************************
wr_leds:
	ld	led_shadow,a		;Update led reg shadow register
	ld	a,#LEDREGADR		;Set up address of led register
	ld	PBDR,a			;as destination of write
	ld	a,led_shadow		;restore write data to a register
	wr_port_a			;write it
	ret				;
;
;	*************************************************************************
; 	ROUTINE;	kp_rd
;	FUNCTION:	READ FRONT PANEL KEYPAD
;	PARAMETERS:	REG A CONTAINS KEYPAD READ DATA ON RETURN
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************

rd_kp: 	ld	a,#KPRDADR		;Set up key pad 
	ld	PBDR,a			;read address
	bres	PCDR,#FPSELECT		;Enable Front Panel Select
	ld	a,PADR			;Get keypad data
	bset	PCDR,#FPSELECT		;Disable Front Panel Selec
	ret 				;
;
;	*************************************************************************
; 	ROUTINE;	wrlcdpg
;	FUNCTION:	FILL LCD PAGE
;	PARAMETERS:	A REG = PAGE # (0-3), X = WRITE DATA PATTERN
;	REGS USED:	A,X
;	REGS MODIFIED:	A
;	STACK BYTES:	
;	*************************************************************************
wrlcdpg:
	and	a,#$03			; Mask off high 6 bits
	or	a,#$B8			; Generate page set command (0-3)
	wr_lcd_mi			; Send to master
	wr_lcd_si			; Send to slave
	clr	a			; Load set column address to zero cmd
	wr_lcd_mi			; Send to master
	wr_lcd_si			; Send to slave
	ld	a,#62			; Load number of columns to do
wrlcdpg1:
	dec	a			; Caculate next column
	jreq	wrlcdpg2		; If zero no remaing columns to do
	push	a			; Save current column count
	ld	a,x			; Get LCD write data
	wr_lcd_md			; Send to master & auto increment column
	wr_lcd_sd			; Send to slave & auto increment column
	pop	a			; Restore current column count
	jra	wrlcdpg1		; Start on next column
wrlcdpg2: 
	ret
;										
;****************************************************************************************
;	Board Specific R-Net Command Routines
;****************************************************************************************
;                                       ; WRITE FP LEDS
c0r10:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get LED write data
	and	a,#$7F			; Mask off backlight bit (just in case)
	btjt	led_shadow,#BACK_LIGHT,c0r10a ; Check if backlight is enabled
	jra	c0r10b			; No..Go write LEDS with backlight disabled
c0r10a: or	a,#$80			; Yes..Set backlight enabled bit
c0r10b:	call	wr_leds			; Write LEDs and Backlight,update shadow
	jp	exitcmd			; Command End, Wait for SS to negate
					;
					; TURN ON FP LED NUMBER	
c0r11:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get bit position count
	ld	x,#$01			; Set bit 0 in scratach reg 0
	and	a,#$07			; Modulo 0-7 (bit # 0-6,ignore 7)
c0r11a:	jreq	c0r11b			; Move a 1 bit in x
	sll	x			;  to the left until it is
	dec	a			;  in the bit # position
	jra	c0r11a			;  to be set
c0r11b:	ld	a,x			;  then put in A reg
	and	a,#$7F			; If bit #7 (backlight) ignore
	or	a,led_shadow		; Set the selected bit
	call	wr_leds			; Write LEDs and Backlight,update shadow
	jp	exitcmd			; Command End, Wait for SS to negate
					;	
					; TURN OFF FP LED NUMBER	
c0r12:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get bit position count
	ld	x,#$01			; Set bit 0 in scratach reg 0
	and	a,#$07			; Modulo 0-7 (bit # 0-6,ignore 7)
c0r12a:	jreq	c0r12b			; Move a 1 bit in x
	sll	x			;  to the left until it is
	dec	a			;  in the bit # position
	jra	c0r12a			;  to be set
c0r12b:	ld	a,x			;  then put in A reg
	and	a,#$7F			; If bit #7 (backlight) ignore
	cpl	a			; Put zero in selected bit position
	and	a,led_shadow		; Reset the selected bit
	call	wr_leds			; Write LEDs and Backlight,update shadow
	jp	exitcmd			; Command End, Wait for SS to negate
					;
					; TURN UP BAD LED ON/OFF	
c0r13:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get Bad LED enable/disable byte
	call	up_bad			;	
	jp	exitcmd			; Command End, Wait for SS to negate	

c0r15:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  
	cp	a,#$00			; Check if Alarm is ON/OFF
	jreq	c0r15off 
	ld	a,#$FF
	ld	act_buzzer,a
	bset	PCDR,#ALARM		;  and turn on alarm
	jp	exitcmd			; Command End, Wait for SS to negate

c0r15off: 
	clr	act_buzzer
	bres	PCDR,#ALARM		;  and turn off alarm
	jp	exitcmd			; Command End, Wait for SS to negate

					; SOUND ALARM
c0r16:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get alarm timeout value
	ld	{msec_cnt+0},a		; Load the alarm timer
	call	rn_xfer			;  and get alarm timeout value
	ld	{msec_cnt+1},a		; Load the alarm timer
	bset	PCDR,#ALARM		;  and turn on alarm
	jp	exitcmd			; Command End, Wait for SS to negate
					;
					; ENABLE/DISABLE BACKLIGHT
c0r17:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get enable/disable data
	call	bk_light		; Enab/Disable backlight & update led shadow
	jp	exitcmd			; Command End, Wait for SS to negate
	
					; WRITE LCDM MASTER CONTROL REGISTER
c0r20:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get control reg write data
	wr_lcd_mi			; Write data to master ctlr chip
	jra	c0r20			; Continue command until SS negated		
		
					; WRITE LCDM SLAVE CONTROL REGISTER
c0r30:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get control reg write data
	wr_lcd_si			; Write data to slave ctlr chip
	jra	c0r30			; Continue command until SS negated
	                                
	                                ; WRITE DATA TO MASTER LCDM
c0r40:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get master write data
	wr_lcd_md			; Send write data to Master ctlr chip
	jra	c0r40			; Continue command until SS negated

	                             	; WRITE DATA TO SLAVE LCDM
c0r60:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get slave write data		
	wr_lcd_sd			; Send write data to Slave ctlr chip
	jra	c0r60			; Continue command until SS negated	
	
; -----------------------------------------------------------------------------------

					; READ LED SHADOW REGISTER
c1r10:	ld	a,led_shadow		; Get LED shadow register
	and	a,#$7F			;  and mask off the backlight bit
	call	rn_xfer			;  then send on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate
	 
	       				; READ KEYPAD RAW DATA
c1r11:	call	rd_kp			; Get raw keypad data
	call	rn_xfer			; Send to master on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate
	
					; READ KP DEGLITCHED DATA
c1r12:	ld	a,kp_dat		; Get deglitched kp data
	call	rn_xfer			; Send to master on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate	
					; READ KP DELTA DOWN
c1r13:	ld	a,kp_dwn		; Get kp down data
	call	rn_xfer			; Send to master on next MDO
	clr	kp_dwn			; Zero the key pad delta down data
	jp	exitcmd			; Command End, Wait for SS to negate		       				
			
					; READ KP DELTA UP
c1r14:	ld	a,kp_up			; Get kp down data
	call	rn_xfer			; Send to master on next MDO
	clr	kp_up			; Zero the key pad delta up data
	jp	exitcmd			; Command End, Wait for SS to negate
		
					; READ BACKLIGHT SHADOW REG
c1r17:	btjt	led_shadow,#BACK_LIGHT,c1r17a ; Is backlight enabled
	clr	a			; No..set up to send X'00'
	jra	c1r17b			; Go send X'00' to master
c1r17a:	ld	a,#$FF			; Yes..set up to send X'FF' 
c1r17b:	call	rn_xfer			; Send A reg to master on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate
	
					; READ LCDM MASTER STATUS	
c1r20:	rd_lcd_ms			; Get Master status into A reg
	call	rn_xfer			;  and send to master
	jra	c1r20			; Continue command until SS negated

					; READ LCDM SLAVE STATUS
c1r30:	rd_lcd_ss			; Get Slave status into A reg
	call	rn_xfer			;  and send on next MDO
	jra	c1r30			; Continue command until SS negated
	
					; READ LCDM MASTER DATA
c1r40:	rd_lcd_md			; Get Master data into A reg
	call rn_xfer			;  and send on next MDO
	jra	c1r40			; Continue command until SS negated		

					; READ LCDM SLAVE DATA
c1r60:	rd_lcd_sd			; Get Slave data into A reg
	call	rn_xfer			;  and send on next MDO
	jra	c1r60			; Continue command until SS negated		



;+------------------------------------------------------------------------------+
;|	Timer A Initialization Routine    					|
;+------------------------------------------------------------------------------+ 	 
init_timerA:	
		ld	a,#$FF
		ld	TAOC1HR,a	  
		ld	a,#$FF 
		ld	TAOC1LR,a 
		ld	a,#$EF 
		ld	TAOC2LR,a 
		ld	a,#$FF
		ld	TAOC2HR,a	  

		ld	a,#$08
		ld	TACR2,a
		ld	a,#$FC
		ld	ISPR1,a
		clr	TACLR
		clr	TACSR  
		ld	a,#$40
		ld	TACR1,a
		clr	{msec_cnt+0}
		clr	{msec_cnt+1}
		clr	act_buzzer
		ret
		
 
;+------------------------------------------------------------------------------+
;|	Timer A Interrupt Routine    						|
;+------------------------------------------------------------------------------+ 	 
timer_A:        ld	a,#$03
		ld	TAOC1HR,a 
		ld	a,TACSR  
		ld	a,#$E8 
		ld	TAOC1LR,a 
               
    		ld	a,#$03
		ld	TAOC2HR,a	  		
		ld	a,TACSR  
		ld	a,#$E8 
		ld	TAOC2LR,a 

		ld	a,TACSR  
		clr	TACLR
	       
		tnz	act_buzzer
		jrne	timer90
		tnz	{msec_cnt+0}
		jrne 	timer10
		tnz	{msec_cnt+1}
		jrne 	timer10
		bres	PCDR,#ALARM
		clr	act_buzzer
		iret	
				
timer10:	dec 	{msec_cnt+1}  
		ld	a,{msec_cnt+1} 
		cp	a,#$FF
		jrne	timer90	
		dec	{msec_cnt+0}
timer90:	iret
	   			
;
;****************************************************************************************
;     Rabbitnet Command Lookup Table
;****************************************************************************************
.spi_jtbl:	
		dc.w	c0r00			; c0r00
		dc.w	cmd_rej			; c0r01
		dc.w	c0r02			; c0r02
		dc.w	c0r03			; c0r03
		dc.w	c0r04			; c0r04
		dc.w	c0r05			; c0r05
		dc.w	cmd_rej			; c0r06
		dc.w	c0r07			; c0r07
						;
		dc.w	c0r10			; c0r10
		dc.w	c0r11			; c0r11
		dc.w	c0r12			; c0r12
		dc.w	c0r13			; c0r13
		dc.w	cmd_rej			; c0r14
		dc.w	c0r15			; c0r15
		dc.w	c0r16			; c0r16
		dc.w	c0r17			; c0r17
						;
		dc.w	c0r20			; c0r20
		dc.w	cmd_rej			; c0r21
		dc.w	cmd_rej			; c0r22
		dc.w	cmd_rej			; c0r23
		dc.w	cmd_rej			; c0r24
		dc.w	cmd_rej			; c0r25
		dc.w	cmd_rej			; c0r26
		dc.w	cmd_rej			; c0r27
						;
		dc.w	c0r30			; c0r30
		dc.w	cmd_rej			; c0r31
		dc.w	cmd_rej			; c0r32
		dc.w	cmd_rej			; c0r33
		dc.w	cmd_rej			; c0r34
		dc.w	cmd_rej			; c0r35
		dc.w	cmd_rej			; c0r36
		dc.w	cmd_rej			; c0r37
						;
		dc.w	c0r40			; c0r40
		dc.w	cmd_rej			; c0r41
		dc.w	cmd_rej			; c0r42
		dc.w	cmd_rej			; c0r43
		dc.w	cmd_rej			; c0r44
		dc.w	cmd_rej			; c0r45
		dc.w	cmd_rej			; c0r46
		dc.w	cmd_rej			; c0r47
						;
		dc.w	cmd_rej			; c0r50
		dc.w	cmd_rej			; c0r51
		dc.w	cmd_rej			; c0r52
		dc.w	cmd_rej			; c0r53
		dc.w	cmd_rej			; c0r54
		dc.w	cmd_rej			; c0r55
		dc.w	cmd_rej			; c0r56
		dc.w	cmd_rej			; c0r57
						;
		dc.w	c0r60			; c0r60
		dc.w	cmd_rej			; c0r61
		dc.w	cmd_rej			; c0r62
		dc.w	cmd_rej			; c0r63
		dc.w	cmd_rej			; c0r64
		dc.w	cmd_rej			; c0r65
		dc.w	cmd_rej			; c0r66
		dc.w	cmd_rej			; c0r67
						;
		dc.w	cmd_rej			; c0r70
		dc.w	cmd_rej			; c0r71
		dc.w	cmd_rej			; c0r72
		dc.w	cmd_rej			; c0r73
		dc.w	cmd_rej			; c0r74
		dc.w	cmd_rej			; c0r75
		dc.w	cmd_rej			; c0r76
		dc.w	cmd_rej			; c0r77

		dc.w	c1r00			; c1r00
		dc.w	c1r01			; c1r01
		dc.w	c1r02			; c1r02
		dc.w	c1r03			; c1r03
		dc.w	c1r04			; c1r04
		dc.w	c1r05			; c1r05
		dc.w	c1r06			; c1r06
		dc.w	c1r07			; c1r07
						;
		dc.w	c1r10			; c1r10
		dc.w	c1r11			; c1r11
		dc.w	c1r12			; c1r12
		dc.w	c1r13			; c1r13
		dc.w	c1r14			; c1r14
		dc.w	cmd_rej			; c1r15
		dc.w	cmd_rej			; c1r16
		dc.w	c1r17			; c1r17
						;
		dc.w	c1r20			; c1r20
		dc.w	cmd_rej			; c1r21
		dc.w	cmd_rej			; c1r22
		dc.w	cmd_rej			; c1r23
		dc.w	cmd_rej			; c1r24
		dc.w	cmd_rej			; c1r25
		dc.w	cmd_rej			; c1r26 
		dc.w	cmd_rej			; c1r27
						;
		dc.w	c1r30			; c1r30
		dc.w	cmd_rej			; c1r31
		dc.w	cmd_rej			; c1r32
		dc.w	cmd_rej			; c1r33
		dc.w	cmd_rej			; c1r34
		dc.w	cmd_rej			; c1r35
		dc.w	cmd_rej			; c1r36
		dc.w	cmd_rej			; c1r37
						;
		dc.w	c1r40			; c1r40
		dc.w	cmd_rej			; c1r41
		dc.w	cmd_rej			; c1r42
		dc.w	cmd_rej			; c1r43
		dc.w	cmd_rej			; c1r44
		dc.w	cmd_rej			; c1r45
		dc.w	cmd_rej			; c1r46
		dc.w	cmd_rej			; c1r47
						;
		dc.w	cmd_rej			; c1r50
		dc.w	cmd_rej			; c1r51
		dc.w	cmd_rej			; c1r52
		dc.w	cmd_rej			; c1r53
		dc.w	cmd_rej			; c1r54
		dc.w	cmd_rej			; c1r55
		dc.w	cmd_rej			; c1r56
		dc.w	cmd_rej			; c1r57
						;
		dc.w	c1r60			; c1r60
		dc.w	cmd_rej			; c1r61
		dc.w	cmd_rej			; c1r62
		dc.w	cmd_rej			; c1r63
		dc.w	cmd_rej			; c1r64
		dc.w	cmd_rej			; c1r65
		dc.w	cmd_rej			; c1r66
		dc.w	cmd_rej			; c1r67
						;
		dc.w	cmd_rej			; c1r70
		dc.w	cmd_rej			; c1r71
		dc.w	cmd_rej			; c1r72
		dc.w	cmd_rej			; c1r73
		dc.w	cmd_rej			; c1r74
		dc.w	cmd_rej			; c1r75
		dc.w	cmd_rej			; c1r76
		dc.w	cmd_rej			; c1r77


;****************************************************************************************
; Segment: 	Relocatable_vectors 
; Description: 	The primary interrupt vector table is located in the read only area 
;               of flash (segment 0) and can not be programmed via the application.
;	
; 		This second jump table allows interrupt routines to be placed in the 
;		writable flash area (segment 1) and made relocatable.
;****************************************************************************************
 
 WORDS
	SEGMENT 'Relocatable_vectors'
						  					
.unused1_jp:	jp    dummy           	;FFE0-FFE1h Unimplemented Interrupt 
.unused2_jp:    jp    dummy           	;FFE2-FFE3h Unimplemented Interrupt
.i2c_jp		jp    dummy          	;FFE4-FFE5h I2C Interrupt  
.sci_jp:        jp    dummy          	;FFE6-FFE7h Serial Comm Interrupt  
.unused3_jp:    jp    dummy          	;FFE8-FFE9h Unimplemented Interrupt  
.unused4_jp:    jp    dummy           	;FFEA-FFEBh Unimplemented Interrupt  
.avd_jp         jp    dummy          	;FFEC-FFEDh Aux Voltage Detector Interrupt               
.timb_jp:	jp    timer_A          	;FFEE-FFEFh Timer B Interrupt
.mcc_jp:	jp    mcc_rt         	;FFF0-FFF1h Master Clock Control Interrupt                 
.tima_jp:	jp    timer_A          	;FFF2-FFF3h Timer A Interrupt                       
.spi_jp:	jp    spi_rt         	;FFF4-FFF5h SPI Interrupt   
.css_jp:      	jp    dummy          	;FFF6-FFF7h Clock Security System Interrupt             
.ext1_jp:	jp    dummy          	;FFF8-FFF9h Port B External Interrupt 
.ext0_jp:	jp    dummy         	;FFFA-FFFBh Port A External Interrupt                      
.trap_jp:       jp    dummy          	;FFFC-FFFDh interrupt vector location                       
.reset_jp:  	jp    hard_reset        ;FFFE-FFFFh interrupt vector location  


;********************************************************************************
;  Function to ENABLE/DISABLE application CRC error checking 
;********************************************************************************
  
WORDS                    
  	SEGMENT 'Protected'

.crcEnable: 	
 #ifdef CS_DISABLED                     
		ld	a,#$00		; Load A with value to Disable CRC checking
 #else                                  
	 	ld	a,#$FF		; Load A with value to Enable CRC checking
 #endif
		ret

		
;****************************************************************************************				
;     End of Source file
;****************************************************************************************		
	END  


