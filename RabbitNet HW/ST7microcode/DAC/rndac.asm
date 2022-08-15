st7/  	; This is required for specifying the instruction set for the targeted processor         

;****************************************************************************************
;|										
;|		     Rabbitnet Board Specific Microcode 
;|			Copyright (c), Z-World 2003				
;|			Copyright (c), Digi International 2022
;|										
;****************************************************************************************
;|  Program Name:	rndac.asm	
;|  Date:   		5/9/03
;|  Revision History:	Rev 1.00 Internal Release  
;|                      Rev 1.01 Swapped logic for gain control of DAC channels 0 and 1   
;****************************************************************************************
        TITLE    "rndac.asm"	; This title will appear on each
                                ; page of the listing file
        MOTOROLA                ; This directive forces the Motorola 
                                ; format for the assembly (default)
                                                                                
;****************************************************************************************				
;     Header files to include MACRO's for Rabbitnet Projects 
;****************************************************************************************     	
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_101\st72264.inc"                                         	
        #INCLUDE "rndac.inc"         		
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_101\common.inc"
   	
;****************************************************************************************				
;     Extern references for accessing the common portion of the Rabbitnet microcode  
;****************************************************************************************     	
 BYTES
 	EXTERN	rn_cmd
 	EXTERN	rn_stat
 	EXTERN	swdt_reload 
 	EXTERN	act_tmr
 	EXTERN	swdt_tmr
 	EXTERN	cmd07_stat
 	EXTERN	comm_stat
 	EXTERN	pend
 	EXTERN	flashkey1
 	EXTERN	flashkey2
 	EXTERN	nbytes
 	EXTERN	flashaddr
 	EXTERN	e2data     
 	EXTERN	temp     
 	EXTERN  fptrCal

 WORDS
	EXTERN	exitcmd
	EXTERN  swdt_ival
 	EXTERN	rn_xfer
 	EXTERN	spi_pc
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
    
            
;****************************************************************************************				
;     Application Ram segment, all accesses to this area uses the short addressing mode  
;****************************************************************************************   
 BYTES
	SEGMENT 'Application_ram'    
dac0_cmd 	ds.b   	1    	; Channel 0 command byte, used for D/A access
dac1_cmd 	ds.b   	1  	; Channel 1 command byte, used for D/A access
dac2_cmd 	ds.b   	1      	; Channel 2 command byte, used for D/A access
dac3_cmd 	ds.b   	1      	; Channel 3 command byte, used for D/A access
dac4_cmd 	ds.b   	1      	; Channel 4 command byte, used for D/A access
dac5_cmd 	ds.b   	1   	; Channel 5 command byte, used for D/A access
dac6_cmd 	ds.b   	1      	; Channel 6 command byte, used for D/A access
dac7_cmd 	ds.b   	1      	; Channel 7 command byte, used for D/A access
                   
mac_cmd         ds.b    1      	; Data variable used for DAC command data
mac_data        ds.b    1      	; Data variable used for DAC data values
dac_config	ds.b	1	; Configuration data byte for a given D/A channel
dac_strobe	ds.b	1	; Flag for strobing the DAC outputs 
                              
dac_mode	ds.b    1      	; Sync vs. Async mode of operation                                
dac_convert	ds.b    1      	; Start conversion flag
dac_initial	ds.b    1      	; Configure DAC chip
dac_access	ds.b	1      	; User access the D/A chip directly
ptrDACdata	ds.w	1       ; Pointer for DAC data to be written 
                      
dly_tmr		ds.b 	1	; Delay Timer
led_tmr		ds.b	1	; Timer for controller LED OFF time    
led_toggle	ds.b	1	; LED control flay  
led_active	ds.b	1	; LED activity flag   
 
dac_Data0	ds.b	1      	; Command for DAC via RN cmd c0R40 or c1R40
dac_Data1	ds.b	1	; Data value to write to DAC via RN cmd c0R40 or c1R40
  
ptrCalTable	ds.w	1	; Pointer used for indirect access to cal data 
ptrRdCal	ds.w	1       ; Pointer used for indirect access to cal data  
ptrTemp 	ds.w	1       ; Temp pointer, used by microcode 
ptrCal		ds.w	1	; Pointer used for indirect access into cal pointers   

channel_save	ds.b	1

                                
;****************************************************************************************				
;     Application Ram segment, all accesses to this area uses the long addressing mode  
;****************************************************************************************   
 WORDS
	SEGMENT 'iap_&_stack_ram'     
                                   
dac0_data 	ds.b   	2 	; D/A 12 bit rawdata value for channel 0  
dac1_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 1
dac2_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 2
dac3_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 3
dac4_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 4
dac5_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 5
dac6_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 6
dac7_data 	ds.b   	2       ; D/A 12 bit rawdata value for channel 7

ptr_cal0    	ds.w	1  	; Pointer to calibation data for channel 0
ptr_cal1        ds.w	1       ; Pointer to calibation data for channel 1
ptr_cal2        ds.w	1       ; Pointer to calibation data for channel 2
ptr_cal3        ds.w	1       ; Pointer to calibation data for channel 3
ptr_cal4        ds.w	1       ; Pointer to calibation data for channel 4
ptr_cal5        ds.w	1       ; Pointer to calibation data for channel 5
ptr_cal6        ds.w	1       ; Pointer to calibation data for channel 6
ptr_cal7        ds.w	1      	; Pointer to calibation data for channel 7

                                                       		
;****************************************************************************************				
;     Product Paramters which get overwritten when microcode is downloaded 
;     in the field.  
;****************************************************************************************   
 WORDS       
        SEGMENT  'Application_parms_1'
.sig_regs.w:	
	dc.b	SS2CMD		; Slave Select to Command (# us)
	dc.b	CMD2DAT		; Command to Data (# us)
	dc.b	DAT2DAT		; Data to Data (# us)
	dc.b	DAT2SS		; Max Data to Slave Select Inactive (# us)
	dc.b    SS_ACTIVE       ; Max Slave Select Active (#ms)
	
.code_rev.w:
	dc.w	CODE_REV
	
.swdt_ival.w:
	dc.b	SWDT2SWDT	; Initial value of SWDT	        
	
 
;****************************************************************************************				
;     Product Paramters which do not get overwritten when microcode is downloaded 
;     in the field.  
;****************************************************************************************    
 WORDS     
        SEGMENT  'Application_parms_2' 
.prod_sn.w:
	dc.l	SERIAL_NUMBER	; 4 byte board unique serial number  		
.prod_id.w:	
	dc.w	PRODUCT_ID
.prod_rev.w:
	dc.b	PRODUCT_REV	
	
;****************************************************************************************				
;     Product calibration paramters which do not get overwritten when microcode is 
;     downloaded in the field.  
;     Note: Segment is commented out so calibration data won't get over written when
;           new code is downloaded in the field.
;****************************************************************************************    
; WORDS     
   
        SEGMENT byte at F800-FC00 'cal'			; 1K

        ; Configuration 0
	dc.l    $C35121BA
	dc.w    $D6FF     	
	dc.l    $014F20BA
	dc.w    $0100
	dc.l    $2DE328BB
	dc.w    $F7FF	
	dc.l    $6B5628BB
	dc.w    $F8FF	
	dc.l    $138D27BB
	dc.w    $F1FF	
	dc.l    $C3642ABB
	dc.w    $FBFF	
	dc.l    $276427BB
	dc.w    $FAFF	
	dc.l    $A9F82ABB
	dc.w    $2500

        ; Configuration 1
	dc.l    $EB48A4BA
	dc.w    $D6FF     	
	dc.l    $9747A2BA
	dc.w    $0100
	dc.l    $91CB28BB
	dc.w    $F6FF	
	dc.l    $C7D128BB
	dc.w    $F8FF	
	dc.l    $A17627BB
	dc.w    $F0FF	
	dc.l    $4B752ABB
	dc.w    $FAFF	
	dc.l    $BF9F27BB
	dc.w    $F9FF	
	dc.l    $EFA42ABB
	dc.w    $2200
          
       ; Configuration 2
	dc.l    $41112ABB
	dc.w    $D6FF     	
	dc.l    $135F27BB
	dc.w    $0100
	dc.l    $91CB28BB
	dc.w    $F6FF	
	dc.l    $C7D128BB
	dc.w    $F8FF	
	dc.l    $A17627BB
	dc.w    $F0FF	
	dc.l    $4B752ABB
	dc.w    $FAFF	
	dc.l    $BF9F27BB
	dc.w    $F9FF	
	dc.l    $EFA42ABB
	dc.w    $2200

      ; Configuration 3
	dc.l    $2794A0BA
	dc.w    $EAFF     	
	dc.l    $4D3CA0BA
	dc.w    $0000
	dc.l    $7F57A9BB
	dc.w    $FBFF	
	dc.l    $29F3A8BB
	dc.w    $FCFF	
	dc.l    $87AFA7BB
	dc.w    $F7FF	
	dc.l    $AF32AABB
	dc.w    $FEFF	
	dc.l    $5D2AA7BB
	dc.w    $FCFF	
	dc.l    $B741AABB
	dc.w    $1100
               

      ; Configuration 4
	dc.l    $E14E24BB
	dc.w    $ECFF     	
	dc.l    $9D3322BB
	dc.w    $0000
	dc.l    $5B19A9BB
	dc.w    $FBFF	
	dc.l    $0B20A9BB
	dc.w    $FDFF	
	dc.l    $9DEFA7BB
	dc.w    $F8FF	
	dc.l    $37DFA9BB
	dc.w    $FEFF	
	dc.l    $EF96A7BB
	dc.w    $FEFF	
	dc.l    $0F30AABB
	dc.w    $1200
               
      ; Configuration 5
	dc.l    $D928AABB
	dc.w    $ECFF     	
	dc.l    $BB9BA7BB
	dc.w    $0000
	dc.l    $C742A9BB
	dc.w    $FBFF	
	dc.l    $29F3A8BB
	dc.w    $FCFF	
	dc.l    $87AFA7BB
	dc.w    $F7FF	
	dc.l    $AF32AABB
	dc.w    $FEFF	
	dc.l    $5D2AA7BB
	dc.w    $FCFF	
	dc.l    $CF2BAABB
	dc.w    $1100
               

               
           												
;****************************************************************************************				
;|	D/A chip communication control to/from the ST7 micro-controller		
;|      ---------------------------------------------------------------		   
;|      1. PA4 - PA7 = 4 parallel bits to serial data path                      
;|      2. PC0 = Read 4 bits of data                                            
;|      3. PC1 = Write 4 bits of data                                           
;|      4. PC2 = CLK the D/A and PAL serial shift register                      
;|      5. PC3 = D/A Reset                                                      
;|      6. PC4 = D/A Chip Select                                                    
;|	                                                                        
;|	D/A input signal control single-ended vs. bipolar                       
;|	-------------------------------------------------			
;|      1. PIO_0 2.5v ref enable ch 0&1                                         
;|      2. PIO_1 GND ref enable ch 0&1                                          
;|      3. PIO_2 2.5v ref enable ch 2&3                                         
;|      4. PIO_3 GND ref enable ch 2&3                        			
;|      5. PB0  2.5v ref enable ch 4&5                                                                
;|	6. PB1  GND ref enable ch 4&5                                                                
;|      7. PB2  2.5v ref enable ch 6&7                                                               
;|      8. PB3	GND ref enable ch 6&7						
;****************************************************************************************				
 
 
;****************************************************************************************
;MACRO:		DACLK
;DESCRIPTION:	Provides 4 CLK pulses to clock the D/A and PLD chip. Use this 
;              	clocking method when using the ST7 debugging tools.
;PARAMETERS:	None
;RETURN VALUES: None
;****************************************************************************************
DACLK   	MACRO    
		bres   PCDR,#2			; Clock Cycle #1 low/high       
		bset   PCDR,#2          

		bres   PCDR,#2          	; Clock Cycle #2 low/high  
		bset   PCDR,#2

		bres   PCDR,#2          	; Clock Cycle #3 low/high  
		bset   PCDR,#2

	   	bres   PCDR,#2          	; Clock Cycle #4 low/high  
		bset   PCDR,#2
		MEND


;****************************************************************************************
;MACRO:		AD_WR16
;DESCRIPTION:	Writes a byte to the DAC chip to the cmd register specified.
;PARAMETER1:	Command byte for the DAC chip.
;PARAMETER2:    Data to be written to the D/A chip cmd register 
;RETURN VALUES: None  
;****************************************************************************************                    
AD_WR16  	MACRO	cmd, data 
		bres    PCDR,#3         	; Assert /DAC_CS signal 
		ld	x,#$FC          	; Set Port A as output lines
		ld      PADDR,x		            		
		ld      y,cmd       		; Put data on port A for latch
		ld  	PADR,y
		bres    PCDR,#1			; Strobe register  
		bset    PCDR,#1 
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		DACLK     
		bset    PCDR,#0			; Unassert /OE signal      
		ld      PADDR,x			; Set Port A as output lines               
		swap    y
		ld      PADR,y
		bres    PCDR,#1			; Strobe data into latch   
		bset    PCDR,#1                  
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal                          
     		DACLK	  
  
	       	bset    PCDR,#0  		; Unassert /OE signal     
		ld      PADDR,x			; Set Port A as output lines            		
		ld      y,data          	; Put data on port A for latch
		ld  	PADR,y
		bres    PCDR,#1			; Strobe register  
		bset    PCDR,#1 
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		DACLK     
		bset    PCDR,#0			; Unassert /OE signal      
		ld      PADDR,x			; Set Port A as output lines               
		swap    y
		ld      PADR,y
	       	bres    PCDR,#1			; Strobe data into latch   
		bset    PCDR,#1	                  
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal                          
       	 	DACLK   
        	bset    PCDR,#0  		; Unassert /OE signal   
		bset    PCDR,#3         	; Unassert /DAC_CS signal  
        	MEND
   
   
;****************************************************************************************
;MACRO:		ADDWORD
;DESCRIPTION:	Adds two 16 bits values
;PARAMETER1:	1st 16 bit value to be added.
;PARAMETER2:    2nd 16 bit value to be added. 
;RETURN VALUES: None  
;****************************************************************************************                       
ADDWORD		MACRO	add_a, add_b
		ld 	a,{add_a+1} 		; get number A’ LSB
		add 	a,{add_b+1} 		; add number B’ LSB
		ld	x,a
		ld 	a,{add_a+0} 		; get number A’ MSB
		adc 	a,{add_b+0} 		; add number B’ MSB with LSB’s carry
		MEND
     
 WORDS         	
	SEGMENT  'Application_code'
	 
;****************************************************************************************
;ROUTINE:	main
;DESCRIPTION:	Backgound task for handling D/A conversions and Rabbit commands  
;PARAMETERS:	None 
;RETURN VALUES: None
;**************************************************************************************** 
.main: 		bres	WDGCR,#7		; Disable WDT      			   
		bset	PCDDR,#LED    
		bset    PCOR,#LED    
		bset    PCDR,#LED                                
 		rim				; Enable all interrupts

main10:        	bres	PCDR,#5			; Assert LDAC for async loading of DAC's
		btjf	cmd07_stat,#HWDT,main20 ; Check for HWDT enabled
		ld	a,#HWDT_TIMEOUT		; Yes.. Enable and/or reload
		ld	WDGCR,a			;  with max count 	        	        	
main20:		btjt	dac_initial,#0,main70   ; Jump if configure D/A flag ia set
		btjt	dac_access,#0,main80    ; Jump if direct access flag is set
		btjt	dac_mode,#0,main55
		btjf	dac_convert,#0,main10	; Jump if convert flag set 	
		call    SingleCh		; Do DAC conversion   
 		clr	dac_convert		; Clear conversion flag
		call    RN_Ready                ; Set Rabbitnet status back to READY
	        jra	main10                  ; Loop and wait for next DAC command 
	        	                                              
main50:		call    SingleCh		; Do DAC conversion   
 		clr	dac_convert		; Clear conversion flag
		call    RN_Ready                ; Set Rabbitnet status back to READY
main55:	        bset	PCDR,#5                 ; Unassert LDAC for Sync loading of DAC's
	 	btjf	cmd07_stat,#HWDT,main60 ; Check for HWDT enabled
		ld	a,#HWDT_TIMEOUT		; Yes.. Enable and/or reload
		ld	WDGCR,a			;  with max count
main60:		btjt	dac_initial,#0,main70   ; Jump if configure D/A flag ia set
		btjt	dac_access,#0,main80    ; Jump if direct access flag is set
		btjf	dac_mode,#0,main10
		btjt	dac_convert,#0,main50	; Jump if convert flag set
		btjf	dac_strobe,#0,main55	; Check for DAC output strobe  	 	
		bres	PCDR,#5
		bset	PCDR,#5
		clr	dac_strobe
	        jra	main55                  ; Continue waiting for command 

main70:	        call    ConfigDAC	        ; Go configure D/A channel
		clr	dac_initial		; Clear configure D/A flag
		call    RN_Ready                ; Set Rabbitnet status back to READY
	        jra	main10                  ; Jump back to main execution loop 

main80:		call	DAC_Access              ; Access the DAC directly
		clr	dac_access              ; Clear the DAC direct access flag
		call    RN_Ready                ; Set Rabbitnet status back to READY 
                jp	main10                  ; Jump back to main execution loop
        
			
;****************************************************************************************
;ROUTINE:	ConfigDAC
;DESCRIPTION:	Routine to configure an D/A channel with command and Calibration Data  
;****************************************************************************************
ConfigDAC:      clr     dac_mode                ; Check Async vs. Sync mode of operation
		btjf    dac_config,#3,Config10  ; If bit set then set DAC for Sync mode
                cpl	dac_mode
Config10:	call	setRefVolt		; Set the reference voltage for the DAC 
                call	setGain01  	        ; Set gain for channel 0 and 1
		call	configCalib             ; Initialize cal pointer for each ch
		ret                             ; Exit back to calling routine
		
; Lookup table for calibration data for all DAC modes of operation  
CAL_CONFIG0: 	dc.w	cal_mode0		; Mode 0  Ch0& 1 = 2.5v   Ch2-7 10v	               
CAL_CONFIG1: 	dc.w	cal_mode1		; Mode 1  Ch0& 1 = 5.0v   Ch2-7 10v             
CAL_CONFIG2: 	dc.w	cal_mode2	        ; Mode 2  Ch0& 1 = 10.0v  Ch2-7 10v
CAL_CONFIG3: 	dc.w	cal_mode3	        ; Mode 3  Ch0& 1 = 5.0v   Ch2-7 10v
CAL_CONFIG4: 	dc.w	cal_mode4	        ; Mode 4  Ch0& 1 = 10.0v  Ch2-7 10v
CAL_CONFIG5: 	dc.w	cal_mode5	        ; Mode 5  Ch0& 1 = 20.0v  Ch2-7 10v
CAL_CONFIG6: 	dc.w	cal_mode6	        ; Mode 6  Currently not being used 
CAL_CONFIG7: 	dc.w	cal_mode7	        ; Mode 7  Currently not being used
					                              
;****************************************************************************************
;ROUTINE:	configCalib
;DESCRIPTION:	Initializes the DAC channel calibration pointers depending on the 
;               mode of operation.      
;
; 		Mode 0  Ch0&1 = 2.5v   Ch2-7 10v calibration starts at cal_mode0 	               
; 		Mode 1  Ch0&1 = 5.0v   Ch2-7 10v calibration starts at cal_mode1             
; 		Mode 2  Ch0&1 = 10.0v  Ch2-7 10v calibration starts at cal_mode2 
; 		Mode 3  Ch0&1 = 5.0v   Ch2-7 10v calibration starts at cal_mode3 
; 		Mode 4  Ch0&1 = 10.0v  Ch2-7 10v calibration starts at cal_mode4 
; 		Mode 5  Ch0&1 = 20.0v  Ch2-7 10v calibration starts at cal_mode5     
;
;****************************************************************************************
configCalib: 	ld	a,dac_config    	; Load config information 
		and	a,#$07                  ; Mask out all except bits 2-0
		clr	x                       ; Set index to zero

		cp	a,#$00                  ; Check for config mode 0
		jrne	Ch01_00
		ld	a,(CAL_CONFIG0,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG0,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60

Ch01_00:	cp	a,#$01		        ; Check for config mode 1
		jrne	Ch01_15  
		ld	a,(CAL_CONFIG1,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG1,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60
                
Ch01_15:	cp	a,#$02                  ; Check for config mode 2
		jrne	Ch01_20
		ld	a,(CAL_CONFIG2,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG2,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60
                  
Ch01_20:	cp	a,#$03                  ; Check for config mode 3
		jrne	Ch01_25
		ld	a,(CAL_CONFIG3,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG3,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60
                
Ch01_25:	cp	a,#$04                  ; Check for config mode 4
		jrne	Ch01_30
		ld	a,(CAL_CONFIG4,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG4,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60
   
Ch01_30:	cp	a,#$05                  ; Check for config mode 5
		jrne	Ch01_35
		ld	a,(CAL_CONFIG5,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG5,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60
   
Ch01_35:	cp	a,#$06                  ; Check for config mode 6
		jrne	Ch01_40
		ld	a,(CAL_CONFIG6,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG6,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a
                jra	Ch01_60
                                                ; Must be config mode 7
Ch01_40:	ld	a,(CAL_CONFIG7,x) 	; Load base addr of cal table
	       	ld	{ptrCalTable+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(CAL_CONFIG7,x)  	; Load LSB of address
	       	ld	{ptrCalTable+1},a

Ch01_60:	clr	{ptrTemp+0}             ; Load temp with cal entry offset
                ld	a,#$06
                ld	{ptrTemp+1},a       	

		clr	x                	; Load Channel 0 calibration pointer
	   	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a  
	   	inc	x
	      	push	x 
	      	
	      	ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x                       ; Load Channel 1 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a 
                inc	x
	      	push	x 
               
                ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x                       ; Load Channel 2 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a
                inc	x
	      	push	x 
 	                  
                ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x                    	; Load Channel 3 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a 
               	inc	x
	      	push	x

                ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x            		; Load Channel 4 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a 
               	inc	x
	      	push	x

                ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x                    	; Load Channel 5 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a 
               	inc	x
	      	push	x
               
                ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x                       ; Load Channel 6 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a 
               	inc	x
	      	push	x

                ADDWORD ptrCalTable,ptrTemp	; Add offset to base address
                ld	{ptrCalTable+0},a    	; Load ptr to D/A command for given ch  
                ld	{ptrCalTable+1},x                             
                
                pop	x                      	; Load Channel 7 calibration pointer
               	ld	a,{ptrCalTable+0} 
	   	ld	(ptr_cal0,x),a
	      	inc	x
	      	ld	a,{ptrCalTable+1} 
	   	ld	(ptr_cal0,x),a        
	    	ret
                             
;****************************************************************************************
;ROUTINE:	setGain01
;DESCRIPTION:	Sets the gain of op amps for channels 0 and 1 depending on the mode 
;               of operation.
;               Mode 0  Sets gain for both channels to have a max voltage of 2.5v 
;               Mode 1  Sets gain for both channels to have a max voltage of 5.0v
;               Mode 2  Sets gain for both channels to have a max voltage of 10.0 
;               Mode 3  Sets gain for both channels to have a max voltage of 5.0v
;               Mode 4  Sets gain for both channels to have a max voltage of 10.0
;               Mode 5  Sets gain for both channels to have a max voltage of 20.0
;****************************************************************************************
setGain01:  	ld	a,dac_config  		; Get configuration command  	
		and	a,#$07                  ; Mask out all bits except bits 2-0 

		cp	a,#$06                  ; Check for valid mode of operation
		jruge	setGain01_90                

		cp	a,#$00                  ; Check for mode 0
		jrne	setGain01_10            ; Set ch0&1 for 2.5 volt operation  
		bset	PBDR,#0                 ; (internal ref = 1v)
		bres	PBDR,#1     
		bset	PBDR,#2  
		bres	PBDR,#3
                jra	setGain01_90  
                
setGain01_10:	cp	a,#$01                	; Check for mode 1
		jrne	setGain01_20            ; Set ch0&1 for 5.0 volt operation
		bset	PBDR,#1                 ; (internal ref = 1v)
		bres	PBDR,#0
		bset	PBDR,#3  
		bres	PBDR,#2		
 	        jra	setGain01_90
 	        
setGain01_20:	cp	a,#$02                	; Check for mode 2
		jrne	setGain01_30            ; Set ch0&1 for 10 volt operation
		bset	PBDR,#0                 ; (internal ref = 1v)
		bset	PBDR,#1
		bset	PBDR,#2  
		bset	PBDR,#3
			
setGain01_30:	cp	a,#$03                 	; Check for mode 3
		jrne	setGain01_40            ; Set ch0&1 for 5 volt operation
		bset	PBDR,#0                 ; (internal ref = 2v)
		bres	PBDR,#1     
		bset	PBDR,#2  
		bres	PBDR,#3
                jra	setGain01_90  
                
setGain01_40:	cp	a,#$04                	; Check for mode 4
		jrne	setGain01_50            ; Set ch0&1 for 10 volt operation
		bset	PBDR,#1                 ; (internal ref = 2v)
		bres	PBDR,#0
		bset	PBDR,#3  
		bres	PBDR,#2		
 	        jra	setGain01_90
 	        
setGain01_50:	bset	PBDR,#0                	; Must be mode 5
		bset	PBDR,#1                 ; Set ch0&1 for 20 volt operation
		bset	PBDR,#2                 ; (internal ref = 2v)
		bset	PBDR,#3		          	
setGain01_90:	ret                             
	
;****************************************************************************************
;ROUTINE:	setRefVolt
;DESCRIPTION:	Sets the DAC chip internal reference voltage to either 1v or 2v, 
;               depending on the mode of operation.
;               Modes 0 - 2  Internal Ref = 1v
;               Modes 3 - 5  Internal Ref = 2v 
;****************************************************************************************
setRefVolt:    	ld	a,#$06            	; Load cmd to set DAC with 2v reference
		ld	mac_data,a
		ld	a,dac_config
		and	a,#$07                  ; Check if config is 3 or higher
		cp	a,#$03
		jruge   setRefVolt10   
		ld	a,#$04                	; Load cmd to set DAC with 2v reference
		ld	mac_data,a

setRefVolt10:	ld	a,#$80                  ; Set command to write to CTRL0 register
		ld      mac_cmd,a       	; Load MACRO parameter with D/A command
		AD_WR16 mac_cmd,mac_data 	; Write Reference setting to DAC chip
		ret		   


;****************************************************************************************
;ROUTINE:	SingleChAsync
;DESCRIPTION:	Routine setup to do coversions on a single channel under control from 
;               the Master.  
;****************************************************************************************
SingleCh: 	; load Reg A with CMD for channel to be converted 		
		clr	x
		ld     	a,([ptrDACdata.w],x)    ; Load Reg with D/A command for given ch
		ld      mac_cmd,a       	; Load MACRO parameter with D/A command
		inc	x
		ld	a,([ptrDACdata.w],x)
		ld	mac_data,a
		AD_WR16 mac_cmd,mac_data 	; Write D/A channel
		ret		   
		       		                                                                                                                         
;****************************************************************************************
;ROUTINE:	DAC_Access
;DESCRIPTION:	Routine to Read/Write the DAC chip directly.  
;****************************************************************************************                                                                                         
DAC_Access:	AD_WR16  dac_Data0, dac_Data1 	; Write data to DAC register		     
		ret                             ; Exit
                                                    

;****************************************************************************************
;ROUTINE:	RN_Ready
;DESCRIPTION:	Routine to change Rabbitnet status back to a slave READY state. Function 
;               will also make sure changing the status occurs during Non-Rabbitnet 
;               activtiy.  
;**************************************************************************************** 
RN_Ready:	btjf	PBDR,#SS,RN_Ready 	; Wait for SS to be inactive
		sim                             ; Disable interrupts
		bres	rn_stat,#BUSY           ; Clear Busy status bit 
		bset	rn_stat,#READY          ; Set Ready status bit
		ld	a,SPICSR		; Clear the SPIF flag again 	
		ld      a,rn_stat               ; Load Reg A with rabbitnet status
		ld     	SPIDR,a                 ; Load SPI send register with rn_stat
		rim    	                        ; Re-enable interrupts
		ret


;****************************************************************************************				
;	User Handler that will get executed in the SPI interrupt routine if
;       USER bit is set in the pend control status byte. 
;****************************************************************************************				    
.user_handler: 	ret   

;****************************************************************************************
;* Board Initialization Routine for Hardware Reset 
;****************************************************************************************
.brd_hard_init:	ld      a,#$00        		; Initialize port A, PA7-PA0 as inputs                  
		ld      PADDR,a  
		ld	PAOR,a                        
		     
		ld      a,#$00      		; Initialize port B, PB7-PB4 as inputs                            
		ld      PBDR,a	                ; and PB3-PB0 as outputs.
		ld      PBOR,a  
		ld	PBDDR,a                      
		ld	a,#$0F
		ld	PBDR,a
		ld	PBDDR,a  
		ld      PBOR,a             
                                                                 
		ld	a,#$00    		; Initialize port C PC5 & PC3 as input 
		ld      PCDR,a                  ; and PC4-PC0 as outputs.
		ld	PCDDR,a    
		ld      PCOR,a                                          
		ld      a,#$3F			
		ld      PCDR,a   
		ld	PCDDR,a    
		ld      PCOR,a
		bres	PCDR,#5                                    

		ld      a,#$80         		; Initialize D/A reg CTRL0  
		ld      mac_cmd,a               ; 
		ld      a,#$04                  ; Set Internal ref to 1 volts 
		ld      mac_data,a              ; 
	        AD_WR16 mac_cmd,mac_data        ; Write command to DAC    
	             	    
  	    	ld      a,#$90         		; Initialize D/A reg CTRL1  
		ld      mac_cmd,a               ; 
		ld      a,#$0F                  ; Config DAC for normal operation
		ld      mac_data,a              ; and fast conversion speed. 
	        AD_WR16 mac_cmd,mac_data 
	              
  		bset	PBDR,#0                 ; Set Ch0 and Ch1 for 10v operation   
		bset	PBDR,#1                 ; (internal ref = 1v) 
		bset	PBDR,#2  
		bset	PBDR,#3
   	  		                   
                ld	a,#$00        		; Setup DAC CMD for a gain of X1         
		ld	dac0_cmd,a     		; Initial ch0 command variable
                ld	a,#$10        		; Setup DAC CMD for a gain of X1         
		ld	dac1_cmd,a     		; Initial ch1 command variable
               	ld	a,#$20        		; Setup DAC CMD for a gain of X1         
		ld	dac2_cmd,a     		; Initial ch2 command variable
               	ld	a,#$30        		; Setup DAC CMD for a gain of X1         
		ld	dac3_cmd,a     		; Initial ch3 command variable
               	ld	a,#$40        		; Setup DAC CMD for a gain of X1         
		ld	dac4_cmd,a     		; Initial ch4 command variable
               	ld	a,#$50        		; Setup DAC CMD for a gain of X1         
		ld	dac5_cmd,a     		; Initial ch5 command variable
               	ld	a,#$60        		; Setup DAC CMD for a gain of X1         
		ld	dac6_cmd,a     		; Initial ch6 command variable
               	ld	a,#$70        		; Setup DAC CMD for a gain of X1         
		ld	dac7_cmd,a     		; Initial ch7 command variable
       	  			
 		clr	dac_initial      	; Clear D/A configure flag
		clr	dac_access      	; Clear direct D/A access flag
		clr	dac_convert     	; Clear single-channel convert flag
		clr	led_active	        ; Clear LED status flag
		clr	dac_mode                ; Set DAc to Async mode
		ret
                           
		
;****************************************************************************************
;* Board Initialization Routine for Software Reset 
;****************************************************************************************
.brd_soft_init:	ret  	

        					
;**************************************************************************************** 
;* General Initialization Routine for items not covered by Soft/Hard Initialization 
;****************************************************************************************
.brd_init:	ret    
     

                          
;****************************************************************************************
;* Rabbitnet Command Functions  
;****************************************************************************************
;----------------------------------------------------------------------------------------
; Command:	c0r10   
; Description:  Configure DAC board   	
;----------------------------------------------------------------------------------------
c0r10: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	dac_config,a 
		ld	a,#$FF
		ld	dac_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r20   
; Description:  Write data to DAC channel 0     	
;----------------------------------------------------------------------------------------
c0r20:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	dac_convert,a   
                call	rn_xfer	
                ld	{dac0_data+1},a  
                call	rn_xfer	    
               	and	a,#$0F  
                or	a,dac0_cmd
                ld	{dac0_data+0},a               
		ld	a,#dac0_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac0_data.h
		ld	{ptrDACdata+0},a
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r21   
; Description:  Write data to DAC channel 1    	
;----------------------------------------------------------------------------------------	 
c0r21:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	dac_convert,a
                call	rn_xfer	
               	ld	{dac1_data+1},a  
                call	rn_xfer	 
               	and	a,#$0F  
                or	a,dac1_cmd
                ld	{dac1_data+0},a               
		ld	a,#dac1_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac1_data.h
		ld	{ptrDACdata+0},a
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r22   
; Description:  Write data to DAC channel 2     	
;----------------------------------------------------------------------------------------		
c0r22:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	dac_convert,a  
               	call	rn_xfer	  
                ld	{dac2_data+1},a
                call	rn_xfer	
               	and	a,#$0F  
                or	a,dac2_cmd
                ld	{dac2_data+0},a               
		ld	a,#dac2_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac2_data.h
		ld	{ptrDACdata+0},a                                                                     
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r23   
; Description:  Write data to DAC channel 3     	
;----------------------------------------------------------------------------------------		
c0r23:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	dac_convert,a
               	call	rn_xfer	
                ld	{dac3_data+1},a  
               	call	rn_xfer	 
               	and	a,#$0F   
 		or	a,dac3_cmd
                ld	{dac3_data+0},a               
		ld	a,#dac3_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac3_data.h
		ld	{ptrDACdata+0},a                              
 		jp	exitcmd			;  and go process it
        
;----------------------------------------------------------------------------------------
; Command:	c0r24   
; Description:  Write data to DAC channel 4     	
;----------------------------------------------------------------------------------------
c0r24:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY         
                ld	a,#$FF
                ld	dac_convert,a 
               	call	rn_xfer	
                ld	{dac4_data+1},a  
                call	rn_xfer	 
               	and	a,#$0F  
                or	a,dac4_cmd
                ld	{dac4_data+0},a                 
		ld	a,#dac4_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac4_data.h
		ld	{ptrDACdata+0},a
  		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r25   
; Description:  Write data to DAC channel 5      	
;----------------------------------------------------------------------------------------
c0r25:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	dac_convert,a 
               	call	rn_xfer	
                ld	{dac5_data+1},a  
                call	rn_xfer	         
              	and	a,#$0F  
                or	a,dac5_cmd
                ld	{dac5_data+0},a               
		ld	a,#dac5_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac5_data.h
		ld	{ptrDACdata+0},a
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r26   
; Description:  Write data to DAC channel 6     	
;----------------------------------------------------------------------------------------
c0r26:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	dac_convert,a
               	call	rn_xfer	
                ld	{dac6_data+1},a  
                call	rn_xfer	     
               	and	a,#$0F  
                or	a,dac6_cmd
                ld	{dac6_data+0},a               
		ld	a,#dac6_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac6_data.h
		ld	{ptrDACdata+0},a
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r27   
; Description:  Write data to DAC channel 7     	
;----------------------------------------------------------------------------------------
c0r27:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
	        ld	a,#$FF
                ld	dac_convert,a
                call	rn_xfer	
                ld	{dac7_data+1},a  
                call	rn_xfer	
               	and	a,#$0F  
                or	a,dac7_cmd
                ld	{dac7_data+0},a               
		ld	a,#dac7_data.l
		ld	{ptrDACdata+1},a
		ld	a,#dac7_data.h
		ld	{ptrDACdata+0},a
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r30   
; Description:  Command to write the calibration data to flash for a given channel, the 
;               master must send the DAC configuration command c0r10 before executing 
;		this command. 
;----------------------------------------------------------------------------------------
c0r30:		ld      a,rn_cmd                ; Load A w/Rabbitnet command received 
                and	a,#$07		       	; Mask all bits except bits 2-0
		ld	channel_save,a
		ld	a,#ptr_cal0.l         	; Load LSB of start Addr of Caldata array
		ld	{fptrCal+1},a
 				
		ld	a,#ptr_cal0.h         	; Load MSB of start Addr of Caldata array
		ld	{fptrCal+0},a
	        
 		ld	a,SPIDR			; Prepare to send previous byte
		call	rn_xfer			; Get 1st byte of key
		ld	flashkey1,a		; Save 1st key
		call	rn_xfer			; Get 2nd byte
 		ld	flashkey2,a		; Save 2nd key
		call	rn_xfer			; Get MSB of gain factor
		ld	{e2data+0},a		; Save in ram
		call	rn_xfer			; Get next byte of gain factor
		ld	{e2data+1},a		; Save in ram
		call	rn_xfer			; Get next byte of gain factor
		ld	{e2data+2},a		; Save in ram		
		call	rn_xfer			; Get LSB of gain factor
		ld	{e2data+3},a		; Save in ram		
		call	rn_xfer			; Get MSB of offset value
		ld	{e2data+4},a		; Save in ram		
		call	rn_xfer			; Get LSB of offset value
		ld	{e2data+5},a		; Save in ram
		ld	x,channel_save
		sll	x                        
		ld	a,([fptrCal.w],x)	
		ld	{flashaddr+0},a		; Save LOW byte of pointer
		inc	x
		ld	a,([fptrCal.w],x)	
		ld	{flashaddr+1},a 	; Save HIGH byte of address
 
		ld	a,#$06			; Set number of bytes for flash write
		ld	nbytes,a		; Save for flashwrite routine
		bset	pend,#FLASH		; Set flag to writeflash on exit
		jp	cmd_exit		; Must exit after flash write 


;----------------------------------------------------------------------------------------
; Command:	c1r30   
; Description:  Command to read the calibration data for a given channel, the master
;               must send the DAC configuration command c0r10 before executing this 
;		command.     	
;----------------------------------------------------------------------------------------
c1r30:	        ld	a,#ptr_cal0.l           ; Load LSB of start Addr of Rawdata array
		ld	{ptrCal+1},a				
		ld	a,#ptr_cal0.h           ; Load MSB of start Addr of Rawdata array
		ld	{ptrCal+0},a
                 
                ld      a,rn_cmd		; Load A with Rabbitnet CMD received 
                and	a,#$07			; Mask out all bits except bits 2-0
        	
		ld	x,a
        	sll	x
          
		ld	a,([ptrCal.w],x)
		ld	{ptrRdCal+0},a  
		inc	x       
		ld	a,([ptrCal.w],x)
		ld	{ptrRdCal+1},a
			
		clr	x        		; Set offet index to zero
		ld	a,([ptrRdCal.w],x)  	; Load Byte 3 of cal gain factor
		call	rn_xfer			; Send to master		          					
		inc	x
		         
		ld	a,([ptrRdCal.w],x)  	; Load byte 2 of cal gain factor       	
		call	rn_xfer			; Send to master		          					
		inc	x
                                   
                ld	a,([ptrRdCal.w],x)  	; Load byte 1 of cal gain factor                     
		call	rn_xfer			; Send to master		          					
		inc	x
                                
                ld	a,([ptrRdCal.w],x)  	; Load byte 0 of cal gain factor                   
		call	rn_xfer			; Send to master		          					
		inc	x
                
             	ld	a,([ptrRdCal.w],x)  	; Load byte 1 of cal offset value  
		call	rn_xfer			; Send to master		          					
		inc	x
                   
                ld	a,([ptrRdCal.w],x)  	; Load byte 0 of cal offset value 
		call	rn_xfer			; Send to master		          					

		jp	exitcmd			; Go wait for command to terminate
 
;----------------------------------------------------------------------------------------
; Command:	c0r40   
; Description:  Write data to the DAC chip, which will cause the board to go 
;               busy, until the command is finished.    	
;----------------------------------------------------------------------------------------		
c0r40: 		call	rn_xfer			; Wait for byte from master
		ld	dac_Data0,a		; Get data command from master 
		call	rn_xfer			; Wait for byte from master
		ld	dac_Data1,a		; Get data command from master	
		ld	a,#$FF                  ; Load A w/value to indicate DAC access
		ld	dac_access,a            ; Set flag to indicate DAC access
		bset	rn_stat,#BUSY           ; Set status to a Slave BUSY state
		bres	rn_stat,#READY          ; Clear Ready bit
		jp	exitcmd			; Go wait for command to terminate
		
;----------------------------------------------------------------------------------------
; Command:	c0r42   
; Description:  Strobe the DAC chip to update all DAC outputs.
;----------------------------------------------------------------------------------------
                                                       
c0r41: 		ld	a,#$FF			; Load A with flag indicator value
		ld	dac_strobe,a            ; Set flag for single-ch accesses 
		jp	exitcmd			; Go wait for command to terminate
                                                                        
		                                                                              
;****************************************************************************************
;  Timer function call by 25ms timer to be used to customize board
;  specific requirements. 
;****************************************************************************************
.interval_tmr:	tnz	dly_tmr			; Test delay timer for zero
		jreq	intvl_10		; Yes..Skip decrement
		dec	dly_tmr			; No..Decrement delay timer

intvl_10:	tnz	led_active              ; Check if LED state machine is activated
		jrne	intvl_20 		; Jump LED state machine
			
		tnz	act_tmr			; Test for Activity (non-zero)
                jreq	intvl_90		; Exit routine if no SPI activity
                 
 		bset 	led_tmr,#1        	; Load led timer with a count of 2		
		bset	led_active,#0				
		bset    led_toggle,#0           ; Set led toggle to non-zero
		bres	PCDR,#LED		; Yes.Turn Activity LED On
		jreq	intvl_90		; Exit with LED turned ON
				
intvl_20:	tnz	led_toggle              ; Check for LED ON vs. OFF state
		jreq	intvl_40		; Jump if flag indicates LED OFF state
		
		dec	led_tmr			; Decrement Activity Timer
		jrne	intvl_30	        ; Check if LED ON state has completed 
		bset 	led_tmr,#1		; Load led timer with a count of 2
		clr	led_toggle              ; Clear led toggle flag
		bset	PCDR,#LED		; Yes.Turn Activity LED Off
intvl_30:	ret				; Done
	
intvl_40:	dec	led_tmr			; Decrement Activity Timer
		jrne	intvl_90	        ; Check if LED OFF state has completed
		clr	led_active              ; Clear led toggle flag
		clr	act_tmr                 ; Clear SPI activity flag 
intvl_90:	ret	                        ; Done
		
											
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
		dc.w	cmd_rej			; c0r11
		dc.w	cmd_rej			; c0r12
		dc.w	cmd_rej			; c0r13
		dc.w	cmd_rej			; c0r14
		dc.w	cmd_rej			; c0r15
		dc.w	cmd_rej			; c0r16
		dc.w	cmd_rej			; c0r17
						;
		dc.w	c0r20			; c0r20
		dc.w	c0r21			; c0r21
		dc.w	c0r22			; c0r22
		dc.w	c0r23			; c0r23
		dc.w	c0r24			; c0r24
		dc.w	c0r25			; c0r25
		dc.w	c0r26			; c0r26
		dc.w	c0r27			; c0r27
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
		dc.w	c0r41			; c0r41
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
		dc.w	cmd_rej			; c1r10
		dc.w	cmd_rej			; c1r11
		dc.w	cmd_rej			; c1r12
		dc.w	cmd_rej			; c1r13
		dc.w	cmd_rej			; c1r14
		dc.w	cmd_rej			; c1r15
		dc.w	cmd_rej			; c1r16
		dc.w	cmd_rej			; c1r17
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
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
						;
		dc.w	cmd_rej			; c1r40
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
		

;****************************************************************************************
;  Function to ENABLE/DISABLE application CRC error checking 
;****************************************************************************************
WORDS                    
  	SEGMENT 'Protected'

.crcEnable: 	
 #ifdef CS_DISABLED                     
		ld	a,#$00			; Set Reg A to Disable CRC checking
 #else                                  
	 	ld	a,#$FF			; Set Reg A to Enable CRC checking
 #endif
		ret


;****************************************************************************************				
;     End of Source file
;****************************************************************************************		
	END  


