st7/  					; The first line is reserved 
                                        ; for specifying the instruction set
                                        ; of the targetted processor
                                  

;+------------------------------------------------------------------------------+
;|
;|		      R-Net Digital I/O Board Microcode
;|			Copyright (c), Z-World 2003
;|			Copyright (c), Digi International 2022
;|
;+------------------------------------------------------------------------------+
;****************************************************************************************
;|  Program Name:	rndio.asm	
;|  Date:   		7/7/03        
;|  Author:             CMG_MCD Application Team
;|  Revision History:	Rev 1.00 RNTeam	Initial Release (Common Rev 1.00)
;|  
;|                      Rev 1.01 V.O.  	Improved executition time of bit set/reset cmds
;|			commands C0R12 through C0R15.  (Common Rev 1.01)	                               
;|                       
;****************************************************************************************


        TITLE    "rndio.ASM"
                                        	; This title will appear on each
                                        	; page of the listing file
        MOTOROLA                        	; This directive forces the Motorola 
                                        	; format for the assembly (default)
                                        
                                        
;+------------------------------------------------------------------------------+
;|	FILES INCLUSION								|
;+------------------------------------------------------------------------------+
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_101\st72264.inc"	; Memory mapping file
        #INCLUDE "rndio.inc"						; Include general constants file
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_101\common.inc"
    	
;+------------------------------------------------------------------------------+
;|	DEFINES INCLUSION							|
;+------------------------------------------------------------------------------+
  	
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
        
	EXTERN  swdt_ival
 	EXTERN	rn_xfer
 	EXTERN	exitcmd
 	EXTERN	cmd_exit
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
  
        
;+------------------------------------------------------------------------------+
;|	DEFINES PAGE 0 RAM							|
;+------------------------------------------------------------------------------+
 BYTES
	SEGMENT 'Application_ram'

shadow_regs:					;
shadow_0_7:	ds.b 1				; Shadow reg for outputs 0-7
shadow_8_15:	ds.b 1				; Shadow reg for outputs 8-15
  
adc_regs:
adc0_hi:	ds.b 1				; ADC 0 high byte
adc0_lo:	ds.b 1				; ADC 0 high byte
adc1_hi:	ds.b 1				; ADC 1 high byte
adc1_lo:	ds.b 1				; ADC 1 high byte
adc2_hi:	ds.b 1				; ADC 2 high byte
adc2_lo:	ds.b 1				; ADC 2 high byte
adc3_hi:	ds.b 1				; ADC 3 high byte
adc3_lo:	ds.b 1				; ADC 3 high byte

adc_ctrl:	ds.b 1				; ADC control/status register  
						; Bit 7 ---> ADC 3 Enabled
						; Bit 6 ---> ADC 2 Enabled
						; Bit 5 ---> ADC 1 Enabled
						; Bit 4 ---> ADC 0 Enabled
						; Bit 3 ---> ADC 3 Updated
						; Bit 2 ---> ADC 2 Updated
						; Bit 1 ---> ADC 1 Updated
						; Bit 0 ---> ADC 0 Updated
						;
scr_0:		ds.b 1				; Scratch Register 0
dly_tmr:	ds.b 1				; Delay Timer

						;
db_leds:	ds.b 1				; Data Bus Indicator LEDs
						; Bit 7 --> R-LED (0 on,1 off)
						; Bit 6-0-> Unused

;****************************************************************************************				
;     Product Unique Paramters  
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
	
 WORDS                    
  	SEGMENT 'Application_parms_2'  
.prod_sn.w:
	dc.l	SERIAL_NUMBER			; 4 byte board unique serial number	
.prod_id.w:	
	dc.w	PRODUCT_ID
.prod_rev.w:
	dc.b	PRODUCT_REV
	
;+------------------------------------------------------------------------------+
;|	DEFINES CODE SPACE							|
;+------------------------------------------------------------------------------+
  WORDS         	
	SEGMENT  'Application_code'

.set_msk.w:
	dc.b	$01
	dc.b	$02
	dc.b	$04
	dc.b	$08
	dc.b	$10
	dc.b	$20
	dc.b	$40
	dc.b	$80	
 
.reset_msk.w:
	dc.b	$FE
	dc.b	$FD
	dc.b	$FB
	dc.b	$F7	
	dc.b	$EF
	dc.b	$DF
	dc.b	$BF
	dc.b	$7F 	
          												
								
;+------------------------------------------------------------------------------+
;|	MAIN SECTION	 							|
;+------------------------------------------------------------------------------+

  WORDS         	
	SEGMENT  'Application_code'

.main:		bset	db_leds,#R_LED		; Turn Off Red LED
		rim				; Enable Interrupts
main_05:	ld	a,db_leds		;
		ld	PADR,a			;	       
	 	btjf	cmd07_stat,#HWDT,main_10 ; Check for HWDT enabled
		ld	a,#HWDT_TIMEOUT		; Yes.. Enable and/or reload
		ld	WDGCR,a			;  with max count
main_10:	btjf	PBDR,#SS,main_25	; If Slave Select not asserted
		ld	a,SPICSR		;  Continulally clear status
		ld	a,SPIDR			;  and data from SPI port
main_25		call	dly_50			; Else.. Wait while processing ADCs
main_40:	jra	main_05			;
	
;			
;+------------------------------------------------------------------------------+
;|	SUB-ROUTINES SECTION							|
;+------------------------------------------------------------------------------+

;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			ST7 HARDWARE INITIALISATION				|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine performs functions unique to a Hard Reset			|
;|										|
;+------------------------------------------------------------------------------+

.brd_hard_init:	ret  				;
        					;                
        					
;+------------------------------------------------------------------------------+
;|										|
;|			ST7 SOFTWARE INITIALISATION				|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine performs functions unique to a Soft Reset			|
;|										|
;+------------------------------------------------------------------------------+

.brd_soft_init:	ret  				;
        					;  
;+------------------------------------------------------------------------------+
;|										|
;|			ST7 USER HANDLER ROUTINE				|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| User Handler that will get executed in the SPI interrupt routine if          |
;| USER bit is set in the pend control status byte. 				|
;+------------------------------------------------------------------------------+
        
.user_handler:	ret       					


;******************************************************************************** 

;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			Main Board Initialization				|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine initialises microcode variables					|
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

.brd_init:	
		ld	A,#$FF			; Load A reg with 1s
		ld	PADDR,A			; Initially set up PA as outputs
		ld	PAOR,A			; and make them push-pull
						; PA4 and PA6 are True Open Drain
		clr	PADR			; Set PA data bus to all 0s
		clr	db_leds			; Set PA shadow reg to match
						;
		ld	A,#$0F			; Set up PB bits as push-pull outputs with
		ld	PBDR,A			;  initial value 1 (high) on bits 3-0. 
		ld	PBDDR,A			; PB bits 7-4 input(SPI)
		ld	PBOR,A			; 
						;
		clr	PCDR			; Set up PC 5 & 2 bits as push-pull outputs with
		ld	A,#$24			;  with initial value 0 (low) 
		ld	PCDDR,A			; Set up PC 3-0 as floating inputs (ADC3-0)  
 		ld	PCOR,A			; 
 		                                ;
 		ld	A,safe_0_7		; Get save state value for OUT_0-7
 		ld	shadow_0_7,a		; Initialize shadow reg for OUT_0-7
 		call	wr_0_7			;  and OUT_0-7		
 		ld	a,safe_8_15		; Get save state value for OUT_8-15
                ld	shadow_8_15,a		; Initialize shadow reg for OUT_8-15
 		call	wr_8_15			;  and OUT_8-15
 		bres	PBDR,#OUT_EN		; Now enable output driver for OUT_0-15 
 						;
		clr	adc0_hi			; ADC 0 high byte data
		clr	adc0_lo			; ADC 0 low byte data
		clr	adc1_hi			; ADC 1 high byte data
		clr	adc1_lo			; ADC 1 low byte data
		clr	adc2_hi			; ADC 2 high byte data
		clr	adc2_lo			; ADC 2 low byte data
		clr	adc3_hi			; ADC 3 high byte data
		clr	adc3_lo			; ADC 3 low byte data
		ld	a,#$F0			; Start with ADC 3-0
		ld	adc_ctrl,a		;  enabled
						;
		clr	dly_tmr			; Set delay timer to zero
		clr	db_leds			; Trun off all data bus LEDs
		cpl	db_leds                 ;  by setting db_leds to FF
                ret

;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			25ms Interval Timer Routine				|
;|										|
;+------------------------------------------------------------------------------+
;|ROUTINE DESCRIPTION:								|
;| This routine is call once each 25ms by the MCC interrupt routine in common	|
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
.interval_tmr:	tnz	dly_tmr			; Test delay timer for zero
		jreq	intvl_10		; Yes..Skip decrement
		dec	dly_tmr			; No..Decrement delay timer

intvl_10:	tnz	act_tmr			; Test for Activity (non-zero)
		jreq	intvl_20		; No Activity, Turn off LED
		bres	db_leds,#R_LED		; Yes.Turn Activity LED On
		dec	act_tmr			; Decrement Activity Timer
		ret				; Done

intvl_20:	bset	db_leds,#R_LED		; Turn Off Activity LED
		ret				; Done 
                        
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
dly_50:		push	a			; Save register A from calling routine
 		ld	a,#2			; Load wait period 2*25ms = 50ms
		ld	dly_tmr,a		; Put it in the count down timer 0
		jra	dly			; Now wait	
dly_100:	push	a			; Save register A from calling routine
 		ld	a,#4			; Load wait period 4*25ms = 100ms
		ld	dly_tmr,a		; Put it in the count down timer 0
		jra	dly			; Now wait
dly_250:	push	a			; Save register A from calling routine
 		ld	a,#10			; Load wait period 10*25ms = 250ms
		ld	dly_tmr,a		; Put it in the count down timer 0
		jra	dly			; Now wait
dly_500:	push	a			; Save register A from calling routine
 		ld	a,#20			; Load wait period 20*25ms = 500ms
		ld	dly_tmr,a		; Put it in the count down timer 0
		jra	dly			; Now wait
dly_750:	push	a			; Save register A from calling routine
 		ld	a,#30			; Load wait period 30*25ms = 750ms
		ld	dly_tmr,a		; Put it in the count down timer 0
		jra	dly			; Now wait
dly_1000:	push	a			; Save register A from calling routine
 		ld	a,#40			; Load wait period 40*25ms = 1000ms
		ld	dly_tmr,a		; Put it in the count down timer 0
						;
dly:		btjf	adc_ctrl,#ADC0_EN,dly_a	; Check if ADC 0 enabled
		clr	a 			; Yes..Set ADC channel to 0
		call	adc_conv		; Initiate conversion on ADC 0
		sim
		bset	adc_ctrl,#ADC0_OK	; Set ADC 0 updated bit in ctrl/status reg
		bset	rn_stat,#ANALOG		; Set 1st level status bit
		ld	a,rn_stat		; Update R-Net status which will be sent
		ld	SPIDR,a			;  on next command
		rim
						;
dly_a:		btjf	adc_ctrl,#ADC1_EN,dly_b ; Check if ADC 1 enabled
		ld	a,#01			; Yes..Set ADC channel to 1
		call	adc_conv		; Initiate conversion on ADC 1
		sim
		bset	adc_ctrl,#ADC1_OK	; Set ADC 1 updated bit in ctrl/status reg
		bset	rn_stat,#ANALOG		; Set 1st level status bit
		ld	a,rn_stat		; Update R-Net status which will be sent
		ld	SPIDR,a			;  on next command
		rim
						;
dly_b:		btjf	adc_ctrl,#ADC2_EN,dly_c ; Check if ADC 2 enabled
		ld	a,#02			; Yes..Set ADC channel to 2
		call	adc_conv		; Initiate conversion on ADC 2
		sim
		bset	adc_ctrl,#ADC2_OK	; Set ADC 2 updated bit in ctrl/status reg
		bset	rn_stat,#ANALOG		; Set 1st level status bit
		ld	a,rn_stat		; Update R-Net status which will be sent
		ld	SPIDR,a			;  on next command
		rim
						;
dly_c:		btjf	adc_ctrl,#ADC3_EN,dly_d ; Check if ADC 3 enabled
		ld	a,#03			; Yes..Set ADC channel to 3
		call	adc_conv		; Initiate conversion on ADC 3
		sim
		bset	adc_ctrl,#ADC3_OK	; Set ADC 3 updated bit in ctrl/status reg
		bset	rn_stat,#ANALOG		; Set 1st level status bit
		ld	a,rn_stat		; Update R-Net status which will be sent
		ld	SPIDR,a			;  on next command
                rim                             ;		
dly_d:		tnz	dly_tmr			; Check main delay loop for zero
		jrne	dly			; No..Continue
		pop	a 			; Done, so restore A register		
		ret     			; Return to caller
	
;********************************************************************************
;********************************************************************************
;+------------------------------------------------------------------------------+
;|	READ INPUT REGISTERS							|
;+------------------------------------------------------------------------------+
;
;	*************************************************************************
; 	ROUTINE:	RD_INPUTS  RD_0_7, RD_8_15, RD_16_23
;	PARAMETERS:	A Reg Returns read data
;	REGS USED:	A
;	REGS MODIFIED:	A
;	STACK BYTES:	0
;	*************************************************************************


rd_0_7:		clr	PAOR			; Set port A to inputs
		clr	PADDR			;  and make them floating
		bres	PBDR,#IN0_7		; Assert IN_0-7 strobe 
		nop				; Port input setup delay
		ld	a,PADR  		; Move IN_0-7 data to A reg
		bset	PBDR,#IN0_7		; DeAssert IN_0-7 strobe
		cpl	PADDR			; Set port A to output
		cpl	PAOR			;  and make it push-pull
		ret           			; Return with data in A reg
						;
rd_8_15:	clr	PAOR			; Set port A to inputs
		clr	PADDR			;  and make them floating
		bres	PBDR,#IN8_15		; Assert IN_8-15 strobe
		nop				; Port input setup delay
		ld	a,PADR 			; Move IN_8-15 data to A reg
		bset	PBDR,#IN8_15		; DeAssert IN_8-15 strobe
		cpl	PADDR			; Set port A to output
		cpl	PAOR			;  and make it push-pull
		ret				; Return with data in A reg
						;
rd_16_23:	clr	PAOR			; Set port A to inputs
		clr	PADDR			;  and make them floating
		bres	PBDR,#IN16_23		; Assert IN_16-23 strobe
		nop				; Port input setup delay
		ld	a,PADR			; Move IN_16-23 to A reg
		bset	PBDR,#IN16_23		; DeAssert IN_16-23 strobe
		cpl	PADDR			; Set port A to output
		cpl	PAOR			;  and make it push-pull
		ret				; Return with data in A reg
	
;********************************************************************************
;********************************************************************************
;+------------------------------------------------------------------------------+
;|	WRITE OUTPUT REGISTER 0-7						|
;+------------------------------------------------------------------------------+
;
;	*************************************************************************
; 	ROUTINE:	WR_0_7
;	PARAMETERS:	A REG CONTAINS DATA for OUT_0-7
;	REGS USED:	A
;	REGS MODIFIED:	
;	STACK BYTES:	0
;	*************************************************************************
  
wr_0_7:   
	ld	shadow_0_7,a		; Update 0-7 shadow	
	ld	PADR,a			; Put write data on port A
	bset	PCDR,#2 		; Assert write 0-7 strobe
	bres	PCDR,#2			; Deassert write 0-7 strobe
	ld	a,db_leds		; Put LED status back
	ld	PADR,a			;  on port A data bus		
	ret 				; Return
  
;******************************************************************************** 

;********************************************************************************
;********************************************************************************
;+------------------------------------------------------------------------------+
;|	WRITE OUTPUT REGISTER 8-15						|
;+------------------------------------------------------------------------------+
;
;	*************************************************************************
; 	ROUTINE:	WR_8-15
;	PARAMETERS:	A REG CONTAINS DATA for OUT_0-7
;	REGS USED:	A
;	REGS MODIFIED:	
;	STACK BYTES:	0
;	*************************************************************************
  
wr_8_15:     
       	ld	shadow_8_15,a		; Update shadow 8-15
	ld	PADR,a			; Put write data on port A
	bset	PCDR,#5			; Assert write 8-15 strobe
	bres	PCDR,#5			; Deassert write 8-15 strobe
	ld	a,db_leds		; Put LED status back
	ld	PADR,a			;  on port A data bus
	ret   				; Return
 
;********************************************************************************
;
;********************************************************************************
;+------------------------------------------------------------------------------+
;|										|
;|			PERFORM ADC CONVERSION					|
;|										|
;+------------------------------------------------------------------------------+
;	*************************************************************************
; 	ROUTINE:	ADC_CONV
;	PARAMETERS:	A REG CONTAINS ADC CHANNEL TO CONVERT (0-3)
;	REGS USED:	A
;	REGS MODIFIED:	A,Y (Y is reserved for main & not modified by interrupts)
;	STACK BYTES:	0
;	*************************************************************************
;
adc_conv:       
	and	a,#$03			; Use ADC 0-3 only

	cp      a,#$02                  ; If ADC0 or ADC1 use ST7 channels    
	jrult   adc_enable              ;  0 and 1
        inc     a                       ; For ADC2 and ADC3 use ST7 channels 
                                        ;  3 and 4  
adc_enable:
	or	a,#$20			; Set enable ADC bit
	ld	ADCCSR,a		;  and start conversion
	and	a,#$07			; Mask bits for channel
	cp      a,#$03                  ; If ADC0 or ADC1 no need to adjust    
	jrult   adc_index               ;  storage register offset
					; If ADC2 or ADC3 (ST7 ch 3-4) 
        dec     a                       ;  adjust A reg back to 2-3                              
adc_index:        
	sll	a			; Convert channel to an index  
	                                ;  pointer to be able access  
	                                ;  paired ADC conversion values  
	ld	y,a			; Save the pointer  
adc_wait:				;
	btjf	ADCCSR,#7,adc_wait	; Check for EOC bit set
	
	ld	a,ADCDRL		; Get the low two bits of result
	ld	scr_0,a			;  and save in scratch register 0
	ld	a,ADCDRH		; Get the high 8 bits of result
	sll	a			;  and align with low 6 with
	sll	a			;  bits 7-2 of ADC low byte
	or	a,scr_0			;  making 7-0 the low 8
	ld	scr_0,a			;  bits of final low byte (ADCn_Lo)
	ld	a,ADCDRH		; Get the high 8 bits of result again
	and	a,#$C0			;  and mask off all but the high 2 bits
	swap	a			; Move bits 7-6 to bit positions 3-2
	srl	a	 		; Now move the two bits to 
	srl	a			;  to positions 1-0
	sim				; Disable Interrupts so that
	ld	(adc_regs,y),a		;  high and low
	ld	a,scr_0			;  parts of the ADCn
	ld	({adc_regs+1},y),a	;  result are atomic
	rim				; Re-enable interrupts
	ret				; Done

;********************************************************************************
;	Board Specific RabbitNet command routines 
;********************************************************************************
						; c0r10 (WR OUT 0-7)	
c0r10:		ld	a,SPIDR			; Return previous byte to master
		call	rn_xfer			; Send previous receive new wr data
		call	wr_0_7			; Update shadow and out 0-7
		jp	exitcmd			; Exit and wait for SS to unassert
						; c0r11 (WR OUT 8-15)
c0r11:		ld	a,SPIDR			; Return previous byte to master
		call	rn_xfer			; Send previous receive new wr data
		call	wr_8_15			; Update shadow and out 8-15
		jp	exitcmd			; Exit and wait for SS to unassert
						;
						; C0R12 (Reset Bit in out 7-0)
c0r12:		ld	a,SPIDR	 		; Send previous byte to master
		call	rn_xfer			;  and get bit position data 
		and	a,#$07			; Modulo 0-7 (bit # 0-7)
		ld	x,a			; Index into bit set mask table
		ld	a,(reset_msk,x)		; Get the reset mask
		and	a,shadow_0_7		; Form new out reg data in A 
		call	wr_0_7			; Update shadow and out 0-7
		jp	exitcmd			; Exit and wait for SS to unassert
						;
						; C0R13 (Reset Bit in out 15-8)				
c0r13:		ld	a,SPIDR	 		; Send previous byte to master
		call	rn_xfer			;  and get bit position data 
		and	a,#$07			; Modulo 0-7 (bit # 0-7)
		ld	x,a			; Index into bit set mask table
		ld	a,(reset_msk,x)		; Get the reset mask
		and	a,shadow_8_15		; Form new out reg data in A 
		call	wr_8_15			; Update shadow and out 0-7
		jp	exitcmd			; Exit and wait for SS to unassert
		                        	;
						; C0r14 (Set Bit in out 7-0)
c0r14:		ld	a,SPIDR	 		; Send previous byte to master
		call	rn_xfer			;  and get bit position data 
		and	a,#$07			; Modulo 0-7 (bit # 0-7)
		ld	x,a			; Index into bit set mask table
		ld	a,(set_msk,x)		; Get the set mask
		or	a,shadow_0_7		; Form new out reg data in A
		call	wr_0_7			; Update shadow and out 0-7
		jp	exitcmd			; Exit and wait for SS to unassert
						;
						; C0r15 (Set Bit in out 15-8)				
c0r15:		ld	a,SPIDR	 		; Send previous byte to master
		call	rn_xfer			; Send shadow and get bit position data 
 		and	a,#$07			; Modulo 0-7 (bit # 0-7)
		ld	x,a			; Index into bit set mask table
		ld	a,(set_msk,x)		; Get the set mask
		or	a,shadow_8_15		; Form new out reg data in A
		call	wr_8_15			; Update shadow and out 8-15
		jp	exitcmd			; Exit and wait for SS to unassert
			                      	;			                      	
						; c0r16 (Wr Safe State regs 0-7 & 8-15)	
c0r16:		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer			; Send safe data, get 1st byte of key
		ld	flashkey1,a		; Save 1st key
		call	rn_xfer			; Return 1st byte key get 2nd byte
		ld	flashkey2,a		; Save 2nd key
		call	rn_xfer			; Get safe data for 0-7
		ld	e2data,a		;  and save in buffer
		call	rn_xfer			; Get safe data for 8-15
		ld	{e2data+1},a		;  and save in buffer
 		ld	a,#{high safe_0_7} 	; Load pointer to flash save registers
		ld	{flashaddr+0},a		; Load high byte of address
		ld	a,#{low safe_0_7} 	;
		ld	{flashaddr+1},a		; Load low byte of address
		ld	a,#$02			; Load the number of bytes to write
		ld	nbytes,a		;  into FLASH
		bset	pend,#FLASH		; Set flash write pending 
		jp	cmd_exit		; Must exit after flash write 
                                                ;

c0r17:		ld	a,SPIDR			; Return previous byte to master
		call	rn_xfer			; Send previous receive new wr data
		call	wr_0_7			; Update shadow and out 0-7						; c0r11 (WR OUT 8-15)
		call	rn_xfer			; Send previous receive new wr data
		call	wr_8_15			; Update shadow and out 8-15
		jp	exitcmd			; Exit and wait for SS to unassert
						
						; WRITE CALIBRATION PARAMETERS					
						; c0r40 (ADCn Cal Parameters gain and offset)
c0r40:		ld	a,SPIDR			; Prepare to send previous byte
		call	rn_xfer			; Send previous, Get 1st byte of key
		ld	flashkey1,a		; Save 1st key
		call	rn_xfer			; Return 1st byte key get 2nd byte
 		ld	flashkey2,a		; Save 2nd key
		call	rn_xfer			; Send key 1 get gain_lo
		ld	{e2data+3},a		; Save in ram
		call	rn_xfer			; Send gain_lo, get next MSB of gain 
		ld	{e2data+2},a		; Save in ram
		call	rn_xfer			; Send previous gain, get next MSB of gain)
		ld	{e2data+1},a		; Save in ram		
		call	rn_xfer			; Send previous gain, get gain_hi
		ld	{e2data+0},a		; Save in ram		
		call	rn_xfer			; Send gain_hi get offset_lo
		ld	{e2data+5},a		; Save in ram
		call	rn_xfer			; Send offset_lo get offset_hi
		ld	{e2data+4},a		; Save in ram
						;
		ld	a,rn_cmd		; Form offset to desired
		and	a,#$03			;  set of adc calibration
		sll	a			;  constants.
		sll	a			; Multiply by 8 since there are
		sll	a			;  8 bytes per set
		add	a,#{low adc_cal_parms} 	;
		ld	{flashaddr+1},a		; Save LOW byte of pointer
		ld	a,#{high adc_cal_parms} ;
		adc	a,#$00			;
		ld	{flashaddr+0},a 	; Save HIGH byte of address
		ld	a,#$06			; Save the number of
		ld	nbytes,a		;  bytes to write into FLASH
		bset	pend,#FLASH		; Set flash to program on exit
		jp	cmd_exit		; Must exit after flash write 
						;
		                        	; WRITE ADC CONTROL/STATUS REGISTER
c0r47:		ld	a,SPIDR			; Set up previous byte 
		call	rn_xfer			;  and send to master, get wr data
		ld	adc_ctrl,a		; Update control/status reg
		jp	exitcmd			; Exit and wait for SS to unassert

; ***************************************************************************************

	       					
	       					; READ SHADOW REG for 0-7
c1r10:		ld	a,shadow_0_7	 	; Get dig-io register
		call	rn_xfer			; Send selected register to master				
		jp	exitcmd			; Exit and wait for SS to unassert
	       					; READ SHADOW REG for 8-15
c1r11:		ld	a,shadow_8_15	 	; Get dig-io register
		call	rn_xfer			; Send selected register to master				
		jp	exitcmd			; Exit and wait for SS to unassert
						;
	    					; READ DIGIO REGISTERS (RAW)
c1r12:          call	rd_0_7			; Read data on inputs 0-7
		call	rn_xfer			; Send Raw Data to master
		jp	exitcmd			; Exit and wait for SS to unassert
                                                ;
c1r13:          call	rd_8_15			; Read data on inputs 8-15
		call	rn_xfer			; Send Raw Data to master
		jp	exitcmd			; Exit and wait for SS to unassert
                                                ;
c1r14:          call	rd_16_23		; Read data on inputs 16-23
		call	rn_xfer			; Send Raw Data to master
		jp	exitcmd			; Exit and wait for SS to unassert
						;
						; c1r16 (Rd Safe State regs 0-7 & 8-15)	
c1r16:		ld	a,safe_0_7 		; Get safe state register data
		call	rn_xfer			; Send safe data 0-7 on next MDO
		ld	a,safe_8_15		; Get safe state register data 8-15
		call	rn_xfer			; Send to master on next MDO
		jp	exitcmd			; Exit and wait for SS to unassert

c1r17:          call	rd_0_7			; Read data on inputs 0-7
		call	rn_xfer			; Send Raw Data to master
                                                ;
	        call	rd_8_15			; Read data on inputs 8-15
		call	rn_xfer			; Send Raw Data to master
	   
	        call	rd_16_23		; Read data on inputs 16-23
		call	rn_xfer			; Send Raw Data to master
		jp	exitcmd			; Exit and wait for SS to unassert
						;
						; READ ADC REGS 0-7
c1r30:		ld	a,rn_cmd		; Form offset to desired
		sll	a                       ; Multiply X2 to access rawdata register pairs
		and	a,#$07			;  ADC register
		ld	x,a        		;  and place it in X
		ld	a,(adc_regs,x)		; Send MSB of rawdata to Master 
		call	rn_xfer			; Send to Master  
		inc     x 
		ld	a,(adc_regs,x)		; Get ADC register
		call	rn_xfer			; Send LSB of rawdata to Master
		jp	exitcmd			; Exit and wait for SS to unassert
 
						; READ ADC & CALIBRATION PARAMETERS (c1r40-43) 
c1r40:		ld	a,rn_cmd		; Form offset to desired		
		and	a,#$03			;  ADC raw data register pair
		sll	a			;  by multiplying by 2		
		ld	x,a        		;  then place it in X
		ld	a,({adc_regs+1},x) 	; Get raw data lo byte
		call	rn_xfer			; Send to master
		ld	a,({adc_regs+0},x)	; Get raw data hi byte
		call	rn_xfer			; Send to master
		                        	;
		ld	a,rn_cmd		; Form offset to desired
		and	a,#$03			;  set of adc calibration
		sll	a			;  constants.
		sll	a			; Multiply by 8 since there are
		sll	a			;  8 bytes per set
		ld	x,a			; Place in index reg X
		ld	a,({adc_cal_parms+3},x) ; Get LOW byte of gain
		call	rn_xfer			; Send to master
		                        	;
		ld	a,({adc_cal_parms+2},x) ; Get next most significant byte
		call	rn_xfer			; Send to master
				        	;
		ld	a,({adc_cal_parms+1},x) ; Get next most significant byte
		call	rn_xfer			; Send to master
		                        	;
		ld	a,({adc_cal_parms+0},x) ; Get HIGH byte of gain
		call	rn_xfer			; Send to master		
                                        	;
		ld	a,({adc_cal_parms+5},x) ; Get LOW byte of offset
		call	rn_xfer			; Send to master
		                        	;
		ld	a,({adc_cal_parms+4},x) ; Get HIGH byte of offset
		call	rn_xfer			; Send to master					
		jp	exitcmd			; Exit and wait for SS to unassert
						;
						; READ ADC CTRL/STAT REG & RESET UPDATED BITS
c1r47:		ld	a,adc_ctrl		; Get ADC control/status register
		call	rn_xfer			;  and sent master
		ld	a,adc_ctrl		; Now clear (bits 3-0) 
		and	a,#$F0			;  the ADC0-3 UPDATED
		ld	adc_ctrl,a		;  bits
		jp	exitcmd			; Exit and wait for SS to unassert
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
		dc.w	c0r14			; c0r14
		dc.w	c0r15			; c0r15
		dc.w	c0r16			; c0r16
		dc.w	c0r17			; c0r17
						;
		dc.w	cmd_rej			; c0r20
		dc.w	cmd_rej			; c0r21
		dc.w	cmd_rej			; c0r22
		dc.w	cmd_rej			; c0r23
		dc.w	cmd_rej			; c0r24
		dc.w	cmd_rej			; c0r25
		dc.w	cmd_rej			; c0r26
		dc.w	cmd_rej			; c0r27
						;
		dc.w	cmd_rej			; c0r30
		dc.w	cmd_rej			; c0r31
		dc.w	cmd_rej			; c0r32
		dc.w	cmd_rej			; c0r33
		dc.w	cmd_rej			; c0r34
		dc.w	cmd_rej			; c0r35
		dc.w	cmd_rej			; c0r36
		dc.w	cmd_rej			; c0r37
						;
		dc.w	c0r40			; c0r40
		dc.w	c0r40			; c0r41
		dc.w	c0r40			; c0r42
		dc.w	c0r40			; c0r43
		dc.w	cmd_rej			; c0r44
		dc.w	cmd_rej			; c0r45
		dc.w	cmd_rej			; c0r46
		dc.w	c0r47			; c0r47
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
		dc.w	cmd_rej			; c0r60
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
		dc.w	c1r16			; c1r16
		dc.w	c1r17			; c1r17
						;
		dc.w	cmd_rej			; c1r20
		dc.w	cmd_rej			; c1r21
		dc.w	cmd_rej			; c1r22
		dc.w	cmd_rej			; c1r23
		dc.w	cmd_rej			; c1r24
		dc.w	cmd_rej			; c1r25
		dc.w	cmd_rej			; c1r26 
		dc.w	cmd_rej			; c1r27
						;
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	cmd_rej			; c1r30
		dc.w	cmd_rej			; c1r30
		dc.w	cmd_rej			; c1r30
		dc.w	cmd_rej			; c1r30
						;
		dc.w	c1r40			; c1r40
		dc.w	c1r40			; c1r41
		dc.w	c1r40			; c1r42
		dc.w	c1r40			; c1r43
		dc.w	cmd_rej			; c1r44
		dc.w	cmd_rej			; c1r45
		dc.w	cmd_rej			; c1r46
		dc.w	c1r47			; c1r47
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
		dc.w	cmd_rej			; c1r60
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
		
;+------------------------------------------------------------------------------+
;|	APPLICATION PARAMETERS	 						|
;+------------------------------------------------------------------------------+
		
 WORDS         	
	SEGMENT 'Application_parms_3'
							 	
.adc_cal_parms.w:                               ;
adc0_gain:	dc.l	$3C200000		; ADC 0 gain (floating pt constant)
adc0_oset:	dc.w	$0000			; ADC 0 offset (Signed integer)
		dc.w	$1617			; Dummy bytes to make each set 8 bytes
adc1_gain:	dc.l	$3C200000		; ADC 1 gain (floating pt constant)
adc1_oset:	dc.w	$0000			; ADC 1 offset (Signed integer)
		dc.w	$2627			; Dummy bytes to make each set 8 bytes
adc2_gain:	dc.l	$3A800000		; ADC 2 gain (floating pt constant)
adc2_oset:	dc.w	$0000			; ADC 2 offset (Signed integer)
		dc.w	$3637			; Dummy bytes to make each set 8 bytes
adc3_gain:	dc.l	$3A000000		; ADC 3 gain (floating pt constant)
adc3_oset:	dc.w	$01FF			; ADC 3 offset (Signed integer)
		dc.w	$4647			; Dummy bytes to make each set 8 bytes 
                                                ;
.safe_0_7.w:	dc.b 	SAFE_STATE_0_7		; Saved value to initialize out 0-7
.safe_8_15.w:	dc.b	SAFE_STATE_8_15		; Saved value to initialize out 8-15


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


