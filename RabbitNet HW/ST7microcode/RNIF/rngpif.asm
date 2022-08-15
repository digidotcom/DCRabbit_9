st7/  	; This is required for specifying the instruction set for the targeted processor         

;****************************************************************************************
;|										
;|		     Rabbitnet Board Specific Microcode 
;|			Copyright (c), Z-World 2003				
;|			Copyright (c), Digi International 2022
;|										
;****************************************************************************************
;|  Program Name:	rngpif.asm	
;|  Date:   		6/16/03
;|  Revision History:	Rev 1.00 Initial Release
;****************************************************************************************
        TITLE    "R-Net GPIF"               	; This title will appear on each
                                        	; page of the listing file
        MOTOROLA                        	; This directive forces the Motorola 
                                        	; format for the assembly (default)
                                                                                
;****************************************************************************************
;     Header files to include MACRO's for Rabbitnet Projects 
;****************************************************************************************
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_200\st72264.inc"
        #INCLUDE "rngpif.inc"         		
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
 	EXTERN	cmd_exit
 	EXTERN	exitcmd
 	EXTERN	cmd_rej
 	EXTERN	c0r00        
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
	bres	PBDR,#3			;Assert Rd/Wr* line
	ld 	PADR,A			;Load the write data 
	cpl 	PADDR			;Change to output open drain
	cpl 	PAOR			;Change to output push-pull
	enab_strobe			;Assert Strobe
	disab_strobe			;DeAssert Strobe	
	clr	PAOR			;Change back to open drain 
	clr	PADDR			;Change back to input mode
	bset	PBDR,#3			;De-Assert Rd/Wr* line 	
		MEND				
;
;	*************************************************************************
; 	MACRO:		enab_strobe
;	FUNCTION:	ENABLE STROBE
;	PARAMETERS:	NONE
;	REGS USED:	NONE
;	REGS MODIFIED:	NONE
;	STACK BYTES:	0
;	*************************************************************************	
enab_strobe	MACRO
	LOCAL	es_10,es_20		;Labels local to macro envocation
	btjt	polarity,#STROBE,es_10  ;Check for positive active strobe
	bres	PCDR,#STROBE 		;NO..Assert negative active strobe
	jra	es_20			;END
es_10:	bset	PCDR,#STROBE		;YES..Assert positive active strobe
es_20:	MEND				;
;
;	*************************************************************************
; 	MACRO:		disab_strobe
;	FUNCTION:	DISABLE STROBE
;	PARAMETERS:	NONE
;	REGS USED:	NONE
;	REGS MODIFIED:	NONE
;	STACK BYTES:	0
;	*************************************************************************	
disab_strobe	MACRO
	LOCAL	ds_10,ds_20		;Labels local to macro envocation
	btjt	polarity,#STROBE,ds_10  ;Check for positive active strobe
	bset	PCDR,#STROBE 		;NO..DeAssert negative active strobe
	jra	ds_20			;END
ds_10:	bres	PCDR,#STROBE		;YES..DeAssert positive active strobe
ds_20:	MEND				;
;	            
;****************************************************************************************				
;     Application Ram segment, all accesses to this area uses the short addressing mode  
;****************************************************************************************   
 BYTES
	SEGMENT 'Application_ram'

polarity:	ds.b 1			; Ram based sig polarity reg	
dly_tmr:	ds.b 1			; Delay timer
rst_tmr:	ds.b 1			; Target Reset timer
shadow_regs:	ds.b 8			; Shadow registers 0-7
act_buzzer:	ds.b 1			; Flag for buzzer control
msec_cnt:	ds.b 2	                ; Milli-sec counter for the buzzer
mask		ds.b 1			; Mask for boolean operation

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
.sig_polarity.w:
	dc.b	POLARITIES			; Initial ctrl signal polarites for GPIF
	
          												
								
;****************************************************************************************				
;	Mainline....used to do some initialization and background processing
;                   of board specific resources.   
;****************************************************************************************				
  WORDS         	
	SEGMENT  'Application_code'

.main:		ld	a,sig_polarity		; Get Flash based sig polarity
		ld	polarity,a		;  and put in RAM
		clr	rst_tmr			; Target reset deasserted
		clr	x			; Clear the
main_5:		clr	(shadow_regs,x)		;  eight
		inc	x			;  GPIF shadow
		cp	x,#$08			;  registers
		jrne	main_5			;
						; 
		ld	a,#$40			; Setup initial RN status (Ready)
		ld	rn_stat,a		;  byte
		ld	SPIDR,a			;
		call	init_timerA	
		rim				; Enable interrupts				
		                                ;
		btjt	polarity,#RESET,main_10 ; Check for positive active reset		                                
		bset	PCDR,#RESET		; Start w/Reset inactive
		bres	PCDR,#RESET		; No..Reset the target
		call	dly_100			;  hardware for
		bset	PCDR,#RESET 		;  100 ms with negative active reset
		jra	main_20	
						;
main_10:	bres	PCDR,#RESET		; Start w/Reset inactive
		bset	PCDR,#RESET		; Yes..Reset the target
		call	dly_100			;  hardware for
		bres	PCDR,#RESET		;  100 ms with positive active reset
                                        	;
main_20:	clr	{msec_cnt+0}
		ld	a,#4			; Sound the
		ld	{msec_cnt+1},a		;  alarm for
		bset	PCDR,#ALARM		;  100ns
						;
m_loop:		jra	m_loop			;			
	
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
.brd_init:	call	ST7_init		; Initialize ST7 I/O ports
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
intvl_20:	tnz	rst_tmr		; If reset target timer is zero
		jreq	intvl_30	;  deassert target reset
		dec	rst_tmr		;  else decrement reset timer
		jra	intvl_40	;
intvl_30:	btjt	polarity,#RESET,intvl_35 ; Check for positive active reset
		bset	PCDR,#RESET	; No..DeAssert negative active reset
		jra	intvl_40	;
intvl_35:	bres	PCDR,#RESET	; Yes..DeAssert positive active reset
intvl_40:	tnz	act_tmr		; Check for R-Net activity (non-zero)
		jreq	intvl_50	; No..Go turn off Activity LED
		dec	act_tmr		; Yes..Decrement Activity timer
		bset	PCDR,#ACTIVITY	;  and turn on Activity LED
		jra	intvl_60	;
intvl_50:	bres	PCDR,#ACTIVITY	; 
intvl_60:	tnz	dly_tmr		; If delay timer is zero
		jreq	intvl_70	;  do nothing
		dec	dly_tmr		;  else decrement it	
intvl_70:	ret

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
	clr	a			; Load A reg with 0s
	ld	PADDR,a			; Initially set up PA as inputs
	ld	PAOR,a			; and make them floating
					; PA4 and PA6 are True Open Drain
	;
	ld	A,#$0F			; Set up for outputs on bits 3-0 
	ld	PBDDR,a			; PB bits 7-4 input(SPI), bits 3-0 outputs
	ld	PBOR,a			; PB bits 3-0 are push-pull
	ld	A,#$08			; Set register address to zero
	ld	PBDR,a			;  disable wr*
	;
	ld	a,sig_polarity		; Get the signal polarity control reg
	cpl	a			; Create the deasserted state
	and	a,#$03			;  in bits 1-0, negate alarm, alive
	ld	PCDR,a			;  activity and uP bad leds
	ld	a,#$3F			; Load A reg with 
 	ld	PCDDR,a			; PC bits 5-0 are outputs
 	ld	PCOR,a			; PC bits 5-0 are push-pull 	
        ret 
;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			R-NET BIT TRANSFER					|
;| 										|
;+------------------------------------------------------------------------------+
;********************************************************************************
; 	ROUTINE:	RN_BITXFER
;	PARAMETERS:	ON ENTRY A REG CONTAINS DATA TO BE SENT TO MASTER
;			ON EXIT A REG CONTAINS DATA SENT TO SLAVE
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	
;********************************************************************************
;
.rn_bitxfer:	btjt	SPICSR,#OVR,rn_bitxfer20  ; Check for overrun (OVR is cleared)
		ld	SPIDR,a			  ; Prepare so send data to master
rn_bitxfer10:	btjt	SPICSR,#SPIF,rn_bitxfer50 ; NO..Then check for next MDO while
		btjf	PBDR,#SS,rn_bitxfer10 	  ;  monitoring Slave Select
		disab_strobe			  ; DeAssert Strobe
		bset	PBDR,#3			  ; DeAssert /WR
		clr	a			  ; Restore Port A
		ld	PAOR,a			  ;  data bus
		ld	PADDR,a			  ;  to input mode
		pop	a			  ; Adjust
		pop	a			  ;  stack pointer and
		jp	cmd_exit		  ;  exit the current command
rn_bitxfer20:	ld	a,SPIDR			  ; Insure xfer flag is cleared
		bset	comm_stat,#OVR		  ; Overrun condition exists, Save overrun
		ld	a,comm_stat		  ;  status bit
		or	a,SPICSR		  ; Update the existing comm status with that 
		ld	comm_stat,a		  ;  just obtained
		bset	rn_stat,#COMM_ERR	  ; Set Communications Error in 1st level status 
		bset	rn_stat,#REJECT		  ; Set Command REJECT in 1st level status status		
rn_bitxfer30:	btjt	SPICSR,#SPIF,rn_bitxfer40 ; Check for next MDO while
		btjf	PBDR,#SS,rn_bitxfer30     ;  monitoring Slave Select
		pop	a			  ; No Slave Select so adjust
		pop	a			  ;  stack pointer
		disab_strobe			  ; DeAssert Strobe
		bset	PBDR,#3			  ; DeAssert /WR
		clr	a			  ; Restore Port A
		ld	PAOR,a			  ;  data bus
		ld	PADDR,a			  ;  to input mode
		jp	cmd_exit		  ;   then exit the current command
rn_bitxfer40:	ld	a,SPIDR			  ; Clear xfer flag and throw away data
		jra	rn_bitxfer30		  ; Continue until command ends with no Slave Select
rn_bitxfer50:	ld	a,SPIDR			  ; Get master receive data
		ret				  ;  and return
                              
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
												
;****************************************************************************************
;	Board Specific R-Net Command Routines
;****************************************************************************************
					;
					; WRITE CONTROL SIG POLARITIES TO FLASH
c0r10:	ld	a,SPIDR 		; Send previous byte to master
	call	rn_xfer			;  and get 1st byte of key
	ld	flashkey1,a		; Save 1st key
	call	rn_xfer			; Return 1st byte key get 2nd byte
	ld	flashkey2,a		; Save 2nd key
	call	rn_xfer			; Get control signal polarity data byte
	and	a,#$03			; Save only the two low bits
	ld	{e2data+0},a		;  and save in buffer		 		
 	ld	a,#sig_polarity.h 	; Load pointer to flash save registers
	ld	{flashaddr+0},a		; Load high byte of address
	ld	a,#sig_polarity.l	;
	ld	{flashaddr+1},a		; Load low byte of address
	ld	a,#$01			; Load the number of bytes to write
	ld	nbytes,a		;  into FLASH
	bset	pend,#FLASH		; Set flash write pending 
	jp	cmd_exit		; Must exit after flash write 
					;
					; TURN UP BAD LED ON/OFF	
c0r13:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get Bad LED enable/disable byte
	call	up_bad			;	
	jp	exitcmd			; Command End, Wait for SS to negate	
					;

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
					; RESET TARGET
c0r17:	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get reset timeout value
	ld	rst_tmr,a		; Load the reset timer
	cp	a,#$00			; Check for early termination of reset
	jreq	c0r17b			; YES..Turn off reset now
	btjt	polarity,#RESET,c0r17a	; Test for positive active reset
	bres	PCDR,#RESET		; NO..Assert negative active reset
	jp	exitcmd			; Command End, Wait for SS to negate
c0r17a:	bset	PCDR,#RESET		; YES..Assert positive active reset
	jp	exitcmd			; Command End, Wait for SS to negate
c0r17b:	btjt	polarity,#RESET,c0r17c	; Test for positive active reset
	bset	PCDR,#RESET		; NO..DeAssert negatative active reset	
	jp	exitcmd			; Command End, Wait for SS to negate
c0r17c:	bres	PCDR,#RESET		; YES..DeAssert positive active reset	
	jp	exitcmd			; Command End, Wait for SS to negate

	                                ; WRITE GPIF REGISTER 0-7
c0r20:	ld	a,rn_cmd		; Set up write register 
	and	a,#$07			;  address
	ld	PBDR,a			;  in PB bits 3-0
	ld	x,a			; Save index to shadow register
	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get write data byte
	ld	(shadow_regs,x),a	; Update the shadow register
	wr_port_a			; Write the data to addressed register
	jp	exitcmd			; Command End, Wait for SS to negate
					;
                                        ; SET BITS UNDER MASK IN GPIF WR REGISTER
c0r30:	ld	a,rn_cmd		; Get command
	and	a,#$07			;  and select register address bits
	ld	x,a			;  put in X to use as index
	ld	PBDR,a			; Set reg address in PB bits 3-0
	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get bit position byte
	or	a,(shadow_regs,x)	; OR with selected register shadow
	ld	(shadow_regs,x),a	; Update Shadow
	wr_port_a			; Write to target
	jp	exitcmd			; Done
					;
					; RESET BIT UNDER MASK IN GPIF WR REGISTER	
c0r40:	ld	a,rn_cmd		; Get command
	and	a,#$07			;  and select register address bits
	ld	x,a			;  put in X to use as index
	ld	PBDR,a			; Set reg address in PB bits 3-0
	ld	a,SPIDR			; Send command back to master
	call	rn_xfer			;  and get bit position byte
	cpl	a			; Put zeros in bit positions to reset
	and	a,(shadow_regs,x)	; AND with selected register shadow
	ld	(shadow_regs,x),a	; Update Shadow
	wr_port_a			; Write to target
	jp	exitcmd			; Command End, Wait for SS to negate
					; 
	                                ; WRITE MULTIPLE GPIF REGISTER 0-7
c0r50:	ld	a,rn_cmd		; Set up write register 
	and	a,#$07			;  address
	ld	PBDR,a			;  in PB bits 3-0
	ld	x,a			; Save index to shadow register
c0r50a:	ld	a,SPIDR			; Send command or previous byt back to master
	call	rn_xfer			;  and get write data byte
	ld	(shadow_regs,x),a	; Update the shadow register	
	wr_port_a			; Write the data to addressed register
	jra	c0r50a			; Continue until SS is negated
					;
	                                ; WRITE GPIF REGISTER 0-7 SEQUENTIAL
c0r60:	ld	a,rn_cmd		; Set up write register 
c0r60a:	and	a,#$07			;  address
	ld	PBDR,a			;  in PB bits 3-0
	ld	x,a			; Save index to shadow register
	ld	a,SPIDR			; Send command or previous byte back to master
	call	rn_xfer			;  and get write data byte
	ld	(shadow_regs,x),a	; Update the shadow register	
	wr_port_a			; Write the data to addressed register
	inc	x                       ; Move to next
	ld	a,x			;  register in sequence
	jra	c0r60a			; Continue until no Slave Select
					;	
					; READ/WRITE GPIF SERIAL DEVICE 0-7
c0r70:	ld	a,#$81			; Change Port A data bus bits 0 & 7
	ld	PADDR,a			;  from inputs to
	ld	PAOR,a			;  outputs
	ld	a,rn_cmd		; Setup serial device address
	and	a,#$07			;  and assert /WR
	ld	PBDR,a			;
	enab_strobe                     ; Assert Strobe
c0r70b:	ld	a,PADR			; Read port A to get serial data
	and	a,#$02			;  and mask off all but bit 1 
	call	rn_bitxfer		; Send read bit and get write byte 	
	and	a,#$01			; Mask off all but bit 0
	ld	PADR,a			;  and send to port A data bus
	bset	PADR,#7			; Raise then lower serial clock
	bres	PADR,#7			;  on port A bit 7	
	jra	c0r70b			;
	
	
	

					
;****************************************************************************************
					; READ CONTROL SIGNAL (Strobe & RESET) POLARITY
c1r10:	ld	a,sig_polarity		; Get the signal polarity register
	call	rn_xfer			; Send to master on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate
					;
					; READ ALARM TIMER
					;
					; READ RESET TIMER
c1r17:	ld	a,rst_tmr		; Get reset timeout timer register
	call	rn_xfer			; Send to master on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate
					;
	                                ; READ GPIF WRITE REGISTER SHADOW 0-7
c1r20:	ld	a,rn_cmd		; Set up shadow register 
	and	a,#$07			;  index
	ld	x,a			;  in X Reg
	ld	a,(shadow_regs,x)	; Get the selected shadow register
	call	rn_xfer			; Send shadow reg to master on next MDO
	jp	exitcmd			; Command End, Wait for SS to negate
	                                ;  
	                                
	                                ; AND BIT TEST FOR GPIF READ REGISTER 0-7
c1r30:	ld	a,rn_cmd		; Get command
	and	a,#$07			;  setup address and
	or	a,#$08			;  read/wr line of register 
	ld	PBDR,a			;  which will be tested
	ld	a,SPIDR			; Return command to master and
	call	rn_xfer			;  get bit positon byte
	ld	mask,a			; Save value for boolean operation
	enab_strobe			; Assert Strobe
	and	a,PADR			; Test data under mask
	cp	a,mask
	disab_strobe			; DeAssert Strobe
	jrne	c1r30a			; Test for a ONE fails
        ld	a,#$FF			; Test for a ONE passes, send 0xFF
        jra	c1r30b			;  to master
c1r30a:	clr	a			; Fails, prepare to send 0x00
c1r30b: call	rn_xfer			; Send result to master
	jp	exitcmd			; Command End, Wait for SS to negate

					;
                                        ; OR BIT TEST FOR GPIF READ REGISTER 0-7
c1r40:	ld	a,rn_cmd		; Get command
	and	a,#$07			;  setup address and
	or	a,#$08			;  read/wr line of register 
	ld	PBDR,a			;  which will be tested
	ld	a,SPIDR			; Return command to master and
	call	rn_xfer			;  get bit positon byte
	enab_strobe			; Assert Strobe
	and	a,PADR			; Test data under mask
	disab_strobe			; DeAssert Strobe
	jreq	c1r40a			; Test for a ONE fails
        ld	a,#$FF			; Test for a ONE passes, send 0xFF
        jra	c1r40b			;  to master
c1r40a:	clr	a			; Fails, prepare to send 0x00
c1r40b: call	rn_xfer			; Send result to master
	jp	exitcmd			; Command End, Wait for SS to negate
					;
	                                ; READ MUITIPLE GPIF Rd REGISTER 0-7
c1r50:	ld	a,rn_cmd		; Set up read register 
	and	a,#$07			;  address and
	or	a,#$08			;  read/wr line
	ld	PBDR,a			;  in PB bits 3-0	
c1r50a:	enab_strobe			; Assert Strobe
	ld	a,PADR			; Get read data
	disab_strobe			; DeAssert Strobe
	call	rn_xfer			; Send data do master on next MDO
	jra	c1r50a			; Continue reading byte until SS negates
					;
					; READ GPIF WRITE REGISTER 0-7 SHADOW 0-7
c1r60:	ld	a,rn_cmd                ; Set up shadow register
c1r60a:	and	a,#$07			;  address in X reg
	ld	x,a			;  and use as index
	ld	a,(shadow_regs,x)	; Get the selected shadow
	call	rn_xfer			; Send it back to the master
	inc	x                       ; Move to next shadow in sequence
	ld	a,x			;
	jra	c1r60a			; Continue until no Slave Select
					;
	                                ; READ GPIF REGISTER 0-7 SEQUENTIAL
c1r70:	ld	a,rn_cmd		; Set up read register 
c1r70a:	and	a,#$07			;  address and
	or	a,#$08			;  read/wr line
	ld	PBDR,a			;  in PB bits 3-0
	enab_strobe			; Assert Strobe
	ld	a,PADR			; Get read data
	disab_strobe			; DeAssert Strobe
	call	rn_xfer			; Send data to master on next MDO
	ld	a,PBDR			; Move to next GPIF register
	inc	a			; in sequence
	jra	c1r70a			; Continue until no Slave Select			   			


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
timer_A:
        ld	a,#$03
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
				
timer10:
	dec 	{msec_cnt+1}  
	ld	a,{msec_cnt+1} 
	cp	a,#$FF
	jrne	timer90	
	dec	{msec_cnt+0}
timer90:
	iret


;
;****************************************************************************************
;     Rabbitnet Command Lookup Table
;****************************************************************************************
.spi_jtbl:	
		dc.w	c0r00			; c0r00
		dc.w	cmd_rej			; c0r01
		dc.w	cmd_rej			; c0r02
		dc.w	c0r03			; c0r03
		dc.w	c0r04			; c0r04
		dc.w	c0r05			; c0r05
		dc.w	cmd_rej			; c0r06
		dc.w	c0r07			; c0r07
						;
		dc.w	c0r10			; c0r10
		dc.w	cmd_rej			; c0r11
		dc.w	cmd_rej			; c0r12
		dc.w	c0r13			; c0r13
		dc.w	cmd_rej			; c0r14
		dc.w	c0r15			; c0r15
		dc.w	c0r16			; c0r16
		dc.w	c0r17			; c0r17
						;
		dc.w	c0r20			; c0r20
		dc.w	c0r20			; c0r21
		dc.w	c0r20			; c0r22
		dc.w	c0r20			; c0r23
		dc.w	c0r20			; c0r24
		dc.w	c0r20			; c0r25
		dc.w	c0r20			; c0r26
		dc.w	c0r20			; c0r27
						;
		dc.w	c0r30			; c0r30
		dc.w	c0r30			; c0r31
		dc.w	c0r30			; c0r32
		dc.w	c0r30			; c0r33
		dc.w	c0r30			; c0r34
		dc.w	c0r30			; c0r35
		dc.w	c0r30			; c0r36
		dc.w	c0r30			; c0r37
						;
		dc.w	c0r40			; c0r40
		dc.w	c0r40			; c0r41
		dc.w	c0r40			; c0r42
		dc.w	c0r40			; c0r43
		dc.w	c0r40			; c0r44
		dc.w	c0r40			; c0r45
		dc.w	c0r40			; c0r46
		dc.w	c0r40			; c0r47
						;
		dc.w	c0r50			; c0r50
		dc.w	c0r50			; c0r51
		dc.w	c0r50			; c0r52
		dc.w	c0r50			; c0r53
		dc.w	c0r50			; c0r54
		dc.w	c0r50			; c0r55
		dc.w	c0r50			; c0r56
		dc.w	c0r50			; c0r57
						;
		dc.w	c0r60			; c0r60
		dc.w	c0r60			; c0r61
		dc.w	c0r60			; c0r62
		dc.w	c0r60			; c0r63
		dc.w	c0r60			; c0r64
		dc.w	c0r60			; c0r65
		dc.w	c0r60			; c0r66
		dc.w	c0r60			; c0r67
						;
		dc.w	c0r70			; c0r70
		dc.w	c0r70			; c0r71
		dc.w	c0r70			; c0r72
		dc.w	c0r70			; c0r73
		dc.w	c0r70			; c0r74
		dc.w	c0r70			; c0r75
		dc.w	c0r70			; c0r76
		dc.w	c0r70			; c0r77

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
		dc.w	cmd_rej			; c1r11
		dc.w	cmd_rej			; c1r12
		dc.w	cmd_rej			; c1r13
		dc.w	cmd_rej			; c1r14
		dc.w	cmd_rej			; c1r15
		dc.w	cmd_rej			; c1r16
		dc.w	c1r17			; c1r17
						;
		dc.w	c1r20			; c1r20
		dc.w	c1r20			; c1r21
		dc.w	c1r20			; c1r22
		dc.w	c1r20			; c1r23
		dc.w	c1r20			; c1r24
		dc.w	c1r20			; c1r25
		dc.w	c1r20			; c1r26 
		dc.w	c1r20			; c1r27
						;
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r31
		dc.w	c1r30			; c1r32
		dc.w	c1r30			; c1r33
		dc.w	c1r30			; c1r34
		dc.w	c1r30			; c1r35
		dc.w	c1r30			; c1r36
		dc.w	c1r30			; c1r37
						;
		dc.w	c1r40			; c1r40
		dc.w	c1r40			; c1r41
		dc.w	c1r40			; c1r42
		dc.w	c1r40			; c1r43
		dc.w	c1r40			; c1r44
		dc.w	c1r40			; c1r45
		dc.w	c1r40			; c1r46
		dc.w	c1r40			; c1r47
						;
		dc.w	c1r50			; c1r50
		dc.w	c1r50			; c1r51
		dc.w	c1r50			; c1r52
		dc.w	c1r50			; c1r53
		dc.w	c1r50			; c1r54
		dc.w	c1r50			; c1r55
		dc.w	c1r50			; c1r56
		dc.w	c1r50			; c1r57
						;
		dc.w	c1r60			; c1r60
		dc.w	c1r60			; c1r61
		dc.w	c1r60			; c1r62
		dc.w	c1r60			; c1r63
		dc.w	c1r60			; c1r64
		dc.w	c1r60			; c1r65
		dc.w	c1r60			; c1r66
		dc.w	c1r60			; c1r67
						;
		dc.w	c1r70			; c1r70
		dc.w	c1r70			; c1r71
		dc.w	c1r70			; c1r72
		dc.w	c1r70			; c1r73
		dc.w	c1r70			; c1r74
		dc.w	c1r70			; c1r75
		dc.w	c1r70			; c1r76
		dc.w	c1r70			; c1r77


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


