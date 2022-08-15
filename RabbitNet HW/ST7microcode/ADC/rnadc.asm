st7/  	; This is required for specifying the instruction set for the targeted processor         

;****************************************************************************************
;|										
;|		     Rabbitnet Board Specific Microcode 
;|			Copyright (c), Z-World 2003
;|			Copyright (c), Digi International 2022
;|										
;****************************************************************************************
;|  Program Name:	adc.asm	
;|  Date:   		5/9/03
;|  Revision History:	Rev 1.00 Initial Release
;****************************************************************************************
        TITLE    "rnadc.asm"	; This title will appear on each
                                ; page of the listing file
        MOTOROLA                ; This directive forces the Motorola 
                                ; format for the assembly (default)
                                                                                
;****************************************************************************************				
;     Header files to include MACRO's for Rabbitnet Projects 
;****************************************************************************************     	
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_101\st72264.inc"                                         	
        #INCLUDE "rnadc.inc"         		
        #INCLUDE "I:\projects\Rabbitnet_Common\Microcode\Rev_101\common.inc"

 ;#define ST7_Debugging    	
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
ad0_cmd 	ds.b   	1    	; Channel 0 command byte, used for A/D access
ad1_cmd 	ds.b   	1  	; Channel 1 command byte, used for A/D access
ad2_cmd 	ds.b   	1      	; Channel 2 command byte, used for A/D access
ad3_cmd 	ds.b   	1      	; Channel 3 command byte, used for A/D access
ad4_cmd 	ds.b   	1      	; Channel 4 command byte, used for A/D access
ad5_cmd 	ds.b   	1   	; Channel 5 command byte, used for A/D access
ad6_cmd 	ds.b   	1      	; Channel 6 command byte, used for A/D access
ad7_cmd 	ds.b   	1      	; Channel 7 command byte, used for A/D access

                   
mac_cmd         ds.b    1      	; Data variable used for ADC command data
mac_data        ds.b    1      	; Data variable used for ADC data values
adc_channel     ds.b    1      	; Data variable used for ADC channel information
adc_count	ds.b    1      	; Counter for controlling Round-Robin loop
adc_config	ds.b	1	; Configuration data byte for a given A/D channel
adc_command     ds.b	1       ; Command byte value for the A/D chip  

adc_convert	ds.b    1      	; Bit1 start conversion Single Ch mode
adc_initial	ds.b    1      	; Bit2 configure an A/D channel. all modes 
adc_access	ds.b	1      	; Bit3 access the A/D chip directly
adc_regMode	ds.b	1      	; Bit4 flag for ADC access R or W 
adc_index	ds.b	1	; Index for accessing rawdata and cal contants


ptrCMD		ds.b	1      	; Pointer for Round-Robin A/D command data
ptrCmdCH	ds.b	1      	; Pointer for Channel-Only A/D command data

ptrActive	ds.b	1       ; Pointer for Round-Robin A/D channel active flag
ptrActiveCH	ds.b	1       ; Pointer for Channel-Only A/D channel active flag

ptrData		ds.w	1       ; Pointer for Round-Robin A/D rawdata accesses
ptrDataCH	ds.w	1       ; Pointer for Channel-Only A/D rawdata accesses
  
                      
dly_tmr		ds.b 	1	; Delay Timer
led_tmr		ds.b	1	; Timer for controller LED OFF time    
led_toggle	ds.b	1	; LED control flay  
led_active	ds.b	1	; LED activity flag   
 
 
ADIOShadow	ds.b	1       ; Shadow reg for A/D input control, A/D IO port 3-0
PBDRShadow	ds.b	1	; Shadow reg for A/D input control, Port B pins 3-0
 
adc_regCmd	ds.b	1      	; Command for ADC via RN cmd c0R40 or c1R40
adc_regData	ds.b	1	; Data value to write to ADC via RN cmd c0R40 or c1R40
adc_WrData	ds.b	1      	; Write Data for the ADC via RN cmd c0R40 or c1R40  
adc_RdData	ds.b	1      	; Read Data for the ADC via RN cmd c0R40 or c1R40
adc_Tmode	ds.b	1	; Variable used to configure A/D input transistors	
 

ptrRdCal	ds.w	1       ; Pointer used for indirect access to cal data  
ptrWrCal	ds.w	1       ; Pointer used for indirect access to cal data
ptrTemp 	ds.w	1       ; Temp pointer, used by microcode 
ptrCal		ds.w	1	; Pointer used for indirect access into cal pointers   
channel_save	ds.b	1
adcstatus	ds.b	1   
                                
;****************************************************************************************				
;     Application Ram segment, all accesses to this area uses the long addressing mode  
;****************************************************************************************   
 WORDS
	SEGMENT 'iap_&_stack_ram'     
                                   
ad0_gain       	ds.b   	4       ; Gain calibration data for channel 0    
ad0_offset     	ds.b   	2       ; Offset calibration data for channel 0  

ad0_data 	ds.b   	2 	; A/D 12 bit rawdata value for channel 0  
ad1_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 1
ad2_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 2
ad3_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 3
ad4_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 4
ad5_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 5
ad6_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 6
ad7_data 	ds.b   	2       ; A/D 12 bit rawdata value for channel 7

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
; SE Gain 0
	dc.l    $E11A253C
	dc.w    $0600     	
	dc.l    $954F253C
	dc.w    $0600
	dc.l    $3735253C
	dc.w    $0600	
	dc.l    $E19E253C
	dc.w    $0600	
	dc.l    $3735253C
	dc.w    $0600	
	dc.l    $13CC243C
	dc.w    $0500	
	dc.l    $E11A253C
	dc.w    $0600	
	dc.l    $3735253C
	dc.w    $0600

; SE Gain 1      
      	dc.l    $8DEFA43B
	dc.w    $0B00    	
	dc.l    $9D3FA53B
	dc.w    $0C00
	dc.l    $330AA53B
	dc.w    $0B00	
	dc.l    $FD8FA53B
	dc.w    $0B00	
	dc.l    $E524A53B
	dc.w    $0C00	
	dc.l    $EBD4A43B
	dc.w    $0B00	
	dc.l    $330AA53B
	dc.w    $0C00	
	dc.l    $E524A53B
	dc.w    $0C00        
	
; SE Gain 2 
  	dc.l    $D7EE243B
	dc.w    $1700    	
	dc.l    $0F24253B
	dc.w    $1700
	dc.l    $D7EE243B
	dc.w    $1500	
	dc.l    $1F74253B
	dc.w    $1700	
	dc.l    $6D09253B
	dc.w    $1700	
	dc.l    $479F243B
	dc.w    $1600	
	dc.l    $D7EE243B
	dc.w    $1600	
	dc.l    $0F24253B
	dc.w    $1700

; SE Gain 3    
	dc.l    $47FA033B
	dc.w    $1D00    	
	dc.l    $113A043B
	dc.w    $1C00
	dc.l    $830F043B
	dc.w    $1B00	
	dc.l    $197A043B
	dc.w    $1D00	
	dc.l    $C724043B
	dc.w    $1C00	
	dc.l    $E3CF033B
	dc.w    $1C00	
	dc.l    $47FA033B
	dc.w    $1C00	
	dc.l    $C724043B
	dc.w	$1C00

; SE Gain 4               
	dc.l    $DBFAA43A
	dc.w    $2D00    	
	dc.l    $FB65A53A
	dc.w    $2D00
	dc.l    $294BA53A
	dc.w    $2C00	
	dc.l    $DB80A53A
	dc.w    $2C00	
	dc.l    $5B30A53A
	dc.w    $2C00	
	dc.l    $29E0A43A
	dc.w    $2B00	
	dc.l    $9715A53A
	dc.w    $2B00	
	dc.l    $5B30A53A
	dc.w    $2C00
                 
; SE Gain 5  
	dc.l    $E313843A
	dc.w    $3A00     	
	dc.l    $DD54843A
	dc.w    $3900
	dc.l    $8529843A
	dc.w    $3600	
	dc.l    $5580843A
	dc.w    $3800	
	dc.l    $8529843A
	dc.w    $3700	
	dc.l    $B5E8833A
	dc.w    $3700	
	dc.l    $E313843A
	dc.w    $3700	
	dc.l    $8529843A
	dc.w    $3700

; SE Gain 6               
 	dc.l    $1310273A
	dc.w    $5E00    	
	dc.l    $81DF263A
	dc.w    $5C00
	dc.l    $AF7E263A
	dc.w    $5800	
	dc.l    $41C7263A
	dc.w    $5B00	
	dc.l    $09AF263A
	dc.w    $5A00	
	dc.l    $09AF263A
	dc.w    $5A00	
	dc.l    $09AF263A
	dc.w    $5A00	
	dc.l    $41C7263A
	dc.w    $5B00
     
; SE Gain 7 

	dc.l    $8D16043A
	dc.w    $7300     	
	dc.l    $276E043A
	dc.w    $7100
	dc.l    $3558043A
	dc.w    $6D00	
	dc.l    $1F9A043A
	dc.w    $7000	
	dc.l    $4B42043A
	dc.w    $6E00	
	dc.l    $8D16043A
	dc.w    $6E00	
	dc.l    $B700043A
	dc.w    $6D00	
	dc.l    $4B42043A
	dc.w    $6F00
    

; DIFF Gain 0               
 	dc.l    $45AA253C
	dc.w    $0100     	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $5DE9253C
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $3D7A253C
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $A9B5253C
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
		
; DIFF Gain 1               
 	dc.l    $4D68A53B
	dc.w    $0100     	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $C999A53B
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $7D39A53B
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $6767A53B
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF  
	
; DIFF Gain 2               
 	dc.l    $115B253B
	dc.w    $0000    	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $4DAE253B
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $D172253B
	dc.w    $0100
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $DF4B253B
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	                     
; DIFF Gain 3               
 	dc.l    $2B4A043B
	dc.w    $0000   	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $0582043B
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $8936043B
	dc.w    $0100
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $1D3D043B
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	                     
; DIFF Gain 4               
 	dc.l    $8B8CA53A
	dc.w    $0000   	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $C5A2A53A
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $B135A53A
	dc.w    $0000
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $C56CA53A
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF

; DIFF Gain 5               
 	dc.l    $AD5B843A
	dc.w    $0100   	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $C76B843A
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $154A843A
	dc.w    $0100
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $F145843A
	dc.w    $0100	
	dc.l    $FFFFFFFF
	dc.w    $FFFF

; DIFF Gain 6               
 	dc.l    $8968253A
	dc.w    $0000   	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $8996253A
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $D138253A 
	dc.w    $0000
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $F152253A
	dc.w    $0000	
	dc.l    $FFFFFFFF
	dc.w    $FFFF

; DIFF Gain 7               
 	dc.l    $7579043A
	dc.w    $0000  	
	dc.l    $FFFFFFFF
	dc.w    $FFFF
	dc.l    $61CE043A
	dc.w    $0200	
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $9F3C043A
	dc.w    $0000
	dc.l    $FFFFFFFF
	dc.w    $FFFF	
	dc.l    $093C043A
	dc.w    $FEFF	
	dc.l    $FFFFFFFF
	dc.w    $FFFF


; 4-20 ma               
 	dc.l    $13084F3C
	dc.w    $2F00  	
	dc.l    $257B4F3C
	dc.w    $2E00
	dc.l    $5F364E3C
	dc.w    $2B00	
	dc.l    $DF684F3C
	dc.w    $2E00	
 	dc.l    $13084F3C
	dc.w    $2F00  	
	dc.l    $257B4F3C
	dc.w    $2E00
	dc.l    $5F364E3C
	dc.w    $2B00	
	dc.l    $DF684F3C
	dc.w    $2E00	
	    
          												
;****************************************************************************************				
;|	A/D chip communication control to/from the ST7 micro-controller		
;|      ---------------------------------------------------------------		   
;|      1. PA4 - PA7 = 4 parallel bits to serial data path                      
;|      2. PC0 = Read 4 bits of data                                            
;|      3. PC1 = Write 4 bits of data                                           
;|      4. PC2 = CLK the A/D and PAL serial shift register                      
;|      5. PC3 = A/D Reset                                                      
;|      6. PC4 = A/D Chip Select                                                    
;|	                                                                        
;|	A/D input signal control single-ended vs. bipolar                       
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
;MACRO:		ADCLK
;DESCRIPTION:	Provides 4 CLK pulses to clock the A/D and PLD chip. Use this 
;              	clocking method when using the ST7 debugging tools.
;PARAMETERS:	None
;RETURN VALUES: None
;****************************************************************************************
ADCLK   	MACRO            
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
;MACRO:		ADCLK
;DESCRIPTION:	Provides a burst of 4 8 Mhz CLK pulses to clock the A/D and PLD 
;               chip. This clocking method doesn't work when using the ST7 
;		debugging tools.
;PARAMETERS:	None 
;RETURN VALUES: Reg X contains bits 12-8 of A/D rawdata.
;               Reg A contains bits  7-0 of A/D rawdata.  
;****************************************************************************************
AD_RD12		MACRO   cmd  
		LOCAL	AD_busy
		LOCAL   AD_error
		bres    PCDR,#4         	; Assert /ADC_CS signal 
		ld	x,#$FC          	; Set Port A as output lines
		ld      PADDR,x		            		
		ld      y,cmd       		; Put data on port A for latch
		ld  	PADR,y
		bres    PCDR,#1			; Strobe register  
		bset    PCDR,#1 
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		ADCLK     
		bset    PCDR,#0			; Unassert /OE signal      
		ld      PADDR,x			; Set Port A as output lines               
		swap    y
		ld      PADR,y
		bres    PCDR,#1			; Strobe data into latch   
		bset    PCDR,#1                  
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal                          
     		ADCLK   
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	    
		
		ld	a,#$30          	; Setup timeout if ADC not working
AD_busy:  	dec     a   			; Check if conversion is done  
		cp	a,#0            	; Check for timeout 
		jreq	AD_error        	; ADC not working so exit                         
     		btjt	PCDR,#5,AD_busy 	; Check if conversion still in progress

		ADCLK      			; CLK bits 12-8 of ADC data into PLD reg 
		ld	a,PADR          	; Read 4 bits of ADC data, bits 12-8
		and     a,#$F0         	 	; MASK out lower Port A, bits 3-0 
		swap    a               	; SWAP nibbles, data is bits 12-8
		ld	x,a             	; Save in register X, ADC data bits 12-8
       	
      		ADCLK                   	; CLK bits 7-4 of ADC data into PLD reg
		ld	a,PADR          	; Read 4 bit of ADC data, bits 7-4
		and     a,#$F0          	; MASK out lower Port A bits 3-0 
		ld      temp,a      		; Save in temp register  
	
		ADCLK                   	; CLK bits 3-0 of ADC data into PLD reg
		ld	a,PADR          	; Read 4 bit of ADC data, bits 3-0
		and     a,#$F0          	; MASK out lower Port A bits 3-0   
		swap 	a	        	; Swap nibbles
		or	a,temp     		; OR data bits 3-0 with previous bits 7-4
AD_error:
       		bset    PCDR,#0  		; Unassert /OE signal   
		bset    PCDR,#4         	; Unassert /ADC_CS signal  
		MEND 
 
;****************************************************************************************
;MACRO:		AD_RD8
;DESCRIPTION:	Reads a byte from a ADC cmd register specified. 
;PARAMETER1:	Command byte for the ADC chip. 
;RETURN VALUES: Data byte from the A/D cmd register specified.    
;****************************************************************************************
AD_RD8  	MACRO   cmd
		bres    PCDR,#4         	; Assert /ADC_CS signal 
		ld	x,#$FC          	; Set Port A as output lines
		ld      PADDR,x		            		
		ld      y,cmd       		; Put data on port A for latch
		ld  	PADR,y
		bres    PCDR,#1			; Strobe register  
		bset    PCDR,#1 
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		ADCLK     
		bset    PCDR,#0			; Unassert /OE signal      
		ld      PADDR,x			; Set Port A as output lines               
		swap    y
		ld      PADR,y
		bres    PCDR,#1			; Strobe data into latch   
		bset    PCDR,#1                  
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal                          
     		ADCLK	  
	   
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		ADCLK                   	; CLK bits 7-4 of ADC data into PLD reg
		ld	a,PADR          	; Read ADC data bits 7-4 from PLD 
		and     a,#$F0          	; MASK out lower Port A bits 3-0   
		ld	temp,a      		; Save in temp register

      		ADCLK                   	; CLK bits 3-0 of ADC data into PLD reg
		ld	a,PADR          	; Read ADC data bits 3-0 from PLD
		and     a,#$F0          	; MASK out lower Port A bits 3-0   
		swap    a               	; Swap nibbles
		or	a,temp     		; OR data bits 3-0 with previous bits 7-4 
                ;------------------------------------------------------------------------
		; Register A contains ADC data bits  7-0 
		;------------------------------------------------------------------------
       		bset    PCDR,#0  		; Unassert /OE signal   
		bset    PCDR,#4         	; Unassert /ADC_CS signal  
		MEND

;****************************************************************************************
;MACRO:		AD_WR8
;DESCRIPTION:	Writes a byte to the ADC chip to the cmd register specified.
;PARAMETER1:	Command byte for the ADC chip.
;PARAMETER2:    Data to be written to the A/D chip cmd register 
;RETURN VALUES: None  
;****************************************************************************************                    
AD_WR8  	MACRO	cmd, data 
		bres    PCDR,#4         	; Assert /ADC_CS signal 
		ld	x,#$FC          	; Set Port A as output lines
		ld      PADDR,x		            		
		ld      y,cmd       		; Put data on port A for latch
		ld  	PADR,y
		bres    PCDR,#1			; Strobe register  
		bset    PCDR,#1 
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		ADCLK     
		bset    PCDR,#0			; Unassert /OE signal      
		ld      PADDR,x			; Set Port A as output lines               
		swap    y
		ld      PADR,y
		bres    PCDR,#1			; Strobe data into latch   
		bset    PCDR,#1                  
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal                          
     		ADCLK	  
  
	       	bset    PCDR,#0  		; Unassert /OE signal     
		ld      PADDR,x			; Set Port A as output lines            		
		ld      y,data          	; Put data on port A for latch
		ld  	PADR,y
		bres    PCDR,#1			; Strobe register  
		bset    PCDR,#1 
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal      	
		ADCLK     
		bset    PCDR,#0			; Unassert /OE signal      
		ld      PADDR,x			; Set Port A as output lines               
		swap    y
		ld      PADR,y
	       	bres    PCDR,#1			; Strobe data into latch   
		bset    PCDR,#1	                  
		clr	PADDR			; Set Port A as input lines
		bres    PCDR,#0         	; Assert /OE signal                          
       	 	ADCLK   
        	bset    PCDR,#0  		; Unassert /OE signal   
		bset    PCDR,#4         	; Unassert /ADC_CS signal  
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
;DESCRIPTION:	Backgound task for handling A/D conversions and Rabbit commands  
;PARAMETERS:	None 
;RETURN VALUES: None
;**************************************************************************************** 
.main: 		bres	WDGCR,#7		; Disable WDT      			   
		bset	PCDDR,#LED    
		bset    PCOR,#LED    
		bset    PCDR,#LED 
		rim				; Enable all interrupts

main10:         btjf	cmd07_stat,#HWDT,main30 ; Check for HWDT enabled
		ld	a,#HWDT_TIMEOUT		; Yes.. Enable and/or reload
		ld	WDGCR,a			;  with max count	        
main30:	        btjt	adc_convert,#0,main40   ; Round-robin vs. single ch mode? 
	        btjt    adc_initial,#0,main70   ; A/D ch configuration command?
		btjt	adc_access,#0,main80    ; Direct A/D read/write access?	        	        	 
		jra	main10
main40:		call    SingleCHAN		; Do Single-Channel conversion   
		clr	adc_convert		; Clear conversion flag
		call    RN_Ready                ; Set Rabbitnet status back to READY
	        jra	main10                  ; Jump back to Round-Robin code 
	        	        
main70:	        call    ConfigCHAN              ; Go configure A/D channel
		clr	adc_initial		; Clear configure A/D flag
		call    RN_Ready                ; Set Rabbitnet status back to READY
	        jp	main30                  ; Jump to Round-Robin code 

main80:		call	ADC_Access              ; Access the ADC directly
		clr	adc_access              ; Clear the ADC direct access flag
		call    RN_Ready                ; Set Rabbitnet status back to READY 
                jp	main30                  ; Jump to Round-Robin code
        
			
;****************************************************************************************
;ROUTINE:	ConfigCHAN
;DESCRIPTION:	Routine to configure an A/D channel with command and Calibration Data  
;PARAMETERS:	None 
;RETURN VALUES: None
;****************************************************************************************
ConfigCHAN:     ld      a,#ptr_cal0.l           ; Load LSB w/start Addr of Cal data array
                ld	{ptrCal+1},a  
                ld	a,#ptr_cal0.h           ; Load MSB w/start Addr of Cal data array
                ld	{ptrCal+0},a 
                 
		ld	x,adc_channel   	; Load Reg with channel to be configured  
		ld	adc_command,x		
		ld	a,adc_config		; Load Reg A with A/D command, mode/Gain
		and     a,#$07          	; Mask out bits except gain information
		swap	a               	; Move gain infor to upper nibble
		or	a,adc_command   	; OR'in A/D channel information
		ld	adc_command,a   	; Save-off Gain/Channel information
                
		ld	a,adc_config    	; Load config information again
		and	a,#$18
		cp	a,#$00          	; Check for single-ended A/D mode  
	       	jrne    config20  	 
	      	call	SE_Config               ; Configure A/D channel for SE operation
	      	jra 	config40        	; Continue with A/D configuration

config20:	cp	a,#$08          	; Check for DIFF A/D mode 
		jrne    config30    
		call	DI_Config 		; Configure A/D ch for DIFF operation
 	       	jra 	config40        	; Continue with A/D configuration

config30:       call	MA_Config		; Configure A/D ch for 4-20 ma operation

		       		       	                      	
config40: 	ld	a,adc_channel		; Load A with A/D ch to be configured      		
		and	a,#07           	; Mask out all bits except channel value 
	        ld	x,#6            	; Load X w/num of bytes for Gain/Offset
	        mul	x,a             	; Calculate ptr offset into data array
		clr	{ptrTemp+0}
                ld	{ptrTemp+1},a       	; load into temp for further calculations

              	ADDWORD ptrRdCal,ptrTemp	; Add ptr offset to base address
                ld	{ptrRdCal+0},a    	; Load ptr w/calc. location of cal data  
                ld	{ptrRdCal+1},x                             
		ld	temp,x
		ld	x,adc_channel	        ; Load index w/channel value
		sll	x                       ; Ajust index for 2 bytes per entry
                ld	([ptrCal.w],x),a        ; Save flash location of CAL data 
                inc	x  
		ld	a,temp
                ld	([ptrCal.w],x),a

                ld	a,#ad0_gain.l           ; Calc ptr of calib data for given ch
                ld	{ptrWrCal+1},a
                ld	a,#ad0_gain.h           
                ld	{ptrWrCal+0},a                   
                ld	a,adc_channel		; Calc offset, channel * num_cal bytes    	 	
                and	a,#07           	; Get channel value 
                ld	x,a 
                ld	a,#$6                   ; Number of Calibration bytes, equals 6
                mul	x,a                                                 
      		clr	{ptrTemp+0}
                ld	{ptrTemp+1},a       	
             	ADDWORD ptrWrCal,ptrTemp	; Add offset to base address
                ld	{ptrWrCal+0},a    	; Load ptr w/ram location of cal data  
                ld	{ptrWrCal+1},x                             
                                                                  
config60:       ret                     	; Return back to calling routine

		; Single-Ended Calibration Table for 8 A/D Channels
SE_TABLE:     	dc.w	segain0		               
		dc.w	segain1         
		dc.w	segain2		
		dc.w	segain3		         
		dc.w	segain4                       
		dc.w	segain5		                
		dc.w	segain6           
		dc.w	segain7          
                
		; Differential Calibration Table for 4 A/D Channels:	
DI_TABLE:	dc.w	digain0		 
		dc.w	digain1		
		dc.w	digain2				
		dc.w	digain3
		dc.w	digain4		 
		dc.w	digain5		
		dc.w	digain6				
		dc.w	digain7		
               
		; 4-20ma Calibration Table for 8 Channels
MA_TABLE:	dc.w	ma_cal	 


;----------------------------------------------------------------------------------------
; Segment:	SE_Config
; Subroutine: 	Support Routine   
; Description:  Routine to configure the A/D channels as bipolar inputs via 
;		transistors switches and set channel control structure for
;		single-ended mode of operation. 	
;----------------------------------------------------------------------------------------
SE_Config:	ld	a,adc_config    	; Load config information again
		and	a,#$07
		ld	x,a          		; Check for single-ended A/D mode  
 	        sll	x

		ld	a,(SE_TABLE,x) 	 	; Load base addr of SE cal table
	       	ld	{ptrRdCal+0},a    	; Load MSB of address
	       	inc	x  
	        ld	a,(SE_TABLE,x)  	; Load LSB of address
	       	ld	{ptrRdCal+1},a
                
                bset	adc_command,#3  	; Set bit3 to meet ADC cmd requirement
		bset	adc_command,#7  	; Set bit7 for A/D direct mode access
               
	       	ld	a,#ad0_cmd	        ; Load start Addr of command byte array 
		ld	ptrCMD,a    
		
                ld	a,adc_channel	        ; Load index into A/D command array
                ld	x,a
		ld	a,adc_command   	; Load A with A/D command byte
	        ld	([ptrCMD],x),a 		; Copy CMD to given channel variable
	 	       	
	       	ld	a,#SE_MODE              ; Set flag to indicate single-ended mode
	       	ld	adc_Tmode,a
	       	ld	a,adc_channel           ; Determine which ch is being configured
		and	a,#$06				
		cp	a,#$00                  ; Check if channel is 0 or 1
		jrne	SE_Config10             
		call	AD_CFG0                 ; Configure A/D channel input 0 and 1 
            	jra	SE_Done
		                                
SE_Config10:    cp	a,#$02                  ; Check if channel is 2 or 3 
		jrne	SE_Config20
                call	AD_CFG2                 ; Configure A/D channel input 2 and 3
               	jra	SE_Done

SE_Config20:	cp	a,#$04                  ; Check if channel range is 4 or 5
		jrne	SE_Config30
		call	AD_CFG4                 ; Configure A/D channel inputs 4 and 5
               	jra	SE_Done				                                                                                                        

SE_Config30:    call	AD_CFG6                 ; Configure A/D channel inputs 6 and 7
SE_Done:    	ret

;----------------------------------------------------------------------------------------
; Segment:	DI_Config
; Subroutine: 	Support Routine   
; Description:  Routine to configure the A/D channels as bipolar inputs via 
;		transistors switches and set channel control structure for
;		DIFF mode of operation. 	
;----------------------------------------------------------------------------------------
DI_Config:      ld	a,adc_config    	; Load config information again
		and	a,#$07
		ld	x,a          		; Check for single-ended A/D mode  
 	        sll	x

		ld	a,(DI_TABLE,x)  	; Load base addr of DIFF cal table
	       	ld	{ptrRdCal+0},a    	; Load MSB of address
	       	inc	x 
	       	ld	a,(DI_TABLE,x)  	; Load LSB of Address
	       	ld	{ptrRdCal+1},a  

	       	bset	adc_command,#7  	; Set bit7 for A/D direct mode access

	       	ld	a,#ad0_cmd	        ; Load start Addr of command byte array 
		ld	ptrCMD,a    
	        
	        ld	a,adc_command           ; Get A/D command byte
	        and	a,#$06                  ; Get DIFF mode channel pairs value 
	        ld	x,a                     
	        srl	x                       ; Convert ch format from 0-7 to 0-3  

	        ld	a,adc_command           ; Get copy of command value 
	        and	a,#$F8                  ; All other CMD infor stays the same
	        ld	adc_command,a 
	        ld	a,x
	        or	a,adc_command           ; Reg A now has converted ch infor
 	        ld	adc_command,a
 	        
	        ld	a,adc_channel           ; Load index into A/D command array
                and	a,#$06                  ; Get channel A/D pair infor 
                ld	x,a			; Save as index value

	        ld	a,adc_command
	        ld	([ptrCMD],x),a 		; Copy A to the given ch cmd variable
	       	inc	x
	       	ld	([ptrCMD],x),a 		; Copy A to next ch cmd variable
	       		       	
	       	ld	a,#DI_MODE              ; Load flag to indicate DIFF mode
	       	ld	adc_Tmode,a

	       	ld	a,adc_channel           ; Determine channel to be configured
		and	a,#$06				
		cp	a,#$00      		; Check if channel range is 0 or 1
		jrne	DI_Config10 
		call	AD_CFG0                 ; Configure A/D channel inputs 0 and 1
            	jra	DI_Done
		
DI_Config10:    cp	a,#$02     		; Check if channel range is 2 or 3
		jrne	DI_Config20
                call	AD_CFG2                 ; Configure A/D channel inputs 2 and 3
               	jra	DI_Done

DI_Config20:	cp	a,#$04         		; Check if channel range is 4 or 5
		jrne	DI_Config30
		call	AD_CFG4                 ; Configure A/D channel inputs 4 and 5
               	jra	DI_Done				                                                                                                        

DI_Config30:    call	AD_CFG6            	; Configure A/D channel inputs 6 and 7
DI_Done:    	ret

;----------------------------------------------------------------------------------------
; Segment:	MA_Config
; Subroutine: 	Support Routine   
; Description:  Routine to configure the A/D channels as unipolar inputs via 
;		transistors switches and set channel control structure for
;		4-20 ma mode of operation. 	
;----------------------------------------------------------------------------------------
MA_Config:	ld	x,#$00
		ld    	a,(MA_TABLE,x)  	; Load base addr of MA cal table
	       	ld    	{ptrRdCal+0},a    	; Load MSB of address
	       	inc   	x    
	       	ld	a,(MA_TABLE,x)  	; Load LSB of address
	       	ld	{ptrRdCal+1},a  
                
                bset  	adc_command,#3  	; Set bit3 to meet ADC cmd requirement 
	        bset	adc_command,#7  	; Set bit7 for A/D direct mode access

	       	ld	a,#ad0_cmd	        ; Load start Addr of command byte array 
		ld	ptrCMD,a
  
         
                ld	a,adc_channel           ; Load index into A/D command array
                and	a,#$07
                ld	x,a
		ld	a,adc_command   	; Load A with A/D command byte
	        ld	([ptrCMD],x),a 		; Copy A to the given ch cmd variable

	        
	        ld	a,#MA_MODE  		; Load flag to indicate 4-20ma mode 
	       	ld	adc_Tmode,a
	       	ld	a,adc_channel           ; Determine channel to be configured
		and	a,#$06				
		cp	a,#$00 			; Check if channel range is 0 or 1  
		jrne	MA_Config10 
		call	AD_CFG0 		; Configure A/D channel inputs 0 and 1
            	jra	MA_Done
		
MA_Config10:    cp	a,#$02                  ; Check if channel range is 2 or 3
		jrne	MA_Config20
                call	AD_CFG2       		; Configure A/D channel inputs 2 and 3
               	jra	MA_Done

MA_Config20:	cp	a,#$04                  ; Check if channel range is 4 or 5
		jrne	MA_Config30
		call	AD_CFG4        		; Configure A/D channel inputs 4 and 5
               	jra	MA_Done				                                                                                                        

MA_Config30:    call	AD_CFG6 		; Configure A/D channel inputs 6 and 7
MA_Done:    	ret

;----------------------------------------------------------------------------------------
; Segment:	AD_CFG0
; Subroutine: 	Support Routine   
; Description:  Configures mode of operation (DIFF, SE) for A/D channels 0 & 1.  	
;----------------------------------------------------------------------------------------
AD_CFG0:       	ld	a,ADIOShadow 		; Get current AIO port settings
		and     a,#$FC                	; Set value for ch 0&1 transistors OFF  		                
		ld      mac_data,a              ; Save value for I/O access via A/D chip 
		ld	ADIOShadow,a            ; Update shadow register
                ld      a,#$05     		; ADC reg CMD for accessing AIO bits 0-3
		ld      mac_cmd,a               ; Set ADC CMD for I/O port access
		AD_WR8	mac_cmd,mac_data      	; Force transistors for ch 0&1 to be OFF             

                ld	a,adc_Tmode             ; Get A/D mode flag
                cp	a,#DI_MODE              ; Check for biplor mode
                jrne	AD_CFG0a                ; Jump if unipolar mode of operation
                jp      AD_CFG0b                ; Go set ch 0&1 to bipolar inputs
                
AD_CFG0a:       bset	ADIOShadow,#1  		; Set value, ch0&1 to be unipolar inputs
                ld	a,ADIOShadow	        ; Update shadow register
                ld      mac_data,a       		  
                ld      a,#$05     		; Set ADC CMD for I/O port access
		ld      mac_cmd,a               
		AD_WR8	mac_cmd,mac_data        ; Config ch 0&1 as unipolar inputs
                jp	AD_CFG0c                ; Exit

AD_CFG0b:      	bset	ADIOShadow,#0           ; Set value, ch 0&1 to be bipolar inputs
                ld	a,ADIOShadow	        ; Update shadow register
                ld      mac_data,a       		  
                ld      a,#$05     		; Set ADC CMD for I/O port access
		ld      mac_cmd,a    
		AD_WR8	mac_cmd,mac_data       	; Config ch 0&1 as biplor inputs 
AD_CFG0c:	ret                             ; Exit


;----------------------------------------------------------------------------------------
; Segment:	AD_CFG2
; Subroutine: 	Support Routine   
; Description:  Configures mode of operation (DIFF, SE) for A/D channels 2 & 3.  	
;----------------------------------------------------------------------------------------
AD_CFG2:        ld	a,ADIOShadow            ; Get current AIO port settings
                and	a,#$F3                  ; Set value for ch 2&3 transistors OFF  
		ld      mac_data,a       	; Save value for I/O access via A/D chip	              
		ld	ADIOShadow,a            ; Update shadow register
                ld      a,#$05     		; ADC reg CMD for accessing AIO bits 0-3
		ld      mac_cmd,a               ; Set ADC CMD for I/O port access
		AD_WR8	mac_cmd,mac_data        ; Force transistors for ch 2&3 to be OFF
		
               	ld	a,adc_Tmode             ; Get A/D mode flag
                cp	a,#DI_MODE              ; Check for biplor mode
                jrne	AD_CFG2a                ; Jump if unipolar mode of operation
                jp	AD_CFG2b                ; Go set ch 2&3 to bipolar inputs

AD_CFG2a:       bset	ADIOShadow,#3           ; Set value, ch2&3 to be unipolar inputs
                ld	a,ADIOShadow	        ; Update shadow register
                ld      mac_data,a       		  
                ld      a,#$05     		; Set ADC CMD for I/O port access
		ld      mac_cmd,a    
		AD_WR8	mac_cmd,mac_data        ; Config ch 0&1 as unipolar inputs
                jp	AD_CFG2c                ; Exit
                
AD_CFG2b:      	bset	ADIOShadow,#2           ; Set value, ch 0&1 to be bipolar inputs
                ld	a,ADIOShadow	        ; Update shadow register
                ld      mac_data,a       		  
                ld      a,#$05     		; Set ADC CMD for I/O port access
		ld      mac_cmd,a    
		AD_WR8	mac_cmd,mac_data        ; Config ch 0&1 as biplor inputs 
AD_CFG2c:	ret                             ; Exit


;----------------------------------------------------------------------------------------
; Segment:	AD_CFG4
; Subroutine: 	Support Routine   
; Description:  Configures mode of operation (DIFF, SE) for A/D channels 4 & 5.  	
;----------------------------------------------------------------------------------------
AD_CFG4:	ld	a,PBDRShadow            ; Get current I/O port setting 
               	and	a,#$FC                  ; Set value for ch 4&5 transistors OFF
                ld      PBDR,a                  ; Force transistors for ch 4&5 to be OFF
                ld	PBDRShadow,a            ; Update shadow register
		ld	a,adc_Tmode             ; Get A/D mode flag
                cp	a,#DI_MODE              ; Check for biplor mode
                jreq	AD_CFG4a                ; Jump if biplor mode of operation

                bset	PBDR,#1                 ; Set value, ch4&5 to be unipolar inputs
                bset	PBDRShadow,#1           ; Update shadow register
                jra	AD_CFG4b                ; Exit

AD_CFG4a:       bset	PBDR,#0            	; Config ch 4&5 as biplor inputs
                bset	PBDRShadow,#0           ; Update shadow register       
AD_CFG4b:       ret                             ; Exit
                             
;----------------------------------------------------------------------------------------
; Segment:	AD_CFG6
; Subroutine: 	Support Routine   
; Description:  Configures mode of operation (DIFF, SE) for A/D channels 6 & 7.  	
;----------------------------------------------------------------------------------------
AD_CFG6: 	ld	a,PBDRShadow            ; Get current I/O port setting
                and     a,#$F3 			; Set value for ch 4&5 transistors OFF		        
 		ld      PBDR,a                  ; Force transistors for ch 6&7 to be OFF
		ld	PBDRShadow,a            ; Update shadow register      
                ld	a,adc_Tmode             ; Get A/D mode flag
                cp	a,#DI_MODE              ; Check for biplor mode
                jreq	AD_CFG6a                ; Jump if biplor mode of operation

                bset	PBDR,#3                 ; Set value, ch6&7 to be unipolar inputs
                bset	PBDRShadow,#3 		; Update shadow register
                jra	AD_CFG6b                ; Exit

AD_CFG6a:       bset	PBDR,#2                 ; Config ch 6&7 as biplor inputs
                bset	PBDRShadow,#2		; Update shadow register                  	                                            		                            
AD_CFG6b:       ret                             ; Exit

                              
;****************************************************************************************
;ROUTINE:	SingleCHAN
;DESCRIPTION:	Routine setup to do coversions on a single channel under control from 
;               the Master.  
;PARAMETERS:	None
;RETURN VALUES: None
;****************************************************************************************
SingleCHAN: 	; load Reg A with CMD for channel to be converted 
		ld     	a,[ptrCmdCH]    	; Load Reg with A/D command for given ch
		ld      mac_cmd,a       	; Load MACRO parameter with A/D command
		AD_RD12 mac_cmd        	 	; Read A/D channel
		ld	y,#1            	; Load Y with index value  
		sim                    		; Make updating A/D rawdata atomic
		ld	[ptrDataCH.w],x		; Load var w/12 bit A/D rawdata value 
		ld	([ptrDataCH.w],y),a  					
		rim                 		; Re-Enable interrupts
		ret		                                   
                                                                                         
;****************************************************************************************
;ROUTINE:	ADC_Access
;DESCRIPTION:	Routine to Read/Write the ADC chip directly.  
;PARAMETERS:	None
;RETURN VALUES: None
;****************************************************************************************                                                                                         
ADC_Access:	; Write ADC chip via command register
		ld	a,adc_regMode		; Check if Write or Read operation?
		cp	a,#0    	
		jrne 	Access10        	; Jump to Read A/D chip register	
		jp	Access40
Access10:	; Write A/D chip with data byte 
		AD_WR8  adc_regCmd, adc_regData ; Write data to ADC register		     
		jra	Access80                ; Go to exit point

		; Read ADC chip via command register
Access40:	AD_RD8  adc_regCmd 		; Read ADC register  
		ld	adc_regData,a		; Save data for A/D read CMD c1r40
		   
Access80:	ret                             ; Exit
                                                    

;****************************************************************************************
;ROUTINE:	RN_Ready
;DESCRIPTION:	Routine to change Rabbitnet status back to a slave READY state. Function 
;               will also make sure changing the status occurs during Non-Rabbitnet 
;               activtiy.  
;PARAMETERS:	None
;RETURN VALUES: None
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
;* Board Initialization Routine for Hardware Reset 
;****************************************************************************************
.brd_hard_init:	ret  	

;****************************************************************************************
;* Board Initialization Routine for Software Reset 
;****************************************************************************************
.brd_soft_init:	ret			
     
     
;****************************************************************************************				
;	User Handler that will get executed in the SPI interrupt routine if
;       USER bit is set in the pend control status byte. 
;****************************************************************************************				    
.user_handler: ret   

        					
;**************************************************************************************** 
;* Main Board Initialization Routine 
;****************************************************************************************
.brd_init:	ld      a,#$00        		; Initialize port A, PA7-PA0 as inputs                  
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
		ld      a,#$17			
		ld      PCDR,a   
		ld	PCDDR,a    
		ld      PCOR,a                                    

		ld      a,#$06         		; Initialize A/D chip IO pins 3-0 
		ld      mac_cmd,a               ; as outputs.
		ld      a,#$0F
		ld      mac_data,a
	        AD_WR8  mac_cmd,mac_data
	           
        	ld      a,#$07         		; Initialize A/D Reference/Oscillator 
		ld      mac_cmd,a               ; control register.
		ld      a,#$3E        
		ld      mac_data,a             
                AD_WR8	mac_cmd,mac_data
                      
                ld      a,#$03			; Initialize ADC control register  
                ld      mac_cmd,a               ; (read-back = mode 1)
		ld      a,#$04        
		ld      mac_data,a             
                AD_WR8	mac_cmd,mac_data
                	    
		ld      a,#$05     		; Setup A/D ch 3-0 as unipolar inputs
		ld      mac_cmd,a               
		ld      a,#$0A        
		ld      mac_data,a   
		ld	ADIOShadow,a            ; Update AIO port shadow register
		AD_WR8	mac_cmd,mac_data          
	        	  		    
		ld	a,#$0A          	; Setup A/D ch 7-4 as unipolar inputs
		ld      PBDR,a                  
		ld	PBDRShadow,a  		; Update port B shadow register             
                
                ld	a,#$88        		; Setup ADC CMD for a gain of X1         
		ld	ad0_cmd,a     		; Initial ch0 command variable
		inc	a	            			
		ld	ad1_cmd,a     		; Initial ch1 command variable 	
		inc	a 
		ld	ad2_cmd,a		; Initial ch2 command variable 		
		inc	a 
		ld	ad3_cmd,a		; Initial ch3 command variable 		
		inc	a 
		ld	ad4_cmd,a		; Initial ch4 command variable 		
		inc	a 		
		ld	ad5_cmd,a  		; Initial ch5 command variable
		inc	a 
		ld	ad6_cmd,a		; Initial ch6 command variable 		
		inc	a 
               	ld	ad7_cmd,a 		; Initial ch7 command variable
               	  	
  		clr	adc_initial      	; Clear A/D configure flag
		clr	adc_access      	; Clear direct A/D access flag
		clr	adc_convert     	; Clear single-channel convert flag
		clr	led_active	
		clr 	adcstatus 
		ld	a,#$55
		ld	{ad0_data+0},a
		ld	{ad1_data+0},a
		ld	{ad2_data+0},a
		ld	{ad3_data+0},a
		ld	{ad4_data+0},a
		ld	{ad5_data+0},a
		ld	{ad6_data+0},a
		ld	{ad7_data+0},a 
		ld	a,#$AA
		ld	{ad0_data+1},a
		ld	{ad1_data+1},a
		ld	{ad2_data+1},a
		ld	{ad3_data+1},a
		ld	{ad4_data+1},a
		ld	{ad5_data+1},a
		ld	{ad6_data+1},a
		ld	{ad7_data+1},a
		ret
                           
                          
;****************************************************************************************
;* Rabbitnet Command Functions  
;****************************************************************************************
;----------------------------------------------------------------------------------------
; Command:	c0r10   
; Description:  Configure ADC channel 0   	
;----------------------------------------------------------------------------------------
c0r10: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$00                                                  
		ld	adc_channel,a                                            
		ld	a,#$FF
		ld	adc_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r11   
; Description:  Configure ADC channel 1   	
;----------------------------------------------------------------------------------------
c0r11: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$01
		ld	adc_channel,a  
		ld	a,#$FF
		ld	adc_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r12   
; Description:  Configure ADC channel 2   	
;----------------------------------------------------------------------------------------
c0r12: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$02
		ld	adc_channel,a  
		ld	a,#$FF
		ld	adc_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r13   
; Description:  Configure ADC channel 3   	
;----------------------------------------------------------------------------------------
c0r13: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$03
		ld	adc_channel,a  
		ld	a,#$FF
		ld	adc_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r24   
; Description:  Configure ADC channel 4   	
;----------------------------------------------------------------------------------------
c0r14: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$04
		ld	adc_channel,a  
		ld	a,#$FF
		ld	adc_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r15   
; Description:  Configure ADC channel 5   	
;----------------------------------------------------------------------------------------
c0r15: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$05
		ld	adc_channel,a  
		ld	a,#$FF
		ld	adc_initial,a
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r16   
; Description:  Configure ADC channel 6   	
;----------------------------------------------------------------------------------------
c0r16: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$06
		ld	adc_channel,a
		ld	a,#$FF
		ld	adc_initial,a  
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it


;----------------------------------------------------------------------------------------
; Command:	c0r17   
; Description:  Configure ADC channel 7   	
;----------------------------------------------------------------------------------------
c0r17: 		ld	a,SPIDR 		; Send previous byte to master
		call	rn_xfer	
		ld	adc_config,a 
		ld	a,#$07
		ld	adc_channel,a
		ld	a,#$FF
		ld	adc_initial,a  
		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c1r10   
; Description:  Read ADC update status for channels 0 - 7     	
;----------------------------------------------------------------------------------------
c1r10: 		ld      a,adcstatus		; Load A w\ADC channel status             
      		call	rn_xfer			; Send byte to master
                jp	exitcmd	


;----------------------------------------------------------------------------------------
; Command:	c0r20   
; Description:  Start Conversion on ADC channel 0     	
;----------------------------------------------------------------------------------------
c0r20:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	adc_convert,a   
                ld	a,#ad0_cmd
		ld	ptrCmdCH,a   
		ld	a,#ad0_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad0_data.h
		ld	{ptrDataCH+0},a
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r21   
; Description:  Start Conversion on ADC channel 1     	
;----------------------------------------------------------------------------------------	 
c0r21:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	adc_convert,a     
                ld	a,#ad1_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad1_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad1_data.h
		ld	{ptrDataCH+0},a
		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r22   
; Description:  Start Conversion on ADC channel 2     	
;----------------------------------------------------------------------------------------		
c0r22:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	adc_convert,a  
                 ld	a,#ad2_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad2_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad2_data.h
		ld	{ptrDataCH+0},a                                                                     
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r23   
; Description:  Start Conversion on ADC channel 3     	
;----------------------------------------------------------------------------------------		
c0r23:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	adc_convert,a
                ld	a,#ad3_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad3_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad3_data.h
		ld	{ptrDataCH+0},a                              
 		jp	exitcmd			;  and go process it
        
;----------------------------------------------------------------------------------------
; Command:	c0r24   
; Description:  Start Conversion on ADC channel 4     	
;----------------------------------------------------------------------------------------
c0r24:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY         
                ld	a,#$FF
                ld	adc_convert,a
                ld	a,#ad4_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad4_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad4_data.h
		ld	{ptrDataCH+0},a
  		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r25   
; Description: Start Conversion on ADC channel 5      	
;----------------------------------------------------------------------------------------
c0r25:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	adc_convert,a 
                ld	a,#ad5_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad5_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad5_data.h
		ld	{ptrDataCH+0},a
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r26   
; Description:  Start Conversion on ADC channel 6     	
;----------------------------------------------------------------------------------------
c0r26:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
                ld	a,#$FF
                ld	adc_convert,a
                 ld	a,#ad6_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad6_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad6_data.h
		ld	{ptrDataCH+0},a
 		jp	exitcmd			;  and go process it

;----------------------------------------------------------------------------------------
; Command:	c0r27   
; Description:  Start Conversion on ADC channel 7     	
;----------------------------------------------------------------------------------------
c0r27:		bset	rn_stat,#BUSY   
		bres	rn_stat,#READY
	        ld	a,#$FF
                ld	adc_convert,a
                ld	a,#ad7_cmd
		ld	ptrCmdCH,a    
		ld	a,#ad7_data.l
		ld	{ptrDataCH+1},a
		ld	a,#ad7_data.h
		ld	{ptrDataCH+0},a
 		jp	exitcmd			;  and go process it


;----------------------------------------------------------------------------------------
; Command:	c1r20   
; Description:  Read ADC channel 0 Rawdata and Calibration data.   	
;----------------------------------------------------------------------------------------
c1r20:	        ld      a,rn_cmd		; Load A with Rabbitnet CMD received 
                and	a,#$07			; Mask out all bits except bits 2-0
 		ld	x,a
        	sll	x
        	ld	adc_index,x

		ld	a,#ad0_data.l           ; Load LSB of start Addr of Rawdata array
		ld	{ptrData+1},a				
		ld	a,#ad0_data.h           ; Load MSB of start Addr of Rawdata array
		ld	{ptrData+0},a
                                                      
		inc	x   
		ld	a,([ptrData.w],x)          ; Get ch0 LSB rawdata value                     
       		call	rn_xfer			; Send byte to master
		dec	x
		ld	a,([ptrData.w],x)        
		call	rn_xfer			; Send byte to master
		

		ld	a,#ptr_cal0.l           ; Load LSB of start Addr of Rawdata array
		ld	{ptrTemp+1},a				
		ld	a,#ptr_cal0.h           ; Load MSB of start Addr of Rawdata array
		ld	{ptrTemp+0},a
                            
		ld	x,adc_index 
		ld	a,([ptrTemp.w],x)
		ld	{ptrRdCal+0},a  
		inc	x       
		ld	a,([ptrTemp.w],x)
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
		jp	exitcmd			; Exit  

;----------------------------------------------------------------------------------------
; Command:	c0r30   
; Description:  Command to write the calibration data to flash for a given channel, the 
;               master must send one of the A/D configuration commands c0r10-c0r17 before 
;               sending this command. 
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
;               must send one of the A/D configuration commands c0r10-c0r17 before 
;               sending this command.     	
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
; Description:  Send ADC Read command to ADC chip, which will cause the board to go 
;               busy. The actual data from the ADC chip must be read with a second
;               command c1r40 after the slave status changes back to a ready state.    	
;----------------------------------------------------------------------------------------		
c0r40: 		call	rn_xfer			; Wait for byte from master
		ld	adc_regCmd,a		; Get read command from master
		ld	a,#$FF                  ; Load A w/value to indicate ADC access
		ld	adc_access,a            ; Set flag to indicate ADC access
		cpl	a                       ; Do 1's complement on reg A
		ld	adc_regMode,a	        ; Set flag to indicate Read operation
		bset	rn_stat,#BUSY           ; Set status to a Slave BUSY state
		bres	rn_stat,#READY          ; Clear Ready bit
		jp	exitcmd			; Go wait for command to terminate

;----------------------------------------------------------------------------------------
; Command:	c1r40   
; Description:  Read ADC data which was read previously via command c0r40     	
;----------------------------------------------------------------------------------------		
c1r40: 		ld	a,adc_regData 
		call	rn_xfer			; Send data byte to the master
		jp	exitcmd			; Go wait for command to terminate
 
;----------------------------------------------------------------------------------------
; Command:	c0r41   
; Description:  Write ADC internal register directly with 8 bit data value.   	
;----------------------------------------------------------------------------------------
c0r41: 		call	rn_xfer			; Send byte to master
		ld	adc_regCmd,a           	; Save ADC regData for direct access
		call	rn_xfer			; Send byte to master
	        ld	adc_regData,a           ; Save ADC regCMD for direct access
		ld	a,#$FF                  ; Load A with flag indicator
		ld	adc_access,a            ; Set flag for direct ADC chip access
		ld	adc_regMode,a		; Set flag to indicate write operation
		bset	rn_stat,#BUSY   	; Set status to a Slave BUSY state
		bres	rn_stat,#READY	        ; Clear Ready bit
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
		dc.w	c0r11			; c0r11
		dc.w	c0r12			; c0r12
		dc.w	c0r13			; c0r13
		dc.w	c0r14			; c0r14
		dc.w	c0r15			; c0r15
		dc.w	c0r16			; c0r16
		dc.w	c0r17			; c0r17
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
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
		dc.w	c1r30			; c1r30
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


